using namespace SC;
using namespace SC::Game::Details;

using namespace std;

CDeviceContextAndAllocator::CDeviceContextAndAllocator( RefPtr<CDevice>& device, D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState* pInitialPipelineState ) : CDeviceContext( device, type, nullptr, pInitialPipelineState )
{
	auto& pDevice = *device->pDevice.Get();

	HR( pDevice.CreateCommandAllocator( type, IID_PPV_ARGS( &pAllocators[0] ) ) );
	HR( pDevice.CreateCommandAllocator( type, IID_PPV_ARGS( &pAllocators[1] ) ) );

#if defined( _DEBUG )
	pAllocators[0]->SetName( L"CDeviceContextAndAllocator.pAllocators[0]" );
	pAllocators[1]->SetName( L"CDeviceContextAndAllocator.pAllocators[1]" );
#endif

	if ( pInitialPipelineState )
	{
		Reset( nullptr, nullptr, pInitialPipelineState );
	}
}

CDeviceContextAndAllocator::~CDeviceContextAndAllocator()
{
	GC::Add( App::mFrameIndex, move( pAllocators[0] ), 5 );
	GC::Add( App::mFrameIndex, move( pAllocators[1] ), 5 );
}

void CDeviceContextAndAllocator::Reset( CCommandQueue* pCommandQueue, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialPipelineState )
{
	UNREFERENCED_PARAMETER( pCommandAllocator );

	HR( pAllocators[FrameIndex]->Reset() );
	CDeviceContext::Reset( pCommandQueue, pAllocators[FrameIndex].Get(), pInitialPipelineState );
}