using namespace SC;
using namespace SC::Game::Details;

UploadHeap::UploadHeap( UploadAlignedHeap* pHeapRef, uint64 index )
{
	mRefCount = 0;

	mHeapRef = pHeapRef;
	mIndex = index;
}

UploadHeap::~UploadHeap()
{
	if ( mHeapRef && mIndex )
	{
		mHeapRef->Free( mIndex );
	}
}

HRESULT STDMETHODCALLTYPE UploadHeap::QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject )
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

ULONG STDMETHODCALLTYPE UploadHeap::AddRef()
{
	return mRefCount.fetch_add( 1 ) + 1;
}

ULONG STDMETHODCALLTYPE UploadHeap::Release()
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

D3D12_GPU_VIRTUAL_ADDRESS UploadHeap::GetGPUVirtualAddress()
{
	return mHeapRef->GetVirtualAddress( mIndex );
}

void* UploadHeap::GetCPUBlock()
{
	return mHeapRef->GetCPUBlock( mIndex );
}