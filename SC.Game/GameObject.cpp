using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace physx;

void GameObject::AttachCollider( Collider* pCol )
{
	if ( pxRigidbody )
	{
		pxRigidbody->attachShape( *pCol->pxShape );
	}
	else
	{
		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		auto pxRigid = GlobalVar.pxDevice->createRigidStatic( gp );
		pxRigid->attachShape( *pCol->pxShape );
		RigidSwap( ( PxRigidActor* )pxRigid );
	}
}

bool GameObject::OnAddComponent( size_t typeId, Component* pComponent )
{
	if ( Rigidbody* pIsRigid = dynamic_cast< Rigidbody* >( pComponent ); pIsRigid )
	{
#if defined( _DEBUG )
		if ( pxRigidbody )
		{
			if ( !isStaticRigid )
			{
				throw new Exception( "SC.Game.GameObject.OnAddComponent(): Dynamic rigidbody already added." );
			}
		}
#endif

		// 새로운 Dynamic 리지드바디를 생성합니다.
		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		auto pxRigid = GlobalVar.pxDevice->createRigidDynamic( gp );
		RigidSwap( ( PxRigidActor* )pxRigid );

		// 기존 충돌체 컴포넌트가 있을 경우 추가합니다.
		auto colliders = GetComponentsInChildren<Collider>();
		for ( int i = 0, count = ( int )colliders.size(); i < count; ++i )
		{
			pxRigid->attachShape( *colliders[i]->pxShape );
		}

		pIsRigid->pxRigidbody = pxRigid->is<PxRigidDynamic>();
		isStaticRigid = false;

		// 장면에 새 리지드바디를 추가합니다.
		if ( pScene )
		{
			pScene->pxScene->addActor( *pxRigid );
		}
	}

	else if ( MeshRenderer* pIsMR = dynamic_cast< MeshRenderer* >( pComponent ); pIsMR )
	{
		transform->CreateBuffer();
	}

	else if ( SkinnedMeshRenderer* pIsSMR = dynamic_cast< SkinnedMeshRenderer* >( pComponent ); pIsSMR )
	{
		transform->CreateBuffer();
	}

	else if ( Camera* pIsCam = dynamic_cast< Camera* >( pComponent ); pIsCam )
	{
		if ( pScene )
		{
			pScene->updateSceneGraph = true;
		}
	}

	else if ( Light* pIsLight = dynamic_cast< Light* >( pComponent ); pIsLight )
	{
		if ( pScene )
		{
			pScene->updateSceneGraph = true;
		}
	}

	return true;
}

bool GameObject::OnRemoveComponent( size_t typeId, Component* pComponent )
{
	if ( Collider* pIsCol = dynamic_cast< Collider* >( pComponent ); !AppShutdown && pIsCol )
	{
		pxRigidbody->detachShape( *pIsCol->pxShape );
	}

	else if ( Rigidbody* pIsRigid = dynamic_cast< Rigidbody* >( pComponent ); !AppShutdown && pIsRigid )
	{
		vector<PxShape*> pxShapes;

		if ( pxRigidbody )
		{
			pxShapes.resize( ( size_t )pxRigidbody->getNbShapes() );
			pxRigidbody->getShapes( pxShapes.data(), pxRigidbody->getNbShapes() );
		}

		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		auto pxRigid = GlobalVar.pxDevice->createRigidStatic( gp );
		RigidSwap( ( PxRigidActor* )pxRigid );

		for ( size_t i = 0; i < pxShapes.size(); ++i )
		{
			pxRigid->attachShape( *pxShapes[i] );
		}

		// 장면에 새 리지드바디를 추가합니다.
		if ( pScene )
		{
			pScene->pxScene->addActor( *pxRigid );
		}

		isStaticRigid = true;
	}

	else if ( Camera* pIsCam = dynamic_cast< Camera* >( pComponent ); pIsCam )
	{
		if ( pScene )
		{
			pScene->updateSceneGraph = true;
		}
	}

	else if ( Light* pIsLight = dynamic_cast< Light* >( pComponent ); pIsLight )
	{
		if ( pScene )
		{
			pScene->updateSceneGraph = true;
		}
	}

	return true;
}

