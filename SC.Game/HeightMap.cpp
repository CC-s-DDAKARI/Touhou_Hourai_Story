using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;
using namespace std::filesystem;

bool HeightMap::Lock( RefPtr<CDeviceContext>& deviceContext )
{
	if ( !copySuccessFlag )
	{
		// ���簡 ���� �Ϸ���� �ʾ��� ���,
		if ( auto pFence = GlobalVar.device->DirectQueue[queueIndex]->pFence.Get(); pFence->GetCompletedValue() >= uploadFenceValue )
		{
			copySuccessFlag = true;
			GC.Add( pUploadCommands );
			GC.Add( pUploadHeap );
		}
	}

	return copySuccessFlag;
}

bool HeightMap::IsValid_get()
{
	if ( !copySuccessFlag )
	{
		if ( auto pFence = GlobalVar.device->DirectQueue[queueIndex]->pFence.Get(); pFence->GetCompletedValue() < uploadFenceValue )
		{
			return false;
		}
	}
	return true;
}

HeightMap::~HeightMap()
{
	GC.Add( pTexture2D );
	GC.Add( pShaderResourceView );
	GC.Add( pUploadCommands );
	GC.Add( pUploadHeap );
}

HeightMap::HeightMap( String name, const path& filepath, uint32 width, uint32 height, int queueIndex ) : Assets( name )
	, width( width )
	, height( height )
{
	mHeightBuffer.resize( width * height );
	vector<uint8> buffer( width * height );

	ifstream filedata( filepath, ios::binary );
	if ( filedata.is_open() == false ) throw new Exception( "SC.Game.HeightMap.HeightMap(): ������ ������ �ҷ��� �� �����ϴ�." );
	auto length = filedata.seekg( 0, ios::end ).tellg();
	filedata.seekg( 0 );
	filedata.read( ( char* )buffer.data(), width * height );
	filedata.close();

	for ( uint32 i = 0; i < height; ++i )
	{
		for ( uint32 j = 0; j < width; ++j )
		{
			mHeightBuffer[j * height + ( height - i - 1 )] = { ( uint16 )buffer[i * width + j] };
		}
	}

	InitializeFrom( buffer, queueIndex );
}

uint32 HeightMap::Width_get()
{
	return width;
}

uint32 HeightMap::Height_get()
{
	return height;
}

void HeightMap::InitializeFrom( vector<uint8>& buffer, int queueIndex )
{
	auto& pDevice = *GlobalVar.device->pDevice.Get();

	// �ؽ�ó ������ �����մϴ�.
	D3D12_RESOURCE_DESC textureDesc{ };
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UNORM;
	textureDesc.SampleDesc = { 1, 0 };

	// �ؽ�ó ������ ������� ������ ũ�⸦ ��ȸ�մϴ�.
	UINT64 totalSizeInBytes;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	pDevice.GetCopyableFootprints( &textureDesc, 0, 1, 0, &layout, nullptr, nullptr, &totalSizeInBytes );

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
	D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };
	HR( pDevice.CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeap ) ) );

	// ���� �ؽ�ó �ڿ��� �����մϴ�.
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	HR( pDevice.CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &pTexture2D ) ) );

	// ���ε� ���� �ؽ�ó �����͸� �����մϴ�.
	void* pBlock = nullptr;
	HR( pUploadHeap->Map( 0, nullptr, &pBlock ) );
	for ( uint32 i = 0; i < height; ++i )
	{
		char* pBlockOffset = ( char* )pBlock;
		pBlockOffset += layout.Footprint.RowPitch * i;
		memcpy( pBlockOffset, buffer.data() + width * i, width );
	}
	pUploadHeap->Unmap( 0, nullptr );

	// �ؽ�ó ���� ��� �Ҵ��� ���� ��� �Ҵ�⸦ �غ��մϴ�.
	HR( pDevice.CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &pUploadCommands ) ) );
	var deviceContext = new CDeviceContext( GlobalVar.device, D3D12_COMMAND_LIST_TYPE_DIRECT, pUploadCommands.Get() );

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
	deviceContext->TransitionBarrier( pTexture2D.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0 );

	// ��� ����� �ݰ� ����� ī�� ť�� �����մϴ�.
	deviceContext->Close();
	GlobalVar.device->DirectQueue[queueIndex]->Execute( deviceContext );
	uploadFenceValue = GlobalVar.device->CopyQueue->Signal();

	// ���̴� �ڿ� �����ڸ� �����մϴ�.
	pShaderResourceView = GlobalVar.device->CreateShaderResourceView( pTexture2D.Get(), nullptr );

	this->queueIndex = queueIndex;
}