#pragma once

namespace SC::Game
{
	/// <summary> ��� �浹ü(Collider)�� �⺻ Ŭ������ ��Ÿ���ϴ�. </summary>
	class Collider : public Component
	{
		friend class GameObject;

	protected:
		static sc_game_export_object( physx::PxMaterial* ) mDefaultMat;

	private:
		Vector3 mCenter;
		Quaternion mRotation;
		bool mIsTrigger = false;

		sc_game_export_object( physx::PxRigidActor* ) mActor = nullptr;
		sc_game_export_object( physx::PxShape* ) mShape = nullptr;

	protected:
		sc_game_export_object( std::unique_ptr<physx::PxGeometry> ) mGeometry;
		bool mHasUpdate;

	protected:
		Collider();

		void Clone( Collider* already_object );
		void AttachToActor( sc_game_export_object( physx::PxRigidActor* ) actor );

	public:
		~Collider() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ������Ʈ�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

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

		/// <summary> (Visual Studio ����) �� �浹ü�� �̺�Ʈ�� �߻���Ű�� Ʈ���� �浹ü���� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( bool, IsTrigger );

		/// <summary> �� �浹ü�� �̺�Ʈ�� �߻���Ű�� Ʈ���� �浹ü���� ��Ÿ���� ���� �����ɴϴ�. </summary>
		bool IsTrigger_get();

		/// <summary> �� �浹ü�� �̺�Ʈ�� �߻���Ű�� Ʈ���� �浹ü���� ��Ÿ���� ���� �����մϴ�. </summary>
		void IsTrigger_set( bool value );

	private:
		void ReleaseShape();
	};
}