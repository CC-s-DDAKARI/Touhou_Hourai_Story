using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Diagnostics;
using namespace SC::Threading;
using namespace SC::Drawing;

using namespace std;

StepTimer GameLogic::mTimer;
StepTimer GameLogic::mPhysicsTimer;
StepTimer GameLogic::mRenderTimer;

RefPtr<VisibleViewStorage> GameLogic::mViewStorage;
RefPtr<CDeviceContext> GameLogic::mDeviceContext;
RefPtr<CDeviceContext> GameLogic::mDeviceContextCommit;

RefPtr<GeometryBuffer> GameLogic::mGeometryBuffer;
RefPtr<HDRBuffer> GameLogic::mHDRBuffer;
RefPtr<HDRComputedBuffer> GameLogic::mHDRComputedBuffer;

RefPtr<Mesh> GameLogic::mSkyboxMesh;
RefPtr<Scene> GameLogic::mCurrentScene;

atomic<int> GameLogic::mCompletedValue;
int GameLogic::mCompletedGoal;
Threading::Event GameLogic::mCompletedEvent;
int GameLogic::mLightThreads;

bool GameLogic::mDisposed;

void GameLogic::Initialize()
{
	App::Disposing += Dispose;
	App::Resizing += ResizeApp;

	// ���� Ÿ�̸��� ���� �ֱ⸦ �����մϴ�.
	mPhysicsTimer.TargetElapsedTicksPerSecond = App::mConfiguration.PhysicsUpdatePerSeconds;
	mPhysicsTimer.IsFixedTimeStep = true;

	// ������ ����̽� ���ؽ�Ʈ ��ü�� �����մϴ�.
	mViewStorage = new VisibleViewStorage();
	mDeviceContext = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );
	mDeviceContextCommit = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );

	// �ʿ��� ���� ��ü�� �����մϴ�.
	mGeometryBuffer = new GeometryBuffer( Graphics::mDevice );
	mHDRBuffer = new HDRBuffer( Graphics::mDevice );
	mHDRComputedBuffer = new HDRComputedBuffer( Graphics::mDevice );

	// ��ī�̹ڽ� �޽� ��ü�� �����մϴ�.
	mSkyboxMesh = Mesh::CreateCube( "skybox_mesh" );

	mDisposed = false;
}

void GameLogic::Update()
{
	if ( mCurrentScene )
	{
		// ��� ���� �Լ��� ȣ���մϴ�.
		if ( mCurrentScene->firstUpdate == false )
		{
			mCurrentScene->Start();
			mCurrentScene->firstUpdate = true;
		}

		mCurrentScene->Update();

		// ���� Ÿ�̸� ���¸� �����մϴ�. ���� Ÿ���� �����ٸ� ���� ������ �����մϴ�.
		mPhysicsTimer.Tick( FixedUpdate );
	}
}

void GameLogic::FixedUpdate()
{
	// ����� ���� ���� �Լ��� ȣ���մϴ�.
	// Update �Լ����� ��ü�� ���� ���θ� �˻��ϱ� ������, �߰��� �˻����� �ʽ��ϴ�.
	mCurrentScene->FixedUpdate();
}

