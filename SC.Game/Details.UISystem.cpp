using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Drawing;
using namespace SC::Game::UI;

using namespace std;

RefPtr<Canvas> UISystem::mRootCanvas;
RefPtr<VisibleViewStorage> UISystem::mViewStorage;
RefPtr<CDeviceContext> UISystem::mDeviceContext;

D3D12_VIEWPORT UISystem::mViewport;
D3D12_RECT UISystem::mScissorRect;

void UISystem::Initialize()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	App::Disposing += Dispose;
	App::Resizing += ResizeApp;

	mRootCanvas = new Canvas( "UISystem.mRootCanvas" );
	mViewStorage = new VisibleViewStorage();
	mDeviceContext = new CDeviceContextAndAllocator( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_DIRECT );
	mDeviceContext->CreateShaderInfoBuffers();

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissorRect.left = 0;
	mScissorRect.top = 0;
}

void UISystem::Update()
{
	mRootCanvas->Update( Rect<double>( 0, 0, ( double )mViewport.Width, ( double )mViewport.Height ) );
}

void UISystem::Render( int frameIndex )
{
	auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
	auto pCommandQueue = directQueue->pCommandQueue.Get();

	// �۸��� �������� �����մϴ�.
	for ( auto i : GlobalVar.glyphBuffers )
	{
		i->LockGlyphs();
		i->Restart();
	}

	mDeviceContext->FrameIndex = frameIndex;
	mDeviceContext->Reset( directQueue, ( ID3D12CommandAllocator* )&frameIndex, ShaderBuilder::TextAndRectShader_get().PipelineState.Get() );
	ShaderBuilder::IntegratedUIShader_get().SetAll( mDeviceContext );

	mViewStorage->Reset();
	mDeviceContext->SetVisibleViewStorage( mViewStorage );

	auto pCommandList = mDeviceContext->pCommandList.Get();

	// ���� ü���� �ĸ� ���۸� ���� Ÿ������ �����մϴ�.
	auto idx = Graphics::mSwapChain->Index;
	auto pBackBuffer = Graphics::mSwapChain->ppBackBuffers[idx].Get();
	auto rtvHandle = Graphics::mSwapChain->RTVHandle[idx];
	
	pCommandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );
	pCommandList->RSSetViewports( 1, &mViewport );
	pCommandList->RSSetScissorRects( 1, &mScissorRect );

	// �⺻ �Ű������� �Է��մϴ�.
	if ( auto slot = mDeviceContext->Slot["ScreenRes"]; slot != -1 )
	{
		float resolution[2] = { mViewport.Width, mViewport.Height };
		pCommandList->SetGraphicsRoot32BitConstants( ( UINT )slot, 2, resolution, 0 );
	}

	if ( auto slot = mDeviceContext->Slot["Cursor"]; slot != -1 )
	{
		POINT cursor;
		GetCursorPos( &cursor );
		ScreenToClient( App::mWndHandle, &cursor );
		float resolution[2] = { ( float )cursor.x, ( float )cursor.y };
		pCommandList->SetGraphicsRoot32BitConstants( ( UINT )slot, 2, resolution, 0 );
	}

	// ��Ʈ �������� �������մϴ�.
	mRootCanvas->Render( mDeviceContext );

	// ���� ü���� �ĸ� ���۸� ���� ���·� �����մϴ�.
	mDeviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, 0 );

	// ��� ����� �ݰ� Ǫ���մϴ�.
	mDeviceContext->Close();
	directQueue->Execute( mDeviceContext );
	Graphics::mSwapChain->Present( App::mConfiguration.VSync );
}

void UISystem::ResizeApp( object sender, Point<int> size )
{
	mViewport.Width = ( float )size.X;
	mViewport.Height = ( float )size.Y;

	mScissorRect.right = size.X;
	mScissorRect.bottom = size.Y;

	mRootCanvas->Width = ( double )mViewport.Width;
	mRootCanvas->Height = ( double )mViewport.Height;
}

void UISystem::Dispose( object sender )
{
	mRootCanvas = nullptr;
	mViewStorage = nullptr;
	mDeviceContext = nullptr;
}