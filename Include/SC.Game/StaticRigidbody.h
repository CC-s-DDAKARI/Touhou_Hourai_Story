#pragma once

namespace SC::Game
{
	/// <summary> ����� ���� ������Ʈ�� �浹ü ��ü�� ���� ���� ��� �����մϴ�. </summary>
	class StaticRigidbody : public Component
	{
		friend class Scene;
		friend class Transform;

		sc_game_export_object( physx::PxRigidStatic* ) pxRigidbody = nullptr;

		std::list<Collider*> appliedColliders;

	public:
		/// <summary> <see cref="Rigidbody"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		StaticRigidbody();
		~StaticRigidbody() override;

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> ������Ʈ�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

	private:
		void UpdateRigidbodyPhysics();
	};
}