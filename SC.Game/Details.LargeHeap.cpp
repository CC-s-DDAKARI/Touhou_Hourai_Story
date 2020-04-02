using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

LargeHeap::LargeHeap( D3D12_RESOURCE_STATES initialState, uint64 sizeInBytes )
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = sizeInBytes;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc = { 1, 0 };
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HR( pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialState,
		nullptr,
		IID_PPV_ARGS( &pResource )
		) );

	// 데이터 업로드 공간을 생성합니다.
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeaps[0] ) ) );
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeaps[1] ) ) );

	// 주소를 매핑합니다.
	HR( pUploadHeaps[0]->Map( 0, nullptr, &pUploadAddress[0] ) );
	HR( pUploadHeaps[1]->Map( 0, nullptr, &pUploadAddress[1] ) );

	mInitialState = initialState;
	mSizeInBytes = sizeInBytes;

	HeapAllocator::LinkLargeHeap( this );
}

LargeHeap::~LargeHeap()
{
	HeapAllocator::UnlinkLargeHeap( this );
}

HRESULT STDMETHODCALLTYPE LargeHeap::QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject )
{
	if ( riid == IID_IUnknown )
	{
		*ppvObject = dynamic_cast< IUnknown* >( this );
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE LargeHeap::AddRef()
{
	return mRefCount.fetch_add( 1 ) + 1;
}

ULONG STDMETHODCALLTYPE LargeHeap::Release()
{
	auto c = mRefCount.fetch_sub( 1 ) - 1;
	if ( c == 0 )
	{
		delete this;
		return 0;
	}
	else
	{
		return c;
	}
}

void* LargeHeap::Map()
{
	return pUploadAddress[App::mFrameIndex];
}

void LargeHeap::Unmap( const D3D12_RANGE& range )
{
	mWriteRanges[App::mFrameIndex].insert( { range.Begin, range.End } );
}

void LargeHeap::Unmap()
{
	Unmap( { 0, mSizeInBytes } );
}

D3D12_GPU_VIRTUAL_ADDRESS LargeHeap::GetGPUVirtualAddress()
{
	return pResource->GetGPUVirtualAddress();
}

bool LargeHeap::IsCommittable( int frameIndex )
{
	return mWriteRanges[frameIndex].size();
}

void LargeHeap::Commit( int frameIndex, CDeviceContext& deviceContext )
{
	auto pCommandList = deviceContext.pCommandList.Get();

	if ( IsCommittable( frameIndex ) )
	{
		// 병합 가능한 범위를 병합합니다.
		auto& ranges = mWriteRanges[frameIndex];

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