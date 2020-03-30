using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

PxMaterial* Collider::mDefaultMat = nullptr;

Collider::Collider() : Component()
{
	if ( !mDefaultMat )
	{
		mDefaultMat = Physics::mPhysics->createMaterial( 0.5f, 0.5f, 0.6f );
	}
}

void Collider::Clone( Collider* already_object )
{
	already_object->mCenter = mCenter;
	already_object->mRotation = mRotation;
	already_object->mHasUpdate = true;
}

void Collider::AttachToActor( PxRigidActor* actor )
{
	ReleaseShape();
	mActor = actor;
	mHasUpdate = true;
}

Collider::~Collider()
{
	ReleaseShape();
}

void Collider::Update( Time& time, Input& input )
{
	if ( mHasUpdate && mActor )
	{
		PxTransform localPose = PxTransform( ToPhysX( mCenter ), ToPhysX( mRotation ) );
		if ( mShape )
		{
			mShape->setGeometry( *mGeometry );
		}
		else
		{
			PxShapeFlag::Enum shapeType = mIsTrigger ? PxShapeFlag::eTRIGGER_SHAPE : PxShapeFlag::eSIMULATION_SHAPE;

			mShape = PxRigidActorExt::createExclusiveShape( *mActor, *mGeometry, *mDefaultMat, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | shapeType );
		}
		mShape->setLocalPose( localPose );

		mHasUpdate = false;
	}
}

Vector3 Collider::Center_get()
{
	return mCenter;
}

void Collider::Center_set( Vector3 value )
{
	mCenter = value;
	mHasUpdate = true;
}

Quaternion Collider::Rotation_get()
{
	return mRotation;
}

void Collider::Rotation_set( Quaternion value )
{
	mRotation = value;
	mHasUpdate = true;
}

bool Collider::IsTrigger_get()
{
	return mIsTrigger;
}

void Collider::IsTrigger_set( bool value )
{
	if ( mShape )
	{
		mShape->setFlag( PxShapeFlag::eSIMULATION_SHAPE, !value );
		mShape->setFlag( PxShapeFlag::eTRIGGER_SHAPE, value );
	}
	mIsTrigger = value;
}

void Collider::ReleaseShape()
{
	if ( mShape && !mActor )
	{
		mShape->userData = nullptr;
		mShape->release();
		mShape = nullptr;
	}
}