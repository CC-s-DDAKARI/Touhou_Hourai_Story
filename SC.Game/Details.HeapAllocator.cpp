using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

vector<AlignedHeap> HeapAllocator::mAlignedHeaps;
bool HeapAllocator::mHeapValid = false;
set<LargeHeap*> HeapAllocator::mLargeHeaps;

AlignedHeap::AlignedHeap( uint64 alignment )
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	// 1024���� ������ ������ ���� ���� �Ҵ��մϴ�.
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
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

	// ������ ���ε� ������ �����մϴ�.
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeaps[0] ) ) );
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeaps[1] ) ) );

	// �ּҸ� �����մϴ�.
	mStartAddress = pResource->GetGPUVirtualAddress();

	HR( pUploadHeaps[0]->Map( 0, nullptr, &pUploadAddress[0] ) );
	HR( pUploadHeaps[1]->Map( 0, nullptr, &pUploadAddress[1] ) );

	mAlign = alignment;
	mCount = mAllocUnit;

	// �� ť�� ����� ä��ϴ�.
	for ( uint64 i = 0; i < mAllocUnit; ++i )
	{
		mQueue.push( i );
	}
}

AlignedHeap::AlignedHeap( AlignedHeap&& mov )
{
	pResource = move( mov.pResource );
	pUploadHeaps[0] = move( mov.pUploadHeaps[0] );
	pUploadHeaps[1] = move( mov.pUploadHeaps[1] );

	mStartAddress = mov.mStartAddress;
	mAlign = mov.mAlign;
	mCount = mov.mCount;
	mQueue = move( mov.mQueue );

	pUploadAddress[0] = mov.pUploadAddress[0];
	pUploadAddress[1] = mov.pUploadAddress[1];
}

uint64 AlignedHeap::Alloc()
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

void AlignedHeap::Free( uint64 index )
{
	if ( UploadHeapAllocator::mHeapValid )
	{
		lock_guard<mutex> lock( mLocker );

		mQueue.push( index );
	}
}

void AlignedHeap::Commit( int frameIndex, CDeviceContext& deviceContext )
{
	auto pCommandList = deviceContext.pCommandList.Get();

	if ( IsCommittable( frameIndex ) )
	{
		lock_guard<mutex> lock( mLocker );

		if ( pExpandCopy )
		{
			// Ȯ�� ���� �����͸� �� ���ҽ��� �����մϴ�.
			UINT64 range = pExpandCopy->GetDesc().Width;
			pCommandList->CopyBufferRegion( pResource.Get(), 0, pExpandCopy.Get(), 0, range );
			GC::Add( frameIndex, move( pExpandCopy ), 1000 );
		}

		// ���� ������ ������ �����մϴ�.
		auto& ranges = mCopyRangeQueue[frameIndex];

		list<D3D12_RANGE> results;
		auto it = ranges.begin();
		pair<UINT64, UINT64> current = *it;

		while ( it != ranges.end() )
		{
			if ( current.second >= it->first )
			{
				current.second = max( current.second, it->second );
			}
			else
			{
				results.push_back( { current.first, current.second } );
				current = *it;
			}

			it++;
		}
		results.push_back( { current.first, current.second } );

		for ( auto i : results )
		{
			auto& item = i;

			uint64 sizeInBytes = item.End - item.Begin;
			pCommandList->CopyBufferRegion( pResource.Get(), item.Begin, pUploadHeaps[frameIndex].Get(), item.Begin, sizeInBytes );
		}

		ranges.clear();
	}
}

bool AlignedHeap::IsCommittable( int frameIndex )
{
	return mCopyRangeQueue[frameIndex].size();
}

D3D12_GPU_VIRTUAL_ADDRESS AlignedHeap::GetVirtualAddress( uint64 index )
{
	return mStartAddress + mAlign * index;
}

void* AlignedHeap::GetUploadAddress( int frameIndex, uint64 index )
{
	return ( char* )pUploadAddress[frameIndex] + index * mAlign;
}

void AlignedHeap::AddCopyRange( int frameIndex, const D3D12_RANGE& range )
{
	lock_guard<mutex> lock( mLocker );
	mCopyRangeQueue[frameIndex][range.Begin] = range.End;
}

