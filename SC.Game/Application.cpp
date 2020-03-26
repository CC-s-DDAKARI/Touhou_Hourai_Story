using namespace SC;
using namespace SC::Diagnostics;
using namespace SC::Game;
using namespace SC::Game::UI;
using namespace SC::Game::Details;
using namespace SC::Drawing;
using namespace SC::Threading;

using namespace std;
using namespace std::chrono_literals;

#define CW_USEDEFAULT_ALL CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT

SC::Event<RefPtr<UnhandledErrorDetectedEventArgs>> Application::UnhandledErrorDetected;

inline Drawing::Point<int> LPARAMToPoint( LPARAM lParam )
{
	return Drawing::Point<int>( ( int )LOWORD( lParam ), ( int )HIWORD( lParam ) );
}

Application::Application( AppConfiguration appConfig )
	: appConfig( appConfig )
{
#if defined( _DEBUG )
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 589591 );
#endif

	App::mApp = this;
	App::mConfiguration = appConfig;

	GlobalVar.pApp = this;

	WNDCLASSEXW wcex{ };
	wcex.cbSize = sizeof( wcex );
	wcex.lpfnWndProc = ( WNDPROC )WndProc;
	wcex.lpszClassName = L"SC.Game.Core.Application.CoreWindow";
	wcex.hInstance = GetModuleHandleW( nullptr );

	if ( RegisterClassExW( &wcex ) == 0 )
	{
		throw new Exception( "Cannot initialize window class in Windows Platform." );
	}

	GlobalVar.hWnd = CreateWindowExW( NULL, wcex.lpszClassName, appConfig.AppName.Chars, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT_ALL, nullptr, nullptr, wcex.hInstance, nullptr );
	App::mWndHandle = GlobalVar.hWnd;

	// ���� API �ʱ�ȭ
	App::Initialize();
	InitializeDevice();

	// ����� �׷��� ������� �̸��� �����ɴϴ�.
	auto pAdapter = Graphics::mDevice->pAdapter.Get();
	DXGI_ADAPTER_DESC1 desc;
	pAdapter->GetDesc1( &desc );
	this->appConfig.deviceName = String::Format( "{0} ({1})", ( const wchar_t* )desc.Description, desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ? L"Software" : L"Hardware" );

	mRenderThreadEvent.Set();
}

Application::~Application()
{

}

String Application::ToString()
{
	return appConfig.AppName;
}

int Application::Start( RefPtr<Application> app )
{
#if !defined( _DEBUG )
	try
#endif
	{
		app->frame = new Canvas( "Canvas_root" );

		app->OnStart();
		ShowWindow( GlobalVar.hWnd, SW_SHOW );

		App::Start();
	}
#if !defined( _DEBUG )
	catch ( Exception * e )
	{
		MessageBoxW( GlobalVar.hWnd, e->Message.Chars, e->Name.Chars, MB_OK | MB_ICONERROR );
		delete e;
	}
#endif

	// ���� ����� �� ��� �۾��� �Ϸ�� �������� �˻��մϴ�.
	WaitAllQueues();

	// �� ���� ��û�� �����մϴ�.
	auto ret = app->OnExit();
	AppShutdown = true;

	AssetBundle::Dispose();
	GlobalVar.Release();

	GC.CollectAll();

	return ret;
}

Point<int> Application::ClientSize_get()
{
	RECT rc;
	GetClientRect( GlobalVar.hWnd, &rc );
	return { rc.right - rc.left, rc.bottom - rc.top };
}

AppConfiguration Application::AppConfig_get()
{
	return appConfig;
}

RefPtr<Canvas> Application::Frame_get()
{
	return frame;
}

String Application::AppName_get()
{
	return appConfig.AppName;
}

void Application::AppName_set( String value )
{
	appConfig.AppName = value;
	SetWindowTextW( GlobalVar.hWnd, value.Chars );
}

void Application::InitializeDevice()
{
	GlobalVar.InitializeComponents();
	ShaderBuilder::Initialize();

	LoadSystemFont();

	auto pDevice = Graphics::mDevice->pDevice.Get();

	deviceContextUI = new CDeviceContext( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocators[0] ) ) );
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pCommandAllocators[1] ) ) );
	visibleViewStorage = new VisibleViewStorage( Graphics::mDevice.Get() );

	deviceContextUI->CreateShaderInfoBuffers();
}

