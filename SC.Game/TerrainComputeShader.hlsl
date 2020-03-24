#include "Vertex.hlsli"

struct Constants
{
	uint GridX;
	uint GridY;
};

RWStructuredBuffer<Vertex> gVertexBuffer : register( u0 );
Texture2D gHeightMap : register( t0 );
StructuredBuffer<Vertex> gVertexBufferBase : register( t1 );
ConstantBuffer<Constants> gConstants : register( b0 );
SamplerState gSampler : register( s0 );

uint GetDim( int2 xy, int xCount, int yCount )
{
	int2 v = clamp( xy, int2( 0, 0 ), int2( xCount, yCount ) );
	return v.y * xCount + v.x;
}

[numthreads( 32, 32, 1 )]
void main( uint3 tid : SV_DispatchThreadID )
{
	uint xCount = gConstants.GridX + 1;
	uint yCount = gConstants.GridY + 1;

	if ( tid.x < xCount && tid.y < yCount )
	{		
		float Sxy = 2.0f / gConstants.GridX;
		
		int2 samples[5] = { int2( -1, 0 ), int2( 0, -1 ), int2( 1, 0 ), int2( 0, 1 ), int2( 0, 0 ) };
		Vertex v[5];
		float heights[5];
		
		[unroll]
		for ( uint i = 0; i < 5; ++i )
		{
			samples[i] += ( int2 )tid.xy;
			v[i] = gVertexBufferBase[GetDim( samples[i], xCount, yCount )];
			heights[i] = gHeightMap.SampleLevel( gSampler, v[i].Tex, 0 ).r;
		}
		
		float3 normal;
		normal.x = -( heights[2] - heights[0] );
		normal.y = 2.0f * Sxy;
		normal.z = ( heights[3] - heights[1] );
		normal = normalize( normal );
		
		float3 tangent = normalize( float3( Sxy * 2.0f, heights[2] - heights[0], 0 ) );
		
		v[4].Pos.y = heights[4];
		v[4].Normal = normal;
		v[4].Tangent = tangent;
		gVertexBuffer[GetDim( samples[4], xCount, yCount )] = v[4];
	}
}