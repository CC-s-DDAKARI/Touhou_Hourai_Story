using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Threading;

using namespace physx;
using namespace std;

PxFilterFlags ContactReportFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize
	)
{
	auto ret = PxDefaultSimulationFilterShader( attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize );

	// all initial and persisting reports for everything, with per-point data
	pairFlags
		= PxPairFlag::eSOLVE_CONTACT
		| PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;

	if ( !PxFilterObjectIsTrigger( attributes0 ) && !PxFilterObjectIsTrigger( attributes1 ) )
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	}

	return ret;
}

Scene::Scene()
{
	input.keyboardState.resize( 256, false );
	input.prevKeyboardState.resize( 256, false );

	POINT cursor;
	GetCursorPos( &cursor );
	ScreenToClient( Details::App::mWndHandle, &cursor );

	input.prevCursorPos = input.cursorPos;
	input.cursorPos = { cursor.x, cursor.y };

	mSimulationEventCallback = make_unique<ContactCallback>();
	auto pxSceneDesc = PxSceneDesc( PxTolerancesScale() );
	pxSceneDesc.gravity = PxVec3( 0.0f, -9.8f, 0.0f );
	pxSceneDesc.cpuDispatcher = Physics::mDispatcher;
	pxSceneDesc.filterShader = ContactReportFilterShader;
	pxSceneDesc.cudaContextManager = Physics::mCudaManager;
	pxSceneDesc.simulationEventCallback = mSimulationEventCallback.get();
	pxScene = Physics::mPhysics->createScene( pxSceneDesc );

	mpSkinnedMeshRendererQueue = new SkinnedMeshRendererQueue();

	mDeviceContextForTerrain = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE );
	mViewStorageForTerrain = new VisibleViewStorage();
	mDeviceContextForSkinning = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_COMPUTE );
	for ( int i = 0; i < NumThreadsForLight; ++i )
	{
		mViewStoragesForLight[i] = new VisibleViewStorage();
		mDeviceContextsForLight[i] = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );
	}
	mViewStorageForRender = new VisibleViewStorage();
	mDeviceContextForRender = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );
}

Scene::~Scene()
{
	for ( auto i : GetEnumerator() )
	{
		i->OnSceneDetached( this );
	}

	if ( !mFetchResults )
	{
		mFetchResults = pxScene->fetchResults( true );
	}

	if ( pxScene )
	{
		pxScene->release();
		pxScene = nullptr;
	}
}

RefPtr<IEnumerator<RefPtr<GameObject>>> Scene::GetEnumerator()
{
	return new GameObjectEnumerator( gameObjects.begin(), gameObjects.end() );
}

int Scene::Add( RefPtr<GameObject> value )
{
	int index = ( int )gameObjects.size();
	gameObjects.push_back( value );
	value->OnSceneAttached( this );

	updateSceneGraph = true;
	return index;
}

void Scene::Clear()
{
	gameObjects.clear();
	updateSceneGraph = true;
}

bool Scene::Contains( RefPtr<GameObject> value )
{
	for ( int i = 0; i < Count; ++i )
	{
		if ( gameObjects[i]->Equals( value ) )
		{
			return true;
		}
	}

	return false;
}

void Scene::CopyTo( RefPtr<GameObject>* array, int index )
{
	for ( int i = index; i < Count; ++i )
	{
		array[i - index] = gameObjects[i];
	}
}

int Scene::IndexOf( RefPtr<GameObject> value )
{
	for ( int i = 0; i < Count; ++i )
	{
		if ( gameObjects[i]->Equals( value ) )
		{
			return i;
		}
	}

	return -1;
}

void Scene::Insert( int index, RefPtr<GameObject> value )
{
	gameObjects.insert( gameObjects.begin() + index, value );
	updateSceneGraph = true;
}

void Scene::Remove( RefPtr<GameObject> value )
{
	if ( int i = IndexOf( value ); i != -1 )
	{
		Item[i]->OnSceneDetached( this );
		RemoveAt( i );
	}

	updateSceneGraph = true;
}

