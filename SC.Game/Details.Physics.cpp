using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace physx;

PxDefaultAllocator Physics::mDefaultAllocator;
PxDefaultErrorCallback Physics::mDefaultErrorCallback;

PxFoundation* Physics::mFoundation;
PxPvd* Physics::mPVD;
PxPvdTransport* Physics::mPVDTransport;

PxPhysics* Physics::mPhysics;
PxCooking* Physics::mCooking;
PxDefaultCpuDispatcher* Physics::mDispatcher;
PxCudaContextManager* Physics::mCudaManager;

void Physics::Initialize()
{
	bool recordMemoryAllocations = false;

	App::Disposing += Dispose;

	// PhysX 파운데이션 개체를 초기화합니다.
	mFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, mDefaultAllocator, mDefaultErrorCallback );
	if ( !mFoundation ) throw new Exception( "SC.Game.Details.Physics.Initialize(): PxCreateFoundation() 함수에서 PxFoundation 개체를 생성하는데 실패하였습니다." );

#if defined( _DEBUG )
	recordMemoryAllocations = true;

	// 디버그 모드 빌드일 경우 Physics Visual Debugger 개체를 생성합니다.
	mPVD = PxCreatePvd( *mFoundation );
	mPVDTransport = PxDefaultPvdSocketTransportCreate( "localhost", 5425, 1000 );
	mPVD->connect( *mPVDTransport, PxPvdInstrumentationFlag::eALL );
#endif

	// PhysX 장치 개체를 생성합니다.
	mPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mPVD );
	if ( !mPhysics ) throw new Exception( "SC.Game.Details.Physics.Initialize(): PxCreatePhysics() 함수에서 PxPhysics 개체를 생성하는데 실패하였습니다." );

	// 추가 물리 개체를 등록합니다.
	PxRegisterArticulations( *mPhysics );
	PxRegisterHeightFields( *mPhysics );

	// 큰 물리 개체를 관리하는 개체를 생성합니다.
	mCooking = PxCreateCooking( PX_PHYSICS_VERSION, *mFoundation, PxCookingParams( PxTolerancesScale() ) );
	if ( !mCooking ) throw new Exception( "SC.Game.Details.Physics.Initialize(): PxCreateCooking() 함수에서 PxCooking 개체를 생성하는데 실패하였습니다." );

	// PhysX Dispatcher 개체를 생성합니다.
	mDispatcher = PxDefaultCpuDispatcherCreate( 4 );
	mCudaManager = PxCreateCudaContextManager( *mFoundation, PxCudaContextManagerDesc() );
}

void Physics::Dispose( object sender )
{
	if ( mCudaManager )
	{
		mCudaManager->release();
		mCudaManager = nullptr;
	}

	if ( mDispatcher )
	{
		mDispatcher->release();
		mDispatcher = nullptr;
	}

	if ( mCooking )
	{
		mCooking->release();
		mCooking = nullptr;
	}

	if ( mPhysics )
	{
		mPhysics->release();
		mPhysics = nullptr;
	}

	if ( mPVD )
	{
		mPVD->release();
		mPVD = nullptr;
	}

	// PVD Transport 개체는 PVD가 해제된 후 해제되어야 합니다.
	if ( mPVDTransport )
	{
		mPVDTransport->release();
		mPVDTransport = nullptr;
	}

	if ( mFoundation )
	{
		mFoundation->release();
		mFoundation = nullptr;
	}
}