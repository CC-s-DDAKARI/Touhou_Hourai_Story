#pragma once

namespace SC::Game
{
	/// <summary> ���� ������ ǥ���մϴ�. </summary>
	struct Sphere
	{
		/// <summary> ���� ������ ��Ÿ���ϴ�. </summary>
		Vector3 Center;

		/// <summary> ���� �������� ��Ÿ���ϴ�. </summary>
		double Radius;

		/// <summary> <see cref="Sphere"/> ����ü�� �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		Sphere();

		/// <summary> <see cref="Sphere"/> ����ü�� �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="center"> ���� ������ �����մϴ�. </param>
		/// <param name="radius"> ���� �������� �����մϴ�. </param>
		Sphere( Vector3 center, double radius );

		/// <summary> <see cref="Sphere"/> ����ü�� �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		Sphere( const Sphere& copy );

		bool operator==( const Sphere& right ) const;
		bool operator!=( const Sphere& right ) const;
	};
}