uint64 AlignedHeap::Expand()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();
	bool copyBack = false;

	// ���� ��ü�� ��� ��� �Ϸ��� �� �����ϵ��� �մϴ�.
	if ( !pExpandCopy )
	{
		pExpandCopy = move( pResource );
		copyBack = true;
	}
	else
	{
		GC::Add( App::mFrameIndex, move( pResource ), 1000 );
	}
	GC::Add( App::mFrameIndex, move( pUploadHeaps[0] ), 1000 );
	GC::Add( App::mFrameIndex, move( pUploadHeaps[1] ), 1000 );

	// 1024���� �߰� ������ �Ҵ��Ͽ� ���ο� ��ü�� �����մϴ�.
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
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

	// ������ ���ε� ������ �����մϴ�.
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeaps[0] ) ) );
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeaps[1] ) ) );

	// �ּҸ� �����մϴ�.
	mStartAddress = pResource->GetGPUVirtualAddress();

	void* pUploadAddressBack[2] = { pUploadAddress[0], pUploadAddress[1] };
	HR( pUploadHeaps[0]->Map( 0, nullptr, &pUploadAddress[0] ) );
	HR( pUploadHeaps[1]->Map( 0, nullptr, &pUploadAddress[1] ) );

	// Ŀ�Ե��� ���� ���� ���ε� �����͸� �����մϴ�.
	if ( copyBack )
	{
		memcpy( pUploadAddress[0], pUploadAddressBack[0], mAlign * mCount );
		memcpy( pUploadAddress[1], pUploadAddressBack[1], mAlign * mCount );
	}

	// �� ť�� ����� ä��ϴ�.
	for ( uint64 i = 1; i < mAllocUnit; ++i )
	{
		mQueue.push( mCount + i );
	}

	auto index = mCount;
	mCount += 1024;
	return index;
}

void HeapAllocator::Initialize()
{
	App::Disposing += Dispose;

	mHeapValid = true;
}

void HeapAllocator::Commit( int frameIndex, CDeviceContext& deviceContext )
{
	auto pCommandList = deviceContext.pCommandList.Get();

	auto alignedHeapCount = mAlignedHeaps.size();
	auto largeHeapCount = mLargeHeaps.size();
	vector<D3D12_RESOURCE_BARRIER> barriers;
	bool isCommittable = false;

	barriers.reserve( alignedHeapCount + largeHeapCount );

	// �����͸� Ŀ���ؾ� �� �׸��� ���� ��� ���ҽ� �踮� �����մϴ�.
	for ( size_t i = 0; i < alignedHeapCount; ++i )
	{
		if ( mAlignedHeaps[i].IsCommittable( frameIndex ) && mAlignedHeaps[i].pResource )
		{
			D3D12_RESOURCE_BARRIER barrier{ };
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = mAlignedHeaps[i].pResource.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

			barriers.push_back( barrier );
			isCommittable = true;
		}
	}

	for ( auto& i : mLargeHeaps )
	{
		if ( i->IsCommittable( frameIndex ) )
		{
			D3D12_RESOURCE_BARRIER barrier{ };
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = i->pResource.Get();
			barrier.Transition.StateBefore = i->mInitialState;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			
			barriers.push_back( barrier );
			isCommittable = true;
		}
	}

	auto barriersCount = barriers.size();

	if ( isCommittable )
	{
		pCommandList->ResourceBarrier( ( UINT )barriers.size(), barriers.data() );

		for ( size_t i = 0; i < alignedHeapCount; ++i )
		{
			// �����͸� Ŀ���մϴ�.
			mAlignedHeaps[i].Commit( frameIndex, deviceContext );
		}

		for ( auto& i : mLargeHeaps )
		{
			// �����͸� Ŀ���մϴ�.
			i->Commit( frameIndex, deviceContext );
		}

		// �������� �踮�� ���¸� ������� �����ϴ�.
		for ( size_t i = 0; i < barriersCount; ++i )
		{
			swap( barriers[i].Transition.StateBefore, barriers[i].Transition.StateAfter );
		}

		pCommandList->ResourceBarrier( ( UINT )barriersCount, barriers.data() );
	}
}

ComPtr<Heap> HeapAllocator::Alloc( uint64 sizeInBytes )
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

	return new Heap( &mAlignedHeaps[alignedIndex], mAlignedHeaps[alignedIndex].Alloc() );
}

void HeapAllocator::LinkLargeHeap( LargeHeap* pLargeHeap )
{
	if ( mHeapValid )
	{
		mLargeHeaps.insert( pLargeHeap );
	}
}

void HeapAllocator::UnlinkLargeHeap( LargeHeap* pLargeHeap )
{
	if ( mHeapValid )
	{
		mLargeHeaps.erase( pLargeHeap );
	}
}

void HeapAllocator::Dispose( object sender )
{
	mHeapValid = false;
}