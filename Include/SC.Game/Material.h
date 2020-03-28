#pragma once

namespace SC::Game
{
	/// <summary> ������ ������ ���� ������ �⺻ �Լ��� �����մϴ�. �� Ŭ������ ��ӵ� �� �����ϴ�. </summary>
	class Material : public Assets
	{
		friend class MeshRenderer;
		friend class SkinnedMeshRenderer;
		friend class Details::GameLogic;
		friend class Terrain;

#pragma pack( push, 4 )
		struct Constants
		{
			int Index;
			int DiffuseMap;
			int NormalMap;
			int AlphaClip;
			sc_game_export_float( XMFLOAT4X4, 16 ) TexWorld;
		};

		struct Reflection
		{
			float Ambient;
			float Diffuse;
			float Specular;
			float SpecExp;
		};
#pragma pack( pop )

		static sc_game_export_object( ComPtr<ID3D12Resource> ) pReflectionBuffer;
		static Reflection* reflectionBufferPtr;
		static sc_game_export_object( ComPtr<Details::CShaderResourceView> ) pShaderResourceView;
		static int capacity;
		static int reference_count;
		static std::vector<bool> locked;
		static sc_game_export_object( ComPtr<Details::CShaderResourceView> ) pNullSRV;

		sc_game_export_object( ComPtr<Details::CDynamicBuffer> ) constantBuffer;
		int lockIndex = 0;

		Reflection frameResourceReflection;
		Constants frameResourceConstants;

		RefPtr<Texture2D> diffuseMap;
		RefPtr<Texture2D> diffuseLayerMap;
		RefPtr<Texture2D> normalMap;
		RenderQueueLayer layer = RenderQueueLayer::Default;

		void SetGraphicsRootConstantBuffers( sc_game_export_object( RefPtr<Details::CDeviceContext> )& deviceContext );
		static void SetGraphicsRootShaderResources( sc_game_export_object( RefPtr<Details::CDeviceContext> )& deviceContext );

	public:
		/// <summary> <see cref="Material"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="name"> ������ �ڻ��� �ĺ��� �̸��� �����մϴ�. </param>
		Material( String name );
		~Material() override;

		/// <summary> (Visual Studio ����) ǥ���� �ֺ��� �ݻ� ������ �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, Ambient );

		/// <summary> ǥ���� �ֺ��� �ݻ� ������ �����ɴϴ�. </summary>
		double Ambient_get();

		/// <summary> ǥ���� �ֺ��� �ݻ� ������ �����մϴ�. </summary>
		void Ambient_set( double value );

		/// <summary> (Visual Studio ����) ǥ���� �л걤 �ݻ� ������ �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, Diffuse );

		/// <summary> ǥ���� �л걤 �ݻ� ������ �����ɴϴ�. </summary>
		double Diffuse_get();

		/// <summary> ǥ���� �л걤 �ݻ� ������ �����մϴ�. </summary>
		void Diffuse_set( double value );

		/// <summary> (Visual Studio ����) ǥ���� �ݿ��� �ݻ� ������ �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, Specular );

		/// <summary> ǥ���� �ݿ��� �ݻ� ������ �����ɴϴ�. </summary>
		double Specular_get();

		/// <summary> ǥ���� �ݿ��� �ݻ� ������ �����մϴ�. </summary>
		void Specular_set( double value );

		/// <summary> (Visual Studio ����) ǥ���� �ݿ��� �ݻ� ���⸦ �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, SpecExp );

		/// <summary> ǥ���� �ݿ��� �ݻ� ���⸦ �����ɴϴ�. </summary>
		double SpecExp_get();

		/// <summary> ǥ���� �ݿ��� �ݻ� ���⸦ �����մϴ�. </summary>
		void SpecExp_set( double value );

		/// <summary> (Visual Studio ����) ǥ���� �л걤 �ݻ� ������ ��Ÿ���� �ؽ�ó ��ü�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( RefPtr<Texture2D>, DiffuseMap );

		/// <summary> ǥ���� �л걤 �ݻ� ������ ��Ÿ���� �ؽ�ó ��ü�� �����ɴϴ�. </summary>
		RefPtr<Texture2D> DiffuseMap_get();

		/// <summary> ǥ���� �л걤 �ݻ� ������ ��Ÿ���� �ؽ�ó ��ü�� �����մϴ�. </summary>
		void DiffuseMap_set( RefPtr<Texture2D> value );

		/// <summary> (Visual Studio ����) ǥ���� �л걤 �ݻ� ������ ���̾ ��Ÿ���� �ؽ�ó ��ü�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( RefPtr<Texture2D>, DiffuseLayerMap );

		/// <summary> ǥ���� �л걤 �ݻ� ������ ���̾ ��Ÿ���� �ؽ�ó ��ü�� �����ɴϴ�. </summary>
		RefPtr<Texture2D> DiffuseLayerMap_get();

		/// <summary> ǥ���� �л걤 �ݻ� ������ ���̾ ��Ÿ���� �ؽ�ó ��ü�� �����մϴ�. </summary>
		void DiffuseLayerMap_set( RefPtr<Texture2D> value );

		/// <summary> (Visual Studio ����) ǥ���� ������ �������� ��Ÿ���� �ؽ�ó ��ü�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( RefPtr<Texture2D>, NormalMap );

		/// <summary> ǥ���� ������ �������� ��Ÿ���� �ؽ�ó ��ü�� �����ɴϴ�. </summary>
		RefPtr<Texture2D> NormalMap_get();

		/// <summary> ǥ���� ������ �������� ��Ÿ���� �ؽ�ó ��ü�� �����մϴ�. </summary>
		void NormalMap_set( RefPtr<Texture2D> value );

		/// <summary> (Visual Studio ����) ������ ���� ť ������ �����ϰų� �����ɴϴ�. </summary>
		vs_property( RenderQueueLayer, Layer );

		/// <summary> ������ ���� ť ������ �����ɴϴ�. </summary>
		RenderQueueLayer Layer_get();

		/// <summary> ������ ���� ť ������ �����մϴ�. </summary>
		void Layer_set( RenderQueueLayer value );

		/// <summary> (Visual Studio ����) �ؽ�ó ���� ��ġ�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector2, TexLocation );

		/// <summary> �ؽ�ó ���� ��ġ�� �����ɴϴ�. </summary>
		Vector2 TexLocation_get();

		/// <summary> �ؽ�ó ���� ��ġ�� �����մϴ�. </summary>
		void TexLocation_set( Vector2 value );

		/// <summary> (Visual Studio ����) �ؽ�ó ���� ũ�⸦ �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector2, TexScale );

		/// <summary> �ؽ�ó ���� ũ�⸦ �����ɴϴ�. </summary>
		Vector2 TexScale_get();

		/// <summary> �ؽ�ó ���� ũ�⸦ �����մϴ�. </summary>
		void TexScale_set( Vector2 value );

	private:
		static int Lock( Material* ptr );
		static int Realloc( int capacity );
	};
}