void GameLogic::Render( int frameIndex )
{
	auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
	auto directQueue1 = Graphics::mDevice->DirectQueue[1].Get();
	auto& pCommandQueue = *directQueue->pCommandQueue.Get();
	auto pScene = mCurrentScene.Get();

	// �������� �����ϱ� �� ��ġ�� �ʱ�ȭ�մϴ�.
	mDeviceContext->FrameIndex = frameIndex;
	mDeviceContext->Reset( directQueue, nullptr, nullptr );

	mViewStorage->Reset();
	mDeviceContext->SetVisibleViewStorage( mViewStorage );

	auto& pCommandList = *mDeviceContext->pCommandList.Get();

	// ī�޶� �÷����� ������ ��쿡�� �������� �����մϴ�.
	auto& cameraCollection = pScene->mSceneCameras;
	if ( pScene && !cameraCollection.empty() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + 1 + Scene::NumThreadsForLight;

		CommitBuffer( frameIndex );
		TerrainBaking( frameIndex );
		MeshSkinning( frameIndex );
		GeometryLighting( frameIndex );
		GeometryWriting( frameIndex );

		// HDR ���۸� ���� Ÿ������ �����մϴ�.
		mHDRBuffer->OMSetRenderTargets( mDeviceContext );

		// HDR �������� �����մϴ�.
		ShaderBuilder::HDRShader_get().SetAll( mDeviceContext );

		// �Ű������� �Է��մϴ�.
		pScene->mSceneCameras.front()->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		mGeometryBuffer->SetGraphicsRootShaderResourcesForLayer( mDeviceContext );

		// �� ���� ���� ���� ���� �н��� �����մϴ�.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Material::SetGraphicsRootShaderResources( mDeviceContext );

		for ( auto i : pScene->mSceneLights )
		{
			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			i->SetGraphicsRootShaderResources( mDeviceContext );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );
		}

		// HDR ���� �������� �����մϴ�.
		ShaderBuilder::HDRColorShader_get().SetAll( mDeviceContext );

		// �Ű������� �Է��մϴ�.
		mGeometryBuffer->SetGraphicsRootShaderResourcesForColor( mDeviceContext );

		// HDR ���ۿ� ���� �������� �����մϴ�.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );

		// HDR ���� ����� �����մϴ�.
		mHDRBuffer->EndDraw( mDeviceContext );

		// �ð� ����� ��ȸ�մϴ�.
		mRenderTimer.Tick();
		double t = mRenderTimer.ElapsedSeconds;

		// �Ű������� �Է��մϴ�.
		ShaderBuilder::HDRComputeShader_get( 0 ).SetAll( mDeviceContext );
		mHDRComputedBuffer->SetComputeRootTimestamp( mDeviceContext, t );
		mHDRBuffer->SetComputeRootShaderResources( mDeviceContext );

		// ���� �н��� �����մϴ�.
		for ( int i = 0; i < 3; ++i )
		{
			// ���̴��� �����մϴ�.
			if ( i != 0 ) ShaderBuilder::HDRComputeShader_get( i ).SetAll( mDeviceContext );
			mHDRComputedBuffer->Compute( mDeviceContext, i );
		}

		// ��� ��� ��� ����� �Ϸ�� ������ ����մϴ�.
		mCompletedEvent.WaitForSingleObject();

		// GPU ��� ���� ���踦 �ۼ��մϴ�.
		auto directQueues = Graphics::mDevice->DirectQueue;
		auto computeQueues = Graphics::mDevice->ComputeQueue;

		auto signal0 = computeQueues[0]->Signal();
		auto signal1 = computeQueues[1]->Signal();

		for ( int i = 0; i < 2; ++i )
		{
			directQueues[i]->pCommandQueue->Wait( computeQueues[0]->pFence.Get(), signal0 );
			directQueues[i]->pCommandQueue->Wait( computeQueues[1]->pFence.Get(), signal1 );
		}

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
	auto idx = Graphics::mSwapChain->Index;
	auto pBackBuffer = Graphics::mSwapChain->ppBackBuffers[idx].Get();
	auto rtvHandle = Graphics::mSwapChain->RTVHandle[idx];

	// ���� ü���� �ĸ� ���۸� ���� Ÿ������ �����մϴ�.
	mDeviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
	pCommandList.OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	// �ĸ� ���۸� �ܻ����� �ʱ�ȭ�մϴ�.
	constexpr float clear[4]{ 0.0f };
	pCommandList.ClearRenderTargetView( rtvHandle, clear, 0, nullptr );

	if ( pScene && !cameraCollection.empty() )
	{
		// �� ���� �������� �����մϴ�.
		ShaderBuilder::ToneMappingShader_get().SetAll( mDeviceContext );

		// HDR ���� �ؽ�ó �� ���� HDR ����� �����ϰ� �������� �����մϴ�.
		mHDRBuffer->SetGraphicsRootShaderResources( mDeviceContext );
		mHDRComputedBuffer->SetGraphicsRootConstantBuffers( mDeviceContext );
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );
	}

	// ��� ����� �ݰ� Ǫ���մϴ�.
	mDeviceContext->Close();
	directQueue->Execute( mDeviceContext );
}

