#pragma once

namespace SC::Game::Details
{
	class App abstract
	{
	public:
		static Application* mApp;
		static HWND mWndHandle;

		static AppConfiguration mConfiguration;
		static bool mDiscardPresent;

		static RefPtr<GameLogic> mGameLogic;

		static Event<RefPtr<UnhandledErrorDetectedEventArgs>> UnhandledErrorDetected;
		static Event<> AppDisposing;
		static Event<Drawing::Point<int>> AppResizing;

	public:
		static void Initialize();
		static void Start();

	private:
		static void Dispose( object sender );
		static void CreateWindow();
		static void InitializePackages();
		static void ResizeApp( Drawing::Point<int> appResizing );
		static void OnIdle();

		static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	};
}