void* __stdcall Application::WndProc( void* arg0, uint32 arg1, void* arg2, void* arg3 )
{
	static Drawing::Point<int> previous;

	HWND hWnd = ( HWND )arg0;
	uint32 uMsg = ( UINT )arg1;
	WPARAM wParam = ( WPARAM )arg2;
	LPARAM lParam = ( LPARAM )arg3;

	switch ( uMsg )
	{
		case WM_SIZE:
			GlobalVar.pApp->ResizeBuffers( ( UINT )LOWORD( lParam ), ( UINT )HIWORD( lParam ) );
			break;
		case WM_MOUSEMOVE:
		{
			auto cur = LPARAMToPoint( lParam );
			auto del = cur - previous;
			previous = cur;
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseMove, UI::MouseMoveEventArgs( cur, del ) ) );
			break;
		}
		case WM_LBUTTONDOWN:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::LeftButton, true, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_LBUTTONUP:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::LeftButton, false, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_RBUTTONDOWN:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::RightButton, true, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_RBUTTONUP:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::RightButton, false, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_MBUTTONDOWN:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::MiddleButton, true, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_MBUTTONUP:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( UI::MouseButtonType::MiddleButton, false, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_XBUTTONDOWN:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( HIWORD( wParam ) == XBUTTON1 ? UI::MouseButtonType::X1Button : UI::MouseButtonType::X2Button, true, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_XBUTTONUP:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::MouseClick, UI::MouseClickEventArgs( HIWORD( wParam ) == XBUTTON1 ? UI::MouseButtonType::X1Button : UI::MouseButtonType::X2Button, false, LPARAMToPoint( lParam ) ) ) );
			break;
		case WM_MOUSEWHEEL:
			GlobalVar.scrollDelta.Y += ( double )( short )HIWORD( wParam ) / 120.0;
			break;
		case WM_KEYDOWN:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::KeyboardEvent, UI::KeyboardEventArgs( ( KeyCode )wParam, true ) ) );
			break;
		case WM_KEYUP:
			GlobalVar.pApp->Frame->ProcessEvent( new UI::DispatcherEventArgs( UI::DispatcherEventType::KeyboardEvent, UI::KeyboardEventArgs( ( KeyCode )wParam, false ) ) );
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
	}

	return ( void* )DefWindowProcW( hWnd, uMsg, wParam, lParam );
}

void Application::ResizeBuffers( uint32 width, uint32 height )
{
	if ( width != 0 && height != 0 )
	{
		WaitPrimaryQueue();
		mRenderThreadEvent.WaitForSingleObject();
		mRenderThreadEvent.Set();

		Graphics::mSwapChain->ResizeBuffers( width, height );
		GlobalVar.gameLogic->ResizeBuffers( width, height );

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = ( float )width;
		viewport.Height = ( float )height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

		scissor.left = 0;
		scissor.top = 0;
		scissor.right = width;
		scissor.bottom = height;

		discardApp = false;
	}
	else
	{
		discardApp = true;
	}
}

void Application::IdleProcess()
{
	GlobalVar.frameIndex += 1;
	if ( GlobalVar.frameIndex >= 2 ) GlobalVar.frameIndex = 0;

	// ����� �Ϸ�� �ڿ��� �����մϴ�.
	GC.Collect();

	Update();

	if ( !discardApp ) Render();
}

void Application::Update()
{
	// ��� ��ȯ ��û�� ���� ��� ����� ��ȯ�մϴ�.
	if ( SceneManager::currentScene.Get() )
	{
		mRenderThreadEvent.WaitForSingleObject();
		GlobalVar.gameLogic->currentScene = move( SceneManager::currentScene );
		mRenderThreadEvent.Set();
	}

	GlobalVar.gameLogic->Update();

	DXGI_SWAP_CHAIN_DESC1 desc;
	HR( Graphics::mSwapChain->pSwapChain->GetDesc1( &desc ) );

	frame->Width = ( double )( desc.Width );
	frame->Height = ( double )( desc.Height );
	Drawing::Rect<double> contentRect( 0, 0, frame->Width, frame->Height );
	frame->Update( contentRect );
}

