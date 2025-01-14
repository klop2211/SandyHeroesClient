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
		ID3D12RootSignature* root_signature) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildFrameResources(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildDescriptorHeap(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;

private:

};

