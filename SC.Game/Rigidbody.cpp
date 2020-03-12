using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

Rigidbody::Rigidbody() : Component()
{
	auto gp = PxTransform( PxVec3( 0.0f ) );
	pxRigidbody = GlobalVar.pxDevice->createRigidDynamic( gp );

	pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true );
	pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true );
	pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true );
}

Rigidbody::~Rigidbody()
{
	if ( !AppShutdown && pxRigidbody )
	{
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}
}

object Rigidbody::Clone()
{
	var clone = new Rigidbody();
	return clone;
}

void Rigidbody::Start()
{
	auto pos = Transform->Position;
	auto quat = Transform->Rotation;

	PxTransform gp;
	gp.p = PxVec3( ( float )pos.X, ( float )pos.Y, ( float )pos.Z );
	gp.q = PxQuat( ( float )quat.X, ( float )quat.Y, ( float )quat.Z, ( float )quat.W );

	pxRigidbody->setGlobalPose( gp );
}

void Rigidbody::Update( Time& time, Input& input )
{
	UpdateRigidbodyPhysics();
}

bool Rigidbody::IsKinematic_get()
{
	return pxRigidbody->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC;
}

void Rigidbody::IsKinematic_set( bool value )
{
	pxRigidbody->setRigidBodyFlag( PxRigidBodyFlag::eKINEMATIC, value );
}

bool Rigidbody::FreezeRotation_get( int param0 )
{
	if ( param0 == 0 ) return pxRigidbody->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	else if ( param0 == 1 ) return pxRigidbody->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	else if ( param0 == 2 ) return pxRigidbody->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
#if defined( _DEBUG )
	else throw new ArgumentException( "SC.Game.Details.Rigidbody.FreezeRotation_get(): param0 must between 0 and 3." );
#endif
	return false;
}

void Rigidbody::FreezeRotation_set( int param0, bool value )
{
	if ( param0 == 0 ) pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, value );
	else if ( param0 == 1 ) pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, value );
	else if ( param0 == 2 ) pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, value );
#if defined( _DEBUG )
	else throw new ArgumentException( "SC.Game.Details.Rigidbody.FreezeRotation_set(): param0 must between 0 and 3." );
#endif
}

void Rigidbody::UpdateRigidbodyPhysics()
{
	set<Collider*> updateRigidbodies;
	list<list<Collider*>::iterator> remove_list;

	// 오브젝트에서 파생된 모든 게임 오브젝트의 컬라이더 컴포넌트를 가져옵니다.
	auto collect = Linked->GetRawComponentsInChildren<Collider>();
	updateRigidbodies.insert( collect.begin(), collect.end() );

	for ( auto i = appliedColliders.begin(); i != appliedColliders.end(); ++i )
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
		auto pShape = ( *( *i ) )->pxShape;
		if ( pShape )
		{
			pxRigidbody->detachShape( *pShape );
		}
		appliedColliders.erase( *i );
	}

	// 남은 업데이트 목록은 새로 추가된 리지드바디입니다.
	for ( auto i : updateRigidbodies )
	{
		if ( i->pxShape )
		{
			i->Start();
			pxRigidbody->attachShape( *i->pxShape );
			appliedColliders.push_back( i );
		}
	}
}