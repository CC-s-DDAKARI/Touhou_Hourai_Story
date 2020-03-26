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

	/*
	CreateWindow();
	*/
	InitializePackages();
}

void App::Start()
{
	// ���� �ʱ�ȭ�մϴ�.
	//Initialize();

	// �ۿ� ��ŸƮ ��û�� ������, â�� ȭ�鿡 ǥ���մϴ�.
	//mApp->OnStart();
	//ShowWindow( mWndHandle, SW_SHOW );

	// ���� �� �޽��� ������ �����մϴ�.
	MSG msg{ };
	while ( true )
	{
#if !defined( _DEBUG )
		try
#endif
		{
			// �޽��� ť�� �޽����� ���� ��� �޽����� ���� ó���� �����մϴ�.
			// �޽��� ó���� �������� �ʵ��� �׻� �켱������ ó���մϴ�.
			if ( PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) )
			{
				if ( msg.message == WM_QUIT )
				{
					break;
				}

				TranslateMessage( &msg );
				DispatchMessageW( &msg );
			}

			// �޽��� ť�� �޽����� ���� ��� ���� ���� ó���մϴ�.
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

	// ���� ����� �� ��Ű���� Dispose �Լ��� ȣ���մϴ�.
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
		throw new Exception( "SC.Game.Details.App.CreateWindow(): Windows â Ŭ������ �ʱ�ȭ�ϴµ� �����Ͽ����ϴ�." );
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
		throw new Exception( "SC.Game.Details.App.CreateWindow(): Windows â�� �����ϴµ� �����Ͽ����ϴ�." );
	}
}

void App::InitializePackages()
{
	Graphics::Initialize();
	/*
	Physics::Initialize();
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