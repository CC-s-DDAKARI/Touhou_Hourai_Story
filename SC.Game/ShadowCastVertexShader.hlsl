#include "RenderingShaderCommon.hlsli"

struct Fragment
{
	float4 PosH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
};

Fragment main( Vertex vIn )
{
	Fragment fg;

	fg.PosH = mul( float4( vIn.Pos, 1.0f ), mul( gTransform.World, gLight.ViewProj ) );
	fg.Tex = vIn.Tex;
	fg.Color = vIn.Color;

	return fg;
}