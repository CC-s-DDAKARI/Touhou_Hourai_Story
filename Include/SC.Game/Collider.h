#pragma once

namespace SC::Game
{
	/// <summary> 모든 충돌체(Collider)의 기본 클래스를 나타냅니다. </summary>
	class Collider : public Component
	{
		friend class GameObject;

		bool started = false;
		Vector3 changeCenter;
		Quaternion changeRotation;

	protected:
		static sc_game_export_object( physx::PxMaterial* ) pxDefaultMat;
		sc_game_export_object( physx::PxShape* ) pxShape = nullptr;

		Collider();

		void Clone( Collider* already_object );

	public:
		~Collider() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 연결된 개체가 시작될 때 실행됩니다. </summary>
		void Start() override;

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
	};
}