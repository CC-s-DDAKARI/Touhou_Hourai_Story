using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

CBuffer::CBuffer( RefPtr<CDevice>& device, uint64 sizeInBytes, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS resourceFlags, const void* pInitialData, uint64 initialDataSize, int queueIndex ) : Object()
, initialState( initialState )
, deviceRef( device )
{
	// 초기 데이터가 있으면 복사 상태로, 없으면 초기 상태로 생성합니다.
	auto createState = pInitialData ? D3D12_RESOURCE_STATE_COPY_DEST : initialState;
	auto pDevice = device->pDevice.Get();

	D3D12_HEAP_PROPERTIES heapProp{ };
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC bufferDesc{ };
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = sizeInBytes;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.SampleDesc = { 1, 0 };
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = resourceFlags;

	// 버퍼 자원을 생성합니다.
	HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS( &pResource ) ) );

	if ( pInitialData )
	{
		ComPtr<ID3D12Resource> pUploadHeap;
		ComPtr<ID3D12CommandAllocator> pUploadCommands;

		// 초기 데이터가 지정되었지만 크기가 지정되지 않았을 경우 버퍼의 크기로 설정합니다.
		if ( initialDataSize == 0 ) initialDataSize = sizeInBytes;

		// 초기 데이터를 복사할 업로드 공간을 생성합니다.
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pUploadHeap ) ) );

		// 데이터를 업로드 힙에 복사합니다.
		void* pBlock;
		HR( pUploadHeap->Map( 0, nullptr, &pBlock ) );
		memcpy( pBlock, pInitialData, ( size_t )initialDataSize );
		pUploadHeap->Unmap( 0, nullptr );

		// 데이터 업로드 명령을 수행할 명령 목록을 구성합니다.
		HR( pDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS( &pUploadCommands ) ) );
		var deviceContext = new CDeviceContext( device, D3D12_COMMAND_LIST_TYPE_COPY, pUploadCommands.Get() );
		deviceContext->pCommandList->CopyResource( pResource.Get(), pUploadHeap.Get() );

		// 디바이스의 복사 큐에 명령을 제출합니다.
		deviceContext->Close();
		device->CopyQueue->Execute( deviceContext );
		uploadFenceValue = device->CopyQueue->Signal();

		// 복사 명령이 완료될 때까지 리소스를 해제하지 않습니다.
		GC::Add( device->CopyQueue->pFence.Get(), uploadFenceValue, pUploadCommands.Get() );
		GC::Add( device->CopyQueue->pFence.Get(), uploadFenceValue, pUploadHeap.Get() );

		copySuccessFlag = false;
	}
}

CBuffer::~CBuffer()
{
	GC::Add( App::mFrameIndex, pResource.Get(), 5 );
}

bool CBuffer::Lock( RefPtr<CDeviceContext>& deviceContext, bool sync )
{
	if ( !copySuccessFlag )
	{
		// 복사가 완료되었을 경우 임시 버퍼를 해제합니다.
		if ( auto pFence = deviceRef->CopyQueue->pFence.Get(); pFence->GetCompletedValue() >= uploadFenceValue )
		{
			copySuccessFlag = true;
		}
	}

	return copySuccessFlag;
}