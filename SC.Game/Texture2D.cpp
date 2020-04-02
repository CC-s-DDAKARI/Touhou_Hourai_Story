using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace std::filesystem;

bool Texture2D::Lock( RefPtr<CDeviceContext>& deviceContext )
{
	if ( !copySuccessFlag )
	{
		// ���簡 ���� �Ϸ���� �ʾ��� ���,
		if ( auto pFence = Graphics::mDevice->CopyQueue->pFence.Get(); pFence->GetCompletedValue() >= uploadFenceValue )
		{
			copySuccessFlag = true;
		}
	}

	return copySuccessFlag;
}

Texture2D::~Texture2D()
{
	GC::Add( App::mFrameIndex, pTexture2D.Get(), 5 );
	GC::Add( App::mFrameIndex, pShaderResourceView.Get(), 5 );
}

Texture2D::Texture2D( String name, void* textureData, uint32 sizeInBytes, TextureFormat format, int queueIndex ) : Assets( name )
{
	auto pImagingFactory = Graphics::mFactory->pWICFactory.Get();

	ComPtr<IWICStream> pStream;
	ComPtr<IWICBitmapDecoder> pDecoder;

	// ��Ʈ���� ���� �޸� �����͸� �ҷ��ɴϴ�.
	HR( pImagingFactory->CreateStream( &pStream ) );
	HR( pStream->InitializeFromMemory( ( WICInProcPointer )textureData, sizeInBytes ) );

	// ��Ʈ�� �����͸� ������� �̹��� ���ڴ��� �����մϴ�.
	HR( pImagingFactory->CreateDecoderFromStream( pStream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &pDecoder ) );

	// ���ڴ� ��ü�� �̿��Ͽ� �̹��� ��ü�� �ʱ�ȭ�մϴ�.
	InitializeFrom( pDecoder.Get(), format, queueIndex );
}

Texture2D::Texture2D( String name, const path& filepath, TextureFormat format, int queueIndex ) : Assets( name )
{
	auto pImagingFactory = Graphics::mFactory->pWICFactory.Get();
	ComPtr<IWICBitmapDecoder> pDecoder;

	// ���� �����͸� ������� �̹��� ���ڴ��� �����մϴ�.
	HR( pImagingFactory->CreateDecoderFromFilename( filepath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder ) );

	// ���ڴ� ��ü�� �̿��Ͽ� �̹��� ��ü�� �ʱ�ȭ�մϴ�.
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

	// �̹����� 0�� �������� ���ɴϴ�.
	HR( pDecoder->GetFrame( 0, &pFrame ) );

	// ������ ��ü�� ������ �̹����� ������ ��ȯ�մϴ�.
	HR( pWICImagingFactory->CreateFormatConverter( &pConverter ) );
	HR( pConverter->Initialize( pFrame.Get(), wicPixelFormat, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom ) );

	// �̹����� ũ�⸦ ��ȸ�մϴ�.
	UINT width, height;
	HR( pConverter->GetSize( &width, &height ) );

	// �ؽ�ó ������ �����մϴ�.
	D3D12_RESOURCE_DESC textureDesc{ };
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = dxgiPixelFormat;
	textureDesc.SampleDesc = { 1, 0 };

	// �ؽ�ó ������ ������� ������ ũ�⸦ ��ȸ�մϴ�.
	UINT64 totalSizeInBytes;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	pDevice->GetCopyableFootprints( &textureDesc, 0, 1, 0, &layout, nullptr, nullptr, &totalSizeInBytes );

	// ���� ������ �����մϴ�.
	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = totalSizeInBytes;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc = { 1, 0 };
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���ε� �� �ڿ��� �����մϴ�.
	ComPtr<ID3D12Resource> pUploadHeap;
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeap ) ) );

	// ���� �ؽ�ó �ڿ��� �����մϴ�.
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS( &pTexture2D ) ) );

	// ���ε� ���� �ؽ�ó �����͸� �����մϴ�.
	void* pBlock = nullptr;
	HR( pUploadHeap->Map( 0, nullptr, &pBlock ) );
	HR( pConverter->CopyPixels( nullptr, ( UINT )layout.Footprint.RowPitch, ( UINT )totalSizeInBytes, ( BYTE* )pBlock ) );
	pUploadHeap->Unmap( 0, nullptr );

	// �ؽ�ó ���� ��� �Ҵ��� ���� ��� �Ҵ�⸦ �غ��մϴ�.
	ComPtr<ID3D12CommandAllocator> pUploadCommands;
	HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS( &pUploadCommands ) ) );
	var deviceContext = new CDeviceContext( Graphics::mDevice, D3D12_COMMAND_LIST_TYPE_COPY, pUploadCommands.Get() );

	D3D12_TEXTURE_COPY_LOCATION dst{ };
	auto src = dst;

	// ���� ��� ���� ������ �����մϴ�.
	dst.pResource = pTexture2D.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// ���� ���� ���� ������ �����մϴ�.
	src.pResource = pUploadHeap.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = layout;

	// ���� ����� �߰��մϴ�.
	deviceContext->pCommandList->CopyTextureRegion( &dst, 0, 0, 0, &src, nullptr );

	// ��� ����� �ݰ� ����� ī�� ť�� �����մϴ�.
	deviceContext->Close();
	Graphics::mDevice->CopyQueue->Execute( deviceContext );
	uploadFenceValue = Graphics::mDevice->CopyQueue->Signal();

	// ���̴� �ڿ� �����ڸ� �����մϴ�.
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