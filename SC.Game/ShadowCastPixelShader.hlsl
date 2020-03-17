#include "RenderingShaderCommon.hlsli"

struct Fragment
{
	float4 PosH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
};

void main( Fragment pIn )
{
	float4 vertexColor = pIn.Color;
	float4 diffuse0 = gDiffuseMap0.Sample( gSampler, pIn.Tex );
	float4 color = lerp( vertexColor, diffuse0, ( float )min( gMaterial.DiffuseMap, 1 ) );

	if ( gMaterial.DiffuseMap == 2 )
	{
		color.xyz += gDiffuseMap1.Sample( gSampler, pIn.Tex ).xyz;
	}

	if ( true || gMaterial.AlphaClip )
		clip( color.a - 0.9f );
}