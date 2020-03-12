using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

PxMaterial* BoxCollider::pxDefaultMat = nullptr;

BoxCollider::BoxCollider() : Collider()
{
	if ( !pxDefaultMat )
	{
		pxDefaultMat = GlobalVar.pxDevice->createMaterial( 0.5f, 0.5f, 0.6f );
	}

	PxBoxGeometry boxGeo;
	boxGeo.halfExtents = PxVec3( 1.0f, 1.0f, 1.0f );

	pxShape = GlobalVar.pxDevice->createShape( boxGeo, *pxDefaultMat );

	changeCenter = 0.0f;
	changeExtent = 1.0f;
}

BoxCollider::~BoxCollider()
{
	if ( !AppShutdown && pxShape )
	{
		pxShape->release();
		pxShape = nullptr;
	}
}

object BoxCollider::Clone()
{
	var clone = new BoxCollider();
	return clone;
}

void BoxCollider::Start()
{
	auto changeScale = Transform->Scale;

	Vector3 extent = changeExtent * changeScale;
	Vector3 center = changeCenter * changeScale;

	PxVec3 extent_ = PxVec3( ( float )extent.X, ( float )extent.Y, ( float )extent.Z );
	PxVec3 center_ = PxVec3( ( float )center.X, ( float )center.Y, ( float )center.Z );

	PxBoxGeometry boxGeo;
	boxGeo.halfExtents = extent_;

	PxTransform lp = PxTransform( center_ );
	pxShape->setGeometry( boxGeo );
	pxShape->setLocalPose( lp );
}

Vector3 BoxCollider::Center_get()
{
	return changeCenter;
}

void BoxCollider::Center_set( Vector3 value )
{
	changeCenter = value;
}

Vector3 BoxCollider::Extent_get()
{
	return changeExtent;
}

void BoxCollider::Extent_set( Vector3 value )
{
	changeExtent = value;
}