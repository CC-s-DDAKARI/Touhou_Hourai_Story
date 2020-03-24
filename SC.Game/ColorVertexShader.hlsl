#include "RenderingShaderCommon.hlsli"

struct Fragment
{
	float4 PosH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

Fragment main( Vertex vIn )
{
	Fragment fg;
	fg.Color = vIn.Color;
	fg.Tex = mul( float4( vIn.Tex, 0, 1.0f ), gMaterial.TexWorld ).xy;

	fg.PosH = mul( float4( vIn.Pos, 1.0f ), mul( gTransform.World, gCamera.ViewProj ) );
	fg.Normal = mul( vIn.Normal, ( float3x3 )gTransform.WorldInvTranspose );
	fg.Tangent = mul( vIn.Tangent, ( float3x3 )gTransform.World );

	return fg;
}