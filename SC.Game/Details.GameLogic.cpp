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

	// �� ������ ����̽� ���ؽ�Ʈ ��ü�� �����մϴ�.
	mVisibleViewStorage = new VisibleViewStorage( GlobalVar.device.Get() );
	mDeviceContext = new CDeviceContextAndAllocator( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );

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
	// ����� ���� ���� �Լ��� ȣ���մϴ�.
	currentScene->FixedUpdate();
}

void GameLogic::Render( int frameIndex )
{
	auto directQueue = GlobalVar.device->DirectQueue[0].Get();
	auto directQueue1 = GlobalVar.device->DirectQueue[1].Get();
	auto& pCommandQueue = *directQueue->pCommandQueue.Get();
	auto pScene = currentScene.Get();

	// �������� �����ϱ� �� ��ġ�� �ʱ�ȭ�մϴ�.
	mVisibleViewStorage->Reset();
	mDeviceContext->Reset( directQueue, ( ID3D12CommandAllocator* )&frameIndex, nullptr );
	mDeviceContext->SetVisibleViewStorage( mVisibleViewStorage );

	auto& pCommandList = *mDeviceContext->pCommandList.Get();

	// ī�޶� �÷����� ������ ��쿡�� �������� �����մϴ�.
	auto& cameraCollection = currentScene->mSceneCameras;
	if ( !cameraCollection.empty() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + 1 + Scene::NumThreadsForLight;

		TerrainBaking( frameIndex );
		MeshSkinning( frameIndex );
		GeometryLighting( frameIndex );
		GeometryWriting( frameIndex );

		// HDR ���۸� ���� Ÿ������ �����մϴ�.
		hdrBuffer->OMSetRenderTargets( mDeviceContext );

		// HDR �������� �����մϴ�.
		ShaderBuilder::HDRShader_get().SetAll( mDeviceContext );

		// �Ű������� �Է��մϴ�.
		pScene->mSceneCameras.front()->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		geometryBuffer->SetGraphicsRootShaderResourcesForLayer( mDeviceContext );

		// �� ���� ���� ���� ���� �н��� �����մϴ�.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Material::SetGraphicsRootShaderResources( mDeviceContext );

		for ( auto i : currentScene->mSceneLights )
		{
			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			i->SetGraphicsRootShaderResources( mDeviceContext );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );
		}

		// HDR ���� �������� �����մϴ�.
		ShaderBuilder::HDRColorShader_get().SetAll( mDeviceContext );

		// �Ű������� �Է��մϴ�.
		geometryBuffer->SetGraphicsRootShaderResourcesForColor( mDeviceContext );

		// HDR ���ۿ� ���� �������� �����մϴ�.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );

		// HDR ���� ����� �����մϴ�.
		hdrBuffer->EndDraw( mDeviceContext );

		// �ð� ����� ��ȸ�մϴ�.
		timer->Tick();
		double t = timer->ElapsedSeconds;

		// �Ű������� �Է��մϴ�.
		ShaderBuilder::HDRComputeShader_get( 0 ).SetAll( mDeviceContext );
		hdrComputedBuffer->SetComputeRootTimestamp( mDeviceContext, t );
		hdrBuffer->SetComputeRootShaderResources( mDeviceContext );

		// ���� �н��� �����մϴ�.
		for ( int i = 0; i < 3; ++i )
		{
			// ���̴��� �����մϴ�.
			if ( i != 0 ) ShaderBuilder::HDRComputeShader_get( i ).SetAll( mDeviceContext );
			hdrComputedBuffer->Compute( mDeviceContext, i );
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

		// ����� ����մϴ�.
		directQueue1->Execute( mLightThreads, deviceContextsForLight );
		auto signal = directQueue1->Signal();
		directQueue->Execute( pScene->mDeviceContextForRender );
		HR( directQueue->pCommandQueue->Wait( directQueue1->pFence.Get(), signal ) );
	}

	// ���� �ĸ� ������ �ε����� �����ɴϴ�.
	auto idx = GlobalVar.swapChain->Index;
	auto pBackBuffer = GlobalVar.swapChain->ppBackBuffers[idx].Get();
	auto rtvHandle = GlobalVar.swapChain->RTVHandle[idx];

	// ���� ü���� �ĸ� ���۸� ���� Ÿ������ �����մϴ�.
	mDeviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
	pCommandList.OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	// �ĸ� ���۸� �ܻ����� �ʱ�ȭ�մϴ�.
	constexpr float clear[4]{ 0.0f };
	pCommandList.ClearRenderTargetView( rtvHandle, clear, 0, nullptr );

	if ( !cameraCollection.empty() )
	{
		// �� ���� �������� �����մϴ�.
		ShaderBuilder::ToneMappingShader_get().SetAll( mDeviceContext );

		// HDR ���� �ؽ�ó �� ���� HDR ����� �����ϰ� �������� �����մϴ�.
		hdrBuffer->SetGraphicsRootShaderResources( mDeviceContext );
		hdrComputedBuffer->SetGraphicsRootConstantBuffers( mDeviceContext );
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );
	}

	// ��� ����� �ݰ� Ǫ���մϴ�.
	mDeviceContext->Close();
	directQueue->Execute( mDeviceContext );
}

