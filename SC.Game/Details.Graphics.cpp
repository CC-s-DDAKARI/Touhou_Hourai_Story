using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Drawing;

using namespace std;

RefPtr<IntegratedFactory> Graphics::mFactory;
RefPtr<CDevice> Graphics::mDevice;
RefPtr<CSwapChain> Graphics::mSwapChain;

set<GlyphBuffer*> Graphics::mGlyphBuffers;

void Graphics::Initialize()
{
	App::Disposing += Dispose;
	App::Resizing += ResizeApp;

	// �⺻ �׷��� ����̽��� �����մϴ�.
	mFactory = new IntegratedFactory();
	mDevice = new CDevice( mFactory->SearchHardwareAdapter().Get() );
	mSwapChain = new CSwapChain();
}

void Graphics::Dispose( object sender )
{
	mGlyphBuffers.clear();

	mSwapChain = nullptr;
	mDevice = nullptr;
	mFactory = nullptr;
}

void Graphics::ResizeApp( object sender, Drawing::Point<int> size )
{
	mSwapChain->ResizeBuffers( size.X, size.Y );
}