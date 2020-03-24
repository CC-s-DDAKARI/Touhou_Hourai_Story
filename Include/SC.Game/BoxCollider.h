#pragma once

namespace SC::Game
{
	/// <summary> �ڽ� ������ �⺻ �浹ü�� ��Ÿ���ϴ�. </summary>
	class BoxCollider : public Collider
	{
		Vector3 mHalfExtents;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="BoxCollider"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		BoxCollider();
		~BoxCollider() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ������Ʈ�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (Visual Studio ����) �ڽ��� Ȯ�� ũ�⸦ ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( Vector3, HalfExtents );

		/// <summary> �ڽ��� Ȯ�� ũ�⸦ ��Ÿ���� ���� �����ɴϴ�. </summary>
		Vector3 HalfExtents_get();

		/// <summary> �ڽ��� Ȯ�� ũ�⸦ ��Ÿ���� ���� �����մϴ�. </summary>
		void HalfExtents_set( Vector3 value );
	};
}