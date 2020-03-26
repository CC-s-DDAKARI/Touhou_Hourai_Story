using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Drawing;

using namespace std;

RefPtr<IntegratedFactory> Graphics::mFactory;
RefPtr<CDevice> Graphics::mDevice;
RefPtr<CSwapChain> Graphics::mSwapChain;

void Graphics::Initialize()
{
	App::AppDisposing += Dispose;
	App::AppResizing += Resize;

	// 기본 그래픽 디바이스를 생성합니다.
	mFactory = new IntegratedFactory();
	mDevice = new CDevice( mFactory->SearchHardwareAdapter().Get() );
	mSwapChain = new CSwapChain();
}

void Graphics::Resize( object sender, Point<int> size )
{

}

void Graphics::Dispose( object sender )
{
	mSwapChain = nullptr;
	mDevice = nullptr;
	mFactory = nullptr;
}