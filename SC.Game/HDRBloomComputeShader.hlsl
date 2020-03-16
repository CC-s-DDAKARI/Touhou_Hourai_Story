#include "HDR.hlsli"

RWTexture2D<float4> gHalfHDRBuffer : register( u0 );
ConstantBuffer<HDRConstants> gHDRConstants : register( b0 );

[numthreads( 32, 32, 1 )]
void main( uint3 tid : SV_DispatchThreadID )
{
	float4 color = gHalfHDRBuffer
}