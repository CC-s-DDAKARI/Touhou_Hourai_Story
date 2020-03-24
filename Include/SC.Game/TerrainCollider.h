#pragma once

namespace SC::Game
{
	/// <summary> ���� ���� ����ϴ� ���� ������ �浹ü�� ��Ÿ���ϴ�. </summary>
	class TerrainCollider : public Collider
	{
	private:
		Terrain* mBakedTerrain = nullptr;
		sc_game_export_object( physx::PxHeightField* ) mCookedHeightField = nullptr;
		Vector3 mPrevActualScale;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="TerrainCollider"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		TerrainCollider();
		~TerrainCollider() override;

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ������Ʈ�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

	private:
		void Bake( Terrain* terrain );
	};
}