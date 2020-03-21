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

		// ���ο� Dynamic ������ٵ� �����մϴ�.
		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		auto pxRigid = GlobalVar.pxDevice->createRigidDynamic( gp );
		RigidSwap( ( PxRigidActor* )pxRigid );

		// ���� �浹ü ������Ʈ�� ���� ��� �߰��մϴ�.
		auto colliders = GetComponentsInChildren<Collider>();
		for ( int i = 0, count = ( int )colliders.size(); i < count; ++i )
		{
			pxRigid->attachShape( *colliders[i]->pxShape );
		}

		pIsRigid->pxRigidbody = pxRigid->is<PxRigidDynamic>();
		isStaticRigid = false;

		// ��鿡 �� ������ٵ� �߰��մϴ�.
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

	else if ( ThreadDispatcher* pIsLight = dynamic_cast< ThreadDispatcher* >( pComponent ); pIsLight )
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

		// ��鿡 �� ������ٵ� �߰��մϴ�.
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

	else if ( ThreadDispatcher* pIsLight = dynamic_cast< ThreadDispatcher* >( pComponent ); pIsLight )
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

		var clone = pair.second->Clone().As<Component>();
		gameObject->AddComponent( pair.first, clone.Get() );
	}

	for ( int i = 0, count = ( int )gameObjects.size(); i < count; ++i )
	{
		auto child = gameObjects[i]->Clone().As<GameObject>();
		child->Parent = gameObject.Get();
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
	// ��ü�� �� ������Ʈ�� Start �Լ��� ȣ���մϴ�.
	for ( auto i : components )
	{
		auto cmp = i.second.Get();
		if ( i.second->isEnabled )
		{
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

	// ��ü�� �� ������Ʈ�� ������Ʈ�մϴ�.
	for ( auto i : components )
	{
		auto cmp = i.second.Get();
		if ( cmp->IsEnabled )
		{
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
	// ��ü�� �� ������Ʈ�� ���� ������ ������Ʈ �Լ��� ȣ���մϴ�.
	for ( auto i : components )
	{
		auto cmp = i.second.Get();
		if ( cmp->IsEnabled )
		{
			cmp->FixedUpdate( time );
		}
	}
}

void GameObject::LateUpdate( Time& time, Input& input )
{
	// ��ü�� �� ������Ʈ�� ���� ������Ʈ �Լ��� ȣ���մϴ�.
	for ( auto i : components )
	{
		auto cmp = i.second.Get();
		if ( cmp->IsEnabled )
		{
			cmp->LateUpdate( time, input );
		}
	}
}

void GameObject::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex, int fixedFrameIndex )
{
	transform->SetGraphicsRootConstantBufferView( deviceContext, frameIndex, fixedFrameIndex );

	for ( auto i : components )
	{
		auto cmp = i.second.Get();
		if ( cmp->IsEnabled )
			cmp->Render( deviceContext, frameIndex, fixedFrameIndex );
	}
}

Transform* GameObject::Transform_get()
{
	return transform.Get();
}

GameObject* GameObject::Parent_get()
{
	return parent;
}

void GameObject::Parent_set( GameObject* value )
{
	if ( parent )
	{
		for ( size_t i = 0, count = parent->gameObjects.size(); i < count; ++i )
		{
			if ( parent->gameObjects[i].Get() == this )
			{
				parent->pScene->updateSceneGraph = true;
				parent->gameObjects.erase( parent->gameObjects.begin() + i );
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

GameObject* GameObject::Childs_get( int param0 )
{
	return gameObjects[param0].Get();
}

void GameObject::AddComponent( size_t type_hash, Component* component )
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

Component* GameObject::GetComponent( size_t type_hash, std::function<bool( Component* )> caster )
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
			// ��鿡�� ���� ������ٵ� �����մϴ�.
			pScene->pxScene->removeActor( *pxRigidbody );
		}

		// ������ �÷��� ��Ͽ��� ������ٵ� �����մϴ�.
		GlobalVar.pxRigidActor.erase( pxRigidbody );

		// ������ ������ٵ� �����մϴ�.
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}

	pxRigidbody = ( PxRigidActor* )pxRigid;

	// ������ �÷��� ��Ͽ� ������ٵ� �߰��մϴ�.
	GlobalVar.pxRigidActor.insert( pxRigidbody );

	// ��鿡 �� ������ٵ� �߰��մϴ�.
	if ( pScene )
	{
		pScene->pxScene->addActor( *pxRigidbody );
	}
}