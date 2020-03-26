#pragma once

namespace SC::Game::Details
{
	class DefaultHeapAllocator : virtual public Object
	{
		const int mAlignOf;
		const int mHeapCount;

		ComPtr<ID3D12Device> mDevice;

		std::vector<ComPtr<ID3D12Resource>> mBuffers;
		std::vector<D3D12_GPU_VIRTUAL_ADDRESS> mVirtualAddresses;

		std::mutex mLocker;
		std::vector<std::queue<int>> mAllocQueue;

	public:
		DefaultHeapAllocator( CDevice* device, int alignOf = 512, int heapCount = 2048 );  // 1MB

		int Alloc( D3D12_GPU_VIRTUAL_ADDRESS& virtualAddress );
		void Free( int index );

	private:
		int SearchIndex();
	};
}