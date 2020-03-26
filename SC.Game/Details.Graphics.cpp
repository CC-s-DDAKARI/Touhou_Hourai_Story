using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

RefPtr<IntegratedFactory> Graphics::mFactory;
RefPtr<CDevice> Graphics::mDevice;
RefPtr<CSwapChain> Graphics::mSwapChain;

void Graphics::Initialize()
{
	Application::AppDisposing += Dispose;

	// 기본 그래픽 디바이스를 생성합니다.
	mFactory = new IntegratedFactory();
	mDevice = new CDevice( mFactory->SearchHardwareAdapter().Get() );
	mSwapChain = new CSwapChain();
}

void Graphics::Dispose( object sender )
{
	mSwapChain = nullptr;
	mDevice = nullptr;
	mFactory = nullptr;
}