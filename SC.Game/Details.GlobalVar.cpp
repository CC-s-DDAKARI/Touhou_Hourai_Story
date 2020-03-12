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

		// PhysX 파운데이션 개체를 초기화합니다.
		pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, pxDefaultAllocator, pxDefaultErrorCallback );
		if ( !pxFoundation ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateFoundation failed." );

#if defined( _DEBUG )
		// 디버그 모드 빌드일 경우 Pvd 개체를 생성합니다.
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