void Scene::RemoveAt( int index )
{
	gameObjects.erase( gameObjects.begin() + index );

	updateSceneGraph = true;
}

int Scene::Count_get()
{
	return ( int )gameObjects.size();
}

bool Scene::IsFixedSize_get()
{
	return false;
}

RefPtr<GameObject> Scene::Item_get( int index )
{
	return gameObjects[index];
}

void Scene::Item_set( int index, RefPtr<GameObject> value )
{
	gameObjects[index] = value;
	updateSceneGraph = true;
}

void Scene::Start()
{
	auto e = GetEnumerator();
	for ( auto go : e )
	{
		go->Start();
	}
}

void Scene::Update()
{
	bool updatedSceneGraph = false;

	if ( updateSceneGraph )
	{
		PopulateSceneGraph();
		updateSceneGraph = false;
		updatedSceneGraph = true;
	}

	if ( firstUpdate == false )
	{
		Start();
		firstUpdate = true;
	}

	if ( !mFetchResults )
	{
		mFetchResults = pxScene->fetchResults( true );
	}

	timer.Tick();

	time.time = timer.TotalSeconds;
	time.deltaTime = timer.ElapsedSeconds;

	if ( GetActiveWindow() == Details::App::mWndHandle )
	{
		BYTE vks[256];
		GetKeyboardState( vks );
		for ( int i = 0; i < 256; ++i )
		{
			input.prevKeyboardState[i] = input.keyboardState[i];
			input.keyboardState[i] = ( vks[i] & 0x80 ) == 0x80;
		}

		if ( Input::locked )
		{
			POINT center, cursor;
			RECT rect;
			GetClientRect( Details::App::mWndHandle, &rect );

			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			center.x = width / 2;
			center.y = height / 2;


			GetCursorPos( &cursor );
			ScreenToClient( Details::App::mWndHandle, &cursor );
			input.prevCursorPos = { center.x, center.y };
			input.cursorPos = { cursor.x, cursor.y };

			input.SetCursorToCenter();
		}
		else
		{
			POINT cursor;
			GetCursorPos( &cursor );
			ScreenToClient( Details::App::mWndHandle, &cursor );

			input.prevCursorPos = input.cursorPos;
			input.cursorPos = { cursor.x, cursor.y };
		}

		input.scrollDelta = UISystem::mScrollDelta;
	}

	if ( !updatedSceneGraph && mThreadSceneGraph.size() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + ( int )mThreadSceneGraph.size();

		// 각 스레드 배치 아이템에 대해 스레드 업데이트를 수행합니다.
		for ( auto i : mThreadSceneGraph )
		{
			ThreadPool::QueueUserWorkItem( bind( &Scene::UpdateAndLateUpdate, this, placeholders::_1, i.second ), nullptr );
		}

		// 주 스레드 아이템을 업데이트합니다.
		UpdateAndLateUpdate( nullptr, mCoreThreadSceneGraph );

		// 모든 스레드가 완료될 때까지 대기합니다.
		mCompletedEvent.WaitForSingleObject();
	}
	else
	{
		list<GameObject*>* sceneGraph = nullptr;

		if ( mUpdatedSceneGraph )
		{
			sceneGraph = &mSceneGraphDispatch;
		}
		else
		{
			sceneGraph = &mSceneGraph;
		}

		for ( auto go : *sceneGraph )
		{
			go->Update( time, input );
		}

		for ( auto go : *sceneGraph )
		{
			go->LateUpdate( time, input );
		}
	}
}

