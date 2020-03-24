#pragma once

namespace SC::Game
{
	/// <summary> 구형 정보를 표현합니다. </summary>
	struct Sphere
	{
		/// <summary> 구의 원점을 나타냅니다. </summary>
		Vector3 Center;

		/// <summary> 구의 반지름을 나타냅니다. </summary>
		double Radius;

		/// <summary> <see cref="Sphere"/> 구조체의 새 인스턴스를 초기화합니다. </summary>
		Sphere();

		/// <summary> <see cref="Sphere"/> 구조체의 새 인스턴스를 초기화합니다. </summary>
		/// <param name="center"> 구의 원점을 전달합니다. </param>
		/// <param name="radius"> 구의 반지름을 전달합니다. </param>
		Sphere( Vector3 center, double radius );

		/// <summary> <see cref="Sphere"/> 구조체의 새 인스턴스를 초기화합니다. </summary>
		Sphere( const Sphere& copy );

		bool operator==( const Sphere& right ) const;
		bool operator!=( const Sphere& right ) const;
	};
}