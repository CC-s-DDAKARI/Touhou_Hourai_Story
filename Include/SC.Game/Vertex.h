#pragma once

namespace SC::Game
{
#pragma pack( push, 4 )
	/// <summary> 단일 정점을 표현합니다. </summary>
	struct Vertex
	{
		sc_game_export_float( XMFLOAT3, 3 ) Pos;
		sc_game_export_float( XMFLOAT4, 4 ) Color;
		sc_game_export_float( XMFLOAT2, 2 ) Tex;
		sc_game_export_float( XMFLOAT3, 3 ) Normal;
		sc_game_export_float( XMFLOAT3, 3 ) Tangent;

		bool operator==( const Vertex& copy ) const
		{
			return memcmp( this, &copy, sizeof( Vertex ) ) == 0;
		}

#if defined( __SC_GAME_EXPORT_SYMBOL__ )
		inline static D3D12_INPUT_ELEMENT_DESC InputElements[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA }
		};
#endif
	};

	/// <summary> 단일 스키닝 정점을 표현합니다. </summary>
	struct SkinnedVertex
	{
		sc_game_export_float( XMFLOAT3, 3 ) Pos;
		sc_game_export_float( XMFLOAT4, 4 ) Color;
		sc_game_export_float( XMFLOAT2, 2 ) Tex;
		sc_game_export_float( XMFLOAT3, 3 ) Normal;
		sc_game_export_float( XMFLOAT3, 3 ) Tangent;
		float Weights[3];
		uint16 Indices[4];

		bool operator==( const Vertex& copy ) const
		{
			return memcmp( this, &copy, sizeof( SkinnedVertex ) ) == 0;
		}
	};
#pragma pack( pop )
}