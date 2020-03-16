using namespace physx;

namespace SC::Game::Details
{
	tag_GlobalVar GlobalVar;
	bool AppShutdown;
	GarbageCollector GC;

	void tag_GlobalVar::InitializeComponents()
	{
		// 기본 그래픽 디바이스를 생성합니다.
		factory = new IntegratedFactory();
		device = new CDevice( GlobalVar.factory->SearchHardwareAdapter().Get() );
		swapChain = new CSwapChain();


		// PhysX 파운데이션 개체를 초기화합니다.
		pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, pxDefaultAllocator, pxDefaultErrorCallback );
		if ( !pxFoundation ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateFoundation failed." );

		bool recordMemoryAllocations = false;
#if defined( _DEBUG )
		recordMemoryAllocations = true;
#endif

		// PhysX 장치 개체를 생성합니다.
		pxDevice = PxCreatePhysics( PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), recordMemoryAllocations, pxPvd );
		if ( !pxDevice ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreatePhysics failed." );

		// 추가 라이브러리를 불러옵니다.
		//if ( !PxInitExtensions( *pxDevice, pxPvd ) )
			//throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxInitExtensions failed." );

		PxRegisterArticulations( *pxDevice );
		PxRegisterHeightFields( *pxDevice );

		// 컬라이더 개체를 베이킹하는 Cooking 개체를 생성합니다.
		pxCooking = PxCreateCooking( PX_PHYSICS_VERSION, *pxFoundation, PxCookingParams( PxTolerancesScale() ) );
		if ( !pxCooking ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateCooking failed." );

		// 멀티 스레드를 사용하는 기본 PhysX 태스크 매니저 개체를 생성합니다.
		pxDefaultDispatcher = PxDefaultCpuDispatcherCreate( 4 );

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

		if ( pxDefaultDispatcher )
		{
			pxDefaultDispatcher->release();
			pxDefaultDispatcher = nullptr;
		}

		if ( pxCooking )
		{
			pxCooking->release();
			pxCooking = nullptr;
		}

		if ( pxDevice )
		{
			pxDevice->release();
			pxDevice = nullptr;
		}

		if ( pxFoundation )
		{
			pxFoundation->release();
			pxFoundation = nullptr;
		}
	}
}