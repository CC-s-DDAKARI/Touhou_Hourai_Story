#if !defined( __VERTEX_HLSLI__ )
#define __VERTEX_HLSLI__

struct Vertex
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct SkinnedVertex
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Weights : WEIGHTS;
	uint4 Indices : INDICES;
};

#endif

/*
#if !defined( __VERTEX_HLSLI__ )
#define __VERTEX_HLSLI__

struct Vertex
{
	float3 Pos;
	float4 Color;
	float2 Tex;
	float3 Normal;
	float3 Tangent;
};

struct SkinnedVertex
{
	float3 Pos;
	float4 Color;
	float2 Tex;
	float3 Normal;
	float3 Tangent;
	float3 Weights;
	uint2 Indices;
};

void UInt_Sep( uint value, out uint loword, out uint hiword )
{
	loword = ( value >> 16 ) & 0x0000FFFF;
	hiword = ( value ) & 0x0000FFFF;
}

#endif
*/