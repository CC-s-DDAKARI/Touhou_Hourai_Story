using namespace physx;

namespace SC::Game::Details
{
	tag_GlobalVar GlobalVar;
	bool AppShutdown;
	GarbageCollector GC;

	void tag_GlobalVar::InitializeComponents()
	{
		// 게임 논리를 수행하는 게임 논리 개체를 생성합니다.
		gameLogic = new GameLogic();
	}

	void tag_GlobalVar::Release()
	{
		gameLogic = nullptr;

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