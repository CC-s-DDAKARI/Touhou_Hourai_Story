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
	if ( SceneManager::mCurrentScene.Get() )
	{
		mRenderThreadEvent.WaitForSingleObject();
		GameLogic::mCurrentScene = move( SceneManager::mCurrentScene );
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