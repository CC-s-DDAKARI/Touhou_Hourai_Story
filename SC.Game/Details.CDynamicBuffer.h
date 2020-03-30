#pragma once

namespace SC::Game::Details
{
	class CDynamicBuffer : public IUnknown
	{
		std::atomic<ULONG> mRefCount;

		int indexOf = -1;
		RefPtr<HeapAllocator1> pAllocator;

	public:
		CDynamicBuffer( RefPtr<HeapAllocator1> pAllocator, int indexOf, D3D12_GPU_VIRTUAL_ADDRESS virtualAddress, void* pBlock );
		CDynamicBuffer( ComPtr<ID3D12Resource>&& pResource );
		~CDynamicBuffer();

		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject ) override;
		ULONG STDMETHODCALLTYPE AddRef() override;
		ULONG STDMETHODCALLTYPE Release() override;

		ComPtr<ID3D12Resource> pResource;
		D3D12_GPU_VIRTUAL_ADDRESS VirtualAddress;
		void* pBlock;
	};
}