void Application::Render()
{
	int frameIndex = GlobalVar.frameIndex;
	
	mRenderThreadEvent.WaitForSingleObject();

	// ���� ����� ���� �Ϸ�Ǿ����� �˻��մϴ�. �Ϸ���� �ʾ��� ��� ����մϴ�.
	auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
	directQueue->WaitFor( lastPending[frameIndex], waitingHandle );

	ThreadPool::QueueUserWorkItem( [&, frameIndex]( object )
		{
			if ( !discardApp )
			{
				auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
				int frameIndex_ = frameIndex;

				// ���� �۸��� �������� �����մϴ�.
				for ( auto i : GlobalVar.glyphBuffers )
				{
					i->LockGlyphs();
					i->Restart();
				}

				GlobalVar.gameLogic->Render( frameIndex_ );

				// �������� �����ϱ� �� ��ġ�� �ʱ�ȭ�մϴ�.
				visibleViewStorage->Reset();

				HR( pCommandAllocators[frameIndex_]->Reset() );
				deviceContextUI->Reset( directQueue, pCommandAllocators[frameIndex_].Get(), nullptr );
				deviceContextUI->SetVisibleViewStorage( visibleViewStorage );
				deviceContextUI->FrameIndex = frameIndex_;

				// ���� ü���� �ĸ� ���۸� ���� Ÿ������ �����մϴ�.
				auto idx = Graphics::mSwapChain->Index;
				auto pBackBuffer = Graphics::mSwapChain->ppBackBuffers[idx].Get();
				auto rtvHandle = Graphics::mSwapChain->RTVHandle[idx];
				auto pCommandList = deviceContextUI->pCommandList.Get();
				pCommandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

				pCommandList->RSSetViewports( 1, &viewport );
				pCommandList->RSSetScissorRects( 1, &scissor );

				// ���� ������ ���̴��� �ҷ��ɴϴ�.
				ShaderBuilder::IntegratedUIShader_get().SetAll( deviceContextUI );
				ShaderBuilder::TextAndRectShader_get().SetAll( deviceContextUI );

				// �⺻ �Ű������� �Է��մϴ�.
				if ( auto slot = deviceContextUI->Slot["ScreenRes"]; slot != -1 )
				{
					float resolution[2] = { viewport.Width, viewport.Height };
					pCommandList->SetGraphicsRoot32BitConstants( ( UINT )slot, 2, resolution, 0 );
				}

				if ( auto slot = deviceContextUI->Slot["Cursor"]; slot != -1 )
				{
					POINT cursor;
					GetCursorPos( &cursor );
					ScreenToClient( GlobalVar.hWnd, &cursor );
					float resolution[2] = { ( float )cursor.x, ( float )cursor.y };
					pCommandList->SetGraphicsRoot32BitConstants( ( UINT )slot, 2, resolution, 0 );
				}

				// �������� �������մϴ�.
				frame->Render( deviceContextUI );

				// ���� ü���� �ĸ� ���۸� ���� ���·� �����մϴ�.
				deviceContextUI->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, 0 );

				// ��� ����� �ݰ� Ǫ���մϴ�.
				deviceContextUI->Close();
				directQueue->Execute( deviceContextUI );

				Graphics::mSwapChain->Present( appConfig.verticalSync );

				// ������ ��� ��ȣ�� �����մϴ�.
				lastPending[frameIndex_] = directQueue->Signal();
			}

			mRenderThreadEvent.Set();
		}
	, nullptr );
}

void Application::LoadSystemFont()
{
	auto pDWriteFactory = Graphics::mFactory->pDWriteFactory.Get();
	NONCLIENTMETRICS ncm;

	// �ý��� ������ ��ȸ�մϴ�.
	ncm.cbSize = sizeof( ncm );
	SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0 );

	ComPtr<IDWriteGdiInterop> gdiInterop;
	HR( pDWriteFactory->GetGdiInterop( &gdiInterop ) );

	// �ý��� ��Ʈ�� DirectWrite �������� ��ȸ�մϴ�.
	ComPtr<IDWriteFont> sys_font;
	HR( gdiInterop->CreateFontFromLOGFONT( &ncm.lfMessageFont, &sys_font ) );

	// ��Ʈ �йи��� ������ ��Ʈ �йи� �̸��� ��ȸ�մϴ�.
	ComPtr<IDWriteFontFamily> family;
	HR( sys_font->GetFontFamily( &family ) );
	ComPtr<IDWriteLocalizedStrings> font_family_name;
	HR( family->GetFamilyNames( &font_family_name ) );

	// ko-KR ���� ���ڿ� ��ü�� ��ȸ�մϴ�.
	UINT32 index = 0;
	UINT32 length = 0;
	BOOL exists = false;
	HR( font_family_name->FindLocaleName( L"ko-KR", &index, &exists ) );
	HR( font_family_name->GetStringLength( index, &length ) );

	std::vector<wchar_t> name( length + 1 );
	HR( font_family_name->GetString( index, name.data(), length + 1 ) );
	GlobalVar.systemMessageFontName = name.data();

	// �ؽ�Ʈ ���� ��ü�� �����մϴ�.
	GlobalVar.defaultTextFormat = new UI::TextFormat( GlobalVar.systemMessageFontName );
}

void Application::WaitAllQueues()
{
	/*
	RefPtr<Threading::Event> handle = new Threading::Event();

	CCommandQueue* ppCommandQueues[9]
	{
		Graphics::mDevice->DirectQueue[0].Get(),
		Graphics::mDevice->DirectQueue[1].Get(),
		Graphics::mDevice->DirectQueue[2].Get(),
		Graphics::mDevice->DirectQueue[3].Get(),
		Graphics::mDevice->CopyQueue.Get(),
		Graphics::mDevice->ComputeQueue[0].Get(),
		Graphics::mDevice->ComputeQueue[1].Get(),
		Graphics::mDevice->ComputeQueue[2].Get(),
		Graphics::mDevice->ComputeQueue[3].Get(),
	};

	for ( int i = 0; i < ARRAYSIZE( ppCommandQueues ); ++i )
	{
		auto pFence = ppCommandQueues[i]->pFence.Get();
		auto lastPending = ppCommandQueues[i]->LastPending.load();
		if ( pFence->GetCompletedValue() < lastPending )
		{
			HR( pFence->SetEventOnCompletion( lastPending, handle->Handle ) );

			// �ִ� 1�� ����մϴ�. ��⿡ �����Ͽ��� ��� �����մϴ�.
			handle->WaitForSingleObject( 1000 );
		}
	}
	*/
}

void Application::WaitPrimaryQueue()
{
	auto primaryQueue = Graphics::mDevice->DirectQueue[0].Get();
	primaryQueue->WaitFor( primaryQueue->LastPending, waitingHandle );
}