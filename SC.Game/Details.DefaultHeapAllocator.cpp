using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

DefaultHeapAllocator::DefaultHeapAllocator( CDevice* device, int alignOf, int heapCount ) : Object()
	, mAlignOf( alignOf )
	, mHeapCount( heapCount )
	, mDevice( device->pDevice )
{

}

int DefaultHeapAllocator::Alloc( D3D12_GPU_VIRTUAL_ADDRESS& virtualAddress )
{
	lock_guard<mutex> lock( mLocker );

	// 추가 공간이 필요한 경우 새로운 데이터를 할당합니다.
	auto idx = SearchIndex();
	if ( idx == -1 )
	{
		D3D12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };

		D3D12_RESOURCE_DESC bufferDesc{ };
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = mAlignOf * mHeapCount;
	}

	throw NotImplementedException();
}