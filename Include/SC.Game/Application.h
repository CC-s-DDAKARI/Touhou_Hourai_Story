#pragma once

namespace SC::Game
{
	/// <summary> ���� �ۿ� ���� ���� �Լ��� �����ϸ� ���� ���� ó���⿡ ���� �������̽��� �����մϴ�. </summary>
	class Application : virtual public Object
	{
	public:
		/// <summary> <see cref="Application"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="appConfig"> �� ������ �����մϴ�. </param>
		Application( AppConfiguration appConfig );
		~Application() override;

		/// <summary> ���� ���۵� �� ȣ��˴ϴ�. </summary>
		virtual void OnStart() = 0;

		/// <summary> ���� ����� �� ȣ��˴ϴ�. ���� ���� �ڵ带 ��ȯ�ؾ� �մϴ�. </summary>
		virtual int OnExit() = 0;

		/// <summary> (<see cref="Object"/> Ŭ�������� ��� ��.) ���� ��ü�� �ؽ�Ʈ ǥ���� �����Ͽ� ��ȯ�մϴ�. </summary>
		String ToString() override;

		/// <summary> ���� ���α׷��� �������� ���� ���� �����մϴ�. �� �Լ��� ���� ����� ������ ��ȯ���� �ʽ��ϴ�. </summary>
		/// <param name="app"> ���� �� ��ü�� �����մϴ�. </param>
		static int Start( RefPtr<Application> app );

		/// <summary> (Visual Studio ����) ���� ȭ�� ũ�⸦ �����ɴϴ�. </summary>
		vs_property_get( Drawing::Point<int>, ClientSize );

		/// <summary> ���� ȭ�� ũ�⸦ �����ɴϴ�. </summary>
		Drawing::Point<int> ClientSize_get();

		/// <summary> (Visual Studio ����) ���� ������ �����ɴϴ�. </summary>
		vs_property_get( AppConfiguration, AppConfig );

		/// <summary> ���� ������ �����ɴϴ�. </summary>
		AppConfiguration AppConfig_get();

		/// <summary> (Visual Studio ����) ���� ���� UI �������� �����ɴϴ�. </summary>
		vs_property_get( RefPtr<UI::Canvas>, Frame );

		/// <summary> ���� ���� UI �������� �����ɴϴ�. </summary>
		RefPtr<UI::Canvas> Frame_get();

		/// <summary> (Visual Studio ����) ���� �̸��� ��� ������ �÷������� ���� �̸��� �����ϰų� �����ɴϴ�. </summary>
		vs_property( String, AppName );

		/// <summary> ���� �̸��� ��� ������ �÷������� ���� �̸��� �����ɴϴ�. </summary>
		String AppName_get();

		/// <summary> ���� �̸��� ��� ������ �÷������� ���� �̸��� �����մϴ�. </summary>
		void AppName_set( String value );

		/// <summary> �ۿ��� ó������ ���� �񵿱� �Ϸ� ó���� �Ǵ� �̺�Ʈ ó���⿡ ������ �ִ� ��� �߻��մϴ�. </summary>
		static Event<RefPtr<UnhandledErrorDetectedEventArgs>> UnhandledErrorDetected;
	};
}