using namespace physx;

namespace SC::Game::Details
{
	tag_GlobalVar GlobalVar;
	bool AppShutdown;
	GarbageCollector GC;

	void tag_GlobalVar::InitializeComponents()
	{
		// ���� ���� �����ϴ� ���� �� ��ü�� �����մϴ�.
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