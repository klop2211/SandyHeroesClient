#pragma once
#include "Scene.h"

// 프레임워크 제작용 테스트 씬
// 씬의 가상함수들을 실제로 작성하고 실행해본다.
class TestScene :
    public Scene
{
public:
	TestScene() {}

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, FrameResourceManager* frame_resource_manager,
		DescriptorManager* descriptor_manager) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildConstantBufferViews(ID3D12Device* device) override;

	virtual void Render(ID3D12GraphicsCommandList* command_list) override;

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	virtual void Update(float elapsed_time) override;

private:

};

