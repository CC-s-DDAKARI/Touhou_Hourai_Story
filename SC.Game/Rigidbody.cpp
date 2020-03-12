using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

Rigidbody::Rigidbody() : Component()
{
	auto gp = PxTransform( PxVec3( 0.0f ) );
	pxRigidbody = GlobalVar.pxDevice->createRigidDynamic( gp );
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

	// ������Ʈ���� �Ļ��� ��� ���� ������Ʈ�� �ö��̴� ������Ʈ�� �����ɴϴ�.
	auto collect = Linked->GetRawComponentsInChildren<Collider>();
	updateRigidbodies.insert( collect.begin(), collect.end() );

	for ( auto i = appliedColliders.begin(); i != appliedColliders.end(); ++i )
	{
		// ��� ������ٵ� ������Ʈ ������ٵ� ��Ͽ� ���� ��� ���ŵ˴ϴ�.
		if ( auto it = updateRigidbodies.find( *i ); it == updateRigidbodies.end() )
		{
			remove_list.push_back( i );
		}
		// ���� ��� ������Ʈ ��Ͽ��� ���ŵ˴ϴ�.
		else
		{
			updateRigidbodies.erase( it );
		}
	}

	// ���� ����� ����Ʈ���� ���� ����� �������� �����մϴ�.
	for ( auto i = remove_list.rbegin(); i != remove_list.rend(); ++i )
	{
		auto pShape = ( *( *i ) )->pxShape;
		if ( pShape )
		{
			pxRigidbody->detachShape( *pShape );
		}
		appliedColliders.erase( *i );
	}

	// ���� ������Ʈ ����� ���� �߰��� ������ٵ��Դϴ�.
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