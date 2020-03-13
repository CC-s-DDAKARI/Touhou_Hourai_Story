#pragma once

namespace SC::Game
{
	/// <summary> 연결된 게임 오브젝트가 물리 제어로 동작하도록 합니다. </summary>
	class Rigidbody : public Component
	{
		friend class GameObject;

		sc_game_export_object( physx::PxRigidDynamic* ) pxRigidbody = nullptr;

	public:
		/// <summary> <see cref="Rigidbody"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		Rigidbody();

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

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
	};
}