RWTexture2D<float4> gInput : register( u0 );
RWTexture2D<float4> gTemp : register( u1 );

static float Weights[7] = { 0.003f, 0.048f, 0.262f, 0.415f, 0.262f, 0.048f, 0.003f };

[numthreads( 32, 32, 1 )]
void main( uint3 tid : SV_DispatchThreadID )
{
	float4 color = 0;

	[unroll] for ( uint i = 0; i < 7; ++i )
	{
		color += gTemp[( int2 )tid.xy + int2( ( int )i - 3, 0 )];
	}

	gInput[tid.xy] = color;
}