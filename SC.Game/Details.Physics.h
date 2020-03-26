#pragma once

namespace SC::Game::Details
{
	class Physics abstract
	{
	public:
		static physx::PxDefaultAllocator mDefaultAllocator;
		static physx::PxDefaultErrorCallback mDefaultErrorCallback;

		static physx::PxFoundation* mFoundation;
		static physx::PxPvd* mPVD;
		static physx::PxPvdTransport* mPVDTransport;

		static physx::PxPhysics* mPhysics;
		static physx::PxCooking* mCooking;
		static physx::PxDefaultCpuDispatcher* mDispatcher;
		static physx::PxCudaContextManager* mCudaManager;

	public:
		static void Initialize();

	private:
		static void Dispose( object sender );
	};
}