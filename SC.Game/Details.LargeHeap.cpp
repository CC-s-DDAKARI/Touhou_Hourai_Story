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

void LargeHeap::Unmap( uint64 writeRange )
{
	if ( writeRange > mSizeInBytes ) writeRange = mSizeInBytes;
	mWriteRange[App::mFrameIndex] = writeRange;
}

D3D12_GPU_VIRTUAL_ADDRESS LargeHeap::GetGPUVirtualAddress()
{
	return pResource->GetGPUVirtualAddress();
}

bool LargeHeap::IsCommittable( int frameIndex )
{
	return mWriteRange[frameIndex];
}

void LargeHeap::Commit( int frameIndex, CDeviceContext& deviceContext )
{
	if ( mWriteRange[frameIndex] > 0 )
	{
		auto pCommandList = deviceContext.pCommandList.Get();

		if ( mWriteRange[frameIndex] == mSizeInBytes )
		{
			// 단순 복사 명령을 수행합니다.
			pCommandList->CopyResource( pResource.Get(), pUploadHeaps[frameIndex].Get() );
		}
		else
		{
			// 영역 복사 명령을 수행합니다.
			pCommandList->CopyBufferRegion( pResource.Get(), 0, pUploadHeaps[frameIndex].Get(), 0, mWriteRange[frameIndex] );
		}

		mWriteRange[frameIndex] = 0;
	}
}