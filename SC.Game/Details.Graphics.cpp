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

	// �⺻ �׷��� ����̽��� �����մϴ�.
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