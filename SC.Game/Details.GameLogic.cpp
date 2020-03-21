using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Diagnostics;
using namespace SC::Threading;

using namespace std;

GameLogic::GameLogic() : Object()
{
	auto pDevice = GlobalVar.device->pDevice.Get();

	// 게임 시간을 정밀하게 측정할 타이머 도구를 생성합니다.
	timer = new StepTimer();
	physicsTimer = new StepTimer();
	physicsTimer->TargetElapsedTicksPerSecond = GlobalVar.pApp->AppConfig.PhysicsUpdatePerSeconds;
	physicsTimer->IsFixedTimeStep = true;

	// 명령 할당기 개체를 생성합니다.
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocator[0] ) ) );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocator[1] ) ) );
	deviceContext = new CDeviceContext( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );
	visibleViewStorage = new VisibleViewStorage( GlobalVar.device.Get() );

	// 스레드 대기를 위한 핸들을 생성합니다.
	waitingHandle = new Threading::Event();

	// 필요한 버퍼를 할당합니다.
	geometryBuffer = new GeometryBuffer( GlobalVar.device );
	hdrBuffer = new HDRBuffer( GlobalVar.device );
	hdrComputedBuffer = new HDRComputedBuffer( GlobalVar.device );

	// 기초 데이터 자산을 생성합니다.
	skyboxMesh = Mesh::CreateCube( "skybox_mesh" );
}

GameLogic::~GameLogic()
{

}

void GameLogic::Update()
{
	// 장면 갱신 함수를 호출합니다.
	if ( currentScene->firstUpdate == false )
	{
		currentScene->Start();
		currentScene->firstUpdate = true;
	}
	currentScene->Update();

	// 매 프레임 업데이트가 끝난 후 스크롤 이동량을 초기화합니다.
	GlobalVar.scrollDelta = { 0, 0 };

	// 물리 타이머 상태를 갱신합니다. 물리 타임이 지났다면 물리 연산을 실행합니다.
	physicsTimer->Tick( [this]() { FixedUpdate(); } );
}

void GameLogic::FixedUpdate()
{
	GlobalVar.fixedFrameIndex += 1;
	if ( GlobalVar.fixedFrameIndex >= 2 ) GlobalVar.fixedFrameIndex = 0;

	// 장면의 물리 갱신 함수를 호출합니다.
	currentScene->FixedUpdate();
}

