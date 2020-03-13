using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

PxMaterial* Collider::pxDefaultMat = nullptr;

Collider::Collider() : Component()
{
	if ( !pxDefaultMat )
	{
		pxDefaultMat = GlobalVar.pxDevice->createMaterial( 0.5f, 0.5f, 0.6f );
	}
}

void Collider::Clone( Collider* already_object )
{
	already_object->changeCenter = changeCenter;
	already_object->changeRotation = changeRotation;
}

Collider::~Collider()
{

}

void Collider::Start()
{
	auto changeScale = Transform->Scale;

	Vector3 center = changeCenter * changeScale;
	PxTransform lp = PxTransform( ToPhysX( center ) );
	lp.q = ToPhysX( changeRotation );
	pxShape->setLocalPose( lp );

	Linked->AttachCollider( this );
}

Vector3 Collider::Center_get()
{
	return changeCenter;
}

void Collider::Center_set( Vector3 value )
{
	if ( started == false )
	{
		changeCenter = value;
	}
#if defined( _DEBUG )
	else
	{
		throw new Exception( "SC.Game.Collider.Center_set(): Collider offset cannot change when collider attached." );
	}
#endif
}

Quaternion Collider::Rotation_get()
{
	return changeRotation;
}

void Collider::Rotation_set( Quaternion value )
{
	if ( started == false )
	{
		changeRotation = value;
	}
#if defined( _DEBUG )
	else
	{
		throw new Exception( "SC.Game.Collider.Center_set(): Collider offset cannot change when collider attached." );
	}
#endif
}