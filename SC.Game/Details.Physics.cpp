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

	App::AppDisposing += Dispose;

	// PhysX �Ŀ�̼� ��ü�� �ʱ�ȭ�մϴ�.
	mFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, mDefaultAllocator, mDefaultErrorCallback );
	if ( !mFoundation ) throw new Exception( "SC.Game.Details.Physics.Initialize(): PxCreateFoundation() �Լ����� PxFoundation ��ü�� �����ϴµ� �����Ͽ����ϴ�." );

#if defined( _DEBUG )
	recordMemoryAllocations = true;

	// ����� ��� ������ ��� Physics Visual Debugger ��ü�� �����մϴ�.
	mPVD = PxCreatePvd( *mFoundation );
	mPVDTransport = PxDefaultPvdSocketTransportCreate( "localhost", 5425, 1000 );
	mPVD->connect( *mPVDTransport, PxPvdInstrumentationFlag::eALL );
#endif

	// PhysX ��ġ ��ü�� �����մϴ�.
	mPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mPVD );
	if ( !mPhysics ) throw new Exception( "SC.Game.Details.Physics.Initialize(): PxCreatePhysics() �Լ����� PxPhysics ��ü�� �����ϴµ� �����Ͽ����ϴ�." );

	// �߰� ���� ��ü�� ����մϴ�.
	PxRegisterArticulations( *mPhysics );
	PxRegisterHeightFields( *mPhysics );

	// ū ���� ��ü�� �����ϴ� ��ü�� �����մϴ�.
	mCooking = PxCreateCooking( PX_PHYSICS_VERSION, *mFoundation, PxCookingParams( PxTolerancesScale() ) );
	if ( !mCooking ) throw new Exception( "SC.Game.Details.Physics.Initialize(): PxCreateCooking() �Լ����� PxCooking ��ü�� �����ϴµ� �����Ͽ����ϴ�." );

	// PhysX Dispatcher ��ü�� �����մϴ�.
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

	// PVD Transport ��ü�� PVD�� ������ �� �����Ǿ�� �մϴ�.
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