#pragma once

namespace SC::Game
{
	/// <summary> ĸ�� ������ �⺻ �浹ü�� ��Ÿ���ϴ�. </summary>
	class CapsuleCollider : public Collider
	{
		double mHalfHeight = 1;
		double mRadius = 1;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="CapsuleCollider"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		CapsuleCollider();
		~CapsuleCollider() override;

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ������Ʈ�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> (Visual Studio ����) ĸ���� X�� ���̸� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, Height );

		/// <summary> ĸ���� X�� ���̸� ��Ÿ���� ���� �����ɴϴ�. </summary>
		double Height_get();

		/// <summary> ĸ���� X�� ���̸� ��Ÿ���� ���� �����մϴ�. </summary>
		void Height_set( double value );

		/// <summary> (Visual Studio ����) ĸ���� �������� ��Ÿ���� ���� �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, Radius );

		/// <summary> ĸ���� �������� ��Ÿ���� ���� �����ɴϴ�. </summary>
		double Radius_get();

		/// <summary> ĸ���� �������� ��Ÿ���� ���� �����մϴ�. </summary>
		void Radius_set( double value );
	};
}