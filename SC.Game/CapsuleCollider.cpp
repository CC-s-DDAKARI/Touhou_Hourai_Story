using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

CapsuleCollider::CapsuleCollider() : Collider()
{
	PxCapsuleGeometry capsuleGeo;
	capsuleGeo.radius = 1.0f;
	capsuleGeo.halfHeight = 1.0f;

	pxShape = GlobalVar.pxDevice->createShape( capsuleGeo, *pxDefaultMat );
}

CapsuleCollider::~CapsuleCollider()
{
	if ( !AppShutdown && pxShape )
	{
		pxShape->release();
		pxShape = nullptr;
	}
}

object CapsuleCollider::Clone()
{
	var clone = new CapsuleCollider();
	Clone( clone.Get() );
	clone->height = height;
	clone->radius = radius;
	return clone;
}

void CapsuleCollider::Start()
{
	auto changeScale = Transform->Scale;

	double halfHeight = height * changeScale.Y;
	double radius = this->radius * ( changeScale.X + changeScale.Z ) * 0.5;
	
	PxCapsuleGeometry capsuleGeo;
	capsuleGeo.radius = ( PxReal )radius;
	capsuleGeo.halfHeight = ( PxReal )halfHeight;

	pxShape->setGeometry( capsuleGeo );

	return Collider::Start();
}

double CapsuleCollider::Height_get()
{
	return height * 2;
}

void CapsuleCollider::Height_set( double value )
{
	height = value * 0.5;
}

double CapsuleCollider::Radius_get()
{
	return radius;
}

void CapsuleCollider::Radius_set( double value )
{
	radius = value;
}