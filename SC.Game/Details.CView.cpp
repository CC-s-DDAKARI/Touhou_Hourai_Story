using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

CView::CView( RefPtr<ViewStorage>& viewStorage, int lockIndex, D3D12_CPU_DESCRIPTOR_HANDLE handle )
	: viewStorage( viewStorage )
	, lockIndex( lockIndex )
	, Handle( handle )
{
	mRefCount = 0;
}

CView::~CView()
{
	viewStorage->Free( lockIndex );
}

HRESULT STDMETHODCALLTYPE CView::QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject )
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

ULONG STDMETHODCALLTYPE CView::AddRef()
{
	return mRefCount.fetch_add( 1 ) + 1;
}

ULONG STDMETHODCALLTYPE CView::Release()
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