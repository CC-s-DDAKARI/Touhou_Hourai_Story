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

		pCommandList->SetComputeRootShaderResourceView( Slot_Skinning_SkinnedVertexBuffer, skinnedMesh->vertexBuffer->pResource->GetGPUVirtualAddress() );
	}
}

void SkinnedMeshRenderer::Render( RefPtr<CDeviceContext>& deviceContext )
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

		skinnedMesh->DrawIndexed( deviceContext );
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
	return renderer;
}

RefPtr<Mesh> SkinnedMeshRenderer::Mesh_get()
{
	return skinnedMesh;
}

void SkinnedMeshRenderer::Mesh_set( RefPtr<Game::Mesh> value )
{
	skinnedMesh = value;
	
	mVertexBuffer = new CBuffer( GlobalVar.device, sizeof( Vertex ) * skinnedMesh->numVertex, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS );
}

RefPtr<Material> SkinnedMeshRenderer::Material_get()
{
	return material;
}

void SkinnedMeshRenderer::Material_set( RefPtr<Game::Material> value )
{
	material = value;
}