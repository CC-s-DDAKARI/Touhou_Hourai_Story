#pragma once

namespace SC::Game
{
	/// <summary> 높이를 표현하는 2차원 텍스처 데이터를 표현합니다. </summary>
	class HeightMap : public Assets
	{
		friend class Terrain;
		friend class TerrainCollider;

		struct Sample
		{
			int Height : 16;
			int MaterialIndex : 7;
			int TestFlag : 1;
			int MaterialIndex1 : 7;
			int Unused : 1;
		};

		sc_game_export_object( ComPtr<ID3D12Resource> ) pTexture2D;
		sc_game_export_object( RefPtr<Details::CShaderResourceView> ) pShaderResourceView;

		sc_game_export_object( ComPtr<ID3D12CommandAllocator> ) pUploadCommands;
		sc_game_export_object( ComPtr<ID3D12Resource> ) pUploadHeap;
		uint64 uploadFenceValue = 0;

		bool copySuccessFlag = false;
		int queueIndex = 0;

		std::vector<Sample> mHeightBuffer;
		uint32 width;
		uint32 height;

		bool Lock( RefPtr<Details::CDeviceContext>& deviceContext );

		vs_property_get( bool, IsValid );
		bool IsValid_get();

	public:
		/// <summary> <see cref="HeightMap"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		/// <param name="name"> 데이터 자산 이름을 제공합니다. </param>
		/// <param name="filepath"> RAW 형식의 이미지 파일 경로를 전달합니다. </param>
		/// <param name="width"> 이미지의 넓이를 전달합니다. </param>
		/// <param name="height"> 이미지의 높이를 전달합니다. </param>
		/// <param name="queueIndex"> 복사 및 트랜지션 명령을 실행할 큐 번호를 전달합니다. 일반적으로 0을 사용합니다. </param>
		HeightMap( String name, const std::filesystem::path& filepath, uint32 width, uint32 height, int queueIndex = 0 );
		~HeightMap() override;

		/// <summary> (Visual Studio 전용) 텍스처 개체의 픽셀 넓이를 가져옵니다. </summary>
		vs_property_get( uint32, Width );

		/// <summary> 텍스처 개체의 픽셀 넓이를 가져옵니다. </summary>
		uint32 Width_get();

		/// <summary> (Visual Studio 전용) 텍스처 개체의 픽셀 높이를 가져옵니다. </summary>
		vs_property_get( uint32, Height );

		/// <summary> 텍스처 개체의 픽셀 높이를 가져옵니다. </summary>
		uint32 Height_get();

	private:
		void InitializeFrom( std::vector<uint8>& buffer, int queueIndex );
	};
}