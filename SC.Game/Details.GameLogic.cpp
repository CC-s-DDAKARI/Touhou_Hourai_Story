using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Diagnostics;

using namespace std;

GameLogic::GameLogic() : Object()
{
	auto pDevice = GlobalVar.device->pDevice.Get();

	// ���� �ð��� �����ϰ� ������ Ÿ�̸� ������ �����մϴ�.
	timer = new StepTimer();
	physicsTimer = new StepTimer();
	physicsTimer->TargetElapsedTicksPerSecond = GlobalVar.pApp->AppConfig.PhysicsUpdatePerSeconds;
	physicsTimer->IsFixedTimeStep = true;

	// ��� �Ҵ�� ��ü�� �����մϴ�.
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocator[0] ) ) );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocator[1] ) ) );
	deviceContext = new CDeviceContext( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );
	visibleViewStorage = new VisibleViewStorage( GlobalVar.device.Get() );

	// ������ ��⸦ ���� �ڵ��� �����մϴ�.
	waitingHandle = new Threading::Event();

	// �ʿ��� ���۸� �Ҵ��մϴ�.
	geometryBuffer = new GeometryBuffer( GlobalVar.device );
	hdrBuffer = new HDRBuffer( GlobalVar.device );
	hdrComputedBuffer = new HDRComputedBuffer( GlobalVar.device );

	// ���� ������ �ڻ��� �����մϴ�.
	skyboxMesh = Mesh::CreateCube( "skybox_mesh" );

	// ��Ƽ �����带 ���� �߰� ��� �Ҵ�� ��ü�� �����մϴ�.
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
	// ��� ��ȯ ��û�� ���� ��� ����� ��ȯ�մϴ�.
	if ( SceneManager::currentScene.Get() )
	{
		currentScene = move( SceneManager::currentScene );
	}

	// ��� ���� �Լ��� ȣ���մϴ�.
	if ( currentScene->firstUpdate == false )
	{
		currentScene->Start();
		currentScene->firstUpdate = true;
	}
	currentScene->Update();

	// �� ������ ������Ʈ�� ���� �� ��ũ�� �̵����� �ʱ�ȭ�մϴ�.
	GlobalVar.scrollDelta = { 0, 0 };

	// ���� Ÿ�̸� ���¸� �����մϴ�. ���� Ÿ���� �����ٸ� ���� ������ �����մϴ�.
	physicsTimer->Tick( [this]() { FixedUpdate(); } );
}

void GameLogic::FixedUpdate()
{
	GlobalVar.fixedFrameIndex += 1;
	if ( GlobalVar.fixedFrameIndex >= 2 ) GlobalVar.fixedFrameIndex = 0;

	// ����� ���� ���� �Լ��� ȣ���մϴ�.
	currentScene->FixedUpdate();
}

