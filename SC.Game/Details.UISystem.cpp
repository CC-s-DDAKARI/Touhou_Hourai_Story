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

RefPtr<TextFormat> UISystem::mDefaultTextFormat;
set<GlyphBuffer*> UISystem::mGlyphBuffers;

bool UISystem::mDisposed;

Drawing::Point<double> UISystem::mScrollDelta;

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

	mDefaultTextFormat = new TextFormat( "" );

	mDisposed = false;
}

void UISystem::Update()
{
	mRootCanvas->Update( Rect<double>( 0, 0, ( double )mViewport.Width, ( double )mViewport.Height ) );

	// 매 프레임 업데이트가 끝난 후 스크롤 이동량을 초기화합니다.
	UISystem::mScrollDelta = { 0, 0 };
}

void UISystem::Render( int frameIndex )
{
	auto directQueue = Graphics::mDevice->DirectQueue[0].Get();
	auto pCommandQueue = directQueue->pCommandQueue.Get();

	// 글리프 렌더링을 시작합니다.
	for ( auto i : mGlyphBuffers )
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

	// 스왑 체인의 후면 버퍼를 렌더 타겟으로 설정합니다.
	auto idx = Graphics::mSwapChain->Index;
	auto pBackBuffer = Graphics::mSwapChain->ppBackBuffers[idx].Get();
	auto rtvHandle = Graphics::mSwapChain->RTVHandle[idx];
	
	pCommandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );
	pCommandList->RSSetViewports( 1, &mViewport );
	pCommandList->RSSetScissorRects( 1, &mScissorRect );

	// 기본 매개변수를 입력합니다.
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

	// 루트 프레임을 렌더링합니다.
	mRootCanvas->Render( mDeviceContext );

	// 스왑 체인의 후면 버퍼를 원래 상태로 복구합니다.
	mDeviceContext->TransitionBarrier( pBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, 0 );

	// 명령 목록을 닫고 푸쉬합니다.
	mDeviceContext->Close();
	directQueue->Execute( mDeviceContext );
	Graphics::mSwapChain->Present( App::mConfiguration.VSync | GameLogic::mCurrentScene->IsVSyncUpdate );
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

	mDefaultTextFormat = nullptr;

	mDisposed = true;
}