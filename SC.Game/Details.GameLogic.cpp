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

	// 물리 타이머의 갱신 주기를 설정합니다.
	mPhysicsTimer.TargetElapsedTicksPerSecond = App::mConfiguration.PhysicsUpdatePerSeconds;
	mPhysicsTimer.IsFixedTimeStep = true;

	// 렌더링 디바이스 컨텍스트 개체를 생성합니다.
	mViewStorage = new VisibleViewStorage();
	mDeviceContext = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );
	mDeviceContextCommit = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );

	// 필요한 버퍼 개체를 생성합니다.
	mGeometryBuffer = new GeometryBuffer( Graphics::mDevice );
	mHDRBuffer = new HDRBuffer( Graphics::mDevice );
	mHDRComputedBuffer = new HDRComputedBuffer( Graphics::mDevice );

	// 스카이박스 메쉬 개체를 생성합니다.
	mSkyboxMesh = Mesh::CreateCube( "skybox_mesh" );

	mDisposed = false;
}

void GameLogic::Update()
{
	if ( mCurrentScene )
	{
		// 장면 갱신 함수를 호출합니다.
		if ( mCurrentScene->firstUpdate == false )
		{
			mCurrentScene->Start();
			mCurrentScene->firstUpdate = true;
		}

		mCurrentScene->Update();

		// 물리 타이머 상태를 갱신합니다. 물리 타임이 지났다면 물리 연산을 실행합니다.
		mPhysicsTimer.Tick( FixedUpdate );
	}
}

void GameLogic::FixedUpdate()
{
	// 장면의 물리 갱신 함수를 호출합니다.
	// Update 함수에서 개체의 존재 여부를 검사하기 때문에, 추가로 검사하지 않습니다.
	mCurrentScene->FixedUpdate();
}

void GameLogic::Render( int frameIndex )
{
	auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
	auto directQueue1 = Graphics::mDevice->DirectQueue[1].Get();
	auto& pCommandQueue = *directQueue->pCommandQueue.Get();
	auto pScene = mCurrentScene.Get();

	// 렌더링을 실행하기 전 장치를 초기화합니다.
	mDeviceContext->FrameIndex = frameIndex;
	mDeviceContext->Reset( directQueue, nullptr, nullptr );

	mViewStorage->Reset();
	mDeviceContext->SetVisibleViewStorage( mViewStorage );

	auto& pCommandList = *mDeviceContext->pCommandList.Get();

	// 카메라 컬렉션이 존재할 경우에만 렌더링을 수행합니다.
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

		// HDR 버퍼를 렌더 타겟으로 설정합니다.
		mHDRBuffer->OMSetRenderTargets( mDeviceContext );

		// HDR 렌더링을 수행합니다.
		ShaderBuilder::HDRShader_get().SetAll( mDeviceContext );

		// 매개변수를 입력합니다.
		pScene->mSceneCameras.front()->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		mGeometryBuffer->SetGraphicsRootShaderResourcesForLayer( mDeviceContext );

		// 각 조명에 대해 조명 렌더 패스를 진행합니다.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		Material::SetGraphicsRootShaderResources( mDeviceContext );

		for ( auto i : pScene->mSceneLights )
		{
			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			i->SetGraphicsRootShaderResources( mDeviceContext );
			pCommandList.DrawInstanced( 4, 1, 0, 0 );
		}

		// HDR 색상 렌더링을 수행합니다.
		ShaderBuilder::HDRColorShader_get().SetAll( mDeviceContext );

		// 매개변수를 입력합니다.
		mGeometryBuffer->SetGraphicsRootShaderResourcesForColor( mDeviceContext );

		// HDR 버퍼에 색상 렌더링을 수행합니다.
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );

		// HDR 버퍼 사용을 종료합니다.
		mHDRBuffer->EndDraw( mDeviceContext );

		// 시간 경과를 조회합니다.
		mRenderTimer.Tick();
		double t = mRenderTimer.ElapsedSeconds;

		// 매개변수를 입력합니다.
		ShaderBuilder::HDRComputeShader_get( 0 ).SetAll( mDeviceContext );
		mHDRComputedBuffer->SetComputeRootTimestamp( mDeviceContext, t );
		mHDRBuffer->SetComputeRootShaderResources( mDeviceContext );

		// 렌더 패스를 수행합니다.
		for ( int i = 0; i < 3; ++i )
		{
			// 셰이더를 선택합니다.
			if ( i != 0 ) ShaderBuilder::HDRComputeShader_get( i ).SetAll( mDeviceContext );
			mHDRComputedBuffer->Compute( mDeviceContext, i );
		}

		// 모든 명령 목록 기록이 완료될 때까지 대기합니다.
		mCompletedEvent.WaitForSingleObject();

		// GPU 명령 의존 관계를 작성합니다.
		auto directQueues = Graphics::mDevice->DirectQueue;
		auto computeQueues = Graphics::mDevice->ComputeQueue;

		auto signal0 = computeQueues[0]->Signal();
		auto signal1 = computeQueues[1]->Signal();

		for ( int i = 0; i < 2; ++i )
		{
			directQueues[i]->pCommandQueue->Wait( computeQueues[0]->pFence.Get(), signal0 );
			directQueues[i]->pCommandQueue->Wait( computeQueues[1]->pFence.Get(), signal1 );
		}

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

	if ( pScene && !cameraCollection.empty() )
	{
		// 톤 매핑 렌더링을 수행합니다.
		ShaderBuilder::ToneMappingShader_get().SetAll( mDeviceContext );

		// HDR 원본 텍스처 및 계산된 HDR 상수를 설정하고 렌더링을 수행합니다.
		mHDRBuffer->SetGraphicsRootShaderResources( mDeviceContext );
		mHDRComputedBuffer->SetGraphicsRootConstantBuffers( mDeviceContext );
		pCommandList.IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pCommandList.DrawInstanced( 4, 1, 0, 0 );
	}

	// 명령 목록을 닫고 푸쉬합니다.
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
	// 필요한 각종 버퍼에게 크기 변경 요청을 수행합니다.
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
		//i->BuildRaytracingAccelerationStructure( mDeviceContextCommit );
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

	// 디바이스 컨텍스트 사용 전 모두 초기화합니다.
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
		// 기하 버퍼를 렌더 타겟으로 설정합니다.
		mGeometryBuffer->OMSetRenderTargets( mDeviceContext );

		if ( auto skybox = i->ClearMode.TryAs<CameraClearModeSkybox>(); skybox )
		{
			// 장면이 스카이박스를 사용할 경우 우선 스카이박스 렌더링을 진행합니다.
			pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_SkyboxShader.Get() );

			i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
			mDeviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, skybox->SkyboxTexture->pShaderResourceView );
			mSkyboxMesh->DrawIndexed( mDeviceContext );
		}

		// 기하 버퍼 셰이더를 선택합니다.
		pCommandList.SetPipelineState( ShaderBuilder::pPipelineState_ColorShader.Get() );

		// 장면에 포함된 모든 개체를 렌더링합니다.
		i->SetGraphicsRootConstantBufferView( mDeviceContext, frameIndex );
		pScene->Render( mDeviceContext, frameIndex );

		// 기하 버퍼 사용을 종료합니다.
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