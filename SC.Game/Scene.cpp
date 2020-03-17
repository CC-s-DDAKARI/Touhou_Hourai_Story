using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

Scene::Scene()
{
	input.keyboardState.resize( 256, false );
	input.prevKeyboardState.resize( 256, false );

	POINT cursor;
	GetCursorPos( &cursor );
	ScreenToClient( Details::GlobalVar.hWnd, &cursor );

	input.prevCursorPos = input.cursorPos;
	input.cursorPos = { cursor.x, cursor.y };

	auto pxSceneDesc = PxSceneDesc( PxTolerancesScale() );
	pxSceneDesc.gravity = PxVec3( 0.0f, -9.8f, 0.0f );
	pxSceneDesc.cpuDispatcher = GlobalVar.pxDefaultDispatcher;
	pxSceneDesc.filterShader = PxDefaultSimulationFilterShader;
	pxSceneDesc.cudaContextManager = GlobalVar.pxCudaManager;

	pxScene = GlobalVar.pxDevice->createScene( pxSceneDesc );

	GlobalVar.globalMutex.lock();
	GlobalVar.pxSceneList.insert( pxScene );
	GlobalVar.globalMutex.unlock();

	mpSkinnedMeshRendererQueue = new SkinnedMeshRendererQueue();
}

Scene::~Scene()
{
	for ( auto i : GetEnumerator() )
	{
		i->OnSceneDetached( this );
	}

	if ( !AppShutdown && pxScene )
	{
		GlobalVar.globalMutex.lock();
		GlobalVar.pxSceneList.erase( pxScene );
		GlobalVar.globalMutex.unlock();

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
	if ( updateSceneGraph )
	{
		PopulateSceneGraph();
		updateSceneGraph = false;
	}

	if ( firstUpdate == false )
	{
		Start();
		firstUpdate = true;
	}

	timer.Tick();

	time.time = timer.TotalSeconds;
	time.deltaTime = timer.ElapsedSeconds;

	if ( GetActiveWindow() == Details::GlobalVar.hWnd )
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
			GetClientRect( Details::GlobalVar.hWnd, &rect );

			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			center.x = width / 2;
			center.y = height / 2;


			GetCursorPos( &cursor );
			ScreenToClient( Details::GlobalVar.hWnd, &cursor );
			input.prevCursorPos = { center.x, center.y };
			input.cursorPos = { cursor.x, cursor.y };

			input.SetCursorToCenter();
		}
		else
		{
			POINT cursor;
			GetCursorPos( &cursor );
			ScreenToClient( Details::GlobalVar.hWnd, &cursor );

			input.prevCursorPos = input.cursorPos;
			input.cursorPos = { cursor.x, cursor.y };
		}

		input.scrollDelta = GlobalVar.scrollDelta;
	}

	if ( mThreadSceneGraph.size() )
	{
		std::atomic<int> locker = 0;

		// 각 스레드 배치 아이템에 대해 스레드 업데이트를 수행합니다.
		for ( auto i : mThreadSceneGraph )
		{
			Threading::ThreadPool::QueueUserWorkItem( Threading::AsyncTaskDelegate<void>( [&]( object arg )
				{
					list<GameObject*> mylist = arg.As<list<GameObject*>>();

					for ( auto go : mylist )
					{
						go->Update( time, input );
					}

					for ( auto go : mylist )
					{
						go->LateUpdate( time, input );
					}

					locker += 1;
					return nullptr;
				}
			), i.second );
		}

		// 주 스레드 작업은 여기서 진행합니다.
		for ( auto go : mCoreThreadSceneGraph )
		{
			go->Update( time, input );
		}

		// 주 스레드 작업은 여기서 진행합니다.
		for ( auto go : mCoreThreadSceneGraph )
		{
			go->LateUpdate( time, input );
		}

		// 모든 스레드가 작업을 마칠 때까지 대기합니다.
		while ( locker != ( int )mThreadSceneGraph.size() );
	}
	else
	{
		for ( auto go : mSceneGraph )
		{
			go->Update( time, input );
		}

		for ( auto go : mSceneGraph )
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
		std::atomic<int> locker = 0;

		// 각 스레드 배치 아이템에 대해 스레드 업데이트를 수행합니다.
		for ( auto i : mThreadSceneGraph )
		{
			Threading::ThreadPool::QueueUserWorkItem( Threading::AsyncTaskDelegate<void>( [&]( object arg )
				{
					list<GameObject*> mylist = arg.As<list<GameObject*>>();

					for ( auto go : mylist )
					{
						go->FixedUpdate( time );
					}

					locker += 1;
					return nullptr;
				}
			), i.second );
		}

		// 주 스레드 작업은 여기서 진행합니다.
		for ( auto go : mCoreThreadSceneGraph )
		{
			go->FixedUpdate( time );
		}

		// 모든 스레드가 작업을 마칠 때까지 대기합니다.
		while ( locker != ( int )mThreadSceneGraph.size() );
	}
	else
	{
		for ( auto go : mSceneGraph )
		{
			go->FixedUpdate( time );
		}
	}

	pxScene->simulate( 1.0f / GlobalVar.pApp->AppConfig.PhysicsUpdatePerSeconds );
	pxScene->fetchResults( true );
}

void Scene::Load( RefPtr<IAsyncLoad> asyncLoad )
{
	firstUpdate = false;
}

void Scene::Unload()
{

}

void Scene::Render( RefPtr<CDeviceContext>& deviceContext )
{
	for ( auto go : mSceneGraph )
	{
		go->Render( deviceContext );
	}
}

void Scene::PopulateSceneGraph()
{
	mSceneGraph.clear();
	mSceneCameras.clear();
	mSceneLights.clear();
	mThreadSceneGraph.clear();
	mpSkinnedMeshRendererQueue->Clear();

	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		InsertSceneGraph( mSceneGraph, gameObjects[i].Get(), 0 );
	}

	for ( auto i : mSceneGraph )
	{
		if ( auto t = i->GetComponent<Camera>(); t )
		{
			mSceneCameras.push_back( t );
		}

		if ( auto t = i->GetComponent<Light>(); t )
		{
			mSceneLights.push_back( t );
		}

		if ( auto t = i->GetComponent<Animator>(); t )
		{
			mpSkinnedMeshRendererQueue->PushAnimator( t );
		}

		if ( auto t = i->GetComponent<SkinnedMeshRenderer>(); t )
		{
			mpSkinnedMeshRendererQueue->AddRenderer( t );
		}
	}

	mpSkinnedMeshRendererQueue->PushAnimator( nullptr );
}

void Scene::InsertSceneGraph( list<GameObject*>& sceneGraph, GameObject* pGameObject, int threadId )
{
	mSceneGraph.push_back( pGameObject );

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