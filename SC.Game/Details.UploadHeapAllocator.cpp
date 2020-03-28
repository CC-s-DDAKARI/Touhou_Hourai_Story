using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

vector<UploadAlignedHeap> UploadHeapAllocator::mAlignedHeaps;
bool UploadHeapAllocator::mHeapValid = false;

UploadAlignedHeap::UploadAlignedHeap( uint64 alignment )
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	// 1024개의 공간을 가지는 기초 힙을 할당합니다.
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = alignment * 1024;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc = { 1, 0 };
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HR( pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &pResource )
		) );

	// 주소를 매핑합니다.
	mStartAddress = pResource->GetGPUVirtualAddress();
	HR( pResource->Map( 0, nullptr, &pStartAddress ) );

	mAlign = alignment;
	mCount = 1024;

	// 힙 큐에 목록을 채웁니다.
	for ( uint64 i = 0; i < 1024; ++i )
	{
		mQueue.push( i );
	}
}

UploadAlignedHeap::UploadAlignedHeap( UploadAlignedHeap&& mov )
{
	pResource = move( mov.pResource );
	mStartAddress = mov.mStartAddress;
	pStartAddress = move( mov.pStartAddress );
	mAlign = mov.mAlign;
	mCount = mov.mCount;
	mQueue = move( mov.mQueue );
}

uint64 UploadAlignedHeap::Alloc()
{
	lock_guard<mutex> lock( mLocker );

	if ( mQueue.empty() )
	{
		return Expand();
	}
	else
	{
		auto index = mQueue.front();
		mQueue.pop();

		return index;
	}
}

void UploadAlignedHeap::Free( uint64 index )
{
	if ( UploadHeapAllocator::mHeapValid )
	{
		lock_guard<mutex> lock( mLocker );

		mQueue.push( index );
	}
}

D3D12_GPU_VIRTUAL_ADDRESS UploadAlignedHeap::GetVirtualAddress( uint64 index )
{
	return mStartAddress + mAlign * index;
}

void* UploadAlignedHeap::GetCPUBlock( uint64 index )
{
	return ( char* )pStartAddress + mAlign * index;
}

uint64 UploadAlignedHeap::Expand()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	// 이전 개체를 모두 사용 완료한 후 제거하도록 합니다.
	GC::Add( GlobalVar.frameIndex, pResource.Get(), 100 );

	// 1024개의 추가 공간을 할당하여 새로운 개체를 생성합니다.
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = mAlign * ( mCount + 1024 );
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc = { 1, 0 };
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HR( pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &pResource )
		) );

	// 주소를 매핑합니다.
	mStartAddress = pResource->GetGPUVirtualAddress();
	HR( pResource->Map( 0, nullptr, &pStartAddress ) );

	// 힙 큐에 목록을 채웁니다.
	for ( uint64 i = 1; i < 1024; ++i )
	{
		mQueue.push( mCount + i );
	}

	auto index = mCount;
	mCount += 1024;
	return index;
}

void UploadHeapAllocator::Initialize()
{
	mHeapValid = true;
}

ComPtr<UploadHeap> UploadHeapAllocator::Alloc( uint64 sizeInBytes )
{
	uint64 alignedIndex = ( sizeInBytes - 1 ) / 256;
	
	// 해당 정렬된 힙이 존재하지 않으면 정렬 위치까지 할당합니다.
	if ( mAlignedHeaps.size() <= alignedIndex )
	{
		for ( uint64 i = mAlignedHeaps.size(); i <= alignedIndex; ++i )
		{
			mAlignedHeaps.emplace_back( ( i + 1 ) * 256 );
		}
	}

	return new UploadHeap( &mAlignedHeaps[alignedIndex], mAlignedHeaps[alignedIndex].Alloc() );
}

void UploadHeapAllocator::Dispose( object sender )
{
	mHeapValid = false;
}