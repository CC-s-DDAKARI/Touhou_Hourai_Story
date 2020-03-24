using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

BoxCollider::BoxCollider() : Collider()
{
	mHalfExtents = 1;

	auto boxGeo = make_unique<PxBoxGeometry>();
	boxGeo->halfExtents = PxVec3( 1.0f, 1.0f, 1.0f );

	mGeometry = move( boxGeo );
}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::Update( Time& time, Input& input )
{
	if ( mHasUpdate )
	{
		auto geo = static_cast< PxBoxGeometry* >( mGeometry.get() );
		geo->halfExtents = ToPhysX( mHalfExtents );
	}

	Collider::Update( time, input );
}

object BoxCollider::Clone()
{
	var clone = new BoxCollider();
	Clone( clone.Get() );
	clone->mHalfExtents = mHalfExtents;
	return clone;
}

Vector3 BoxCollider::HalfExtents_get()
{
	return mHalfExtents;
}

void BoxCollider::HalfExtents_set( Vector3 value )
{
	mHalfExtents = value;
	mHasUpdate = true;
}