void Scene::FixedUpdate()
{
	fixedTimer.Tick();

	time.fixedTime = fixedTimer.TotalSeconds;
	time.fixedDeltaTime = fixedTimer.ElapsedSeconds;

	if ( mThreadSceneGraph.size() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + ( int )mThreadSceneGraph.size();

		// 각 스레드 배치 아이템에 대해 스레드 업데이트를 수행합니다.
		for ( auto i : mThreadSceneGraph )
		{
			ThreadPool::QueueUserWorkItem( bind( &Scene::FixedUpdate1, this, placeholders::_1, i.second ), nullptr );
		}

		// 주 스레드 아이템을 업데이트합니다.
		FixedUpdate1( nullptr, mCoreThreadSceneGraph );

		// 모든 스레드가 완료될 때까지 대기합니다.
		mCompletedEvent.WaitForSingleObject();
	}
	else
	{
		list<GameObject*>* sceneGraph = nullptr;

		if ( mUpdatedSceneGraph )
		{
			sceneGraph = &mSceneGraphDispatch;
		}
		else
		{
			sceneGraph = &mSceneGraph;
		}

		for ( auto go : *sceneGraph )
		{
			go->FixedUpdate( time );
		}
	}

	pxScene->simulate( 1.0f / App::mApp->AppConfig.PhysicsUpdatePerSeconds );
	mFetchResults = pxScene->fetchResults( false );
}

void Scene::Load( RefPtr<IAsyncLoad> asyncLoad )
{
	firstUpdate = false;
}

void Scene::Unload()
{

}

void Scene::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	for ( auto go : mSceneGraph )
	{
		go->Render( deviceContext, frameIndex );
	}
}

void Scene::PopulateSceneGraph()
{
	ClearSceneGraph();

	// 장면 종속 관계를 풀어놓습니다.
	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		InsertSceneGraph( mSceneGraph, gameObjects[i].Get(), 0 );
	}

	SearchComponents();

	mUpdatedSceneGraph = true;
}

void Scene::ClearSceneGraph()
{
	mCoreThreadSceneGraph.clear();
	mThreadSceneGraph.clear();
	mpSkinnedMeshRendererQueueDispatch = new SkinnedMeshRendererQueue();
}

void Scene::InsertSceneGraph( list<GameObject*>& sceneGraph, GameObject* pGameObject, int threadId )
{
	mSceneGraphDispatch.push_back( pGameObject );

	if ( auto t = pGameObject->GetComponent<ThreadDispatcher>(); t )
	{
		threadId = t->ThreadID;
	}

	if ( threadId == 0 )
		mCoreThreadSceneGraph.push_back( pGameObject );
	else
		mThreadSceneGraph[threadId].push_back( pGameObject );

	for ( int i = 0, count = pGameObject->NumChilds; i < count; ++i )
	{
		InsertSceneGraph( sceneGraph, pGameObject->Childs[i], threadId );
	}
}

void Scene::SearchComponents()
{
	// 특수 컴포넌트 그래프를 미리 계산합니다.
	for ( auto i : mSceneGraphDispatch )
	{
		if ( auto t = i->GetComponent<Camera>(); t )
		{
			mSceneCamerasDispatch.push_back( t );
		}

		if ( auto t = i->GetComponent<Light>(); t )
		{
			mSceneLightsDispatch.push_back( t );
		}

		if ( auto t = i->GetComponent<Animator>(); t )
		{
			mpSkinnedMeshRendererQueueDispatch->PushAnimator( t );
		}

		if ( auto t = i->GetComponent<SkinnedMeshRenderer>(); t )
		{
			mpSkinnedMeshRendererQueueDispatch->AddRenderer( t );
		}

		if ( auto t = i->GetComponent<Terrain>(); t )
		{
			mSceneTerrainsDispatch.push_back( t );
		}
	}

	mpSkinnedMeshRendererQueueDispatch->PushAnimator( nullptr );
}

void Scene::UpdateAndLateUpdate( object, list<GameObject*>& batch )
{
	for ( auto go : batch )
	{
		go->Update( time, input );
	}

	for ( auto go : batch )
	{
		go->LateUpdate( time, input );
	}

	FetchAndSetThread();
}

void Scene::FixedUpdate1( object, list<GameObject*>& batch )
{
	for ( auto go : batch )
	{
		go->FixedUpdate( time );
	}

	FetchAndSetThread();
}

void Scene::FetchAndSetThread()
{
	mCompletedValue.fetch_add( 1 );
	if ( mCompletedValue >= mCompletedGoal )
	{
		mCompletedEvent.Set();
	}
}