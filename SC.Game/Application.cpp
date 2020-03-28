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

	// ���� API �ʱ�ȭ
	App::Initialize();

	App::Resizing += [&]( auto sender, auto size )
	{
		ResizeBuffers( ( uint )size.X, ( uint )size.Y );
	};

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
	App::Start();

	// ���� ����� �� ��� �۾��� �Ϸ�� �������� �˻��մϴ�.
	WaitAllQueues();

	// �� ���� ��û�� �����մϴ�.
	auto ret = app->OnExit();
	AppShutdown = true;

	AssetBundle::Dispose();

	GC.CollectAll();

	return ret;
}

Point<int> Application::ClientSize_get()
{
	RECT rc;
	GetClientRect( App::mWndHandle, &rc );
	return { rc.right - rc.left, rc.bottom - rc.top };
}

AppConfiguration Application::AppConfig_get()
{
	return appConfig;
}

RefPtr<Canvas> Application::Frame_get()
{
	return UISystem::mRootCanvas;
}

String Application::AppName_get()
{
	return appConfig.AppName;
}

void Application::AppName_set( String value )
{
	appConfig.AppName = value;
	SetWindowTextW( App::mWndHandle, value.Chars );
}

void Application::ResizeBuffers( uint32 width, uint32 height )
{
	if ( width != 0 && height != 0 )
	{
		WaitPrimaryQueue();
		mRenderThreadEvent.WaitForSingleObject();
		mRenderThreadEvent.Set();

		Graphics::mSwapChain->ResizeBuffers( width, height );

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
		GameLogic::mCurrentScene = move( SceneManager::currentScene );
		mRenderThreadEvent.Set();
	}

	GameLogic::Update();
	UISystem::Update();
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

				GameLogic::Render( frameIndex_ );
				UISystem::Render( frameIndex_ );

				// ������ ��� ��ȣ�� �����մϴ�.
				lastPending[frameIndex_] = directQueue->Signal();
			}

			mRenderThreadEvent.Set();
		}
	, nullptr );
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