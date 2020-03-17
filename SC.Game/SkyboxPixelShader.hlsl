#include "RenderingShaderCommon.hlsli"

struct Fragment
{
	float4 PosH : SV_Position;
	float2 Tex : TEXCOORD;
};

struct Pixel
{
	float4 Color : SV_Target0;
};

Pixel main( Fragment frag )
{
	Pixel px;
	px.Color = gDiffuseMap0.Sample( gSampler, frag.Tex );
	return px;
}