void GameLogic::Dispose( object sender )
{
	mDisposed = true;

	mCurrentScene = nullptr;
	mSkyboxMesh = nullptr;

	mHDRComputedBuffer = nullptr;
	mHDRBuffer = nullptr;
	mGeometryBuffer = nullptr;

	mDeviceContextCommit = nullptr;
	mDeviceContext = nullptr;
	mViewStorage = nullptr;
}

void GameLogic::ResizeApp( object sender, Point<int> size )
{
	// �ʿ��� ���� ���ۿ��� ũ�� ���� ��û�� �����մϴ�.
	mGeometryBuffer->ResizeBuffers( size.X, size.Y );
	mHDRBuffer->ResizeBuffers( size.X, size.Y );
	mHDRComputedBuffer->ResizeBuffers( size.X, size.Y );
}

void GameLogic::CommitBuffer( int frameIndex )
{
	auto directQueue = Graphics::mDevice->DirectQueue[0];

	mDeviceContextCommit->FrameIndex = frameIndex;
	mDeviceContextCommit->Reset( Graphics::mDevice->DirectQueue[0].Get(), nullptr, nullptr );

	HeapAllocator::Commit( frameIndex, *mDeviceContextCommit.Get() );

	auto& blas = mCurrentScene->mSceneBLAS;
	for ( auto i : blas )
	{
		i->BuildRaytracingAccelerationStructure( mDeviceContextCommit );
	}

	mDeviceContextCommit->Close();
	directQueue->Execute( mDeviceContextCommit.Get() );
}

void GameLogic::TerrainBaking( int frameIndex )
{
	ThreadPool::QueueUserWorkItem( [&, frameIndex]( object )
		{
			auto pScene = mCurrentScene.Get();
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

			FetchAndSetThread();
		}
	, nullptr );
}

void GameLogic::MeshSkinning( int frameIndex )
{
	ThreadPool::QueueUserWorkItem( [&, frameIndex]( object )
		{
			auto pScene = mCurrentScene.Get();
			auto& mDeviceContext = pScene->mDeviceContextForSkinning;
			auto& computeQueue = Graphics::mDevice->ComputeQueue[1];

			auto&& skinningCollection = pScene->mpSkinnedMeshRendererQueue.Get() ? pScene->mpSkinnedMeshRendererQueue->GetCollections() : list<SkinnedMeshRendererQueue::tag_SkinnedPair>();

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

			FetchAndSetThread();
		}
	, nullptr );
}

void GameLogic::GeometryLighting( int frameIndex )
{
	auto* pScene = mCurrentScene.Get();
	int threadIndex = 0;

	// ����̽� ���ؽ�Ʈ ��� �� ��� �ʱ�ȭ�մϴ�.
	int minThreadCount = min( ( int )pScene->mSceneLights.size(), Scene::NumThreadsForLight );
	mLightThreads = minThreadCount;
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
		ThreadPool::QueueUserWorkItem( bind( &GameLogic::RenderSceneGraphForEachThreads, placeholders::_1, i, lightsPerThread[i], frameIndex ), nullptr );
	}
}

void GameLogic::GeometryWriting( int frameIndex )
{
	auto* pScene = mCurrentScene.Get();
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
		// ���� ���۸� ���� Ÿ������ �����մϴ�.
		mGeometryBuffer->OMSetRenderTargets( mDeviceContext );

		if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
		{
			// ����� ��ī�̹ڽ��� ����� ��� �켱 ��ī�̹ڽ� �������� �����մϴ�.
			pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			mDeviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
			mSkyboxMesh->DrawIndexed( mDeviceContext );
		}

		// ���� ���� ���̴��� �����մϴ�.
		pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

		// ��鿡 ���Ե� ��� ��ü�� �������մϴ�.
		i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		pScene->Render( mDeviceContext, frameIndex );

		// ���� ���� ����� �����մϴ�.
		mGeometryBuffer->EndDraw( mDeviceContext );
	}

	mDeviceContext->Close();
}

void GameLogic::RenderSceneGraphForEachThreads( object, int threadIndex, list<Light*>& lights, int frameIndex )
{
	auto* pScene = mCurrentScene.Get();
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