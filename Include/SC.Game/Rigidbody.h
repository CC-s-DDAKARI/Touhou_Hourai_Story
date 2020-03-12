#pragma once

namespace SC::Game
{
	/// <summary> 연결된 게임 오브젝트가 물리 제어로 동작하도록 합니다. </summary>
	class Rigidbody : public Component
	{
		friend class Scene;
		friend class Transform;

		sc_game_export_object( physx::PxRigidDynamic* ) pxRigidbody = nullptr;

		std::list<Collider*> appliedColliders;

	public:
		/// <summary> <see cref="Rigidbody"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		Rigidbody();
		~Rigidbody() override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 연결된 개체가 시작될 때 실행됩니다. </summary>
		void Start() override;

		/// <summary> 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> (Visual Studio 전용) 물리효과가 해당 리지드바디에 영향을 줄 수 있는지 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( bool, IsKinematic );

		/// <summary> 물리효과가 해당 리지드바디에 영향을 줄 수 있는지 타나내는 값을 가져옵니다. </summary>
		bool IsKinematic_get();

		/// <summary> 물리효과가 해당 리지드바디에 영향을 줄 수 있는지 나타내는 값을 설정합니다. </summary>
		void IsKinematic_set( bool value );

		/// <summary> (Visual Studio 전용) 리지드바디의 회전을 잠그는 값을 설정하거나 가져옵니다. X: 0, Y: 1, Z: 2의 값을 가집니다. </summary>
		vs_property( bool, FreezeRotation )[];

		/// <summary> 리지드바디의 회전을 잠그는 값을 가져옵니다. X: 0, Y: 1, Z: 2의 값을 가집니다. </summary>
		bool FreezeRotation_get( int param0 );

		/// <summary> 리지드바디의 회전을 잠그는 값을 설정합니다. X: 0, Y: 1, Z: 2의 값을 가집니다. </summary>
		void FreezeRotation_set( int param0, bool value );

	private:
		void UpdateRigidbodyPhysics();
	};
}