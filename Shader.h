#pragma once

class Material;
class DescriptorManager;
class CameraComponent;
struct FrameResource;

enum class ShaderType
{
	kNone = 0, kColor, kSkinnedMesh, kStandardMesh, kStandardSkinnedMesh, kSkybox, kDebug, kUI, kTransparent, kBreathing, kShadow
};

class Shader
{
public:
	Shader() {}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() = 0;
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) = 0;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) = 0;

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* file_name, LPCSTR shader_name,
		LPCSTR shader_profile, ID3DBlob** shader_blob);

	void CreateShader(ID3D12Device* device, ID3D12RootSignature* root_signature);

	ID3D12PipelineState* GetPipelineState() const;

	ShaderType shader_type() const;

	void ReserveMaterials(UINT capacity);
	void AddMaterial(Material* material);

	void Render(ID3D12GraphicsCommandList* command_list, 
		FrameResource* curr_frame_resource, DescriptorManager* descriptor_manager, CameraComponent* camera);

protected:
	ComPtr<ID3D12PipelineState> d3d_pipeline_state_;

	ShaderType shader_type_ = ShaderType::kNone;

	bool is_frustum_culling_ = true;

	std::vector<Material*> materials_{};
};

