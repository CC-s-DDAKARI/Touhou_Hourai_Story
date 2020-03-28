#pragma once

namespace SC::Game
{
	/// <summary> ��ü�� ��ȯ ���¸� ǥ���մϴ�. �� Ŭ������ ��ӵ� �� �����ϴ�. </summary>
	class Transform final : public Component
	{
		friend class Camera;
		friend class GameObject;
		friend class Collider;

		struct Constants
		{
			sc_game_export_float( XMFLOAT4X4, 16 ) World;
			sc_game_export_float( XMFLOAT4X4, 16 ) WorldInvTranspose;
		};

		Vector3 position;
		Vector3 scale;
		Quaternion rotation;
		GameObject* gameObject = nullptr;

		bool hasBuffer = false;
		std::array<sc_game_export_object( ComPtr<Details::CDynamicBuffer> ), 2> dynamicBuffer;

		sc_game_export_float( XMFLOAT4X4, 16 ) world;
		bool hasUpdate = true;
		bool updated = false;

		float mGlobalPos[3]{ 0, 0, 0 };
		float mGlobalQuat[4]{ 0, 0, 0, 1.0f };

	private:
		void SetGraphicsRootConstantBufferView( RefPtr<Details::CDeviceContext>& deviceContext, int frameIndex );
		void CreateBuffer();

	public:
		/// <summary> <see cref="Transform"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		Transform();
		~Transform() override;

		/// <summary> (<see cref="ICloneable"/>) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ��ȯ ����� �����մϴ�. �θ� ��ȯ�� ������ �޽��ϴ�. </summary>
		/// <param name="time"> (������ �ʴ� �Ű�����) ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> (������ �ʴ� �Ű�����) ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> ����� �ٶ󺸵��� �����մϴ�. </summary>
		/// <param name="target"> ����� �����մϴ�. </param>
		/// <param name="up"> ���� ���͸� �����մϴ�. </param>
		void LookAt( RefPtr<Transform> target, Vector3 up = Vector3::Up );

		/// <summary> ����� �ٶ󺸵��� �����մϴ�. </summary>
		/// <param name="target"> ����� �����մϴ�. </param>
		/// <param name="up"> ���� ���͸� �����մϴ�. </param>
		void LookAt( Vector3 target, Vector3 up = Vector3::Up );

		/// <summary> ������ �ٶ󺸵��� �����մϴ�. </summary>
		/// <param name="direction"> ������ �����մϴ�. </param>
		/// <param name="up"> ���� ���͸� �����մϴ�. </param>
		void LookTo( Vector3 direction, Vector3 up = Vector3::Up );

		/// <summary> (Visual Studio ����) ��ü�� ��ġ ��ȯ�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector3, Position );

		/// <summary> ��ü�� ��ġ ��ȯ�� �����ɴϴ�. </summary>
		Vector3 Position_get();

		/// <summary> ��ü�� ��ġ ��ȯ�� �����մϴ�. </summary>
		void Position_set( Vector3 value );

		/// <summary> (Visual Studio ����) ��ü�� ������ ��ġ ��ȯ�� �����ɴϴ�. </summary>
		vs_property_get( Vector3, ActualPosition );

		/// <summary> ��ü�� ��ġ ��ȯ�� �����ɴϴ�. </summary>
		Vector3 ActualPosition_get();

		/// <summary> (Visual Studio ����) ��ü�� ũ�� ��ȯ�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector3, Scale );

		/// <summary> ��ü�� ũ�� ��ȯ�� �����ɴϴ�. </summary>
		Vector3 Scale_get();

		/// <summary> ��ü�� ũ�� ��ȯ�� �����մϴ�. </summary>
		void Scale_set( Vector3 value );

		/// <summary> (Visual Studio ����) ��ü�� ������ ũ�� ��ȯ�� �����ɴϴ�. </summary>
		vs_property_get( Vector3, ActualScale );

		/// <summary> ��ü�� ������ ũ�� ��ȯ�� �����ɴϴ�. </summary>
		Vector3 ActualScale_get();

		/// <summary> (Visual Studio ����) ��ü�� ȸ�� ��ȯ�� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Quaternion, Rotation );

		/// <summary> ��ü�� ȸ�� ��ȯ�� �����ɴϴ�. </summary>
		Quaternion Rotation_get();

		/// <summary> ��ü�� ȸ�� ��ȯ�� �����մϴ�. </summary>
		void Rotation_set( Quaternion value );

		/// <summary> (Visual Studio ����) ��ü�� ������ ȸ�� ��ȯ�� �����ɴϴ�. </summary>
		vs_property_get( Quaternion, ActualRotation );

		/// <summary> ��ü�� ������ ȸ�� ��ȯ�� �����ɴϴ�. </summary>
		Quaternion ActualRotation_get();

		/// <summary> (Visual Studio ����) ��ü�� ���� �ٶ󺸴� ������ �����ɴϴ�. </summary>
		vs_property_get( Vector3, Forward );

		/// <summary> ��ü�� ���� �ٶ󺸴� ������ �����ɴϴ�. </summary>
		Vector3 Forward_get();

		/// <summary> (Visual Studio ����) ��ü�� ���� �ٶ󺸴� ������ ������ �����ɴϴ�. </summary>
		vs_property_get( Vector3, ActualForward );

		/// <summary> ��ü�� ���� �ٶ󺸴� ������ ������ �����ɴϴ�. </summary>
		Vector3 ActualForward_get();
	};
}