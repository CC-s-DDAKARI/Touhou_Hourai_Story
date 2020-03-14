#include "Common.hlsli"
#include "Brush.hlsli"

struct Fragment
{
	float4 PosH : SV_Position;
	float2 Tex : TEXCOORD;
};

struct Pixel
{
	float4 Color : SV_Target0;
};

Texture2D gTexture : register( t0 );
ConstantBuffer<Resolution> gResolution : register( b0 );
ConstantBuffer<Resolution> gCursor : register( b1 );
ConstantBuffer<Brush> gBrush : register( b2 );

SamplerState gSamplerLinear : register( s0 );
SamplerState gSamplerPoint : register( s1 );

struct tag_RenderType
{
	uint Type;
	float Opacity;
};

ConstantBuffer<tag_RenderType> gRenderType : register( b3 );

Pixel main( Fragment fg )
{
	Pixel px;

	if ( gRenderType.Type == 0 )
	{
		px.Color = GetBrushColor( gBrush, gResolution, gCursor, fg.PosH.xy );
		px.Color.a *= gTexture[uint2( ( uint )fg.Tex.x, ( uint )fg.Tex.y )].a;
	}
	else if ( gRenderType.Type == 1 )
	{
		px.Color = GetBrushColor( gBrush, gResolution, gCursor, fg.PosH.xy );
	}
	else if ( gRenderType.Type == 2 )
	{
		px.Color = gTexture.Sample( gSamplerLinear, fg.Tex );
		px.Color.a *= gRenderType.Opacity;
	}
	else
	{
		px.Color = 0;
	}
	
	return px;
}