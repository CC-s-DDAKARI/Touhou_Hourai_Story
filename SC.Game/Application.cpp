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
	App::mConfiguration.deviceName = String::Format( "{0} ({1})", ( const wchar_t* )desc.Description, desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ? L"Software" : L"Hardware" );
}

Application::~Application()
{

}

String Application::ToString()
{
	return App::mConfiguration.AppName;
}

int Application::Start( RefPtr<Application> app )
{
	App::Start();

	// 앱 종료 요청을 수행합니다.
	auto ret = app->OnExit();
	AppShutdown = true;

	AssetBundle::Dispose();

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
	return App::mConfiguration;
}

RefPtr<Canvas> Application::Frame_get()
{
	return UISystem::mRootCanvas;
}

String Application::AppName_get()
{
	return App::mConfiguration.AppName;
}

void Application::AppName_set( String value )
{
	App::mConfiguration.AppName = value;
	SetWindowTextW( App::mWndHandle, value.Chars );
}