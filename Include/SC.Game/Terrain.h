#pragma once

namespace SC::Game
{
	/// <summary> 지형을 표현합니다. </summary>
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
		/// <summary> <see cref="Terrain"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		Terrain();
		~Terrain() override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 컴포넌트가 개체와 연결될 때 호출됩니다. </summary>
		void Awake() override;

		/// <summary> (Visual Studio 전용) 지형 높이맵 텍스처를 설정하거나 가져옵니다. </summary>
		vs_property( RefPtr<Game::HeightMap>, HeightMap );

		/// <summary> 지형 높이맵 텍스처를 가져옵니다. </summary>
		RefPtr<Game::HeightMap> HeightMap_get();

		/// <summary> 지형 높이맵 텍스처를 설정합니다. </summary>
		void HeightMap_set( RefPtr<Game::HeightMap> value );

		/// <summary> (Visual Studio 전용) 재질을 설정하거나 가져옵니다. </summary>
		vs_property( RefPtr<Game::Material>, Material );

		/// <summary> 재질을 가져옵니다. </summary>
		RefPtr<Game::Material> Material_get();

		/// <summary> 재질을 설정합니다. </summary>
		void Material_set( RefPtr<Game::Material> value );
	};
}