#pragma once

namespace SC::Game::Details
{
	class UploadAlignedHeap
	{
	public:
		ComPtr<ID3D12Resource> pResource;
		D3D12_GPU_VIRTUAL_ADDRESS mStartAddress;
		void* pStartAddress;
		uint64 mAlign;
		uint64 mCount;
		std::queue<uint64> mQueue;
		std::mutex mLocker;

		UploadAlignedHeap( uint64 alignment );
		UploadAlignedHeap( UploadAlignedHeap&& mov );

		uint64 Alloc();
		void Free( uint64 index );

		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress( uint64 index );
		void* GetCPUBlock( uint64 index );

	private:
		uint64 Expand();
	};

	class UploadHeapAllocator abstract
	{
		static std::vector<UploadAlignedHeap> mAlignedHeaps;

	public:
		static bool mHeapValid;

	public:
		static void Initialize();
		static ComPtr<UploadHeap> Alloc( uint64 sizeInBytes );

	private:
		static void Dispose( object sender );
	};
}