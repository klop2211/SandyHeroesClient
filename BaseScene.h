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

	//TODO: 단일 플레이어 뿐만 아니라 적, 다른 플레이어에 대한 ground 체크도 필요
	void CheckPlayerIsGround();

private:
	//TODO: Player 객체 구현
	Object* player_ = nullptr;

};

