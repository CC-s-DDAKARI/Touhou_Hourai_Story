using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

Material* SkinnedMeshRenderer::mDefaultMaterial;
uint64 SkinnedMeshRenderer::reference_count = 0;

void SkinnedMeshRenderer::Skinning( RefPtr<CDeviceContext>& deviceContext )
{
	if ( skinnedMesh.IsValid )
	{
		auto pCommandList = deviceContext->pCommandList.Get();

		auto numVertex = skinnedMesh->mVertexCount;

		pCommandList->SetComputeRootShaderResourceView( Slot_Skinning_SkinnedVertexBuffer, skinnedMesh->mVertexBuffer->pResource->GetGPUVirtualAddress() );
		pCommandList->SetComputeRootUnorderedAccessView( Slot_Skinning_VertexBuffer, mVertexBuffer->pResource->GetGPUVirtualAddress() );
		pCommandList->SetComputeRoot32BitConstant( Slot_Skinning_SkinningConstants, skinnedMesh->mVertexCount, 0 );

		int dispatch = ( numVertex - 1 ) / 32 + 1;
		pCommandList->Dispatch( ( UINT )dispatch, 1, 1 );
	}
}

void SkinnedMeshRenderer::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	if ( skinnedMesh.IsValid )
	{
		if ( material.IsValid )
		{
			material->SetGraphicsRootConstantBuffers( deviceContext );
		}
		else
		{
			mDefaultMaterial->SetGraphicsRootConstantBuffers( deviceContext );
		}

		skinnedMesh->DrawSkinnedIndexed( mVertexBuffer->pResource->GetGPUVirtualAddress(), deviceContext );
	}
}

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
	if ( !mDefaultMaterial )
	{
		mDefaultMaterial = AssetBundle::GetItemAs<Game::Material>( "default_material" ).Get();
	}
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

object SkinnedMeshRenderer::Clone()
{
	RefPtr renderer = new SkinnedMeshRenderer();
	renderer->material = material;
	renderer->skinnedMesh = skinnedMesh;
	renderer->mVertexBuffer = new CBuffer( Graphics::mDevice, sizeof( Vertex ) * skinnedMesh->mVertexCount, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
	return renderer;
}

RefPtr<Mesh> SkinnedMeshRenderer::Mesh_get()
{
	return skinnedMesh;
}

void SkinnedMeshRenderer::Mesh_set( RefPtr<Game::Mesh> value )
{
	skinnedMesh = value;
	
	mVertexBuffer = new CBuffer( Graphics::mDevice, sizeof( Vertex ) * skinnedMesh->mVertexCount, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
}

RefPtr<Material> SkinnedMeshRenderer::Material_get()
{
	return material;
}

void SkinnedMeshRenderer::Material_set( RefPtr<Game::Material> value )
{
	material = value;
}