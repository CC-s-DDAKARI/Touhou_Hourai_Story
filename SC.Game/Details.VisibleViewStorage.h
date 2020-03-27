#pragma once

namespace SC::Game::Details
{
	class VisibleViewStorage : virtual public Object
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handleBase{ };
		D3D12_GPU_DESCRIPTOR_HANDLE handleBaseGpu{ };
		UINT stride = 0;

		int capacity;
		int lockId = 0;
		bool failure = false;

	public:
		VisibleViewStorage( int capacity = 256 );

		int Lock();
		void Reset();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle( int lockIndex );
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle( int lockIndex );

		ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;
	};
}