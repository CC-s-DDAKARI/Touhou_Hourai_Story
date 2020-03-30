#pragma once

namespace SC::Game::Details
{
	class LargeHeap : public IUnknown
	{
		std::atomic<ULONG> mRefCount;
		uint64 mSizeInBytes;

	public:
		ComPtr<ID3D12Resource> pResource;
		ComPtr<ID3D12Resource> pUploadHeaps[2];

		D3D12_GPU_VIRTUAL_ADDRESS mStartAddress;
		void* pUploadAddress[2];
		uint64 mWriteRange[2]{ };
		D3D12_RESOURCE_STATES mInitialState;

	public:
		LargeHeap( D3D12_RESOURCE_STATES initialState, uint64 sizeInBytes );
		virtual ~LargeHeap();

		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, _COM_Outptr_ void** ppvObject ) override;
		ULONG STDMETHODCALLTYPE AddRef() override;
		ULONG STDMETHODCALLTYPE Release() override;

		void* Map();
		void Unmap( uint64 writeRange = std::numeric_limits<uint64>::max() );
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

		bool IsCommittable( int frameIndex );
		void Commit( int frameIndex, CDeviceContext& deviceContext );
	};
}