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

	pxScene = GlobalVar.pxDevice->createScene( pxSceneDesc );

	GlobalVar.globalMutex.lock();
	GlobalVar.pxSceneList.insert( pxScene );
	GlobalVar.globalMutex.unlock();

	pSkinnedMeshRendererQueue = new SkinnedMeshRendererQueue();
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
	return index;
}

void Scene::Clear()
{
	gameObjects.clear();
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
}

void Scene::Remove( RefPtr<GameObject> value )
{
	if ( int i = IndexOf( value ); i != -1 )
	{
		Item[i]->OnSceneDetached( this );
		RemoveAt( i );
	}
}

void Scene::RemoveAt( int index )
{
	gameObjects.erase( gameObjects.begin() + index );
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
	pSkinnedMeshRendererQueue->Clear();

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

	auto e = GetEnumerator();
	for ( auto go : e )
	{
		go->Update( time, input );
	}
}

void Scene::FixedUpdate()
{
	fixedTimer.Tick();

	time.fixedTime = fixedTimer.TotalSeconds;
	time.fixedDeltaTime = fixedTimer.ElapsedSeconds;

	auto e = GetEnumerator();
	for ( auto go = std::begin( e ); go != std::end( e ); ++go )
	{
		( *go )->FixedUpdate( time );
	}

	pxScene->simulate( 1.0f / GlobalVar.pApp->AppConfig.PhysicsUpdatePerSeconds );
	pxScene->fetchResults( true );
}

void Scene::LateUpdate()
{
	auto e = GetEnumerator();
	for ( auto go = std::begin( e ); go != std::end( e ); ++go )
	{
		( *go )->LateUpdate( time, input );
	}
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
	auto e = GetEnumerator();
	for ( auto go : e )
	{
		go->Render( deviceContext );
	}
}