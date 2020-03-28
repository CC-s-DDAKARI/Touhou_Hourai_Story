using namespace SC;
using namespace SC::Game::Details;

CDeviceContextAndAllocator::CDeviceContextAndAllocator( RefPtr<CDevice>& device, D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState* pInitialPipelineState ) : CDeviceContext( device, type, nullptr, pInitialPipelineState )
{
	auto& pDevice = *device->pDevice.Get();

	HR( pDevice.CreateCommandAllocator( type, IID_PPV_ARGS( &pAllocators[0] ) ) );
	HR( pDevice.CreateCommandAllocator( type, IID_PPV_ARGS( &pAllocators[1] ) ) );

	if ( pInitialPipelineState )
	{
		Reset( nullptr, nullptr, pInitialPipelineState );
	}
}

CDeviceContextAndAllocator::~CDeviceContextAndAllocator()
{
	GC::Add( GlobalVar.frameIndex, pAllocators[0].Get(), 1 );
	GC::Add( GlobalVar.frameIndex, pAllocators[1].Get(), 1 );
}

void CDeviceContextAndAllocator::Reset( CCommandQueue* pCommandQueue, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialPipelineState )
{
	UNREFERENCED_PARAMETER( pCommandAllocator );

	HR( pAllocators[FrameIndex]->Reset() );
	CDeviceContext::Reset( pCommandQueue, pAllocators[FrameIndex].Get(), pInitialPipelineState );
}