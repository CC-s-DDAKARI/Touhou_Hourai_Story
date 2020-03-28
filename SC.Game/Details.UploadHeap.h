#pragma once

namespace SC::Game::Details
{
	class UploadHeap : public IUnknown
	{
		std::atomic<ULONG> mRefCount;

	public:
		UploadAlignedHeap* mHeapRef;
		uint64 mIndex;

	public:
		UploadHeap( UploadAlignedHeap* pHeapRef, uint64 index );
		virtual ~UploadHeap();

		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject ) override;
		ULONG STDMETHODCALLTYPE AddRef() override;
		ULONG STDMETHODCALLTYPE Release() override;

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();
		void* GetCPUBlock();
	};
}