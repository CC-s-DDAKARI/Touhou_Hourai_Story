#pragma once

namespace SC::Game::Details
{
	class ContactCallback : public physx::PxSimulationEventCallback
	{
	public:
		void onConstraintBreak( physx::PxConstraintInfo* constraints, physx::PxU32 count ) override;
		void onWake( physx::PxActor** actors, physx::PxU32 count ) override;
		void onSleep( physx::PxActor** actors, physx::PxU32 count ) override;
		void onContact( const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* contactPairs, physx::PxU32 contactCount ) override;
		void onTrigger( physx::PxTriggerPair* pairs, physx::PxU32 count ) override;
		void onAdvance( const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count );
	};
}