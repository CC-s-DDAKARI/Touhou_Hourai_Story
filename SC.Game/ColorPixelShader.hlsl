#include "RenderingShaderCommon.hlsli"

struct Fragment
{
	float4 PosH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct Pixel
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	uint Material : SV_Target2;
};

Pixel main( Fragment pIn )
{
	Pixel px;

	float4 vertexColor = pIn.Color;
	float4 diffuse0 = gDiffuseMap0.Sample( gSampler, pIn.Tex );
	float4 color = lerp( vertexColor, diffuse0, ( float )min( gMaterial.DiffuseMap, 1 ) );

	if ( gMaterial.DiffuseMap == 2 )
	{
		color.xyz += gDiffuseMap1.Sample( gSampler, pIn.Tex ).xyz;
	}

	if ( true || gMaterial.AlphaClip )
		clip( color.a - 0.9f );

	float3 normal = normalize( pIn.Normal );

	if ( gMaterial.NormalMap )
	{
		float3 bump = gNormalMap0.Sample( gSampler, pIn.Tex ).xyz;
		normal = NormalSampleToWorldSpace( bump, normal, normalize( pIn.Tangent ) );
	}

	px.Color = color;
	px.Normal = float4( ( normal + 1.0f ) * 0.5f, 1.0f );
	px.Material = gMaterial.Index;
	return px;
}