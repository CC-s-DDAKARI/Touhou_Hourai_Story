#pragma once

namespace SC::Game
{
	/// <summary> 2���� �ؽ�ó �����͸� ǥ���մϴ�. </summary>
	class Texture2D : public Assets
	{
		friend class Material;
		friend class Details::GameLogic;
		friend class UI::Image;

		sc_game_export_object( ComPtr<ID3D12Resource> ) pTexture2D;
		sc_game_export_object( RefPtr<Details::CShaderResourceView> ) pShaderResourceView;

		sc_game_export_object( ComPtr<ID3D12CommandAllocator> ) pUploadCommands;
		sc_game_export_object( ComPtr<ID3D12Resource> ) pUploadHeap;
		uint64 uploadFenceValue = 0;

		bool copySuccessFlag = false;

		uint32 width;
		uint32 height;

		bool Lock( RefPtr<Details::CDeviceContext>& deviceContext );
		
		vs_property_get( bool, IsValid );
		bool IsValid_get();
		
	public:
		~Texture2D() override;

		/// <summary> <see cref="Texture2D"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="name"> ������ �ڻ� �̸��� �����մϴ�. </param>
		/// <param name="filepath"> �̹��� ���� ��θ� �����մϴ�. </param>
		/// <param name="queueIndex"> ���� �� Ʈ������ ����� ������ ť ��ȣ�� �����մϴ�. �Ϲ������� 0�� ����մϴ�. </param>
		Texture2D( String name, const std::filesystem::path& filepath, int queueIndex = 0 );

		/// <summary> <see cref="Texture2D"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="name"> ������ �ڻ� �̸��� �����մϴ�. </param>
		/// <param name="textureData"> �ؽ�ó �����͸� �����մϴ�. </param>
		/// <param name="sizeInBytes"> �ؽ�ó �������� ũ�⸦ �����մϴ�. </param>
		/// <param name="queueIndex"> ���� �� Ʈ������ ����� ������ ť ��ȣ�� �����մϴ�. �Ϲ������� 0�� ����մϴ�. </param>
		Texture2D( String name, void* textureData, uint32 sizeInBytes, int queueIndex = 0 );

		/// <summary> (Visual Studio ����) �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		vs_property_get( uint32, Width );

		/// <summary> �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		uint32 Width_get();

		/// <summary> (Visual Studio ����) �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		vs_property_get( uint32, Height );

		/// <summary> �ؽ�ó ��ü�� �ȼ� ���̸� �����ɴϴ�. </summary>
		uint32 Height_get();

	private:
		void InitializeFrom( sc_game_export_object( IWICBitmapDecoder* ) pDecoder, int queueIndex );
	};
}