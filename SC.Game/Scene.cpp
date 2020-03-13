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
}

Scene::~Scene()
{
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
	if ( firstUpdate == false )
	{
		Start();
		firstUpdate = true;
	}

	// 장면에 존재하는 모든 리지드바디를 가져옵니다.
	UpdateRigidbodyPhysics();
	UpdateStaticRigidbodyPhysics();

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

		//input.scrollDelta = Details::scrollDelta;
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

void Scene::UpdateRigidbodyPhysics()
{
	std::set<Rigidbody*> updateRigidbodies;
	list<list<Rigidbody*>::iterator> remove_list;

	// 각 루트 오브젝트에서 파생된 모든 게임 오브젝트의 리지드바디 컴포넌트를 가져옵니다.
	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		auto collect = gameObjects[i]->GetRawComponentsInChildren<Rigidbody>();
		updateRigidbodies.insert( collect.begin(), collect.end() );
	}

	for ( auto i = appliedRigidbodies.begin(); i != appliedRigidbodies.end(); ++i )
	{
		// 장면 리지드바디가 업데이트 리지드바디 목록에 없을 경우 제거됩니다.
		if ( auto it = updateRigidbodies.find( *i ); it == updateRigidbodies.end() )
		{
			remove_list.push_back( i );
		}
		// 있을 경우 업데이트 목록에서 제거됩니다.
		else
		{
			updateRigidbodies.erase( it );
		}
	}

	// 제거 목록의 리스트에서 적용 목록의 아이템을 제거합니다.
	for ( auto i = remove_list.rbegin(); i != remove_list.rend(); ++i )
	{
		auto pRigid = ( *( *i ) )->pxRigidbody;
		if ( pRigid )
		{
			pxScene->removeActor( *pRigid );
		}
		appliedRigidbodies.erase( *i );
	}

	// 남은 업데이트 목록은 새로 추가된 리지드바디입니다.
	for ( auto i : updateRigidbodies )
	{
		if ( i->pxRigidbody )
		{
			pxScene->addActor( *i->pxRigidbody );
			appliedRigidbodies.push_back( i );
		}
	}
}

void Scene::UpdateStaticRigidbodyPhysics()
{
	std::set<StaticRigidbody*> updateRigidbodies;
	list<list<StaticRigidbody*>::iterator> remove_list;

	// 각 루트 오브젝트에서 파생된 모든 게임 오브젝트의 리지드바디 컴포넌트를 가져옵니다.
	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		auto collect = gameObjects[i]->GetRawComponentsInChildren<StaticRigidbody>();
		updateRigidbodies.insert( collect.begin(), collect.end() );
	}

	for ( auto i = appliedStaticRigidbodies.begin(); i != appliedStaticRigidbodies.end(); ++i )
	{
		// 장면 리지드바디가 업데이트 리지드바디 목록에 없을 경우 제거됩니다.
		if ( auto it = updateRigidbodies.find( *i ); it == updateRigidbodies.end() )
		{
			remove_list.push_back( i );
		}
		// 있을 경우 업데이트 목록에서 제거됩니다.
		else
		{
			updateRigidbodies.erase( it );
		}
	}

	// 제거 목록의 리스트에서 적용 목록의 아이템을 제거합니다.
	for ( auto i = remove_list.rbegin(); i != remove_list.rend(); ++i )
	{
		auto pRigid = ( *( *i ) )->pxRigidbody;
		if ( pRigid )
		{
			pxScene->removeActor( *pRigid );
		}
		appliedStaticRigidbodies.erase( *i );
	}

	// 남은 업데이트 목록은 새로 추가된 리지드바디입니다.
	for ( auto i : updateRigidbodies )
	{
		if ( i->pxRigidbody )
		{
			pxScene->addActor( *i->pxRigidbody );
			appliedStaticRigidbodies.push_back( i );
		}
	}
}