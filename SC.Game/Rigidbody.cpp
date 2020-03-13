using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

Rigidbody::Rigidbody() : Component()
{

}

object Rigidbody::Clone()
{
	var clone = new Rigidbody();
	return clone;
}

bool Rigidbody::IsKinematic_get()
{
	if ( pxRigidbody )
	{
		return pxRigidbody->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC;
	}
#if defined( _DEBUG )
	else
	{
		throw new Exception( "SC.Game.Rigidbody.IsKinematic_get(): 컴포넌트가 게임 오브젝트와 연결되지 않았습니다." );
	}
#endif
}

void Rigidbody::IsKinematic_set( bool value )
{
	if ( pxRigidbody )
	{
		pxRigidbody->setRigidBodyFlag( PxRigidBodyFlag::eKINEMATIC, value );
	}
#if defined( _DEBUG )
	else
	{
		throw new Exception( "SC.Game.Rigidbody.IsKinematic_set(): 컴포넌트가 게임 오브젝트와 연결되지 않았습니다." );
	}
#endif
}

bool Rigidbody::FreezeRotation_get( int param0 )
{
	if ( pxRigidbody )
	{
		if ( param0 == 0 ) return pxRigidbody->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
		else if ( param0 == 1 ) return pxRigidbody->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
		else if ( param0 == 2 ) return pxRigidbody->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
	#if defined( _DEBUG )
		else throw new ArgumentException( "SC.Game.Details.Rigidbody.FreezeRotation_get(): param0 must between 0 and 3." );
	#endif
		return false;
	}
#if defined( _DEBUG )
	else
	{
		throw new Exception( "SC.Game.Rigidbody.FreezeRotation_get(): 컴포넌트가 게임 오브젝트와 연결되지 않았습니다." );
	}
#endif
}

void Rigidbody::FreezeRotation_set( int param0, bool value )
{
	if ( pxRigidbody )
	{
		if ( param0 == 0 ) pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, value );
		else if ( param0 == 1 ) pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, value );
		else if ( param0 == 2 ) pxRigidbody->setRigidDynamicLockFlag( PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, value );
	#if defined( _DEBUG )
		else throw new ArgumentException( "SC.Game.Details.Rigidbody.FreezeRotation_set(): param0 must between 0 and 3." );
	#endif
	}
#if defined( _DEBUG )
	else
	{
		throw new Exception( "SC.Game.Rigidbody.FreezeRotation_set(): 컴포넌트가 게임 오브젝트와 연결되지 않았습니다." );
	}
#endif
}