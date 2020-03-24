using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

#include "CompiledShaders/ColorVertexShader.h"
#include "CompiledShaders/ColorPixelShader.h"

#include "CompiledShaders/ShadowCastVertexShader.h"
#include "CompiledShaders/ShadowCastPixelShader.h"

#include "CompiledShaders/SkyboxVertexShader.h"
#include "CompiledShaders/SkyboxPixelShader.h"

void ShaderBuilder::LoadRenderingShader()
{
	auto pDevice = GlobalVar.device->pDevice.Get();

	D3D12_DESCRIPTOR_RANGE textures[]
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
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
	rootSignatureDesc.VertexShaderRootAccess = true;
	rootSignatureDesc.PixelShaderRootAccess = true;
	rootSignatureDesc.InputAssemblerInputLayout = true;

	rootSignatureDesc.AddConstantBufferView( 0, D3D12_SHADER_VISIBILITY_VERTEX );  // gCamera
	rootSignatureDesc.AddConstantBufferView( 1, D3D12_SHADER_VISIBILITY_VERTEX );  // gTransform
	rootSignatureDesc.AddConstantBufferView( 2, D3D12_SHADER_VISIBILITY_VERTEX );  // gLight
	rootSignatureDesc.AddConstantBufferView( 3 );  // gMaterial
	rootSignatureDesc.AddDescriptorTable( textures, D3D12_SHADER_VISIBILITY_PIXEL );  // gDiffuseMap0, 1, gNormalMap0

	rootSignatureDesc.AddStaticSampler( D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 16, 0 );
	
	// 루트 서명 개체를 생성합니다.
	pRootSignature_Rendering = GlobalVar.device->CreateRootSignature( *rootSignatureDesc.Get() );

	// 컬러 렌더링 파이프라인 개체를 생성합니다.
	CGraphicsPipelineStateDesc pipelineDesc( pRootSignature_Rendering.Get() );
	pipelineDesc.RTVCount = 3;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R11G11B10_FLOAT;
	pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R16_UINT;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.SetVertexShader( pColorVertexShader, ARRAYSIZE( pColorVertexShader ) );
	pipelineDesc.SetPixelShader( pColorPixelShader, ARRAYSIZE( pColorPixelShader ) );
	pipelineDesc.AddInputLayout( Vertex::InputElements );
	pipelineDesc.SetDepthStencilState( true );
	pipelineDesc.CullMode = D3D12_CULL_MODE_NONE;
	//pipelineDesc.WireframeMode = true;
	pipelineDesc.SetBlendState( 0, true, D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD );
	HR( pDevice->CreateGraphicsPipelineState( pipelineDesc.Get(), IID_PPV_ARGS( &pPipelineState_ColorShader ) ) );

	// 그림자 렌더링 파이프라인 개체를 생성합니다.
	pipelineDesc = CGraphicsPipelineStateDesc( pRootSignature_Rendering.Get() );
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.SetVertexShader( pShadowCastVertexShader, ARRAYSIZE( pShadowCastVertexShader ) );
	pipelineDesc.SetPixelShader( pShadowCastPixelShader, ARRAYSIZE( pShadowCastPixelShader ) );
	pipelineDesc.AddInputLayout( Vertex::InputElements );
	pipelineDesc.SetDepthStencilState( true );
	pipelineDesc.CullMode = D3D12_CULL_MODE_NONE;
	pipelineDesc.SlopeScaledDepthBias = 3.0;
	HR( pDevice->CreateGraphicsPipelineState( pipelineDesc.Get(), IID_PPV_ARGS( &pPipelineState_ShadowCastShader ) ) );

	// 스카이박스 렌더링 파이프라인 개체를 생성합니다.
	pipelineDesc = CGraphicsPipelineStateDesc( pRootSignature_Rendering.Get() );
	pipelineDesc.RTVCount = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineDesc.SetVertexShader( pSkyboxVertexShader, ARRAYSIZE( pSkyboxVertexShader ) );
	pipelineDesc.SetPixelShader( pSkyboxPixelShader, ARRAYSIZE( pSkyboxPixelShader ) );
	pipelineDesc.AddInputLayout( Vertex::InputElements );
	pipelineDesc.CullMode = D3D12_CULL_MODE_FRONT;
	pipelineDesc.SetDepthStencilState( true );
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	HR( pDevice->CreateGraphicsPipelineState( pipelineDesc.Get(), IID_PPV_ARGS( &pPipelineState_SkyboxShader ) ) );
}