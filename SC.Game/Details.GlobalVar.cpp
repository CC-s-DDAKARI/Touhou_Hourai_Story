using namespace physx;

namespace SC::Game::Details
{
	tag_GlobalVar GlobalVar;
	bool AppShutdown;
	GarbageCollector GC;

	void tag_GlobalVar::InitializeComponents()
	{
	}

	void tag_GlobalVar::Release()
	{
		for ( auto i : pxRigidActor )
		{
			i->release();
		}

		for ( auto i : pxSceneList )
		{
			i->release();
		}
	}
}