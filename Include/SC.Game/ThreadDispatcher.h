#pragma once

namespace SC::Game
{
	/// <summary> 멀티 스레드 환경을 위해 개체가 사용할 스레드를 지정하는 컴포넌트입니다. 이 컴포넌트로 지정된 ID는 힌트일 뿐입니다. </summary>
	class ThreadDispatcher : public Component
	{
		int threadId = 0;

	public:
		/// <summary> <see cref="ThreadDispatcher"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		ThreadDispatcher();

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (Visual Studio 전용) 스레드 식별 번호를 설정하거나 가져옵니다. 0일 경우 주 스레드를 사용합니다. </summary>
		vs_property( int, ThreadID );

		/// <summary> 스레드 식별 번호를 가져옵니다. 0일 경우 주 스레드를 나타냅니다. </summary>
		int ThreadID_get();

		/// <summary> 스데르 식별 번호를 설정합니다. 0일 경우 주 스레드를 사용합니다. </summary>
		void ThreadID_set( int value );
	};
}