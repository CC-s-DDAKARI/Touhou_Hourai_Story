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
	GC.Add( pAllocators[0] );
	GC.Add( pAllocators[1] );
}

void CDeviceContextAndAllocator::Reset( CCommandQueue* pCommandQueue, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialPipelineState )
{
	auto frameIndex = *( int* )pCommandAllocator;

	HR( pAllocators[frameIndex]->Reset() );
	CDeviceContext::Reset( pCommandQueue, pAllocators[frameIndex].Get(), pInitialPipelineState );
}