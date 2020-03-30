#pragma once

namespace SC::Game::Details
{
	class AlignedHeap
	{
		static constexpr const uint64 mAllocUnit = 256;

	public:
		ComPtr<ID3D12Resource> pResource;
		ComPtr<ID3D12Resource> pUploadHeaps[2];

		D3D12_GPU_VIRTUAL_ADDRESS mStartAddress;
		uint64 mAlign;
		uint64 mCount;
		std::queue<uint64> mQueue;
		std::mutex mLocker;

		std::queue<D3D12_RANGE> mCopyRangeQueue[2];

		void* pUploadAddress[2];

	public:
		AlignedHeap( uint64 alignment );
		AlignedHeap( AlignedHeap&& mov );

		uint64 Alloc();
		void Free( uint64 index );
		void Commit( int frameIndex, CDeviceContext& deviceContext );
		bool IsCommittable( int frameIndex );

		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress( uint64 index );
		void* GetUploadAddress( int frameIndex, uint64 index );
		void AddCopyRange( int frameIndex, const D3D12_RANGE& range );

	private:
		uint64 Expand();
	};

	class HeapAllocator abstract
	{
		static std::vector<AlignedHeap> mAlignedHeaps;
		static std::set<LargeHeap*> mLargeHeaps;

	public:
		static bool mHeapValid;

	public:
		static void Initialize();
		static void Commit( int frameIndex, CDeviceContext& deviceContext );
		static ComPtr<Heap> Alloc( uint64 sizeInBytes );
		static void LinkLargeHeap( LargeHeap* pLargeHeap );
		static void UnlinkLargeHeap( LargeHeap* pLargeHeap );

	private:
		static void Dispose( object sender );
	};
}