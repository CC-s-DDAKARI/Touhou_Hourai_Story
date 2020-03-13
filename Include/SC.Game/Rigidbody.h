#pragma once

namespace SC::Game
{
	/// <summary> ����� ���� ������Ʈ�� ���� ����� �����ϵ��� �մϴ�. </summary>
	class Rigidbody : public Component
	{
		friend class GameObject;

		sc_game_export_object( physx::PxRigidDynamic* ) pxRigidbody = nullptr;

	public:
		/// <summary> <see cref="Rigidbody"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		Rigidbody();

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (Visual Studio ����) ����ȿ���� �ش� ������ٵ� ������ �� �� �ִ��� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( bool, IsKinematic );

		/// <summary> ����ȿ���� �ش� ������ٵ� ������ �� �� �ִ��� Ÿ������ ���� �����ɴϴ�. </summary>
		bool IsKinematic_get();

		/// <summary> ����ȿ���� �ش� ������ٵ� ������ �� �� �ִ��� ��Ÿ���� ���� �����մϴ�. </summary>
		void IsKinematic_set( bool value );

		/// <summary> (Visual Studio ����) ������ٵ��� ȸ���� ��״� ���� �����ϰų� �����ɴϴ�. X: 0, Y: 1, Z: 2�� ���� �����ϴ�. </summary>
		vs_property( bool, FreezeRotation )[];

		/// <summary> ������ٵ��� ȸ���� ��״� ���� �����ɴϴ�. X: 0, Y: 1, Z: 2�� ���� �����ϴ�. </summary>
		bool FreezeRotation_get( int param0 );

		/// <summary> ������ٵ��� ȸ���� ��״� ���� �����մϴ�. X: 0, Y: 1, Z: 2�� ���� �����ϴ�. </summary>
		void FreezeRotation_set( int param0, bool value );
	};
}