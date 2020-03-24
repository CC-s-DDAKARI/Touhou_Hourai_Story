using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

void Terrain::Bake( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	auto& pCommandList = *deviceContext->pCommandList.Get();

	if ( mUpdateTerrain )
	{
		if ( mHeightMap && mHeightMap->Lock( deviceContext ) )
		{
			deviceContext->SetComputeRootShaderResources( Slot_Terrain_HeightMap, mHeightMap->pShaderResourceView );
		}

		pCommandList.SetComputeRootShaderResourceView( ( UINT )Slot_Terrain_VertexBufferBase, mBaseMesh->vertexBuffer->pResource->GetGPUVirtualAddress() );
		pCommandList.SetComputeRootUnorderedAccessView( ( UINT )Slot_Terrain_VertexBuffer, mVertexBuffer->pResource->GetGPUVirtualAddress() );

#pragma pack( push, 4 )
		struct
		{
			uint GridX;
			uint GridY;
		} grid;
#pragma pack( pop )

		grid.GridX = mResolution;
		grid.GridY = mResolution;
		pCommandList.SetComputeRoot32BitConstants( ( UINT )Slot_Terrain_Constants, 2, &grid, 0 );
			
		uint dispatchX = ( ( mResolution + 1 ) - 1 ) / 32 + 1;
		uint dispatchY = dispatchX;
		pCommandList.Dispatch( dispatchX, dispatchY, 1 );

		mUpdateTerrain = false;
	}
}

void Terrain::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	if ( mMaterial ) mMaterial->SetGraphicsRootConstantBuffers( deviceContext );
	else MeshRenderer::defaultMaterial->SetGraphicsRootConstantBuffers( deviceContext );
	mBaseMesh->DrawSkinnedIndexed( mVertexBuffer->pResource->GetGPUVirtualAddress(), deviceContext );
}

Terrain::Terrain() : Component()
{
	mBaseMesh = Mesh::CreatePlane( "Terrain.Mesh", 1, 1, mResolution, mResolution );
	mVertexBuffer = new CBuffer( GlobalVar.device, sizeof( Vertex ) * mBaseMesh->numVertex, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
	mUpdateTerrain = true;
}

Terrain::~Terrain()
{

}

object Terrain::Clone()
{
	var clone = new Terrain();
	return clone;
}

void Terrain::Awake()
{

}

RefPtr<Game::HeightMap> Terrain::HeightMap_get()
{
	return mHeightMap;
}

void Terrain::HeightMap_set( RefPtr<Game::HeightMap> value )
{
	mHeightMap = value;
	mUpdateTerrain = true;
}

RefPtr<Material> Terrain::Material_get()
{
	return mMaterial;
}

void Terrain::Material_set( RefPtr<Game::Material> value )
{
	mMaterial = value;
}