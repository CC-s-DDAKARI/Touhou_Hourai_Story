using namespace SC;
using namespace SC::Game::Details;

#include "CompiledShaders/GaussianBlurHComputeShader.h"
#include "CompiledShaders/GaussianBlurVComputeShader.h"

void ShaderBuilder::LoadGaussianBlurShader()
{
	ShaderModule shader;
	auto pDevice = GlobalVar.device->pDevice.Get();

	D3D12_DESCRIPTOR_RANGE blurBuffer[]
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	CRootSignatureDesc rootSignatureDesc;
	rootSignatureDesc.AddDescriptorTable( blurBuffer );
	shader.RootSignature = GlobalVar.device->CreateRootSignature( *rootSignatureDesc.Get() );

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{ };
	computePipelineStateDesc.pRootSignature = shader.RootSignature.Get();

	shader.RootParameterIndex =
	{
		{ "BlurBuffers", 0 }
	};
	shader.IsCompute = true;

	computePipelineStateDesc.CS = { pGaussianBlurVComputeShader, ARRAYSIZE( pGaussianBlurVComputeShader ) };
	HR( pDevice->CreateComputePipelineState( &computePipelineStateDesc, IID_PPV_ARGS( &shader.PipelineState ) ) );
	gaussianBlurShader[0] = shader;

	shader.RootSignature = nullptr;
	shader.RootParameterIndex = { };

	computePipelineStateDesc.CS = { pGaussianBlurHComputeShader, ARRAYSIZE( pGaussianBlurHComputeShader ) };
	HR( pDevice->CreateComputePipelineState( &computePipelineStateDesc, IID_PPV_ARGS( &shader.PipelineState ) ) );
	gaussianBlurShader[1] = shader;
}