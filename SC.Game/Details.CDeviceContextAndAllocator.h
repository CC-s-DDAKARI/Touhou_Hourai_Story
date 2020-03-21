#pragma once

namespace SC::Game::Details
{
	class CDeviceContextAndAllocator: public CDeviceContext
	{
		ComPtr<ID3D12CommandAllocator> pAllocators[2];

	public:
		CDeviceContextAndAllocator( RefPtr<CDevice>& device, D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState* pInitialPipelineState = nullptr );
		~CDeviceContextAndAllocator() override;

		void Reset( CCommandQueue* pCommandQueue, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialPipelineState = nullptr ) override;
	};
}