void GameLogic::Render()
{
	int frameIndex = GlobalVar.frameIndex;
	auto directQueue = GlobalVar.device->DirectQueue[0].Get();

	// �������� �����ϱ� �� ��ġ�� �ʱ�ȭ�մϴ�.
	visibleViewStorage->Reset();

	HR( pCommandAllocator[frameIndex]->Reset() );
	deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
	deviceContext->SetVisibleViewStorage( visibleViewStorage );

	auto& pCommandList = *deviceContext->pCommandList.Get();

	try
	{
		// ī�޶� �÷����� ������ ��쿡�� �������� �����մϴ�.
		auto& cameraCollection = currentScene->mSceneCameras;
		if ( !cameraCollection.empty() )
		{
			{	// �ִϸ��̼� ������ �޽��� �̸� ��Ű�� �۾��� �����մϴ�.
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
					// ����Ʈ ������ ���̴��� �����մϴ�.
					HR( pCommandAllocatorLights[frameIndex]->Reset() );

					mDeviceContextLights->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocatorLights[frameIndex].Get(), nullptr );
					mDeviceContextLights->SetVisibleViewStorage( currentScene->mVisibleViewStorage );
					auto& pCommandList = *mDeviceContextLights->pCommandList.Get();

					pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

					// �� ����Ʈ ��ü�� ���� �������� �����մϴ�.
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

			{	// �� ī�޶� ��ü�� ���� �������� �����մϴ�.
				HR( pCommandAllocatorRender[frameIndex]->Reset() );

				mDeviceContextRender->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocatorRender[frameIndex].Get(), nullptr );
				mDeviceContextRender->SetVisibleViewStorage( currentScene->mVisibleViewStorage );
				auto& pCommandList = *mDeviceContextRender->pCommandList.Get();

				pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

				for ( auto i : cameraCollection )
				{
					pViewCamera = i;

					// ���� ���۸� ���� Ÿ������ �����մϴ�.
					geometryBuffer->OMSetRenderTargets( mDeviceContextRender );

					if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
					{
						// ����� ��ī�̹ڽ��� ����� ��� �켱 ��ī�̹ڽ� �������� �����մϴ�.
						//pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

						//i->SetGraphicsRootConstantBufferView( deviceContext );
						//deviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
						//skyboxMesh->DrawIndexed( deviceContext );
					}

					// ���� ���� ���̴��� �����մϴ�.
					pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

					// ��鿡 ���Ե� ��� ��ü�� �������մϴ�.
					i->SetGraphicsRootConstantBufferView( mDeviceContextRender );
					pCommandList.ExecuteBundle( currentScene->mSceneBundleRender[GlobalVar.frameIndex][GlobalVar.fixedFrameIndex]->pCommandList.Get() );

					// ���� ���� ����� �����մϴ�.
					geometryBuffer->EndDraw( mDeviceContextRender );
				}

				mDeviceContextRender->Close();
				GlobalVar.device->DirectQueue[0]->Execute( mDeviceContextRender );
				lastPending[frameIndex] = directQueue->Signal();
			}

			// ������ �������� �Ϸ�� ������ ����մϴ�.
			while ( lightCompleted == 0 );

			deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
			deviceContext->SetVisibleViewStorage( visibleViewStorage );

			// HDR ���۸� ���� Ÿ������ �����մϴ�.
			hdrBuffer->OMSetRenderTargets( deviceContext );

			// HDR �������� �����մϴ�.
			ShaderBuilder::HDRShader_get().SetAll( deviceContext );

			// �Ű������� �Է��մϴ�.
			pViewCamera->SetGraphicsRootConstantBufferView( deviceContext );
			geometryBuffer->SetGraphicsRootShaderResourcesForLayer( deviceContext );

			// �� ���� ���� ���� ���� �н��� �����մϴ�.
			pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			Material::SetGraphicsRootShaderResources( deviceContext );

			for ( auto i : lightCollection )
			{
				i->SetGraphicsRootConstantBufferView( deviceContext );
				i->SetGraphicsRootShaderResources( deviceContext );
				pCommandList.DrawInstanced( 4, 1, 0, 0 );
			}

			// HDR ���� �������� �����մϴ�.
			ShaderBuilder::HDRColorShader_get().SetAll( deviceContext );

			// �Ű������� �Է��մϴ�.
			geometryBuffer->SetGraphicsRootShaderResourcesForColor( deviceContext );

			// HDR ���ۿ� ���� �������� �����մϴ�.
			pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );

			// HDR ���� ����� �����մϴ�.
			hdrBuffer->EndDraw( deviceContext );

			// �ð� ����� ��ȸ�մϴ�.
			timer->Tick();
			double t = timer->ElapsedSeconds;

			// �Ű������� �Է��մϴ�.
			ShaderBuilder::HDRComputeShader_get( 0 ).SetAll( deviceContext );
			hdrComputedBuffer->SetComputeRootTimestamp( deviceContext, t );
			hdrBuffer->SetComputeRootShaderResources( deviceContext );

			// ���� �н��� �����մϴ�.
			for ( int i = 0; i < 3; ++i )
			{
				// ���̴��� �����մϴ�.
				if ( i != 0 ) ShaderBuilder::HDRComputeShader_get( i ).SetAll( deviceContext );
				hdrComputedBuffer->Compute( deviceContext, i );
			}
		}

		// ���� �ĸ� ������ �ε����� �����ɴϴ�.
		auto idx = GlobalVar.swapChain->Index;
		auto pBackBuffer = GlobalVar.swapChain->ppBackBuffers[idx].Get();
		auto rtvHandle = GlobalVar.swapChain->RTVHandle[idx];

		// ���� ü���� �ĸ� ���۸� ���� Ÿ������ �����մϴ�.
		deviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
		pCommandList.OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

		// �ĸ� ���۸� �ܻ����� �ʱ�ȭ�մϴ�.
		constexpr float clear[4]{ 0.0f };
		pCommandList.ClearRenderTargetView( rtvHandle, clear, 0, nullptr );

		if ( !cameraCollection.empty() )
		{
			// �� ���� �������� �����մϴ�.
			ShaderBuilder::ToneMappingShader_get().SetAll( deviceContext );

			// HDR ���� �ؽ�ó �� ���� HDR ����� �����ϰ� �������� �����մϴ�.
			hdrBuffer->SetGraphicsRootShaderResources( deviceContext );
			hdrComputedBuffer->SetGraphicsRootConstantBuffers( deviceContext );
			pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );
		}

		// ��� ����� �ݰ� Ǫ���մϴ�.
		deviceContext->Close();
		directQueue->Execute( deviceContext );

		// ������ ��� ��ȣ�� �����մϴ�.
		lastPending[frameIndex++] = directQueue->Signal();

		// ������ �ε����� �����մϴ�.
		if ( frameIndex >= 2 ) frameIndex -= 2;
	}
	catch ( Exception* e )
	{
		// ���� ��ü�� �����ϰ� �⺻ �۾��� �����մϴ�.
		delete e;

		// ���� ����� ��� �����մϴ�.
		deviceContext->Close();
		visibleViewStorage->Reset();

		HR( pCommandAllocator[frameIndex]->Reset() );
		deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );

		// ���� �ĸ� ������ �ε����� �����ɴϴ�.
		auto idx = GlobalVar.swapChain->Index;
		auto pBackBuffer = GlobalVar.swapChain->ppBackBuffers[idx].Get();
		auto rtvHandle = GlobalVar.swapChain->RTVHandle[idx];

		// ���� ü���� �ĸ� ���۸� ���� Ÿ������ �����մϴ�.
		deviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
		pCommandList.OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

		// �ĸ� ���۸� �ܻ����� �ʱ�ȭ�մϴ�.
		constexpr float clear[4]{ 0.0f };
		pCommandList.ClearRenderTargetView( rtvHandle, clear, 0, nullptr );

		// ��� ����� �ݰ� Ǫ���մϴ�.
		deviceContext->Close();
		directQueue->Execute( deviceContext );

		// ������ ��� ��ȣ�� �����մϴ�.
		lastPending[frameIndex++] = directQueue->Signal();

		// ������ �ε����� �����մϴ�.
		if ( frameIndex >= 2 ) frameIndex -= 2;
	}
}

void GameLogic::ResizeBuffers( uint32 width, uint32 height )
{
	// �ʿ��� ���� ���ۿ��� ũ�� ���� ��û�� �����մϴ�.
	geometryBuffer->ResizeBuffers( width, height );
	hdrBuffer->ResizeBuffers( width, height );
	hdrComputedBuffer->ResizeBuffers( width, height );
}