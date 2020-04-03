using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

void BottomLevelAccelerationStructure::BuildRaytracingAccelerationStructure( RefPtr<Details::CDeviceContext>& deviceContext )
{
	if ( !mBuilded )
	{
		auto pCommandList = deviceContext->pCommandList.Get();

		// ����Ʈ���̽� �Է� �����͸� �����մϴ�.
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{ };
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
		inputs.NumDescs = ( uint )mTriangles.size();
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.pGeometryDescs = mTriangles.data();

		// ����Ʈ���̽� ���� ������ �����մϴ�.
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc{ };
		buildDesc.Inputs = inputs;
		buildDesc.ScratchAccelerationStructureData = mScratch->GetGPUVirtualAddress();
		buildDesc.DestAccelerationStructureData = mAccel->GetGPUVirtualAddress();

		pCommandList->BuildRaytracingAccelerationStructure( &buildDesc, 0, nullptr );
	}
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure()
{

}

object BottomLevelAccelerationStructure::Clone()
{
	return new BottomLevelAccelerationStructure();
}

BottomLevelAccelerationStructure::~BottomLevelAccelerationStructure()
{
	GC::Add( App::mFrameIndex, mAccel.Get(), 4 );
}

void BottomLevelAccelerationStructure::Update( Time& time, Input& input )
{

}

void BottomLevelAccelerationStructure::InitializeBuffers()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	vector<D3D12_RAYTRACING_GEOMETRY_DESC> triangles( mMeshRenderers.size() );

	// �޽� �������� ���ϴ� ��� �޽� �������� �ﰢ�� ������ �����մϴ�.
	for ( size_t i = 0; i < triangles.size(); ++i )
	{
		auto meshFilter = mMeshRenderers[i]->GetComponent<MeshFilter>();
		auto mesh = meshFilter->Mesh.Get();

		triangles[i].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		triangles[i].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		triangles[i].Triangles = mesh->mTriangleDesc;
	}

	mTriangles = move( triangles );

	// ����Ʈ���̽� ������ ũ�⸦ ��ȸ�մϴ�.
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{ };
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = ( uint )mTriangles.size();
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.pGeometryDescs = mTriangles.data();
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo;
	pDevice->GetRaytracingAccelerationStructurePrebuildInfo( &inputs, &prebuildInfo );

	// ����Ʈ���̽� ���۸� �����մϴ�.
	mScratch = Graphics::mDevice->CreateBuffer( prebuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
	mAccel = Graphics::mDevice->CreateBuffer( prebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );

	mBuilded = false;
}