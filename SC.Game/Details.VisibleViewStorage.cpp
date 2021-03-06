using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

VisibleViewStorage::VisibleViewStorage( int capacity ) : Object()
	, capacity( capacity )
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	// 서술자 힙 개체를 생성합니다.
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{ };
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = ( UINT )capacity;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	HR( pDevice->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &pDescriptorHeap ) ) );
	handleBase = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleBaseGpu = pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	// 서술자 핸들의 크기를 조회합니다.
	stride = pDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
}

int VisibleViewStorage::Lock()
{
	if ( failure )
	{
		return -1;
	}
	else
	{
		if ( lockId >= capacity )
		{
			auto pDevice = Graphics::mDevice->pDevice.Get();

			GC::Add( App::mFrameIndex, move( pDescriptorHeap ), 5 );

			// 서술자 힙 개체를 다시 생성합니다.
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{ };
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heapDesc.NumDescriptors = ( UINT )capacity * 2;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			HR( pDevice->CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &pDescriptorHeap ) ) );
			handleBase = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			handleBaseGpu = pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

			capacity *= 2;

			failure = true;
			return -1;
		}

		return lockId++;
	}
}

void VisibleViewStorage::Reset()
{
	lockId = 0;
	failure = false;
}

D3D12_CPU_DESCRIPTOR_HANDLE VisibleViewStorage::GetCPUHandle( int lockIndex )
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = handleBase;
	handle.ptr += stride * lockIndex;

	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE VisibleViewStorage::GetGPUHandle( int lockIndex )
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = handleBaseGpu;
	handle.ptr += stride * lockIndex;

	return handle;
}