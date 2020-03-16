#pragma once

namespace SC::Game::Details
{
	class HeapAllocator : virtual public Object
	{
		const int alignOf;
		const int heapCount;

		ComPtr<ID3D12Device> pDevice;

		std::vector<ComPtr<ID3D12Resource>> pDynamicBuffers;
		std::vector<D3D12_GPU_VIRTUAL_ADDRESS> virtualAddress;
		std::vector<char*> pBlock;

		std::mutex locker;
		std::vector<std::queue<int>> allocQueue;

	public:
		HeapAllocator( CDevice* device, int alignOf = 512, int heapCount = 2048 );  // 1MB

		int Alloc( D3D12_GPU_VIRTUAL_ADDRESS& virtualAddress, void*& pBlock );
		void Free( int index );

	private:
		int SearchIndex();
	};
}