void GameObject::OnSceneAttached( Scene* pScene )
{
	if ( pxRigidbody )
	{
		pScene->pxScene->addActor( *pxRigidbody );
	}

	for ( auto i : GetEnumerator() )
	{
		i->OnSceneAttached( pScene );
	}

	this->pScene = pScene;
}

void GameObject::OnSceneDetached( Scene* pScene )
{
	for ( auto i : GetEnumerator() )
	{
		i->OnSceneDetached( pScene );
	}

	if ( !AppShutdown && pxRigidbody )
	{
		pScene->pxScene->removeActor( *pxRigidbody );
	}

	this->pScene = nullptr;
}

GameObject::GameObject( String name ) : Assets( name )
{
	transform = new Game::Transform();
	transform->Component::gameObject = this;
	transform->gameObject = this;
}

GameObject::~GameObject()
{
	for ( size_t i = 0; i < components.size(); ++i )
	{
		OnRemoveComponent( components[i].first, components[i].second.Get() );
	}

	if ( !AppShutdown && pxRigidbody )
	{
		GlobalVar.pxRigidActor.erase( pxRigidbody );

		pxRigidbody->release();
		pxRigidbody = nullptr;
	}
}

RefPtr<IEnumerator<RefPtr<GameObject>>> GameObject::GetEnumerator()
{
	return new GameObjectEnumerator( gameObjects.begin(), gameObjects.end() );
}

object GameObject::Clone()
{
	RefPtr gameObject = new GameObject( String::Format( "{0} Clone", Name ) );
	gameObject->transform = transform->Clone().As<Game::Transform>();
	gameObject->transform->Component::gameObject = gameObject;
	gameObject->transform->gameObject = gameObject.Get();

	for ( int i = 0, count = ( int )components.size(); i < count; ++i )
	{
		auto pair = components[i];

		auto clone = pair.second->Clone().As<Component>();
		gameObject->AddComponent( pair.first, clone );
	}

	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		auto child = gameObjects[i]->Clone().As<GameObject>();
		child->Parent = gameObject;
	}

	return gameObject;
}

void GameObject::Start()
{
	for ( auto i : gameObjects )
	{
		i->Start();
	}
}

void GameObject::Update( Time& time, Input& input )
{
	// 개체의 각 컴포넌트의 Start 함수를 호출합니다.
	for ( auto i : components )
	{
		if ( i.second->IsEnabled )
		{
			auto cmp = i.second;
			if ( cmp->isFirst )
			{
				cmp->Start();
				cmp->isFirst = false;
			}
		}
	}

	// 개체의 트랜스폼을 업데이트합니다.
	// 개체 업데이트 중 변경 내용은 다음 프레임에 반영됩니다.
	transform->Update( time, input );

	// 개체의 각 컴포넌트를 업데이트합니다.
	for ( auto i : components )
	{
		if ( i.second->IsEnabled )
		{
			auto cmp = i.second;
			if ( cmp->isFirst )
			{
				cmp->Start();
				cmp->isFirst = false;
			}
			cmp->Update( time, input );
		}
	}
}

void GameObject::FixedUpdate( Time& time )
{
	// 개체의 각 컴포넌트의 고정 프레임 업데이트 함수를 호출합니다.
	for ( auto i : components )
	{
		if ( i.second->IsEnabled )
		{
			auto cmp = i.second;
			cmp->FixedUpdate( time );
		}
	}
}

void GameObject::LateUpdate( Time& time, Input& input )
{
	// 개체의 각 컴포넌트의 늦은 업데이트 함수를 호출합니다.
	for ( auto i : components )
	{
		if ( i.second->IsEnabled )
		{
			i.second->LateUpdate( time, input );
		}
	}
}

void GameObject::Render( RefPtr<CDeviceContext>& deviceContext )
{
	transform->SetGraphicsRootConstantBufferView( deviceContext );

	for ( auto i : components )
	{
		if ( i.second->IsEnabled )
			i.second->Render( deviceContext );
	}
}

