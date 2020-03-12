#pragma once

namespace SC::Game
{
	/// <summary> �ڽ� ������ �⺻ �浹ü�� ��Ÿ���ϴ�. </summary>
	class BoxCollider : public Collider
	{
		static sc_game_export_object( physx::PxMaterial* ) pxDefaultMat;

		Vector3 changeCenter;
		Vector3 changeExtent;

	public:
		/// <summary> <see cref="BoxCollider"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		BoxCollider();
		~BoxCollider() override;

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ����� ��ü�� ���۵� �� ����˴ϴ�. </summary>
		void Start() override;

		/// <summary> (Visual Studio ����) �ڽ��� �߽��� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector3, Center );

		/// <summary> �ڽ��� �߽��� ��Ÿ���� ���� �����ɴϴ�. </summary>
		Vector3 Center_get();

		/// <summary> �ڽ��� �߽��� ��Ÿ���� ���� �����մϴ�. </summary>
		void Center_set( Vector3 value );

		/// <summary> (Visual Studio ����) �ڽ��� Ȯ�� ũ�⸦ ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector3, Extent );

		/// <summary> �ڽ��� Ȯ�� ũ�⸦ ��Ÿ���� ���� �����ɴϴ�. </summary>
		Vector3 Extent_get();

		/// <summary> �ڽ��� Ȯ�� ũ�⸦ ��Ÿ���� ���� �����մϴ�. </summary>
		void Extent_set( Vector3 value );
	};
}