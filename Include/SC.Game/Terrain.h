#pragma once

namespace SC::Game
{
	/// <summary> ������ ǥ���մϴ�. </summary>
	class Terrain : public Component
	{
		friend class Details::GameLogic;
		friend class TerrainCollider;

		int mResolution = 1000;
		RefPtr<Mesh> mBaseMesh;
		sc_game_export_object( RefPtr<Details::CBuffer> ) mVertexBuffer;
		RefPtr<Game::HeightMap> mHeightMap;
		RefPtr<Material> mMaterial;

		bool mUpdateTerrain = false;

	protected:
		void Bake( RefPtr<Details::CDeviceContext>& deviceContext, int frameIndex );
		void Render( RefPtr<Details::CDeviceContext>& deviceContext, int frameIndex ) override;

	public:
		/// <summary> <see cref="Terrain"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		Terrain();
		~Terrain() override;

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ������Ʈ�� ��ü�� ����� �� ȣ��˴ϴ�. </summary>
		void Awake() override;

		/// <summary> (Visual Studio ����) ���� ���̸� �ؽ�ó�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( RefPtr<Game::HeightMap>, HeightMap );

		/// <summary> ���� ���̸� �ؽ�ó�� �����ɴϴ�. </summary>
		RefPtr<Game::HeightMap> HeightMap_get();

		/// <summary> ���� ���̸� �ؽ�ó�� �����մϴ�. </summary>
		void HeightMap_set( RefPtr<Game::HeightMap> value );

		/// <summary> (Visual Studio ����) ������ �����ϰų� �����ɴϴ�. </summary>
		vs_property( RefPtr<Game::Material>, Material );

		/// <summary> ������ �����ɴϴ�. </summary>
		RefPtr<Game::Material> Material_get();

		/// <summary> ������ �����մϴ�. </summary>
		void Material_set( RefPtr<Game::Material> value );
	};
}