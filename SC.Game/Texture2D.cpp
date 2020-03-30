using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace std::filesystem;

bool Texture2D::Lock( RefPtr<CDeviceContext>& deviceContext )
{
	if ( !copySuccessFlag )
	{
		// 복사가 아직 완료되지 않았을 경우,
		if ( auto pFence = Graphics::mDevice->CopyQueue->pFence.Get(); pFence->GetCompletedValue() >= uploadFenceValue )
		{
			copySuccessFlag = true;
		}
	}

	return copySuccessFlag;
}

Texture2D::~Texture2D()
{
	GC::Add( App::mFrameIndex, pTexture2D.Get(), 2 );
	GC::Add( App::mFrameIndex, pShaderResourceView.Get(), 2 );
}

Texture2D::Texture2D( String name, void* textureData, uint32 sizeInBytes, TextureFormat format, int queueIndex ) : Assets( name )
{
	auto pImagingFactory = Graphics::mFactory->pWICFactory.Get();

	ComPtr<IWICStream> pStream;
	ComPtr<IWICBitmapDecoder> pDecoder;

	// 스트림을 열어 메모리 데이터를 불러옵니다.
	HR( pImagingFactory->CreateStream( &pStream ) );
	HR( pStream->InitializeFromMemory( ( WICInProcPointer )textureData, sizeInBytes ) );

	// 스트림 데이터를 기반으로 이미지 디코더를 생성합니다.
	HR( pImagingFactory->CreateDecoderFromStream( pStream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &pDecoder ) );

	// 디코더 개체를 이용하여 이미지 개체를 초기화합니다.
	InitializeFrom( pDecoder.Get(), format, queueIndex );
}

Texture2D::Texture2D( String name, const path& filepath, TextureFormat format, int queueIndex ) : Assets( name )
{
	auto pImagingFactory = Graphics::mFactory->pWICFactory.Get();
	ComPtr<IWICBitmapDecoder> pDecoder;

	// 파일 데이터를 기반으로 이미지 디코더를 생성합니다.
	HR( pImagingFactory->CreateDecoderFromFilename( filepath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder ) );

	// 디코더 개체를 이용하여 이미지 개체를 초기화합니다.
	InitializeFrom( pDecoder.Get(), format, queueIndex );
}

uint32 Texture2D::Width_get()
{
	return width;
}

uint32 Texture2D::Height_get()
{
	return height;
}

void Texture2D::InitializeFrom( IWICBitmapDecoder* pDecoder, TextureFormat format, int queueIndex )
{
	auto pWICImagingFactory = Graphics::mFactory->pWICFactory.Get();
	auto pDevice = Graphics::mDevice->pDevice.Get();
	ComPtr<IWICBitmapFrameDecode> pFrame;
	ComPtr<IWICFormatConverter> pConverter;

	GUID wicPixelFormat;
	DXGI_FORMAT dxgiPixelFormat;

	switch ( format )
	{
	case TextureFormat::PRGBA_32bpp:
		wicPixelFormat = GUID_WICPixelFormat32bppPRGBA;
		dxgiPixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case TextureFormat::Gray_8bpp:
		wicPixelFormat = GUID_WICPixelFormat8bppGray;
		dxgiPixelFormat = DXGI_FORMAT_R8_UNORM;
		break;
	default:
		throw new ArgumentException();
	}

	// 이미지의 0번 프레임을 얻어옵니다.
	HR( pDecoder->GetFrame( 0, &pFrame ) );

	// 컨버터 개체를 생성해 이미지의 형식을 변환합니다.
	HR( pWICImagingFactory->CreateFormatConverter( &pConverter ) );
	HR( pConverter->Initialize( pFrame.Get(), wicPixelFormat, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom ) );

	// 이미지의 크기를 조회합니다.
	UINT width, height;
	HR( pConverter->GetSize( &width, &height ) );

	// 텍스처 정보를 생성합니다.
	D3D12_RESOURCE_DESC textureDesc{ };
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = dxgiPixelFormat;
	textureDesc.SampleDesc = { 1, 0 };

	// 텍스처 정보를 기반으로 버퍼의 크기를 조회합니다.
	UINT64 totalSizeInBytes;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	pDevice->GetCopyableFootprints( &textureDesc, 0, 1, 0, &layout, nullptr, nullptr, &totalSizeInBytes );

	// 버퍼 정보를 생성합니다.
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = totalSizeInBytes;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc = { 1, 0 };
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 업로드 힙 자원을 생성합니다.
	ComPtr<ID3D12Resource> pUploadHeap;
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeap ) ) );

	// 실제 텍스처 자원을 생성합니다.
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS( &pTexture2D ) ) );

	// 업로드 힙에 텍스처 데이터를 복사합니다.
	void* pBlock = nullptr;
	HR( pUploadHeap->Map( 0, nullptr, &pBlock ) );
	HR( pConverter->CopyPixels( nullptr, ( UINT )layout.Footprint.RowPitch, ( UINT )totalSizeInBytes, ( BYTE* )pBlock ) );
	pUploadHeap->Unmap( 0, nullptr );

	// 텍스처 복사 명령 할당을 위해 명령 할당기를 준비합니다.
	ComPtr<ID3D12CommandAllocator> pUploadCommands;
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS( &pUploadCommands ) ) );
	var deviceContext = new CDeviceContext( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_COPY, pUploadCommands.Get() );

	D3D12_TEXTURE_COPY_LOCATION dst{ };
	auto src = dst;

	// 복사 대상 지점 정보를 서술합니다.
	dst.pResource = pTexture2D.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// 복사 원본 지점 정보를 서술합니다.
	src.pResource = pUploadHeap.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = layout;

	// 복사 명령을 추가합니다.
	deviceContext->pCommandList->CopyTextureRegion( &dst, 0, 0, 0, &src, nullptr );

	// 명령 목록을 닫고 명령을 카피 큐에 제출합니다.
	deviceContext->Close();
	Graphics::mDevice->CopyQueue->Execute( deviceContext );
	uploadFenceValue = Graphics::mDevice->CopyQueue->Signal();

	// 셰이더 자원 서술자를 생성합니다.
	pShaderResourceView = Graphics::mDevice->CreateShaderResourceView( pTexture2D.Get(), nullptr );

	GC::Add( Graphics::mDevice->CopyQueue->pFence.Get(), uploadFenceValue, pUploadHeap.Get() );
	GC::Add( Graphics::mDevice->CopyQueue->pFence.Get(), uploadFenceValue, pUploadCommands.Get() );

#if defined( _DEBUG )
	pTexture2D->SetName( L"Texture2D.pTexture2D" );
	pUploadHeap->SetName( L"Texture2D.pUploadHeap" );
	pUploadCommands->SetName( L"Texture2D.pUploadCommands" );
#endif

	this->width = width;
	this->height = height;
	this->queueIndex = queueIndex;
}