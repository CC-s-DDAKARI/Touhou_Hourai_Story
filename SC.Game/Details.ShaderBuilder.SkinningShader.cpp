using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

#include "CompiledShaders/SkinningComputeShader.h"

void ShaderBuilder::LoadSkinningShader()
{
	auto pDevice = GlobalVar.device->pDevice.Get();

	D3D12_DESCRIPTOR_RANGE skinnedBuffer[]
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	D3D12_DESCRIPTOR_RANGE boneTransform[]
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	D3D12_DESCRIPTOR_RANGE vertexBuffer[]
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	CRootSignatureDesc rootSignatureDesc;
	rootSignatureDesc.AddDescriptorTable( skinnedBuffer );
	rootSignatureDesc.AddDescriptorTable( boneTransform );
	rootSignatureDesc.AddDescriptorTable( vertexBuffer );
	rootSignatureDesc.AddRoot32BitConstants( 0, 1 );
	pRootSignature_Skinning = GlobalVar.device->CreateRootSignature( *rootSignatureDesc.Get() );

	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{ };
	pipelineStateDesc.pRootSignature = pRootSignature_Skinning.Get();
	pipelineStateDesc.CS = { pSkinningComputeShader, ARRAYSIZE( pSkinningComputeShader ) };
	HR( pDevice->CreateComputePipelineState( &pipelineStateDesc, IID_PPV_ARGS( &pPipelineState_Skinning ) ) );
}