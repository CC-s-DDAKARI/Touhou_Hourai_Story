using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

void BottomLevelAccelerationStructure::BuildRaytracingAccelerationStructure( RefPtr<Details::CDeviceContext>& deviceContext )
{
	if ( !mBuilded )
	{
		auto pCommandList = deviceContext->pCommandList.Get();

		// 레이트레이싱 입력 데이터를 전달합니다.
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{ };
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
		inputs.NumDescs = ( uint )mTriangles.size();
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.pGeometryDescs = mTriangles.data();

		// 레이트레이싱 빌드 정보를 설정합니다.
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
	GC::Add( App::mFrameIndex, move( mAccel ), 4 );
}

void BottomLevelAccelerationStructure::Update( Time& time, Input& input )
{

}

void BottomLevelAccelerationStructure::InitializeBuffers()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	vector<D3D12_RAYTRACING_GEOMETRY_DESC> triangles( mMeshRenderers.size() );

	// 메쉬 렌더러에 속하는 모든 메쉬 데이터의 삼각형 정보를 저장합니다.
	for ( size_t i = 0; i < triangles.size(); ++i )
	{
		auto meshFilter = mMeshRenderers[i]->GetComponent<MeshFilter>();
		auto mesh = meshFilter->Mesh.Get();

		triangles[i].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		triangles[i].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		triangles[i].Triangles = mesh->mTriangleDesc;
	}

	mTriangles = move( triangles );

	// 레이트레이싱 데이터 크기를 조회합니다.
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs{ };
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	inputs.NumDescs = ( uint )mTriangles.size();
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.pGeometryDescs = mTriangles.data();
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo;
	pDevice->GetRaytracingAccelerationStructurePrebuildInfo( &inputs, &prebuildInfo );

	// 레이트레이싱 버퍼를 생성합니다.
	mScratch = Graphics::mDevice->CreateBuffer( prebuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
	mAccel = Graphics::mDevice->CreateBuffer( prebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );

	mBuilded = false;
}