#pragma once

namespace SC::Game::Details
{
	class CView : public IUnknown
	{
		std::atomic<ULONG> mRefCount;

		RefPtr<ViewStorage> viewStorage;
		int lockIndex;

	public:
		CView( RefPtr<ViewStorage>& viewStorage, int lockIndex, D3D12_CPU_DESCRIPTOR_HANDLE handle );
		virtual ~CView();

		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject ) override;
		ULONG STDMETHODCALLTYPE AddRef() override;
		ULONG STDMETHODCALLTYPE Release() override;

		D3D12_CPU_DESCRIPTOR_HANDLE Handle;
	};
}