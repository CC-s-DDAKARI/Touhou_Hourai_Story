using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

HeapAllocator1::HeapAllocator1( CDevice* device, int alignOf, int heapCount ) : Object()
	, alignOf( alignOf )
	, heapCount( heapCount )
	, pDevice( device->pDevice )
{

}

int HeapAllocator1::Alloc( D3D12_GPU_VIRTUAL_ADDRESS& virtualAddress, void*& pBlock )
{
	lock_guard<mutex> lock( locker );

	// 추가 공간이 필요할 경우 새로운 데이터를 할당합니다.
	auto idx = SearchIndex();
	if ( idx == -1 )
	{
		D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_UPLOAD };

		D3D12_RESOURCE_DESC bufferDesc{ };
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = alignOf * heapCount;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.SampleDesc = { 1, 0 };
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		void* block;

		ComPtr<ID3D12Resource> pDynamicBuffer;
		HR( pDevice->CreateCommittedResource( &heapProp, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &pDynamicBuffer ) ) );
		HR( pDynamicBuffer->Map( 0, nullptr, &block ) );
		this->virtualAddress.push_back( pDynamicBuffer->GetGPUVirtualAddress() );
		this->pBlock.push_back( ( char* )block );

		idx = ( int )pDynamicBuffers.size() * heapCount;
		pDynamicBuffers.push_back( pDynamicBuffer );

		auto q = queue<int>();
		for ( int i = 0; i < ( int )heapCount - 1; ++i )
		{
			q.push( i + 1 );
		}

		allocQueue.emplace_back( move( q ) );
	}

	int heapIndex = idx / heapCount;
	int allocIndex = idx % heapCount;

	virtualAddress = this->virtualAddress[heapIndex] + allocIndex * alignOf;
	pBlock = ( void* )( this->pBlock[heapIndex] + allocIndex * alignOf );

	return idx;
}

void HeapAllocator1::Free( int index )
{
	lock_guard<mutex> lock( locker );

	int heapIndex = index / heapCount;
	int allocIndex = index % heapCount;

	allocQueue[heapIndex].push( allocIndex );
}

int HeapAllocator1::SearchIndex()
{
	for ( int i = 0, count = ( int )allocQueue.size(); i < count; ++i )
	{
		auto& queue = allocQueue[i];

		if ( !queue.empty() )
		{
			auto front = queue.front();
			queue.pop();
			return i * heapCount + front;
		}
	}

	return -1;
}