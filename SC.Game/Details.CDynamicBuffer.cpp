using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

CDynamicBuffer::CDynamicBuffer( RefPtr<HeapAllocator> pAllocator, int indexOf, D3D12_GPU_VIRTUAL_ADDRESS virtualAddress, void* pBlock )
	: indexOf( indexOf )
	, pAllocator( pAllocator )
	, VirtualAddress( virtualAddress )
	, pBlock( pBlock )
{
	mRefCount = 0;
}

CDynamicBuffer::CDynamicBuffer( ComPtr<ID3D12Resource>&& pResource )
	: indexOf( -1 )
	, pResource( move( pResource ) )
{
	HR( this->pResource->Map( 0, nullptr, &pBlock ) );
	VirtualAddress = this->pResource->GetGPUVirtualAddress();
}

CDynamicBuffer::~CDynamicBuffer()
{
	if ( indexOf != -1 )
		pAllocator->Free( indexOf );
}

HRESULT STDMETHODCALLTYPE CDynamicBuffer::QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject )
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

ULONG STDMETHODCALLTYPE CDynamicBuffer::AddRef()
{
	return mRefCount.fetch_add( 1 ) + 1;
}

ULONG STDMETHODCALLTYPE CDynamicBuffer::Release()
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