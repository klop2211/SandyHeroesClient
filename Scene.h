#pragma once

class Object;
class Shader;
class FrameResourceManager;
class DescriptorManager;
class Mesh;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {};

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
		ID3D12RootSignature* root_signature, FrameResourceManager* frame_resource_manager,
		DescriptorManager* descriptor_manager) = 0;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) = 0;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildFrameResources(ID3D12Device* device) = 0;
	virtual void BuildDescriptorHeap(ID3D12Device* device) = 0;
	virtual void BuildConstantBufferViews(ID3D12Device* device) = 0;
	virtual void Render(ID3D12GraphicsCommandList* command_list) = 0;

protected:
	std::list<std::unique_ptr<Object>> object_list_;
	std::vector<std::unique_ptr<Shader>> shaders_;
	std::vector<std::unique_ptr<Mesh>> meshes_;

	FrameResourceManager* frame_resource_manager_ = nullptr;
	DescriptorManager* descriptor_manager_ = nullptr;

	// �Ϲ� �޽��� ����ϴ� ������Ʈ�� �ִ� �뷮
	// �� �뷮��ŭ ������۰� gpu�� ���������.
	int cb_object_capacity_ = 0;
	int cb_skinned_mesh_object_capacity_ = 0;

};

