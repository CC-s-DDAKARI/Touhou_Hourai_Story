#pragma once

namespace SC::Game
{
	/// <summary> 지정된 Mesh 개체를 화면에 렌더링합니다. 개체는 IsSkinned 속성이 true 값을 가집니다. </summary>
	class SkinnedMeshRenderer : public Component
	{
		friend class Details::GameLogic;

		static Material* mDefaultMaterial;
		static uint64 reference_count;

		RefPtr<Mesh> skinnedMesh;
		RefPtr<Material> material;
		sc_game_export_object( RefPtr<Details::CBuffer> ) mVertexBuffer;

	protected:
		void Skinning( RefPtr<Details::CDeviceContext>& deviceContext );
		void Render( RefPtr<Details::CDeviceContext>& deviceContext, int frameIndex ) override;

	public:
		/// <summary> <see cref="SkinnedMeshRenderer"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		SkinnedMeshRenderer();
		~SkinnedMeshRenderer() override;

		/// <summary> (<see cref="ICloneable"/>) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (Visual Studio 전용) 참조 Mesh 개체를 설정하거나 가져옵니다. </summary>
		vs_property( RefPtr<Game::Mesh>, Mesh );

		/// <summary> 참조 Mesh 개체를 가져옵니다. </summary>
		RefPtr<Game::Mesh> Mesh_get();

		/// <summary> 참조 Mesh 개체를 설정합니다. </summary>
		void Mesh_set( RefPtr<Game::Mesh> value );

		/// <summary> (Visual Studio 전용) 재질을 설정하거나 가져옵니다. </summary>
		vs_property( RefPtr<Game::Material>, Material );

		/// <summary> 재질을 가져옵니다. </summary>
		RefPtr<Game::Material> Material_get();

		/// <summary> 재질을 설정합니다. </summary>
		void Material_set( RefPtr<Game::Material> value );
	};
}