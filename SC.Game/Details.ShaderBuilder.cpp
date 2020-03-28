using namespace SC;
using namespace SC::Game::Details;

ComPtr<ID3D12RootSignature> ShaderBuilder::pRootSignature_Terrain;
ComPtr<ID3D12PipelineState> ShaderBuilder::pPipelineState_Terrain;

ComPtr<ID3D12RootSignature> ShaderBuilder::pRootSignature_Skinning;
ComPtr<ID3D12PipelineState> ShaderBuilder::pPipelineState_Skinning;

ComPtr<ID3D12RootSignature> ShaderBuilder::pRootSignature_Rendering;
ComPtr<ID3D12PipelineState> ShaderBuilder::pPipelineState_ColorShader;
ComPtr<ID3D12PipelineState> ShaderBuilder::pPipelineState_ShadowCastShader;
ComPtr<ID3D12PipelineState> ShaderBuilder::pPipelineState_SkyboxShader;

ShaderModule ShaderBuilder::skyboxShader;
ShaderModule ShaderBuilder::hdrShader;
ShaderModule ShaderBuilder::hdrColorShader;
ShaderModule ShaderBuilder::toneMappingShader;
ShaderModule ShaderBuilder::hdrComputeShader[3];

ShaderModule ShaderBuilder::integratedUIShader;
ShaderModule ShaderBuilder::textAndRectShader;

void ShaderModule::SetRootSignatureAndPipelineState( RefPtr<CDeviceContext>& deviceContext )
{
	auto pCommandList = deviceContext->pCommandList.Get();

	if ( RootSignature )
		if ( IsCompute )
			pCommandList->SetComputeRootSignature( RootSignature.Get() );
		else
			pCommandList->SetGraphicsRootSignature( RootSignature.Get() );

	if ( PipelineState )
		pCommandList->SetPipelineState( PipelineState.Get() );
}

void ShaderModule::SetRootParameterIndex( RefPtr<CDeviceContext>& deviceContext )
{
	if ( RootSignature )
	{
		deviceContext->SetSlotMap( RootParameterIndex );
	}
}

void ShaderModule::SetAll( RefPtr<CDeviceContext>& deviceContext )
{
	SetRootSignatureAndPipelineState( deviceContext );
	SetRootParameterIndex( deviceContext );
}

void ShaderBuilder::Initialize()
{
	LoadTerrainShader();
	LoadSkinningShader();
	LoadRenderingShader();

	//LoadSkyboxShader();
	LoadHDRShader();
	LoadHDRColorShader();
	LoadToneMappingShader();
	LoadHDRComputeShader();

	LoadIntegratedUIShader();
	LoadTextAndRectShader();
}

ShaderModule ShaderBuilder::SkyboxShader_get()
{
	return skyboxShader;
}

ShaderModule ShaderBuilder::HDRShader_get()
{
	return hdrShader;
}

ShaderModule ShaderBuilder::HDRColorShader_get()
{
	return hdrColorShader;
}

ShaderModule ShaderBuilder::ToneMappingShader_get()
{
	return toneMappingShader;
}

ShaderModule ShaderBuilder::HDRComputeShader_get( int param0 )
{
	return hdrComputeShader[param0];
}

ShaderModule ShaderBuilder::IntegratedUIShader_get()
{
	return integratedUIShader;
}

ShaderModule ShaderBuilder::TextAndRectShader_get()
{
	return textAndRectShader;
}

void ShaderBuilder::Dispose( object sender )
{
	pRootSignature_Terrain = nullptr;
	pPipelineState_Terrain = nullptr;

	pRootSignature_Skinning = nullptr;
	pPipelineState_Skinning = nullptr;

	pRootSignature_Rendering = nullptr;
	pPipelineState_ColorShader = nullptr;
	pPipelineState_ShadowCastShader = nullptr;
	pPipelineState_SkyboxShader = nullptr;
}