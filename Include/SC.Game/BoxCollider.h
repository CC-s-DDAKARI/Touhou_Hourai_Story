#pragma once

namespace SC::Game
{
	/// <summary> 박스 형태의 기본 충돌체를 나타냅니다. </summary>
	class BoxCollider : public Collider
	{
		Vector3 mHalfExtents;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="BoxCollider"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		BoxCollider();
		~BoxCollider() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (Visual Studio 전용) 박스의 확장 크기를 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( Vector3, HalfExtents );

		/// <summary> 박스의 확장 크기를 나타내는 값을 가져옵니다. </summary>
		Vector3 HalfExtents_get();

		/// <summary> 박스의 확장 크기를 나타내는 값을 설정합니다. </summary>
		void HalfExtents_set( Vector3 value );
	};
}