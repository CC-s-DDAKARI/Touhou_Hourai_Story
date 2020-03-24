#pragma once

namespace SC::Game
{
	/// <summary> 캡슐 형태의 기본 충돌체를 나타냅니다. </summary>
	class CapsuleCollider : public Collider
	{
		double mHalfHeight = 1;
		double mRadius = 1;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="CapsuleCollider"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		CapsuleCollider();
		~CapsuleCollider() override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> (Visual Studio 전용) 캡슐의 X축 높이를 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( double, Height );

		/// <summary> 캡슐의 X축 높이를 나타내는 값을 가져옵니다. </summary>
		double Height_get();

		/// <summary> 캡슐의 X축 높이를 나타내는 값을 설정합니다. </summary>
		void Height_set( double value );

		/// <summary> (Visual Studio 전용) 캡슐의 반지름을 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( double, Radius );

		/// <summary> 캡슐의 반지름을 나타내는 값을 가져옵니다. </summary>
		double Radius_get();

		/// <summary> 캡슐의 반지름을 나타내는 값을 설정합니다. </summary>
		void Radius_set( double value );
	};
}