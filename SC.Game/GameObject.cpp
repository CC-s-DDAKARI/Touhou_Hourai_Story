using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace physx;

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

		auto pxRigid = Physics::mPhysics->createRigidDynamic( gp );
		RigidSwap( ( PxRigidActor* )pxRigid );
		isStaticRigid = false;

		// ���� �浹ü ������Ʈ�� ���� ��� �߰��մϴ�.
		auto colliders = GetComponentsInChildren<Collider>();
		for ( int i = 0, count = ( int )colliders.size(); i < count; ++i )
		{
			colliders[i]->AttachToActor( pxRigidbody );
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

		if ( mBLAS )
		{
			mBLAS->mMeshRenderers = GetComponentsInChildren<MeshRenderer>();
			mBLAS->InitializeBuffers();
		}
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

	else if ( Terrain* pIsTerrain = dynamic_cast< Terrain* >( pComponent ); pIsTerrain )
	{
		transform->CreateBuffer();
	}
	
	else if ( Collider* pIsCol = dynamic_cast< Collider* >( pComponent ); pIsCol )
	{
		if ( !pxRigidbody )
		{
			auto pos = transform->position;
			auto quat = transform->rotation;

			PxTransform gp;
			gp.p = ToPhysX( pos );
			gp.q = ToPhysX( quat );

			auto pxRigid = Physics::mPhysics->createRigidStatic( gp );
			RigidSwap( ( PxRigidActor* )pxRigid );
			isStaticRigid = true;
		}

		pIsCol->AttachToActor( pxRigidbody );
	}

	else if ( BottomLevelAccelerationStructure* pIsBLAS = dynamic_cast< BottomLevelAccelerationStructure* >( pComponent ); pIsBLAS )
	{
		mBLAS = pIsBLAS;
		mBLAS->mMeshRenderers = GetComponentsInChildren<MeshRenderer>();
		mBLAS->InitializeBuffers();

		if ( pScene )
		{
			pScene->updateSceneGraph = true;
		}
	}

	return true;
}

bool GameObject::OnRemoveComponent( size_t typeId, Component* pComponent )
{
	if ( Rigidbody* pIsRigid = dynamic_cast< Rigidbody* >( pComponent ); pIsRigid )
	{
		auto pos = transform->position;
		auto quat = transform->rotation;

		PxTransform gp;
		gp.p = ToPhysX( pos );
		gp.q = ToPhysX( quat );

		auto pxRigid = Physics::mPhysics->createRigidStatic( gp );
		RigidSwap( ( PxRigidActor* )pxRigid );
		isStaticRigid = true;

		auto colliders = GetComponentsInChildren<Collider>();
		for ( auto i : colliders )
		{
			i->AttachToActor( pxRigid );
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

	else if ( BottomLevelAccelerationStructure* pIsBLAS = dynamic_cast< BottomLevelAccelerationStructure* >( pComponent ); pIsBLAS )
	{
		mBLAS = nullptr;

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

	if ( pxRigidbody )
	{
		pScene->pxScene->removeActor( *pxRigidbody );
	}

	this->pScene = nullptr;
}

void GameObject::OnCollisionEnter( GameObject* pGameObject )
{
	for ( auto i : components )
	{
		i.second->OnCollisionEnter( pGameObject );
	}
}

void GameObject::OnCollisionExit( GameObject* pGameObject )
{
	for ( auto i : components )
	{
		i.second->OnCollisionExit( pGameObject );
	}
}

void GameObject::OnCollisionStay( GameObject* pGameObject )
{
	for ( auto i : components )
	{
		i.second->OnCollisionStay( pGameObject );
	}
}

void GameObject::OnTriggerEnter( Collider* pCollider )
{
	for ( auto i : components )
	{
		i.second->OnTriggerEnter( pCollider );
	}
}

void GameObject::OnTriggerExit( Collider* pCollider )
{
	for ( auto i : components )
	{
		i.second->OnTriggerExit( pCollider );
	}
}

GameObject::GameObject( String name ) : Assets( name )
{
	transform = new Game::Transform();
	transform->Component::gameObject = this;
	transform->gameObject = this;
}

GameObject::~GameObject()
{
	if ( !GameLogic::mDisposed && pxRigidbody )
	{
		pxRigidbody->userData = nullptr;
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}

	for ( size_t i = 0; i < components.size(); ++i )
	{
		OnRemoveComponent( components[i].first, components[i].second.Get() );
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
	gameObject->transform->Component::gameObject = gameObject.Get();
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
	for ( int i = 0; i < ( int )components.size(); ++i )
	{
		auto cmp = components[i].second.Get();
		cmp->Start();
	}
}

void GameObject::Update( Time& time, Input& input )
{
	// ��ü�� �� ������Ʈ�� Start �Լ��� ȣ���մϴ�.
	for ( int i = 0; i < ( int )components.size(); ++i )
	{
		auto cmp = components[i].second.Get();
		if ( cmp->isEnabled )
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
	for ( int i = 0; i < ( int )components.size(); ++i )
	{
		auto cmp = components[i].second.Get();
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
	for ( int i = 0; i < ( int )components.size(); ++i )
	{
		auto cmp = components[i].second.Get();
		if ( cmp->IsEnabled )
		{
			cmp->FixedUpdate( time );
		}
	}
}

void GameObject::LateUpdate( Time& time, Input& input )
{
	// ��ü�� �� ������Ʈ�� ���� ������Ʈ �Լ��� ȣ���մϴ�.
	for ( int i = 0; i < ( int )components.size(); ++i )
	{
		auto cmp = components[i].second.Get();
		if ( cmp->IsEnabled )
		{
			cmp->LateUpdate( time, input );
		}
	}
}

void GameObject::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	transform->SetGraphicsRootConstantBufferView( deviceContext, frameIndex );

	for ( int i = 0; i < ( int )components.size(); ++i )
	{
		auto cmp = components[i].second.Get();
		if ( cmp->IsEnabled )
			cmp->Render( deviceContext, frameIndex );
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

		// ������ ������ٵ� �����մϴ�.
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}

	pxRigidbody = ( PxRigidActor* )pxRigid;
	pxRigidbody->userData = this;

	// ��鿡 �� ������ٵ� �߰��մϴ�.
	if ( pScene )
	{
		pScene->pxScene->addActor( *pxRigidbody );
	}
}

bool GameObject::CheckRigidbody()
{
	if ( pxRigidbody && !isStaticRigid )
	{
		return true;
	}
	else
	{
		return false;
	}
}