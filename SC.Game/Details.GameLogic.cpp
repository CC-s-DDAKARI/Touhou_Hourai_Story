using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Diagnostics;

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

	// 멀티 스레드를 위한 추가 명령 할당기 개체를 생성합니다.
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocatorLights[0] ) ) );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocatorLights[1] ) ) );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocatorRender[0] ) ) );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocatorRender[1] ) ) );
	mDeviceContextLights = new CDeviceContext( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );
	mDeviceContextRender = new CDeviceContext( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );
}

GameLogic::~GameLogic()
{

}

void GameLogic::Update()
{
	// 장면 전환 요청이 있을 경우 장면을 전환합니다.
	if ( SceneManager::currentScene.Get() )
	{
		currentScene = move( SceneManager::currentScene );
	}

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

void GameLogic::Render()
{
	int frameIndex = GlobalVar.frameIndex;
	auto directQueue = GlobalVar.device->DirectQueue[0].Get();

	// 렌더링을 실행하기 전 장치를 초기화합니다.
	visibleViewStorage->Reset();

	HR( pCommandAllocator[frameIndex]->Reset() );
	deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
	deviceContext->SetVisibleViewStorage( visibleViewStorage );

	auto& pCommandList = *deviceContext->pCommandList.Get();

	try
	{
		// 카메라 컬렉션이 존재할 경우에만 렌더링을 수행합니다.
		auto& cameraCollection = currentScene->mSceneCameras;
		if ( !cameraCollection.empty() )
		{
			{	// 애니메이션 가능한 메쉬는 미리 스키닝 작업을 수행합니다.
				auto& skinningCollection = currentScene->mpSkinnedMeshRendererQueue->GetCollections();
				for ( auto i : skinningCollection )
				{
					pCommandList.SetComputeRootSignature( ShaderBuilder::pRootSignature_Skinning.Get() );
					pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_Skinning.Get() );

					i.pAnimator->SetInput( deviceContext );
					for ( auto j : i.SkinnedMeshRenderers )
					{
						j->Skinning( deviceContext );
					}
				}
			}

			deviceContext->Close();
			GlobalVar.device->DirectQueue[0]->Execute( deviceContext );
			lastPending[frameIndex] = directQueue->Signal();

			std::atomic<int> lightCompleted = 0;
			auto& lightCollection = currentScene->mSceneLights;
			Threading::ThreadPool::QueueUserWorkItem( [&]( object )
				{
					// 라이트 렌더링 셰이더를 설정합니다.
					HR( pCommandAllocatorLights[frameIndex]->Reset() );

					mDeviceContextLights->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocatorLights[frameIndex].Get(), nullptr );
					mDeviceContextLights->SetVisibleViewStorage( currentScene->mVisibleViewStorage );
					auto& pCommandList = *mDeviceContextLights->pCommandList.Get();

					pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

					// 각 라이트 객체에 대해 렌더링을 수행합니다.
					for ( auto i : lightCollection )
					{
						if ( i->IsShadowCast )
						{
							i->BeginDraw( mDeviceContextLights );
							i->SetGraphicsRootConstantBufferView( mDeviceContextLights );
							pCommandList.ExecuteBundle( currentScene->mSceneBundleLight[GlobalVar.frameIndex][GlobalVar.fixedFrameIndex]->pCommandList.Get() );
							i->EndDraw( mDeviceContextLights );
						}
					}

					mDeviceContextLights->Close();
					GlobalVar.device->DirectQueue[0]->Execute( mDeviceContextLights );
					lastPending[frameIndex] = directQueue->Signal();

					lightCompleted += 1;
				}
			, nullptr );

			Camera* pViewCamera = nullptr;

			{	// 각 카메라 개체에 대해 렌더링을 수행합니다.
				HR( pCommandAllocatorRender[frameIndex]->Reset() );

				mDeviceContextRender->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocatorRender[frameIndex].Get(), nullptr );
				mDeviceContextRender->SetVisibleViewStorage( currentScene->mVisibleViewStorage );
				auto& pCommandList = *mDeviceContextRender->pCommandList.Get();

				pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

				for ( auto i : cameraCollection )
				{
					pViewCamera = i;

					// 기하 버퍼를 렌더 타겟으로 설정합니다.
					geometryBuffer->OMSetRenderTargets( mDeviceContextRender );

					if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
					{
						// 장면이 스카이박스를 사용할 경우 우선 스카이박스 렌더링을 진행합니다.
						//pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

						//i->SetGraphicsRootConstantBufferView( deviceContext );
						//deviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
						//skyboxMesh->DrawIndexed( deviceContext );
					}

					// 기하 버퍼 셰이더를 선택합니다.
					pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

					// 장면에 포함된 모든 개체를 렌더링합니다.
					i->SetGraphicsRootConstantBufferView( mDeviceContextRender );
					pCommandList.ExecuteBundle( currentScene->mSceneBundleRender[GlobalVar.frameIndex][GlobalVar.fixedFrameIndex]->pCommandList.Get() );

					// 기하 버퍼 사용을 종료합니다.
					geometryBuffer->EndDraw( mDeviceContextRender );
				}

				mDeviceContextRender->Close();
				GlobalVar.device->DirectQueue[0]->Execute( mDeviceContextRender );
				lastPending[frameIndex] = directQueue->Signal();
			}

			// 라이팅 렌더링이 완료될 때까지 대기합니다.
			while ( lightCompleted == 0 );

			deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
			deviceContext->SetVisibleViewStorage( visibleViewStorage );

			// HDR 버퍼를 렌더 타겟으로 설정합니다.
			hdrBuffer->OMSetRenderTargets( deviceContext );

			// HDR 렌더링을 수행합니다.
			ShaderBuilder::HDRShader_get().SetAll( deviceContext );

			// 매개변수를 입력합니다.
			pViewCamera->SetGraphicsRootConstantBufferView( deviceContext );
			geometryBuffer->SetGraphicsRootShaderResourcesForLayer( deviceContext );

			// 각 조명에 대해 조명 렌더 패스를 진행합니다.
			pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			Material::SetGraphicsRootShaderResources( deviceContext );

			for ( auto i : lightCollection )
			{
				i->SetGraphicsRootConstantBufferView( deviceContext );
				i->SetGraphicsRootShaderResources( deviceContext );
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
	catch ( Exception* e )
	{
		// 예외 개체를 제거하고 기본 작업을 수행합니다.
		delete e;

		// 기존 명령을 모두 제거합니다.
		deviceContext->Close();
		visibleViewStorage->Reset();

		HR( pCommandAllocator[frameIndex]->Reset() );
		deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );

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

		// 명령 목록을 닫고 푸쉬합니다.
		deviceContext->Close();
		directQueue->Execute( deviceContext );

		// 마지막 명령 번호를 저장합니다.
		lastPending[frameIndex++] = directQueue->Signal();

		// 프레임 인덱스를 한정합니다.
		if ( frameIndex >= 2 ) frameIndex -= 2;
	}
}

void GameLogic::ResizeBuffers( uint32 width, uint32 height )
{
	// 필요한 각종 버퍼에게 크기 변경 요청을 수행합니다.
	geometryBuffer->ResizeBuffers( width, height );
	hdrBuffer->ResizeBuffers( width, height );
	hdrComputedBuffer->ResizeBuffers( width, height );
}