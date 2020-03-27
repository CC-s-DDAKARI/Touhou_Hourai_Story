using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Diagnostics;
using namespace SC::Threading;

using namespace std;

GameLogic::GameLogic() : Object()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	// 게임 시간을 정밀하게 측정할 타이머 도구를 생성합니다.
	timer = new StepTimer();
	physicsTimer = new StepTimer();
	physicsTimer->TargetElapsedTicksPerSecond = App::mApp->AppConfig.PhysicsUpdatePerSeconds;
	physicsTimer->IsFixedTimeStep = true;

	// 주 렌더링 디바이스 컨텍스트 개체를 생성합니다.
	mVisibleViewStorage = new VisibleViewStorage();
	mDeviceContext = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );

	// 스레드 대기를 위한 핸들을 생성합니다.
	waitingHandle = new Threading::Event();

	// 필요한 버퍼를 할당합니다.
	geometryBuffer = new GeometryBuffer( Graphics::mDevice );
	hdrBuffer = new HDRBuffer( Graphics::mDevice );
	hdrComputedBuffer = new HDRComputedBuffer( Graphics::mDevice );

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
	// 장면의 물리 갱신 함수를 호출합니다.
	currentScene->FixedUpdate();
}

void GameLogic::Render( int frameIndex )
{
	auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
	auto directQueue1 = Graphics::mDevice->DirectQueue[1].Get();
	auto& pCommandQueue = *directQueue->pCommandQueue.Get();
	auto pScene = currentScene.Get();

	// 렌더링을 실행하기 전 장치를 초기화합니다.
	mVisibleViewStorage->Reset();
	mDeviceContext->FrameIndex = frameIndex;
	mDeviceContext->Reset( directQueue, nullptr, nullptr );
	mDeviceContext->SetVisibleViewStorage( mVisibleViewStorage );

	auto& pCommandList = *mDeviceContext->pCommandList.Get();

	// 카메라 컬렉션이 존재할 경우에만 렌더링을 수행합니다.
	auto& cameraCollection = currentScene->mSceneCameras;
	if ( !cameraCollection.empty() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + 1 + Scene::NumThreadsForLight;

		TerrainBaking( frameIndex );
		MeshSkinning( frameIndex );
		GeometryLighting( frameIndex );
		GeometryWriting( frameIndex );

		// HDR 버퍼를 렌더 타겟으로 설정합니다.
		hdrBuffer->OMSetRenderTargets( mDeviceContext );

		// HDR 렌더링을 수행합니다.
		ShaderBuilder::HDRShader_get().SetAll( mDeviceContext );

		// 매개변수를 입력합니다.
		pScene->mSceneCameras.front()->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		geometryBuffer->SetGraphicsRootShaderResourcesForLayer( mDeviceContext );

		// 각 조명에 대해 조명 렌더 패스를 진행합니다.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Material::SetGraphicsRootShaderResources( mDeviceContext );

		for ( auto i : currentScene->mSceneLights )
		{
			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			i->SetGraphicsRootShaderResources( mDeviceContext );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );
		}

		// HDR 색상 렌더링을 수행합니다.
		ShaderBuilder::HDRColorShader_get().SetAll( mDeviceContext );

		// 매개변수를 입력합니다.
		geometryBuffer->SetGraphicsRootShaderResourcesForColor( mDeviceContext );

		// HDR 버퍼에 색상 렌더링을 수행합니다.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );

		// HDR 버퍼 사용을 종료합니다.
		hdrBuffer->EndDraw( mDeviceContext );

		// 시간 경과를 조회합니다.
		timer->Tick();
		double t = timer->ElapsedSeconds;

		// 매개변수를 입력합니다.
		ShaderBuilder::HDRComputeShader_get( 0 ).SetAll( mDeviceContext );
		hdrComputedBuffer->SetComputeRootTimestamp( mDeviceContext, t );
		hdrBuffer->SetComputeRootShaderResources( mDeviceContext );

		// 렌더 패스를 수행합니다.
		for ( int i = 0; i < 3; ++i )
		{
			// 셰이더를 선택합니다.
			if ( i != 0 ) ShaderBuilder::HDRComputeShader_get( i ).SetAll( mDeviceContext );
			hdrComputedBuffer->Compute( mDeviceContext, i );
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

		// 명령을 출력합니다.
		directQueue1->Execute( mLightThreads, deviceContextsForLight );
		auto signal = directQueue1->Signal();
		directQueue->Execute( pScene->mDeviceContextForRender );
		HR( directQueue->pCommandQueue->Wait( directQueue1->pFence.Get(), signal ) );
	}

	// 현재 후면 버퍼의 인덱스를 가져옵니다.
	auto idx = Graphics::mSwapChain->Index;
	auto pBackBuffer = Graphics::mSwapChain->ppBackBuffers[idx].Get();
	auto rtvHandle = Graphics::mSwapChain->RTVHandle[idx];

	// 스왑 체인의 후면 버퍼를 렌더 타겟으로 설정합니다.
	mDeviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
	pCommandList.OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	// 후면 버퍼를 단색으로 초기화합니다.
	constexpr float clear[4]{ 0.0f };
	pCommandList.ClearRenderTargetView( rtvHandle, clear, 0, nullptr );

	if ( !cameraCollection.empty() )
	{
		// 톤 매핑 렌더링을 수행합니다.
		ShaderBuilder::ToneMappingShader_get().SetAll( mDeviceContext );

		// HDR 원본 텍스처 및 계산된 HDR 상수를 설정하고 렌더링을 수행합니다.
		hdrBuffer->SetGraphicsRootShaderResources( mDeviceContext );
		hdrComputedBuffer->SetGraphicsRootConstantBuffers( mDeviceContext );
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );
	}

	// 명령 목록을 닫고 푸쉬합니다.
	mDeviceContext->Close();
	directQueue->Execute( mDeviceContext );
}

