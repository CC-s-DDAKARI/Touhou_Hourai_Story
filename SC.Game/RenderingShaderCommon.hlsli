#if !defined( __RENDERING_SHADER_COMMON_HLSLI__ )
#define __RENDERING_SHADER_COMMON_HLSLI__

#include "Camera.hlsli"
#include "Transform.hlsli"
#include "Vertex.hlsli"
#include "Light.hlsli"

struct Material_Constants
{
	int Index;
	int DiffuseMap;
	int NormalMap;
	int AlphaClip;
	row_major matrix TexWorld;
};

ConstantBuffer<Camera> gCamera : register( b0 );
ConstantBuffer<Transform> gTransform : register( b1 );
ConstantBuffer<Light> gLight : register( b2 );
ConstantBuffer<Material_Constants> gMaterial : register( b3 );

Texture2D<float4> gDiffuseMap0 : register( t0 );
Texture2D<float4> gDiffuseMap1 : register( t1 );
Texture2D<float4> gNormalMap0 : register( t2 );

SamplerState gSampler : register( s0 );

float3 NormalSampleToWorldSpace( float3 SampledValue, float3 NormalW, float3 TangentW )
{
	float3 normalT = 2.0f * SampledValue - 1.0f;
	float3 T = normalize( TangentW - dot( TangentW, NormalW ) * NormalW );
	float3 B = cross( NormalW, T );
	return normalize( mul( normalT, float3x3( T, B, NormalW ) ) );
}

#endif