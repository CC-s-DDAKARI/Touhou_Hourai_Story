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

	// �߰� ������ �ʿ��� ��� ���ο� �����͸� �Ҵ��մϴ�.
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