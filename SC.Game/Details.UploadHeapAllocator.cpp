using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

vector<UploadAlignedHeap> UploadHeapAllocator::mAlignedHeaps;
bool UploadHeapAllocator::mHeapValid = false;

UploadAlignedHeap::UploadAlignedHeap( uint64 alignment )
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	// 1024���� ������ ������ ���� ���� �Ҵ��մϴ�.
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = alignment * mAllocUnit;
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

	// �ּҸ� �����մϴ�.
	mStartAddress = pResource->GetGPUVirtualAddress();
	HR( pResource->Map( 0, nullptr, &pStartAddress ) );

	mAlign = alignment;
	mCount = mAllocUnit;

	// �� ť�� ����� ä��ϴ�.
	for ( uint64 i = 0; i < mAllocUnit; ++i )
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

	// ���� ��ü�� ��� ��� �Ϸ��� �� �����ϵ��� �մϴ�.
	GC::Add( App::mFrameIndex, pResource.Get(), 60 );

	// 1024���� �߰� ������ �Ҵ��Ͽ� ���ο� ��ü�� �����մϴ�.
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = mAlign * ( mCount + mAllocUnit );
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

	// �ּҸ� �����մϴ�.
	mStartAddress = pResource->GetGPUVirtualAddress();
	HR( pResource->Map( 0, nullptr, &pStartAddress ) );

	// �� ť�� ����� ä��ϴ�.
	for ( uint64 i = 1; i < mAllocUnit; ++i )
	{
		mQueue.push( mCount + i );
	}

	auto index = mCount;
	mCount += mAllocUnit;
	return index;
}

void UploadHeapAllocator::Initialize()
{
	App::Disposing += Dispose;

	mHeapValid = true;
}

ComPtr<UploadHeap> UploadHeapAllocator::Alloc( uint64 sizeInBytes )
{
	uint64 alignedIndex = ( sizeInBytes - 1 ) / 256;
	
	// �ش� ���ĵ� ���� �������� ������ ���� ��ġ���� �Ҵ��մϴ�.
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