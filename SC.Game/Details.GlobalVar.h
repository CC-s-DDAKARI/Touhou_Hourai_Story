#pragma once

namespace SC::Game::Details
{
	struct tag_GlobalVar
	{
		std::mutex globalMutex;

		Drawing::Point<double> scrollDelta;

		int frameIndex = 0;

		std::set<physx::PxScene*> pxSceneList;
		std::set<physx::PxRigidActor*> pxRigidActor;

		void InitializeComponents();
		void Release();
	};

	extern tag_GlobalVar GlobalVar;
	extern bool AppShutdown;
	extern GarbageCollector GC;
}