void GameLogic::ResizeBuffers( uint32 width, uint32 height )
{
	// 필요한 각종 버퍼에게 크기 변경 요청을 수행합니다.
	geometryBuffer->ResizeBuffers( width, height );
	hdrBuffer->ResizeBuffers( width, height );
	hdrComputedBuffer->ResizeBuffers( width, height );
}

void GameLogic::TerrainBaking( int frameIndex )
{
	ThreadPool::QueueUserWorkItem( [&, frameIndex]( object )
		{
			auto pScene = currentScene.Get();
			auto& mDeviceContext = pScene->mDeviceContextForTerrain;
			auto& computeQueue = Graphics::mDevice->ComputeQueue[0];

			pScene->mViewStorageForTerrain->Reset();
			mDeviceContext->FrameIndex = frameIndex;
			mDeviceContext->Reset( computeQueue.Get(), nullptr, ShaderBuilder::pPipelineState_Terrain.Get() );
			mDeviceContext->SetVisibleViewStorage( pScene->mViewStorageForTerrain );

			auto& pCommandList = *mDeviceContext->pCommandList.Get();
			pCommandList.SetComputeRootSignature( ShaderBuilder::pRootSignature_Terrain.Get() );

			for ( auto i : pScene->mSceneTerrains )
			{
				i->Bake( mDeviceContext, frameIndex );
			}

			mDeviceContext->Close();

			computeQueue->Execute( mDeviceContext );
			auto fenceValue = computeQueue->Signal();
			HR( Graphics::mDevice->DirectQueue[0]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );
			HR( Graphics::mDevice->DirectQueue[1]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );

			FetchAndSetThread();
		}
	, nullptr );
}

void GameLogic::MeshSkinning( int frameIndex )
{
	ThreadPool::QueueUserWorkItem( [&, frameIndex]( object )
		{
			auto pScene = currentScene.Get();
			auto& mDeviceContext = pScene->mDeviceContextForSkinning;
			auto& computeQueue = Graphics::mDevice->ComputeQueue[0];

			auto& skinningCollection = pScene->mpSkinnedMeshRendererQueue->GetCollections();

			mDeviceContext->FrameIndex = frameIndex;
			mDeviceContext->Reset( computeQueue.Get(), nullptr, ShaderBuilder::pPipelineState_Skinning.Get() );

			auto& pCommandList = *mDeviceContext->pCommandList.Get();
			pCommandList.SetComputeRootSignature( ShaderBuilder::pRootSignature_Skinning.Get() );

			for ( auto i : skinningCollection )
			{
				i.pAnimator->SetInput( mDeviceContext, frameIndex );
				for ( auto j : i.SkinnedMeshRenderers )
				{
					j->Skinning( mDeviceContext );
				}
			}

			mDeviceContext->Close();

			computeQueue->Execute( mDeviceContext );
			auto fenceValue = computeQueue->Signal();
			HR( Graphics::mDevice->DirectQueue[0]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );
			HR( Graphics::mDevice->DirectQueue[1]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );

			FetchAndSetThread();
		}
	, nullptr );
}

void GameLogic::GeometryLighting( int frameIndex )
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
			mDeviceContext->FrameIndex = frameIndex;
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
		ThreadPool::QueueUserWorkItem( bind( &GameLogic::RenderSceneGraphForEachThreads, this, placeholders::_1, i, lightsPerThread[i], frameIndex ), nullptr );
	}
}

void GameLogic::GeometryWriting( int frameIndex )
{
	auto* pScene = currentScene.Get();
	auto& mDeviceContext = pScene->mDeviceContextForRender;

	mDeviceContext->FrameIndex = frameIndex;
	mDeviceContext->Reset( nullptr, nullptr, ShaderBuilder::pPipelineState_ColorShader.Get() );
	auto& pCommandList = *mDeviceContext->pCommandList.Get();
	pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

	auto& mVisibleStorage = pScene->mViewStorageForRender;
	mVisibleStorage->Reset();
	mDeviceContext->SetVisibleViewStorage( mVisibleStorage );

	for ( auto i : pScene->mSceneCameras )
	{
		// 기하 버퍼를 렌더 타겟으로 설정합니다.
		geometryBuffer->OMSetRenderTargets( mDeviceContext );

		if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
		{
			// 장면이 스카이박스를 사용할 경우 우선 스카이박스 렌더링을 진행합니다.
			pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			mDeviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
			skyboxMesh->DrawIndexed( mDeviceContext );
		}

		// 기하 버퍼 셰이더를 선택합니다.
		pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

		// 장면에 포함된 모든 개체를 렌더링합니다.
		i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		pScene->Render( mDeviceContext, frameIndex );

		// 기하 버퍼 사용을 종료합니다.
		geometryBuffer->EndDraw( mDeviceContext );
	}

	mDeviceContext->Close();
}

void GameLogic::RenderSceneGraphForEachThreads( object, int threadIndex, list<Light*>& lights, int frameIndex )
{
	auto* pScene = currentScene.Get();
	auto& mDeviceContext = pScene->mDeviceContextsForLight[threadIndex];
	auto& pCommandList = *mDeviceContext->pCommandList.Get();

	for ( auto pLight : lights )
	{
		pLight->BeginDraw( mDeviceContext );
		pLight->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		pScene->Render( mDeviceContext, frameIndex );
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