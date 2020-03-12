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

		bool recordMemoryAllocations = false;

		// PhysX �Ŀ�̼� ��ü�� �ʱ�ȭ�մϴ�.
		pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, pxDefaultAllocator, pxDefaultErrorCallback );
		if ( !pxFoundation ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateFoundation failed." );

#if defined( _DEBUG )
		// ����� ��� ������ ��� Pvd ��ü�� �����մϴ�.
		pxPvd = PxCreatePvd( *pxFoundation );
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
		pxPvd->connect( *transport, PxPvdInstrumentationFlag::eALL );

		recordMemoryAllocations = true;
#endif

		// PhysX ��ġ ��ü�� �����մϴ�.
		pxDevice = PxCreatePhysics( PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), recordMemoryAllocations, pxPvd );
		if ( !pxDevice ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreatePhysics failed." );

		// �߰� ���̺귯���� �ҷ��ɴϴ�.
		if ( !PxInitExtensions( *pxDevice, pxPvd ) )
			throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxInitExtensions failed." );

		PxRegisterArticulations( *pxDevice );
		PxRegisterHeightFields( *pxDevice );

		// �ö��̴� ��ü�� ����ŷ�ϴ� Cooking ��ü�� �����մϴ�.
		pxCooking = PxCreateCooking( PX_PHYSICS_VERSION, *pxFoundation, PxCookingParams( PxTolerancesScale() ) );
		if ( !pxCooking ) throw new Exception( "SC.Game.Details.tag_GlobalVar.InitializeComponents(): PxCreateCooking failed." );
	}

	void tag_GlobalVar::Release()
	{
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

#if defined( _DEBUG )
		if ( pxPvd )
		{
			pxPvd->release();
			pxPvd = nullptr;
		}
#endif

		if ( pxFoundation )
		{
			pxFoundation->release();
			pxFoundation = nullptr;
		}
	}
}