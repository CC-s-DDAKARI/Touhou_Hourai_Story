#pragma once

namespace SC::Game
{
	/// <summary> ��Ƽ ������ ȯ���� ���� ��ü�� ����� �����带 �����ϴ� ������Ʈ�Դϴ�. �� ������Ʈ�� ������ ID�� ��Ʈ�� ���Դϴ�. </summary>
	class ThreadDispatcher : public Component
	{
		int threadId = 0;

	public:
		/// <summary> <see cref="ThreadDispatcher"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		ThreadDispatcher();

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (Visual Studio ����) ������ �ĺ� ��ȣ�� �����ϰų� �����ɴϴ�. 0�� ��� �� �����带 ����մϴ�. </summary>
		vs_property( int, ThreadID );

		/// <summary> ������ �ĺ� ��ȣ�� �����ɴϴ�. 0�� ��� �� �����带 ��Ÿ���ϴ�. </summary>
		int ThreadID_get();

		/// <summary> ������ �ĺ� ��ȣ�� �����մϴ�. 0�� ��� �� �����带 ����մϴ�. </summary>
		void ThreadID_set( int value );
	};
}