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

	// 내부 API 초기화
	App::Initialize();

	// 사용한 그래픽 어댑터의 이름을 가져옵니다.
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

	// 앱 종료 요청을 수행합니다.
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

	// 사용이 완료된 자원을 수집합니다.
	GC.Collect();

	Update();

	if ( !discardApp ) Render();
}

void Application::Update()
{
	// 장면 전환 요청이 있을 경우 장면을 전환합니다.
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

	// 이전 명령이 실행 완료되었는지 검사합니다. 완료되지 않았을 경우 대기합니다.
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

				// 마지막 명령 번호를 저장합니다.
				lastPending[frameIndex_] = directQueue->Signal();
			}

			mRenderThreadEvent.Set();
		}
	, nullptr );
}