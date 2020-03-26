using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Drawing;
using namespace SC::Game::UI;

using namespace std;

Application* App::mApp;
HWND App::mWndHandle;

AppConfiguration App::mConfiguration;
bool App::mDiscardPresent = false;

RefPtr<GameLogic> App::mGameLogic;

SC::Event<RefPtr<UnhandledErrorDetectedEventArgs>> App::UnhandledErrorDetected;
SC::Event<> App::AppDisposing;
SC::Event<Point<int>> App::AppResizing;

void App::Initialize()
{
	AppDisposing += Dispose;

	CreateWindow();
	InitializePackages();
}

void App::Start()
{
	// 앱을 초기화합니다.
	//Initialize();

	// 앱에 스타트 요청을 보내고, 창을 화면에 표시합니다.
	//mApp->OnStart();
	//ShowWindow( mWndHandle, SW_SHOW );

	// 앱의 주 메시지 루프를 시작합니다.
	MSG msg{ };
	while ( true )
	{
#if !defined( _DEBUG )
		try
#endif
		{
			// 메시지 큐에 메시지가 있을 경우 메시지에 대한 처리를 수행합니다.
			// 메시지 처리가 지연되지 않도록 항상 우선적으로 처리합니다.
			if ( PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) )
			{
				if ( msg.message == WM_QUIT )
				{
					break;
				}

				TranslateMessage( &msg );
				DispatchMessageW( &msg );
			}

			// 메시지 큐에 메시지가 없을 경우 앱의 논리를 처리합니다.
			else
			{
				OnIdle();
			}
		}
#if !defined( _DEBUG )
		catch ( Exception* e )
		{
			UnhandledErrorDetected( nullptr, new UnhandledErrorDetectedEventArgs( e ) );
		}
#endif
	}

	// 앱이 종료될 때 패키지의 Dispose 함수를 호출합니다.
	AppDisposing( nullptr );
}

void App::Dispose( object sender )
{

}

void App::CreateWindow()
{
	WNDCLASSEXW wcex{ };
	wcex.cbSize = sizeof( wcex );
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = L"SC.Game.Details.App.CoreWindow";
	wcex.hInstance = GetModuleHandleW( nullptr );

	if ( RegisterClassExW( &wcex ) == 0 )
	{
		throw new Exception( "SC.Game.Details.App.CreateWindow(): Windows 창 클래스를 초기화하는데 실패하였습니다." );
	}

	mWndHandle = CreateWindowExW(
		NULL,
		wcex.lpszClassName,
		mConfiguration.AppName.Chars,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		nullptr,
		wcex.hInstance,
		nullptr
		);
	if ( !mWndHandle )
	{
		throw new Exception( "SC.Game.Details.App.CreateWindow(): Windows 창을 생성하는데 실패하였습니다." );
	}
}

void App::InitializePackages()
{
	Graphics::Initialize();
	Physics::Initialize();
	/*
	ShaderBuilder::Initialize();

	mGameLogic = new GameLogic();
	*/
}

void App::ResizeApp( Point<int> appResizing )
{
	AppResizing( IntPtr( mWndHandle ), appResizing );
}

void App::OnIdle()
{
	mApp->IdleProcess();
}

LRESULT CALLBACK App::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_SIZE:
		ResizeApp( { ( int )( short )LOWORD( lParam ), ( int )( short )HIWORD( lParam ) } );
		break;
	}

	return DefWindowProcW( hWnd, uMsg, wParam, lParam );
}