#pragma once

namespace SC::Game
{
	/// <summary> 연결된 게임 오브젝트가 충돌체 개체를 통해 물리 제어를 구현합니다. </summary>
	class StaticRigidbody : public Component
	{
		friend class Scene;
		friend class Transform;

		sc_game_export_object( physx::PxRigidStatic* ) pxRigidbody = nullptr;

		std::list<Collider*> appliedColliders;

	public:
		/// <summary> <see cref="Rigidbody"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		StaticRigidbody();
		~StaticRigidbody() override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

	private:
		void UpdateRigidbodyPhysics();
	};
}