#pragma once

namespace SC::Game
{
	/// <summary> 높이 맵을 사용하는 지형 형태의 충돌체를 나타냅니다. </summary>
	class TerrainCollider : public Collider
	{
	private:
		Terrain* mBakedTerrain = nullptr;
		sc_game_export_object( physx::PxHeightField* ) mCookedHeightField = nullptr;
		Vector3 mPrevActualScale;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="TerrainCollider"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		TerrainCollider();
		~TerrainCollider() override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

	private:
		void Bake( Terrain* terrain );
	};
}