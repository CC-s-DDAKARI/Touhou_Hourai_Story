#pragma once

namespace SC::Game
{
	/// <summary> 박스 형태의 기본 충돌체를 나타냅니다. </summary>
	class BoxCollider : public Collider
	{
		Vector3 changeExtent;

	protected:
		using Collider::Clone;

	public:
		/// <summary> <see cref="BoxCollider"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		BoxCollider();
		~BoxCollider() override;

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 연결된 개체가 시작될 때 실행됩니다. </summary>
		void Start() override;

		/// <summary> (Visual Studio 전용) 박스의 확장 크기를 나타내는 값을 설정하거나 가져옵니다. </summary>
		vs_property( Vector3, Extent );

		/// <summary> 박스의 확장 크기를 나타내는 값을 가져옵니다. </summary>
		Vector3 Extent_get();

		/// <summary> 박스의 확장 크기를 나타내는 값을 설정합니다. </summary>
		void Extent_set( Vector3 value );
	};
}