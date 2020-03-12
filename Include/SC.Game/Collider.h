#pragma once

namespace SC::Game
{
	/// <summary> 모든 충돌체(Collider)의 기본 클래스를 나타냅니다. </summary>
	class Collider : public Component
	{
		friend class Rigidbody;
		friend class StaticRigidbody;

	protected:
		sc_game_export_object( physx::PxShape* ) pxShape = nullptr;

	public:
		/// <summary> <see cref="Collider"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		Collider();
		~Collider() override;
	};
}