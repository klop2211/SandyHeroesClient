#pragma once
#include "Scene.h"

// �����ӿ�ũ ���ۿ� �׽�Ʈ ��
// ���� �����Լ����� ������ �ۼ��ϰ� �����غ���.
class TestScene :
    public Scene
{
public:
	TestScene() {}

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, FrameResourceManager* frame_resource_manager,
		DescriptorManager* descriptor_manager, InputManager* input_manager) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildFrameResources(ID3D12Device* device) override;
	virtual void BuildDescriptorHeap(ID3D12Device* device) override;
	virtual void BuildConstantBufferViews(ID3D12Device* device) override;
	virtual void BuildShaderResourceViews(ID3D12Device* device) override;

	virtual void Render(ID3D12GraphicsCommandList* command_list) override;

	virtual void Update(float elapsed_time) override;

private:

};

