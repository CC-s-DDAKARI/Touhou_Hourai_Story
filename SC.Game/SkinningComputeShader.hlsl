#include "Vertex.hlsli"
#include "Transform.hlsli"

struct SkinnedVertexT
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

struct SkinningConstants
{
	int VertexCount;
};

StructuredBuffer<SkinnedVertexT> gSkinnedVertexBuffer : register( t0 );
StructuredBuffer<BoneTransform> gBoneTransform : register( t1 );
ConstantBuffer<SkinningConstants> gSkinningConstant : register( b0 );

RWStructuredBuffer<Vertex> gVertexBuffer : register( u0 );

[numthreads( 32, 1, 1 )]
void main( uint3 tid : SV_DispatchThreadID )
{
	SkinnedVertexT sv = gSkinnedVertexBuffer[tid.x];
	Vertex v;

	// 기본 정보는 바로 넘깁니다.
	v.Color = sv.Color;
	v.Tex = sv.Tex;

	// 뼈대 가중치는 최대 1로 한정합니다.
	float weights[4] = { sv.Weights.x, sv.Weights.y, sv.Weights.z, 0.0f };
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	// 뼈대 색인을 배열 형식으로 가져옵니다.
	uint indices[4];
	//UInt_Sep( sv.Indices.x, indices[0], indices[1] );
	//UInt_Sep( sv.Indices.y, indices[2], indices[3] );

	v.Pos = 0;
	v.Normal = 0;
	v.Tangent = 0;

	[unroll] for ( uint i = 0; i < 4; ++i )
	{
		matrix bone = gBoneTransform[indices[i]].World;
		float weight = weights[i];

		v.Pos += mul( float4( sv.Pos.xyz, 1.0f ), bone ).xyz * weight;
		v.Normal += mul( sv.Normal, ( float3x3 )bone ) * weight;
		v.Tangent += mul( sv.Tangent, ( float3x3 )bone ) * weight;
	}

	gVertexBuffer[tid.x] = v;
}