void GameLogic::ResizeBuffers( uint32 width, uint32 height )
{
	// �ʿ��� ���� ���ۿ��� ũ�� ���� ��û�� �����մϴ�.
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
			auto& computeQueue = GlobalVar.device->ComputeQueue[0];

			pScene->mViewStorageForTerrain->Reset();
			mDeviceContext->Reset( computeQueue.Get(), ( ID3D12CommandAllocator* )&frameIndex, ShaderBuilder::pPipelineState_Terrain.Get() );
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
			HR( GlobalVar.device->DirectQueue[0]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );
			HR( GlobalVar.device->DirectQueue[1]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );

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
			auto& computeQueue = GlobalVar.device->ComputeQueue[0];

			auto& skinningCollection = pScene->mpSkinnedMeshRendererQueue->GetCollections();

			mDeviceContext->Reset( computeQueue.Get(), ( ID3D12CommandAllocator* )&frameIndex, ShaderBuilder::pPipelineState_Skinning.Get() );

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
			HR( GlobalVar.device->DirectQueue[0]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );
			HR( GlobalVar.device->DirectQueue[1]->pCommandQueue->Wait( computeQueue->pFence.Get(), fenceValue ) );

			FetchAndSetThread();
		}
	, nullptr );
}

void GameLogic::GeometryLighting( int frameIndex )
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
			mDeviceContext->Reset( nullptr, ( ID3D12CommandAllocator* )&frameIndex, ShaderBuilder::pPipelineState_ShadowCastShader.Get() );
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
		ThreadPool::QueueUserWorkItem( bind( &GameLogic::RenderSceneGraphForEachThreads, this, placeholders::_1, i, lightsPerThread[i], frameIndex ), nullptr );
	}
}

void GameLogic::GeometryWriting( int frameIndex )
{
	auto* pScene = currentScene.Get();
	auto& mDeviceContext = pScene->mDeviceContextForRender;

	mDeviceContext->Reset( nullptr, ( ID3D12CommandAllocator* )&frameIndex, ShaderBuilder::pPipelineState_ColorShader.Get() );
	auto& pCommandList = *mDeviceContext->pCommandList.Get();
	pCommandList.SetGraphicsRootSignature( ShaderBuilder::pRootSignature_Rendering.Get() );

	auto& mVisibleStorage = pScene->mViewStorageForRender;
	mVisibleStorage->Reset();
	mDeviceContext->SetVisibleViewStorage( mVisibleStorage );

	for ( auto i : pScene->mSceneCameras )
	{
		// ���� ���۸� ���� Ÿ������ �����մϴ�.
		geometryBuffer->OMSetRenderTargets( mDeviceContext );

		if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
		{
			// ����� ��ī�̹ڽ��� ����� ��� �켱 ��ī�̹ڽ� �������� �����մϴ�.
			pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			mDeviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
			skyboxMesh->DrawIndexed( mDeviceContext );
		}

		// ���� ���� ���̴��� �����մϴ�.
		pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

		// ��鿡 ���Ե� ��� ��ü�� �������մϴ�.
		i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		pScene->Render( mDeviceContext, frameIndex );

		// ���� ���� ����� �����մϴ�.
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