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

SC::Event<RefPtr<UnhandledErrorDetectedEventArgs>> App::UnhandledErrorDetected;
SC::Event<> App::Disposing;
SC::Event<Point<int>> App::Resizing;

ComPtr<ID3D12Fence> App::mFence;
uint64 App::mFenceValue;
Threading::Event App::mFenceEvent;

void App::Initialize()
{
	Disposing += Dispose;

	CreateWindow();
	InitializePackages();

	auto pDevice = Graphics::mDevice->pDevice.Get();
	HR( pDevice->CreateFence( 1, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &mFence ) ) );
	mFenceValue = 1;
}

void App::Start()
{
	// 앱에 스타트 요청을 보내고, 창을 화면에 표시합니다.
	mApp->OnStart();
	ShowWindow( mWndHandle, SW_SHOW );

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

	// 앱의 모든 작업이 완료될 때까지 대기합니다.
	if ( mFence->GetCompletedValue() < mFenceValue )
	{
		HR( mFence->SetEventOnCompletion( mFenceValue, mFenceEvent.Handle ) );
		mFenceEvent.WaitForSingleObject( 1000 );
	}

	// 앱이 종료될 때 패키지의 Dispose 함수를 호출합니다.
	Disposing( nullptr );
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
	UISystem::Initialize();
	GameLogic::Initialize();
	ShaderBuilder::Initialize();
}

void App::ResizeApp( Point<int> size )
{
	if ( size.X != 0 && size.Y != 0 )
	{
		if ( mFence->GetCompletedValue() < mFenceValue )
		{
			HR( mFence->SetEventOnCompletion( mFenceValue, mFenceEvent.Handle ) );
			mFenceEvent.WaitForSingleObject( 1000 );
		}

		Resizing( IntPtr( mWndHandle ), size );
		mDiscardPresent = false;
	}
	else
	{
		mDiscardPresent = true;
	}
}

void App::OnIdle()
{
	mApp->IdleProcess();
}

LRESULT CALLBACK App::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static Point<int> previous( -10000, -10000 );

	switch ( uMsg )
	{
	case WM_SIZE:
		ResizeApp( { ( int )( short )LOWORD( lParam ), ( int )( short )HIWORD( lParam ) } );
		break;
	case WM_MOUSEMOVE:
	{
		auto cur = LPARAMToPoint( lParam );

		if ( previous == Point<int>( -10000, -10000 ) )
		{
			previous = cur;
		}

		auto del = cur - previous;
		previous = cur;
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseMove, UI::MouseMoveEventArgs( cur, del ) ) );
		break;
	}
	case WM_LBUTTONDOWN:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::LeftButton, true, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_LBUTTONUP:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::LeftButton, false, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_RBUTTONDOWN:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::RightButton, true, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_RBUTTONUP:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::RightButton, false, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_MBUTTONDOWN:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::MiddleButton, true, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_MBUTTONUP:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::MiddleButton, false, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_XBUTTONDOWN:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( HIWORD( wParam ) == XBUTTON1 ? UI::MouseButtonType::X1Button : UI::MouseButtonType::X2Button, true, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_XBUTTONUP:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( HIWORD( wParam ) == XBUTTON1 ? UI::MouseButtonType::X1Button : UI::MouseButtonType::X2Button, false, LPARAMToPoint( lParam ) ) ) );
		break;
	case WM_MOUSEWHEEL:
		GlobalVar.scrollDelta.Y += ( double )( short )HIWORD( wParam ) / 120.0;
		break;
	case WM_KEYDOWN:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::KeyboardEvent, UI::KeyboardEventArgs( ( KeyCode )wParam, true ) ) );
		break;
	case WM_KEYUP:
		App::mApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::KeyboardEvent, UI::KeyboardEventArgs( ( KeyCode )wParam, false ) ) );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}

	return DefWindowProcW( hWnd, uMsg, wParam, lParam );
}

Point<int> App::LPARAMToPoint( LPARAM lParam )
{
	return Point<int>( ( int )LOWORD( lParam ), ( int )HIWORD( lParam ) );
}