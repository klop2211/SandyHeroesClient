#pragma once

class Object;
class Shader;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {};

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
		ID3D12RootSignature* root_signature) = 0;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) = 0;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildFrameResources(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildDescriptorHeap(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;

protected:
	std::list<std::unique_ptr<Object>> object_list_;
	std::vector<std::unique_ptr<Shader>> shaders_;
};

