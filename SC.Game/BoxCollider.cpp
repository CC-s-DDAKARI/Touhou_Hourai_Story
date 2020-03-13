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
	Clone( clone.Get() );
	clone->changeExtent = changeExtent;
	return clone;
}

void BoxCollider::Start()
{
	if ( started == false )
	{
		auto changeScale = Transform->Scale;

		Vector3 extent = changeExtent * changeScale;
		PxVec3 extent_ = PxVec3( ( float )extent.X, ( float )extent.Y, ( float )extent.Z );
		PxBoxGeometry boxGeo;
		boxGeo.halfExtents = extent_;
		pxShape->setGeometry( boxGeo );
		
		started = true;
	}
}

Vector3 BoxCollider::Extent_get()
{
	return changeExtent;
}

void BoxCollider::Extent_set( Vector3 value )
{
	changeExtent = value;
}