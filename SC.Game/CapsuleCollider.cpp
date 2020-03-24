using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

CapsuleCollider::CapsuleCollider() : Collider()
{
	auto capsuleGeo = make_unique<PxCapsuleGeometry>();
	capsuleGeo->radius = 1.0f;
	capsuleGeo->halfHeight = 1.0f;

	mGeometry = move( capsuleGeo );
}

CapsuleCollider::~CapsuleCollider()
{

}

object CapsuleCollider::Clone()
{
	var clone = new CapsuleCollider();
	Clone( clone.Get() );
	clone->mHalfHeight = mHalfHeight;
	clone->mRadius = mRadius;
	return clone;
}

void CapsuleCollider::Update( Time& time, Input& input )
{
	if ( mHasUpdate )
	{
		auto geo = static_cast< PxCapsuleGeometry* >( mGeometry.get() );
		geo->radius = ( PxReal )( mRadius );
		geo->halfHeight =( PxReal )( mHalfHeight );
	}

	Collider::Update( time, input );
}

double CapsuleCollider::Height_get()
{
	return mHalfHeight * 2;
}

void CapsuleCollider::Height_set( double value )
{
	mHalfHeight = value * 0.5;
	mHasUpdate = true;
}

double CapsuleCollider::Radius_get()
{
	return mRadius;
}

void CapsuleCollider::Radius_set( double value )
{
	mRadius = value;
	mHasUpdate = true;
}