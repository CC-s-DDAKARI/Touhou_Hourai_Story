#pragma once

namespace SC::Game::Details
{
	struct ShaderModule
	{
		bool IsCompute = false;
		ComPtr<ID3D12RootSignature> RootSignature;
		ComPtr<ID3D12PipelineState> PipelineState;

		std::map<std::string, int, std::less<>> RootParameterIndex;

		void SetRootSignatureAndPipelineState( RefPtr<CDeviceContext>& commandList );
		void SetRootParameterIndex( RefPtr<CDeviceContext>& commandList );
		void SetAll( RefPtr<CDeviceContext>& commandList );
	};

	constexpr int Slot_Skinning_SkinnedVertexBuffer = 0;
	constexpr int Slot_Skinning_BoneTransform = 1;
	constexpr int Slot_Skinning_VertexBuffer = 2;
	constexpr int Slot_Skinning_SkinningConstants = 3;

	class ShaderBuilder : virtual public Object
	{
	public:
		static ComPtr<ID3D12RootSignature> pRootSignature_Skinning;
		static ComPtr<ID3D12PipelineState> pPipelineState_Skinning;

	private:
		static ShaderModule colorShader;
		static ShaderModule skyboxShader;
		static ShaderModule hdrShader;
		static ShaderModule hdrColorShader;
		static ShaderModule toneMappingShader;
		static ShaderModule shadowCastShader;
		static ShaderModule hdrComputeShader[3];

		static ShaderModule integratedUIShader;
		static ShaderModule textAndRectShader;

	public:
		static void Initialize();

		static ShaderModule ColorShader_get();
		static ShaderModule SkyboxShader_get();
		static ShaderModule HDRShader_get();
		static ShaderModule HDRColorShader_get();
		static ShaderModule ToneMappingShader_get();
		static ShaderModule ShadowCastShader_get();
		static ShaderModule HDRComputeShader_get( int param0 );

		static ShaderModule IntegratedUIShader_get();
		static ShaderModule TextAndRectShader_get();

	private:
		static void LoadSkinningShader();
		static void LoadColorShader();
		static void LoadSkyboxShader();
		static void LoadHDRShader();
		static void LoadHDRColorShader();
		static void LoadToneMappingShader();
		static void LoadShadowCastShader();
		static void LoadHDRComputeShader();

		static void LoadIntegratedUIShader();
		static void LoadTextAndRectShader();
	};
}