using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

RefPtr<Material> SkinnedMeshRenderer::defaultMaterial;
uint64 SkinnedMeshRenderer::reference_count = 0;

void SkinnedMeshRenderer::Skinning( RefPtr<CDeviceContext>& deviceContext )
{
	if ( skinnedMesh.IsValid )
	{
		auto pCommandList = deviceContext->pCommandList.Get();

		auto numVertex = skinnedMesh->numVertex;

		pCommandList->SetComputeRootShaderResourceView( Slot_Skinning_SkinnedVertexBuffer, skinnedMesh->vertexBuffer->pResource->GetGPUVirtualAddress() );
		pCommandList->SetComputeRootUnorderedAccessView( Slot_Skinning_VertexBuffer, mVertexBuffer->pResource->GetGPUVirtualAddress() );
		pCommandList->SetComputeRoot32BitConstant( Slot_Skinning_SkinningConstants, skinnedMesh->numVertex, 0 );

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
			defaultMaterial->SetGraphicsRootConstantBuffers( deviceContext );
		}

		skinnedMesh->DrawSkinnedIndexed( mVertexBuffer->pResource->GetGPUVirtualAddress(), deviceContext );
	}
}

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
	if ( !defaultMaterial.IsValid )
	{
		defaultMaterial = new Game::Material( "defaultMaterial" );
	}
	reference_count += 1;
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	if ( ( reference_count -= 1 ) == 0 )
	{
		defaultMaterial = nullptr;
	}
}

object SkinnedMeshRenderer::Clone()
{
	RefPtr renderer = new SkinnedMeshRenderer();
	renderer->material = material;
	renderer->skinnedMesh = skinnedMesh;
	renderer->mVertexBuffer = new CBuffer( Graphics::mDevice, sizeof( Vertex ) * skinnedMesh->numVertex, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
	return renderer;
}

RefPtr<Mesh> SkinnedMeshRenderer::Mesh_get()
{
	return skinnedMesh;
}

void SkinnedMeshRenderer::Mesh_set( RefPtr<Game::Mesh> value )
{
	skinnedMesh = value;
	
	mVertexBuffer = new CBuffer( Graphics::mDevice, sizeof( Vertex ) * skinnedMesh->numVertex, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
}

RefPtr<Material> SkinnedMeshRenderer::Material_get()
{
	return material;
}

void SkinnedMeshRenderer::Material_set( RefPtr<Game::Material> value )
{
	material = value;
}