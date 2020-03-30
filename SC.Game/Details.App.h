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

		static ComPtr<ID3D12Fence> mFence;
		static uint64 mFenceValue;
		static Threading::Event mFenceEvent;

		static int mFrameIndex;
		static uint64 mLastPending[2];
		static Threading::Event mRenderThreadEvent;
		static int mRenderFrameIndex;
		static RefPtr<Scene> mRenderScene;

		static Event<RefPtr<UnhandledErrorDetectedEventArgs>> UnhandledErrorDetected;
		static Event<> Disposing;
		static Event<Drawing::Point<int>> Resizing;

	public:
		static void Initialize();
		static void Start();

	private:
		static void Dispose( object sender );
		static void CreateWindow();
		static void InitializePackages();
		static void ResizeApp( Drawing::Point<int> size );
		static void OnIdle();
		static void Update();
		static void Render();
		static void RenderLoop( object arg0 );

		static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		static Drawing::Point<int> LPARAMToPoint( LPARAM lParam );
	};
}