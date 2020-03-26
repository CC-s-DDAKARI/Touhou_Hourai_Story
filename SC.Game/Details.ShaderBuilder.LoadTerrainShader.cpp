using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

#include "CompiledShaders/TerrainComputeShader.h"

void ShaderBuilder::LoadTerrainShader()
{
	auto pDevice = Graphics::mDevice->pDevice.Get();

	D3D12_DESCRIPTOR_RANGE heightMap[]
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	CRootSignatureDesc rootSignatureDesc;
	rootSignatureDesc.AddUnorderedAccessView( 0 );
	rootSignatureDesc.AddDescriptorTable( heightMap );
	rootSignatureDesc.AddRoot32BitConstants( 0, 2 );
	rootSignatureDesc.AddShaderResourceView( 1 );
	rootSignatureDesc.AddStaticSampler( D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP );
	pRootSignature_Terrain = Graphics::mDevice->CreateRootSignature( *rootSignatureDesc.Get() );

	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{ };
	pipelineStateDesc.pRootSignature = pRootSignature_Terrain.Get();
	pipelineStateDesc.CS = { pTerrainComputeShader, ARRAYSIZE( pTerrainComputeShader ) };
	HR( pDevice->CreateComputePipelineState( &pipelineStateDesc, IID_PPV_ARGS( &pPipelineState_Terrain ) ) );
}