void GameLogic::Render( int frameIndex, int fixedFrameIndex )
{
	auto directQueue = GlobalVar.device->DirectQueue[0].Get();
	auto& pCommandQueue = *directQueue->pCommandQueue.Get();
	auto pScene = currentScene.Get();

	// 렌더링을 실행하기 전 장치를 초기화합니다.
	visibleViewStorage->Reset();

	HR( pCommandAllocator[frameIndex]->Reset() );
	deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
	deviceContext->SetVisibleViewStorage( visibleViewStorage );

	auto& pCommandList = *deviceContext->pCommandList.Get();

	// 카메라 컬렉션이 존재할 경우에만 렌더링을 수행합니다.
	auto& cameraCollection = currentScene->mSceneCameras;
	if ( !cameraCollection.empty() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + Scene::NumThreadsForLight;

		MeshSkinning( frameIndex, fixedFrameIndex );
		GeometryWriting( frameIndex, fixedFrameIndex );

		Camera* pViewCamera = nullptr;

		{	// 각 카메라 개체에 대해 렌더링을 수행합니다.
			//HR( pCommandAllocatorRender[frameIndex]->Reset() );

			//deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocatorRender[frameIndex].Get(), nullptr );
			deviceContext->SetVisibleViewStorage( visibleViewStorage );
			//auto& pCommandList = *deviceContext->pCommandList.Get();

			pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );
			pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

			for ( auto i : cameraCollection )
			{
				pViewCamera = i;

				// 기하 버퍼를 렌더 타겟으로 설정합니다.
				geometryBuffer->OMSetRenderTargets( deviceContext );

				if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
				{
					// 장면이 스카이박스를 사용할 경우 우선 스카이박스 렌더링을 진행합니다.
					pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

					i->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
					deviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
					skyboxMesh->DrawIndexed( deviceContext );
				}

				// 기하 버퍼 셰이더를 선택합니다.
				pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

				// 장면에 포함된 모든 개체를 렌더링합니다.
				i->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
				currentScene->Render( deviceContext, frameIndex, fixedFrameIndex );

				// 기하 버퍼 사용을 종료합니다.
				geometryBuffer->EndDraw( deviceContext );
			}

			//deviceContext->Close();
			//GlobalVar.device->DirectQueue[0]->Execute( deviceContext );
			//lastPending[frameIndex] = directQueue->Signal();
		}

		//deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
		//deviceContext->SetVisibleViewStorage( visibleViewStorage );

		// HDR 버퍼를 렌더 타겟으로 설정합니다.
		hdrBuffer->OMSetRenderTargets( deviceContext );

		// HDR 렌더링을 수행합니다.
		ShaderBuilder::HDRShader_get().SetAll( deviceContext );

		// 매개변수를 입력합니다.
		pViewCamera->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
		geometryBuffer->SetGraphicsRootShaderResourcesForLayer( deviceContext );

		// 각 조명에 대해 조명 렌더 패스를 진행합니다.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Material::SetGraphicsRootShaderResources( deviceContext );

		for ( auto i : currentScene->mSceneLights )
		{
			i->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
			i->SetGraphicsRootShaderResources( deviceContext, frameIndex, fixedFrameIndex );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );
		}

		// HDR 색상 렌더링을 수행합니다.
		ShaderBuilder::HDRColorShader_get().SetAll( deviceContext );

		// 매개변수를 입력합니다.
		geometryBuffer->SetGraphicsRootShaderResourcesForColor( deviceContext );

		// HDR 버퍼에 색상 렌더링을 수행합니다.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );

		// HDR 버퍼 사용을 종료합니다.
		hdrBuffer->EndDraw( deviceContext );

		// 시간 경과를 조회합니다.
		timer->Tick();
		double t = timer->ElapsedSeconds;

		// 매개변수를 입력합니다.
		ShaderBuilder::HDRComputeShader_get( 0 ).SetAll( deviceContext );
		hdrComputedBuffer->SetComputeRootTimestamp( deviceContext, t );
		hdrBuffer->SetComputeRootShaderResources( deviceContext );

		// 렌더 패스를 수행합니다.
		for ( int i = 0; i < 3; ++i )
		{
			// 셰이더를 선택합니다.
			if ( i != 0 ) ShaderBuilder::HDRComputeShader_get( i ).SetAll( deviceContext );
			hdrComputedBuffer->Compute( deviceContext, i );
		}

		// 모든 명령 목록 기록이 완료될 때까지 대기합니다.
		mCompletedEvent.WaitForSingleObject();

		// 열려있던 라이팅 디바이스 컨텍스트를 종료합니다.
		CDeviceContext* deviceContextsForLight[4]{ };
		for ( int i = 0; i < mLightThreads; ++i )
		{
			deviceContextsForLight[i] = pScene->mDeviceContextsForLight[i].Get();
			deviceContextsForLight[i]->Close();
		}

		// 라이팅 명령을 출력합니다.
		directQueue->Execute( mLightThreads, deviceContextsForLight );
	}

	// 현재 후면 버퍼의 인덱스를 가져옵니다.
	auto idx = GlobalVar.swapChain->Index;
	auto pBackBuffer = GlobalVar.swapChain->ppBackBuffers[idx].Get();
	auto rtvHandle = GlobalVar.swapChain->RTVHandle[idx];

	// 스왑 체인의 후면 버퍼를 렌더 타겟으로 설정합니다.
	deviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
	pCommandList.OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	// 후면 버퍼를 단색으로 초기화합니다.
	constexpr float clear[4]{ 0.0f };
	pCommandList.ClearRenderTargetView( rtvHandle, clear, 0, nullptr );

	if ( !cameraCollection.empty() )
	{
		// 톤 매핑 렌더링을 수행합니다.
		ShaderBuilder::ToneMappingShader_get().SetAll( deviceContext );

		// HDR 원본 텍스처 및 계산된 HDR 상수를 설정하고 렌더링을 수행합니다.
		hdrBuffer->SetGraphicsRootShaderResources( deviceContext );
		hdrComputedBuffer->SetGraphicsRootConstantBuffers( deviceContext );
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );
	}

	// 명령 목록을 닫고 푸쉬합니다.
	deviceContext->Close();
	directQueue->Execute( deviceContext );

	// 마지막 명령 번호를 저장합니다.
	lastPending[frameIndex++] = directQueue->Signal();

	// 프레임 인덱스를 한정합니다.
	if ( frameIndex >= 2 ) frameIndex -= 2;
}