RefPtr<Transform> GameObject::Transform_get()
{
	return transform;
}

RefPtr<GameObject> GameObject::Parent_get()
{
	return parent.TryResolveAs<GameObject>();
}

void GameObject::Parent_set( RefPtr<GameObject> value )
{
	if ( parent.IsValid )
	{
		auto par = parent.ResolveAs<GameObject>();

		for ( size_t i = 0, count = par->gameObjects.size(); i < count; ++i )
		{
			if ( par->gameObjects[i].Get() == this )
			{
				par->pScene->updateSceneGraph = true;
				par->gameObjects.erase( par->gameObjects.begin() + i );
				break;
			}
		}
	}

	if ( value )
	{
		value->gameObjects.push_back( this );
		parent = value;
		pScene = value->pScene;
	}

	if ( pScene )
	{
		pScene->updateSceneGraph = true;
	}
}

int GameObject::NumChilds_get()
{
	return ( int )gameObjects.size();
}

RefPtr<GameObject> GameObject::Childs_get( int param0 )
{
	return gameObjects[param0];
}

void GameObject::AddComponent( size_t type_hash, RefPtr<Component> component )
{
	ComponentPair pair;
	pair.first = type_hash;
	pair.second = component;

	if ( OnAddComponent( pair.first, pair.second.Get() ) )
	{
		components.push_back( pair );
		component->gameObject = this;

		component->Awake();
	}
}

size_t GameObject::GetComponentIndex( size_t beginIndex, size_t type_hash, function<bool( Component* )> caster )
{
	if ( type_hash == typeid( Game::Transform ).hash_code() )
	{
		return -1;
	}

	for ( size_t i = beginIndex, count = components.size(); i < count; ++i )
	{
		auto& comp = components[i];
		if ( comp.first == type_hash )
		{
			return i;
		}

		if ( caster && caster( comp.second.Get() ) )
		{
			return i;
		}
	}

	return -1;
}

RefPtr<Component> GameObject::GetComponent( size_t type_hash, std::function<bool( Component* )> caster )
{
	if ( type_hash == typeid( Game::Transform ).hash_code() )
	{
		return transform;
	}

	if ( auto idx = GetComponentIndex( 0, type_hash, caster ); idx != -1 )
	{
		return components[idx].second;
	}
	else
	{
		return nullptr;
	}
}

Component* GameObject::GetRawComponent( size_t type_hash, std::function<bool( Component* )> caster )
{
	if ( type_hash == typeid( Game::Transform ).hash_code() )
	{
		return transform.Get();
	}

	if ( auto idx = GetComponentIndex( 0, type_hash, caster ); idx != -1 )
	{
		return components[idx].second.Get();
	}
	else
	{
		return nullptr;
	}
}

void GameObject::RemoveComponent( size_t type_hash, function<bool( Component* )> caster )
{
	if ( auto idx = GetComponentIndex( 0, type_hash, caster ); idx != -1 )
	{
		auto cmp = components[idx].second.Get();

		if ( OnRemoveComponent( type_hash, cmp ) )
		{
			components.erase( components.begin() + idx );
		}
	}
}

void GameObject::RigidSwap( void* pxRigid )
{
	if ( pxRigidbody )
	{
		if ( pScene )
		{
			// 장면에서 기존 리지드바디를 제거합니다.
			pScene->pxScene->removeActor( *pxRigidbody );
		}

		// 가비지 컬렉션 목록에서 리지드바디를 제거합니다.
		GlobalVar.pxRigidActor.erase( pxRigidbody );

		// 기존의 리지드바디를 제거합니다.
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}

	pxRigidbody = ( PxRigidActor* )pxRigid;

	// 가비지 컬렉션 목록에 리지드바디를 추가합니다.
	GlobalVar.pxRigidActor.insert( pxRigidbody );

	// 장면에 새 리지드바디를 추가합니다.
	if ( pScene )
	{
		pScene->pxScene->addActor( *pxRigidbody );
	}
}