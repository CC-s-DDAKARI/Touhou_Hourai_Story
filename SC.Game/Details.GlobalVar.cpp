using namespace physx;

namespace SC::Game::Details
{
	tag_GlobalVar GlobalVar;
	bool AppShutdown;
	GarbageCollector GC;

	void tag_GlobalVar::InitializeComponents()
	{
		// �⺻ �׷��� ����̽��� �����մϴ�.
		factory = new IntegratedFactory();
		device = new CDevice( GlobalVar.factory->SearchHardwareAdapter().Get() );
		swapChain = new CSwapChain();


		// PhysX �Ŀ�̼� ��ü�� �ʱ�ȭ�մϴ�.
		pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, pxDefaultAllocator, pxDefaultErrorCallback );
		if ( !pxFoundation ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateFoundation failed." );

		bool recordMemoryAllocations = false;
#if defined( _DEBUG )
		recordMemoryAllocations = true;
#endif

		// PhysX ��ġ ��ü�� �����մϴ�.
		pxDevice = PxCreatePhysics( PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), recordMemoryAllocations, pxPvd );
		if ( !pxDevice ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreatePhysics failed." );

		// �߰� ���̺귯���� �ҷ��ɴϴ�.
		//if ( !PxInitExtensions( *pxDevice, pxPvd ) )
			//throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxInitExtensions failed." );

		PxRegisterArticulations( *pxDevice );
		PxRegisterHeightFields( *pxDevice );

		// �ö��̴� ��ü�� ����ŷ�ϴ� Cooking ��ü�� �����մϴ�.
		pxCooking = PxCreateCooking( PX_PHYSICS_VERSION, *pxFoundation, PxCookingParams( PxTolerancesScale() ) );
		if ( !pxCooking ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateCooking failed." );

		// ��Ƽ �����带 ����ϴ� �⺻ PhysX �½�ũ �Ŵ��� ��ü�� �����մϴ�.
		pxDefaultDispatcher = PxDefaultCpuDispatcherCreate( 4 );

		// ���� ���� �����ϴ� ���� �� ��ü�� �����մϴ�.
		gameLogic = new GameLogic();
	}

	void tag_GlobalVar::Release()
	{
		gameLogic = nullptr;

		for ( auto i : pxSceneList )
		{
			i->release();
		}
		pxSceneList.clear();

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