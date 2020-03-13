#pragma once

namespace SC::Game
{
	/// <summary> ��� �浹ü(Collider)�� �⺻ Ŭ������ ��Ÿ���ϴ�. </summary>
	class Collider : public Component
	{
		friend class GameObject;

		bool started = false;
		Vector3 changeCenter;
		Quaternion changeRotation;

	protected:
		sc_game_export_object( physx::PxShape* ) pxShape = nullptr;

		Collider();

		void Clone( Collider* already_object );

	public:
		~Collider() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ����� ��ü�� ���۵� �� ����˴ϴ�. </summary>
		void Start() override;

		/// <summary> (Visual Studio ����) �浹ü ����� �߽��� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector3, Center );

		/// <summary> �浹ü ����� �߽��� ��Ÿ���� ���� �����ɴϴ�. </summary>
		Vector3 Center_get();

		/// <summary> �浹ü ����� �߽��� ��Ÿ���� ���� �����մϴ�. </summary>
		void Center_set( Vector3 value );

		/// <summary> (Visual Studio ����) �浹ü ����� ȸ���� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Quaternion, Rotation );

		/// <summary> �浹ü ����� ȸ���� ��Ÿ���� ���� �����ɴϴ�. </summary>
		Quaternion Rotation_get();

		/// <summary> �浹ü ����� ȸ���� ��Ÿ���� ���� �����մϴ�. </summary>
		void Rotation_set( Quaternion value );
	};
}