void GameLogic::ResizeBuffers( uint32 width, uint32 height )
{
	// 필요한 각종 버퍼에게 크기 변경 요청을 수행합니다.
	geometryBuffer->ResizeBuffers( width, height );
	hdrBuffer->ResizeBuffers( width, height );
	hdrComputedBuffer->ResizeBuffers( width, height );
}

void GameLogic::MeshSkinning( int frameIndex, int fixedFrameIndex )
{
	ThreadPool::QueueUserWorkItem( [&, frameIndex, fixedFrameIndex]( object )
		{
			auto pScene = currentScene.Get();
			auto& mDeviceContext = pScene->mDeviceContextForSkinning;
			auto& computeQueue = GlobalVar.device->ComputeQueue;

			auto& skinningCollection = pScene->mpSkinnedMeshRendererQueue->GetCollections();

			mDeviceContext->Reset( computeQueue.Get(), nullptr, ShaderBuilder::pPipelineState_Skinning.Get() );

			auto& pCommandList = *mDeviceContext->pCommandList.Get();
			pCommandList.SetComputeRootSignature( ShaderBuilder::pRootSignature_Skinning.Get() );

			for ( auto i : skinningCollection )
			{
				i.pAnimator->SetInput( mDeviceContext, frameIndex, fixedFrameIndex );
				for ( auto j : i.SkinnedMeshRenderers )
				{
					j->Skinning( mDeviceContext );
				}
			}

			mDeviceContext->Close();

			computeQueue->Execute( mDeviceContext );
			auto fenceValue = computeQueue->Signal();
			HR( GlobalVar.device->DirectQueue[0]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );

			FetchAndSetThread();
		}
	, nullptr );
}

void GameLogic::GeometryWriting( int frameIndex, int fixedFrameIndex )
{
	auto* pScene = currentScene.Get();
	int threadIndex = 0;

	// 디바이스 컨텍스트 사용 전 모두 초기화합니다.
	int minThreadCount = min( ( int )pScene->mSceneLights.size(), Scene::NumThreadsForLight );
	this->mLightThreads = minThreadCount;
	for ( int i = 0; i < Scene::NumThreadsForLight; ++i )
	{
		if ( i < minThreadCount )
		{
			auto& mDeviceContext = pScene->mDeviceContextsForLight[i];
			mDeviceContext->Reset( nullptr, nullptr, ShaderBuilder::pPipelineState_ShadowCastShader.Get() );
			mDeviceContext->pCommandList->SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

			auto& mStorage = pScene->mViewStoragesForLight[i];
			mStorage->Reset();
			mDeviceContext->SetVisibleViewStorage( mStorage );
		}
		else
		{
			// 스레드를 생성하지 않을 경우 무시합니다.
			FetchAndSetThread();
		}
	}

	list<Light*> lightsPerThread[Scene::NumThreadsForLight];

	for ( auto& i : pScene->mSceneLights )
	{
		if ( i->IsShadowCast )
		{
			// 그림자를 생성하는 조명을 스레드 개수에 맞게 배분합니다.
			lightsPerThread[threadIndex++].push_back( i );
		}
	}

	// 조명 스레드를 실행합니다.
	for ( int i = 0; i < minThreadCount; ++i )
	{
		ThreadPool::QueueUserWorkItem( bind( &GameLogic::RenderSceneGraphForEachThreads, this, placeholders::_1, i, lightsPerThread[i], frameIndex, fixedFrameIndex ), nullptr );
	}
}

void GameLogic::RenderSceneGraphForEachThreads( object, int threadIndex, list<Light*>& lights, int frameIndex, int fixedFrameIndex )
{
	auto* pScene = currentScene.Get();
	auto& mDeviceContext = pScene->mDeviceContextsForLight[threadIndex];
	auto& pCommandList = *mDeviceContext->pCommandList.Get();

	for ( auto pLight : lights )
	{
		pLight->BeginDraw( mDeviceContext );
		pLight->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex, fixedFrameIndex );
		pScene->Render( mDeviceContext, frameIndex, fixedFrameIndex );
		pLight->EndDraw( mDeviceContext );
	}

	FetchAndSetThread();
}

void GameLogic::FetchAndSetThread()
{
	mCompletedValue.fetch_add( 1 );
	if ( mCompletedValue >= mCompletedGoal )
	{
		mCompletedEvent.Set();
	}
}