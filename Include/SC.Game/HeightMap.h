#pragma once

namespace SC::Game
{
	/// <summary> ���̸� ǥ���ϴ� 2���� �ؽ�ó �����͸� ǥ���մϴ�. </summary>
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
		/// <summary> <see cref="HeightMap"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="name"> ������ �ڻ� �̸��� �����մϴ�. </param>
		/// <param name="filepath"> RAW ������ �̹��� ���� ��θ� �����մϴ�. </param>
		/// <param name="width"> �̹����� ���̸� �����մϴ�. </param>
		/// <param name="height"> �̹����� ���̸� �����մϴ�. </param>
		/// <param name="queueIndex"> ���� �� Ʈ������ ����� ������ ť ��ȣ�� �����մϴ�. �Ϲ������� 0�� ����մϴ�. </param>
		HeightMap( String name, const std::filesystem::path& filepath, uint32 width, uint32 height, int queueIndex = 0 );
		~HeightMap() override;

		/// <summary> (Visual Studio ����) �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		vs_property_get( uint32, Width );

		/// <summary> �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		uint32 Width_get();

		/// <summary> (Visual Studio ����) �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		vs_property_get( uint32, Height );

		/// <summary> �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		uint32 Height_get();

	private:
		void InitializeFrom( std::vector<uint8>& buffer, int queueIndex );
	};
}