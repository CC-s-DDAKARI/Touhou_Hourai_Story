using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Drawing;
using namespace SC::Game::UI;
using namespace SC::Threading;

using namespace std;

Application* App::mApp;
HWND App::mWndHandle;

AppConfiguration App::mConfiguration;
bool App::mDiscardPresent = false;

ComPtr<ID3D12Fence> App::mFence;
uint64 App::mFenceValue;
Threading::Event App::mFenceEvent;

uint64 App::mLastPending[2];
Threading::Event App::mRenderThreadEvent;

SC::Event<RefPtr<UnhandledErrorDetectedEventArgs>> App::UnhandledErrorDetected;
SC::Event<> App::Disposing;
SC::Event<Point<int>> App::Resizing;

void App::Initialize()
{
	Disposing += Dispose;

	CreateWindow();
	InitializePackages();

	auto pDevice = Graphics::mDevice->pDevice.Get();
	HR( pDevice->CreateFence( 1, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &mFence ) ) );
	mFenceValue = 1;

	mRenderThreadEvent.Set();
}

void App::Start()
{
	// �ۿ� ��ŸƮ ��û�� ������, â�� ȭ�鿡 ǥ���մϴ�.
	mApp->OnStart();
	ShowWindow( mWndHandle, SW_SHOW );

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

	// ���� ��� �۾��� �Ϸ�� ������ ����մϴ�.
	if ( mFence->GetCompletedValue() < mFenceValue )
	{
		HR( mFence->SetEventOnCompletion( mFenceValue, mFenceEvent.Handle ) );
		mFenceEvent.WaitForSingleObject( 1000 );
	}

	// ���� ����� �� ��Ű���� Dispose �Լ��� ȣ���մϴ�.
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
	Physics::Initialize();
	UISystem::Initialize();
	GameLogic::Initialize();
	ShaderBuilder::Initialize();
	GC::Initialize();
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
	GlobalVar.frameIndex += 1;
	if ( GlobalVar.frameIndex >= 2 ) GlobalVar.frameIndex = 0;

	Update();
	if ( !mDiscardPresent ) Render();
	//mApp->IdleProcess();

	GC::Collect( GlobalVar.frameIndex );
}

void App::Update()
{
	// ��� ��ȯ ��û�� ���� ��� ����� ��ȯ�մϴ�.
	if ( SceneManager::mCurrentScene.Get() )
	{
		mRenderThreadEvent.WaitForSingleObject();
		GameLogic::mCurrentScene = move( SceneManager::mCurrentScene );
		mRenderThreadEvent.Set();
	}

	GameLogic::Update();
	UISystem::Update();
}

void App::Render()
{
	int frameIndex = GlobalVar.frameIndex;

	mRenderThreadEvent.WaitForSingleObject( 1000 );

	if ( mFence->GetCompletedValue() < mLastPending[frameIndex] )
	{
		HR( mFence->SetEventOnCompletion( mLastPending[frameIndex], mFenceEvent.Handle ) );
		mFenceEvent.WaitForSingleObject( 1000 );
	}

	ThreadPool::QueueUserWorkItem( [frameIndex]( object )
		{
			auto pCommandQueue = Graphics::mDevice->DirectQueue[0]->pCommandQueue.Get();

			GameLogic::Render( frameIndex );
			UISystem::Render( frameIndex );

			// ������ ��� ��ȣ�� �����մϴ�.
			HR( pCommandQueue->Signal( mFence.Get(), ++mFenceValue ) );
			mLastPending[frameIndex] = mFenceValue;

			mRenderThreadEvent.Set();
		}
	, nullptr );
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