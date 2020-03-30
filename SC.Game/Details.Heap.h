#pragma once

namespace SC::Game::Details
{
	class Heap : public IUnknown
	{
		std::atomic<ULONG> mRefCount;

	public:
		AlignedHeap* mHeapRef;
		uint64 mIndex;

	public:
		Heap( AlignedHeap* pHeapRef, uint64 index );
		virtual ~Heap();

		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject ) override;
		ULONG STDMETHODCALLTYPE AddRef() override;
		ULONG STDMETHODCALLTYPE Release() override;

		void* Map();
		void Unmap();
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();
	};
}