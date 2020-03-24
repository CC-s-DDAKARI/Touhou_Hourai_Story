using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

void ContactCallback::onConstraintBreak( PxConstraintInfo* constraints, PxU32 count )
{

}

void ContactCallback::onWake( PxActor** actors, PxU32 count )
{

}

void ContactCallback::onSleep( PxActor** actors, PxU32 count )
{

}

void ContactCallback::onContact( const PxContactPairHeader& pairHeader, const PxContactPair* contactPairs, PxU32 contactCount )
{
	auto gameObject0 = ( GameObject* )pairHeader.actors[0]->userData;
	auto gameObject1 = ( GameObject* )pairHeader.actors[1]->userData;

	for ( int i = 0, count = ( int )contactCount; i < count; ++i )
	{
		auto& pair = contactPairs[i];
		if ( pair.flags & PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH )
		{
			if ( gameObject0 ) gameObject0->OnCollisionEnter( gameObject1 );
			if ( gameObject1 ) gameObject1->OnCollisionEnter( gameObject0 );
		}
		else if ( pair.flags & PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH )
		{
			if ( gameObject0 ) gameObject0->OnCollisionExit( gameObject1 );
			if ( gameObject1 ) gameObject1->OnCollisionExit( gameObject0 );
		}
		else
		{
			if ( gameObject0 ) gameObject0->OnCollisionStay( gameObject1 );
			if ( gameObject1 ) gameObject1->OnCollisionStay( gameObject0 );
		}
	}
}

void ContactCallback::onTrigger( PxTriggerPair* pairs, PxU32 count )
{
	for ( int i = 0, c = ( int )count; i < c; ++i )
	{
		auto& pair = pairs[i];

		auto gameObject0 = ( GameObject* )pair.triggerActor->userData;
		auto gameObject1 = ( GameObject* )pair.otherActor->userData;

		if ( pair.status & PxPairFlag::eNOTIFY_TOUCH_FOUND )
		{
			if ( gameObject0 ) gameObject0->OnTriggerEnter( ( Collider* )pair.otherShape->userData );
			if ( gameObject1 ) gameObject1->OnTriggerEnter( ( Collider* )pair.triggerShape->userData );
		}
		else if ( pair.status & PxPairFlag::eNOTIFY_TOUCH_LOST )
		{
			if ( gameObject0 ) gameObject0->OnTriggerExit( ( Collider* )pair.otherShape->userData );
			if ( gameObject1 ) gameObject1->OnTriggerExit( ( Collider* )pair.triggerShape->userData );
		}
	}
}

void ContactCallback::onAdvance( const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count )
{

}