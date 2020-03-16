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

		pxRigidbody = GlobalVar.pxDevice->createRigidStatic( gp );
		pxRigidbody->attachShape( *pCol->pxShape );

		// ��鿡 �� ������ٵ� �߰��մϴ�.
		if ( pScene )
		{
			pScene->pxScene->addActor( *pxRigidbody );
		}
	}
}

bool GameObject::OnAddComponent( size_t typeId, Component* pComponent )
{
	if ( Rigidbody* pIsRigid = dynamic_cast< Rigidbody* >( pComponent ); pIsRigid )
	{
		if ( pxRigidbody )
		{
#if defined( _DEBUG )
			if ( !isStaticRigid )
			{
				throw new Exception( "SC.Game.GameObject.OnAddComponent(): Dynamic rigidbody already added." );
			}
#endif
			if ( pScene )
			{
				// ��鿡�� �⺻ ������ٵ� �����մϴ�.
				pScene->pxScene->removeActor( *pxRigidbody );
			}

			// ������ ������ٵ� �����մϴ�.
			pxRigidbody->release();
			pxRigidbody = nullptr;
		}

		// ���ο� Dynamic ������ٵ� �����մϴ�.
		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		pxRigidbody = GlobalVar.pxDevice->createRigidDynamic( gp );

		// ���� �浹ü ������Ʈ�� ���� ��� �߰��մϴ�.
		auto colliders = GetComponentsInChildren<Collider>();
		for ( int i = 0, count = ( int )colliders.size(); i < count; ++i )
		{
			pxRigidbody->attachShape( *colliders[i]->pxShape );
		}

		pIsRigid->pxRigidbody = pxRigidbody->is<PxRigidDynamic>();
		isStaticRigid = false;

		// ��鿡 �� ������ٵ� �߰��մϴ�.
		if ( pScene )
		{
			pScene->pxScene->addActor( *pxRigidbody );
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

			if ( pScene )
			{
				// ��鿡�� �⺻ ������ٵ� �����մϴ�.
				pScene->pxScene->removeActor( *pxRigidbody );
			}
		}

		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		pxRigidbody = GlobalVar.pxDevice->createRigidStatic( gp );

		for ( size_t i = 0; i < pxShapes.size(); ++i )
		{
			pxRigidbody->attachShape( *pxShapes[i] );
		}

		// ��鿡 �� ������ٵ� �߰��մϴ�.
		if ( pScene )
		{
			pScene->pxScene->addActor( *pxRigidbody );
		}

		isStaticRigid = true;
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

	GlobalVar.disposableObjects.insert( this );
}

GameObject::~GameObject()
{
	for ( size_t i = 0; i < components.size(); ++i )
	{
		OnRemoveComponent( components[i].first, components[i].second.Get() );
	}

	if ( !AppShutdown && pxRigidbody )
	{
		GlobalVar.disposableObjects.erase( this );

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

void GameObject::Dispose()
{
	if ( pxRigidbody )
	{
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}
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
	// ��ü�� �� ������Ʈ�� Start �Լ��� ȣ���մϴ�.
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

	// ��ü�� Ʈ�������� ������Ʈ�մϴ�.
	// ��ü ������Ʈ �� ���� ������ ���� �����ӿ� �ݿ��˴ϴ�.
	transform->Update( time, input );

	// ���� ��ü���� ��� ������Ʈ�մϴ�.
	for ( auto i : gameObjects )
	{
		i->Update( time, input );
	}

	// ��ü�� �� ������Ʈ�� ������Ʈ�մϴ�.
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
	// ���� ��ü���� ���� ������ ������Ʈ �Լ��� ȣ���մϴ�.
	for ( auto i : gameObjects )
	{
		i->FixedUpdate( time );
	}

	// ��ü�� �� ������Ʈ�� ���� ������ ������Ʈ �Լ��� ȣ���մϴ�.
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
	// ���� ��ü���� ���� ������Ʈ �Լ��� ȣ���մϴ�.
	for ( auto i : gameObjects )
	{
		i->LateUpdate( time, input );
	}

	// ��ü�� �� ������Ʈ�� ���� ������Ʈ �Լ��� ȣ���մϴ�.
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

	for ( auto i : gameObjects )
	{
		i->Render( deviceContext );
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