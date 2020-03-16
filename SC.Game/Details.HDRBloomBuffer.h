#pragma once

namespace SC::Game::Details
{
	class HDRBloomBuffer : virtual public Object
	{
		RefPtr<CDevice>& deviceRef;

		ComPtr<ID3D12Resource> pBloomBuffer;
		ComPtr<ID3D12Resource> pBlurTemp;
		Drawing::Point<int> halfSize;

		RefPtr<CUnorderedAccessView> pUAVs[2];
		RefPtr<CShaderResourceView> pSRV;

	public:
		HDRBloomBuffer( RefPtr<CDevice>& device );

		void SetGraphicsRootShaderResourceView( RefPtr<Details::CDeviceContext>& deviceContext );
		void Compute( RefPtr<Details::CDeviceContext>& deviceContext, int pass );
		void ResizeBuffers( uint32 width, uint32 height );

		const int NumPass = 3;
	};
}