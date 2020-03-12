using namespace physx;

namespace SC::Game::Details
{
	tag_GlobalVar GlobalVar;
	bool AppShutdown;
	GarbageCollector GC;

	void tag_GlobalVar::InitializeComponents()
	{
		GlobalVar.factory = new IntegratedFactory();
		GlobalVar.device = new CDevice( GlobalVar.factory->SearchHardwareAdapter().Get() );
		GlobalVar.swapChain = new CSwapChain();
		GlobalVar.gameLogic = new GameLogic();

		// PhysX �Ŀ�̼� ��ü�� �ʱ�ȭ�մϴ�.
		pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, pxDefaultAllocator, pxDefaultErrorCallback );
		if ( !pxFoundation ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateFoundation failed." );

#if defined( _DEBUG )
		// ����� ��� ������ ��� Pvd ��ü�� �����մϴ�.
#endif
	}

	void tag_GlobalVar::Release()
	{
		if ( pxFoundation )
		{
			pxFoundation->release();
			pxFoundation = nullptr;
		}
	}
}