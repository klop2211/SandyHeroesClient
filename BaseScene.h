#pragma once
#include "Scene.h"
class BaseScene :
    public Scene
{
public:
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;

	virtual void Render(ID3D12GraphicsCommandList* command_list) override;

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	virtual void Update(float elapsed_time) override;

	//TODO: ���� �÷��̾� �Ӹ� �ƴ϶� ��, �ٸ� �÷��̾ ���� ground üũ�� �ʿ�
	void CheckPlayerIsGround();

private:
	//TODO: Player ��ü ����
	Object* player_ = nullptr;

};

