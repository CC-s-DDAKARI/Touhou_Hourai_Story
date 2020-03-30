using namespace SC;
using namespace SC::Game::Details;

Heap::Heap( AlignedHeap* pHeapRef, uint64 index )
{
	mRefCount = 0;

	mHeapRef = pHeapRef;
	mIndex = index;
}

Heap::~Heap()
{
	if ( mHeapRef && mIndex )
	{
		mHeapRef->Free( mIndex );
	}
}

HRESULT STDMETHODCALLTYPE Heap::QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject )
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

ULONG STDMETHODCALLTYPE Heap::AddRef()
{
	return mRefCount.fetch_add( 1 ) + 1;
}

ULONG STDMETHODCALLTYPE Heap::Release()
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

void* STDMETHODCALLTYPE Heap::Map()
{
	return mHeapRef->GetUploadAddress( App::mFrameIndex, mIndex );
}

void STDMETHODCALLTYPE Heap::Unmap()
{
	D3D12_RANGE range;
	range.Begin = mIndex * mHeapRef->mAlign;
	range.End = range.Begin + mHeapRef->mAlign;
	mHeapRef->AddCopyRange( App::mFrameIndex, range );
}

D3D12_GPU_VIRTUAL_ADDRESS Heap::GetGPUVirtualAddress()
{
	return mHeapRef->GetVirtualAddress( mIndex );
}