using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Diagnostics;
using namespace SC::Threading;

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
}

GameLogic::~GameLogic()
{

}

void GameLogic::Update()
{
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

void GameLogic::Render( int frameIndex, int fixedFrameIndex )
{
	auto directQueue = GlobalVar.device->DirectQueue[0].Get();
	auto& pCommandQueue = *directQueue->pCommandQueue.Get();
	auto pScene = currentScene.Get();

	// �������� �����ϱ� �� ��ġ�� �ʱ�ȭ�մϴ�.
	visibleViewStorage->Reset();

	HR( pCommandAllocator[frameIndex]->Reset() );
	deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
	deviceContext->SetVisibleViewStorage( visibleViewStorage );

	auto& pCommandList = *deviceContext->pCommandList.Get();

	// ī�޶� �÷����� ������ ��쿡�� �������� �����մϴ�.
	auto& cameraCollection = currentScene->mSceneCameras;
	if ( !cameraCollection.empty() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + Scene::NumThreadsForLight;

		MeshSkinning( frameIndex, fixedFrameIndex );
		GeometryWriting( frameIndex, fixedFrameIndex );

		Camera* pViewCamera = nullptr;

		{	// �� ī�޶� ��ü�� ���� �������� �����մϴ�.
			//HR( pCommandAllocatorRender[frameIndex]->Reset() );

			//deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocatorRender[frameIndex].Get(), nullptr );
			deviceContext->SetVisibleViewStorage( visibleViewStorage );
			//auto& pCommandList = *deviceContext->pCommandList.Get();

			pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );
			pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

			for ( auto i : cameraCollection )
			{
				pViewCamera = i;

				// ���� ���۸� ���� Ÿ������ �����մϴ�.
				geometryBuffer->OMSetRenderTargets( deviceContext );

				if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
				{
					// ����� ��ī�̹ڽ��� ����� ��� �켱 ��ī�̹ڽ� �������� �����մϴ�.
					pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

					i->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
					deviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
					skyboxMesh->DrawIndexed( deviceContext );
				}

				// ���� ���� ���̴��� �����մϴ�.
				pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

				// ��鿡 ���Ե� ��� ��ü�� �������մϴ�.
				i->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
				currentScene->Render( deviceContext, frameIndex, fixedFrameIndex );

				// ���� ���� ����� �����մϴ�.
				geometryBuffer->EndDraw( deviceContext );
			}

			//deviceContext->Close();
			//GlobalVar.device->DirectQueue[0]->Execute( deviceContext );
			//lastPending[frameIndex] = directQueue->Signal();
		}

		//deviceContext->Reset( GlobalVar.device->DirectQueue[0].Get(), pCommandAllocator[frameIndex].Get(), nullptr );
		//deviceContext->SetVisibleViewStorage( visibleViewStorage );

		// HDR ���۸� ���� Ÿ������ �����մϴ�.
		hdrBuffer->OMSetRenderTargets( deviceContext );

		// HDR �������� �����մϴ�.
		ShaderBuilder::HDRShader_get().SetAll( deviceContext );

		// �Ű������� �Է��մϴ�.
		pViewCamera->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
		geometryBuffer->SetGraphicsRootShaderResourcesForLayer( deviceContext );

		// �� ���� ���� ���� ���� �н��� �����մϴ�.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Material::SetGraphicsRootShaderResources( deviceContext );

		for ( auto i : currentScene->mSceneLights )
		{
			i->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );
			i->SetGraphicsRootShaderResources( deviceContext, frameIndex, fixedFrameIndex );
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

		// ��� ��� ��� ����� �Ϸ�� ������ ����մϴ�.
		mCompletedEvent.WaitForSingleObject();

		// �����ִ� ������ ����̽� ���ؽ�Ʈ�� �����մϴ�.
		CDeviceContext* deviceContextsForLight[4]{ };
		for ( int i = 0; i < mLightThreads; ++i )
		{
			deviceContextsForLight[i] = pScene->mDeviceContextsForLight[i].Get();
			deviceContextsForLight[i]->Close();
		}

		// ������ ����� ����մϴ�.
		directQueue->Execute( mLightThreads, deviceContextsForLight );
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

void GameLogic::ResizeBuffers( uint32 width, uint32 height )
{
	// �ʿ��� ���� ���ۿ��� ũ�� ���� ��û�� �����մϴ�.
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

	// ����̽� ���ؽ�Ʈ ��� �� ��� �ʱ�ȭ�մϴ�.
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
			// �����带 �������� ���� ��� �����մϴ�.
			FetchAndSetThread();
		}
	}

	list<Light*> lightsPerThread[Scene::NumThreadsForLight];

	for ( auto& i : pScene->mSceneLights )
	{
		if ( i->IsShadowCast )
		{
			// �׸��ڸ� �����ϴ� ������ ������ ������ �°� ����մϴ�.
			lightsPerThread[threadIndex++].push_back( i );
		}
	}

	// ���� �����带 �����մϴ�.
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