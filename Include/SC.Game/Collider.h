#pragma once

namespace SC::Game
{
	/// <summary> ��� �浹ü(Collider)�� �⺻ Ŭ������ ��Ÿ���ϴ�. </summary>
	class Collider : public Component
	{
		friend class Rigidbody;
		friend class StaticRigidbody;

	protected:
		sc_game_export_object( physx::PxShape* ) pxShape = nullptr;

	public:
		/// <summary> <see cref="Collider"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		Collider();
		~Collider() override;
	};
}