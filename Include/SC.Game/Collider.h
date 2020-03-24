#pragma once

namespace SC::Game
{
	/// <summary> 모든 충돌체(Collider)의 기본 클래스를 나타냅니다. </summary>
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

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> (Visual Studio 전용) 충돌체 모양의 중심을 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( Vector3, Center );

		/// <summary> 충돌체 모양의 중심을 나타내는 값을 가져옵니다. </summary>
		Vector3 Center_get();

		/// <summary> 충돌체 모양의 중심을 나타내는 값을 설정합니다. </summary>
		void Center_set( Vector3 value );

		/// <summary> (Visual Studio 전용) 충돌체 모양의 회전을 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( Quaternion, Rotation );

		/// <summary> 충돌체 모양의 회전을 나타내는 값을 가져옵니다. </summary>
		Quaternion Rotation_get();

		/// <summary> 충돌체 모양의 회전을 나타내는 값을 설정합니다. </summary>
		void Rotation_set( Quaternion value );

		/// <summary> (Visual Studio 전용) 이 충돌체가 이벤트를 발생시키는 트리거 충돌체임을 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( bool, IsTrigger );

		/// <summary> 이 충돌체가 이벤트를 발생시키는 트리거 충돌체임을 나타내는 값을 가져옵니다. </summary>
		bool IsTrigger_get();

		/// <summary> 이 충돌체가 이벤트를 발생시키는 트리거 충돌체임을 나타내는 값을 설정합니다. </summary>
		void IsTrigger_set( bool value );

	private:
		void ReleaseShape();
	};
}