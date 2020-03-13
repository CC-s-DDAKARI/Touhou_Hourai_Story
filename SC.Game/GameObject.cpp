using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace physx;

bool GameObject::OnAddComponent( size_t typeId, Component* pComponent )
{
	if ( Collider* pIsCol = dynamic_cast< Collider* >( pComponent ); pIsCol )
	{
		if ( pxRigidbody )
		{
			pxRigidbody->attachShape( *pIsCol->pxShape );
		}
		else
		{
			auto pos = transform->position;
			auto quat = transform->rotation;

			PxTransform gp;
			gp.p = ToPhysX( pos );
			gp.q = ToPhysX( quat );

			pxRigidbody = GlobalVar.pxDevice->createRigidStatic( gp );
		}
	}
	else if ( Rigidbody* pIsRigid = dynamic_cast< Rigidbody* >( pComponent ); pIsRigid )
	{
		if ( pxRigidbody )
		{
#if defined( _DEBUG )
			if ( !isStaticRigid )
			{
				throw new Exception( "SC.Game.GameObject.OnAddComponent(): Rigidbody already added." );
			}
#endif

			// 기존의 리지드바디를 제거합니다.
			pxRigidbody->release();
			pxRigidbody = nullptr;

			// 새로운 Dynamic 리지드바디를 생성합니다.
			auto pos = transform->position;
			auto quat = transform->rotation;

			PxTransform gp;
			gp.p = ToPhysX( pos );
			gp.q = ToPhysX( quat );

			pxRigidbody = GlobalVar.pxDevice->createRigidDynamic( gp );

			// 기존 충돌체 컴포넌트가 있을 경우 추가합니다.
			auto colliders = GetComponentsInChildren<Collider>();
			for ( int i = 0; i < colliders.size(); ++i )
			{
				pxRigidbody->attachShape( *colliders[i]->pxShape );
			}

			pIsRigid->pxRigidbody = pxRigidbody;
			isStaticRigid = false;
		}
	}

	return true;
}

bool GameObject::OnRemoveComponent( size_t typeId, Component* pComponent )
{
	if ( Collider* pIsCol = dynamic_cast< Collider* >( pComponent ); pIsCol )
	{
		pxRigidbody->detachShape( *pIsCol->pxShape );
	}

	return true;
}

void GameObject::OnSceneAttached( Scene* pScene )
{

}

void GameObject::OnSceneDetached( Scene* pScene )
{

}

GameObject::GameObject( String name ) : Assets( name )
{
	transform = new Game::Transform();
	transform->gameObject = this;
}

GameObject::~GameObject()
{
	for ( size_t i = 0; i < components.size(); ++i )
	{
#if defined( _DEBUG )
		auto ret =
#endif
		OnRemoveComponent( components[i].first, components[i].second.Get() );
#if defined( _DEBUG )
		if ( ret == false )
		{
			throw new Exception( "SC.Game.GameObject.~GameObject(): OnRemoveComponent() has return false." );
		}
#endif
	}

	if ( pxRigidbody )
	{
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
	gameObject->transform->gameObject = gameObject;

	for ( int i = 0, count = ( int )components.size(); i < count; ++i )
	{
		auto clone = components[i].second->Clone().As<Component>();
		clone->gameObject = gameObject;
		gameObject->components.push_back( { components[i].first, clone } );
		clone->Awake();
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
	for ( auto i : gameObjects )
	{
		i->Update( time, input );
	}

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
	for ( auto i : gameObjects )
	{
		i->FixedUpdate( time );
	}

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
			cmp->FixedUpdate( time );
		}
	}
}

void GameObject::LateUpdate( Time& time, Input& input )
{
	transform->LateUpdate( time, input );

	for ( auto i : components )
	{
		if ( i.second->IsEnabled )
		{
			i.second->LateUpdate( time, input );
		}
	}

	for ( auto i : gameObjects )
	{
		i->LateUpdate( time, input );
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
	size_t dynamic_index = -1;

	if ( type_hash == typeid( Game::Transform ).hash_code() )
	{
		return transform;
	}

	for ( size_t i = beginIndex, count = components.size(); i < count; ++i )
	{
		auto& comp = components[i];
		if ( comp.first == type_hash )
		{
			return comp.second;
		}

		if ( dynamic_index == -1 && caster( comp.second.Get() ) )
		{
			dynamic_index = i;
		}
	}

	return dynamic_index;
}

RefPtr<Component> GameObject::GetComponent( size_t type_hash, std::function<bool( Component* )> caster )
{
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