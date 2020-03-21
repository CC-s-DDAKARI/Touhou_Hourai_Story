using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Threading;

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

	mDeviceContextForSkinning = new CDeviceContextAndAllocator( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_COMPUTE );
	for ( int i = 0; i < NumThreadsForLight; ++i )
	{
		mViewStoragesForLight[i] = new VisibleViewStorage( GlobalVar.device.Get() );
		mDeviceContextsForLight[i] = new CDeviceContextAndAllocator( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );
	}
	mViewStorageForRender = new VisibleViewStorage( GlobalVar.device.Get() );
	mDeviceContextForRender = new CDeviceContextAndAllocator( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT );
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

	if ( !updatedSceneGraph && mThreadSceneGraph.size() )
	{
		mCompletedValue = 0;
		mCompletedGoal = 1 + ( int )mThreadSceneGraph.size();

		// �� ������ ��ġ �����ۿ� ���� ������ ������Ʈ�� �����մϴ�.
		for ( auto i : mThreadSceneGraph )
		{
			ThreadPool::QueueUserWorkItem( bind( &Scene::UpdateAndLateUpdate, this, placeholders::_1, i.second ), nullptr );
		}

		// �� ������ �������� ������Ʈ�մϴ�.
		UpdateAndLateUpdate( nullptr, mCoreThreadSceneGraph );

		// ��� �����尡 �Ϸ�� ������ ����մϴ�.
		mCompletedEvent.WaitForSingleObject();
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
		mCompletedValue = 0;
		mCompletedGoal = 1 + ( int )mThreadSceneGraph.size();

		// �� ������ ��ġ �����ۿ� ���� ������ ������Ʈ�� �����մϴ�.
		for ( auto i : mThreadSceneGraph )
		{
			ThreadPool::QueueUserWorkItem( bind( &Scene::FixedUpdate1, this, placeholders::_1, i.second ), nullptr );
		}

		// �� ������ �������� ������Ʈ�մϴ�.
		FixedUpdate1( nullptr, mCoreThreadSceneGraph );

		// ��� �����尡 �Ϸ�� ������ ����մϴ�.
		mCompletedEvent.WaitForSingleObject();
	}
	else
	{
		for ( auto go : mSceneGraph )
		{
			go->FixedUpdate( time );
		}
	}

	pxScene->simulate( 1.0f / GlobalVar.pApp->AppConfig.PhysicsUpdatePerSeconds );
	mFetchResults = pxScene->fetchResults( false );
}

void Scene::Load( RefPtr<IAsyncLoad> asyncLoad )
{
	firstUpdate = false;
}

void Scene::Unload()
{

}

void Scene::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex, int fixedFrameIndex )
{
	for ( auto go : mSceneGraph )
	{
		go->Render( deviceContext, frameIndex, fixedFrameIndex );
	}
}

void Scene::PopulateSceneGraph()
{
	ClearSceneGraph();

	// ��� ���� ���踦 Ǯ������ϴ�.
	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		InsertSceneGraph( mSceneGraph, gameObjects[i].Get(), 0 );
	}

	SearchComponents();
}

void Scene::ClearSceneGraph()
{
	mSceneGraph.clear();
	mSceneCameras.clear();
	mSceneLights.clear();
	mThreadSceneGraph.clear();
	mpSkinnedMeshRendererQueue->Clear();
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

void Scene::SearchComponents()
{
	// Ư�� ������Ʈ �׷����� �̸� ����մϴ�.
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