#pragma once
#include "Scene.h"

class MeshColliderComponent;

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

	virtual void AddObject(Object* object) override;
	virtual void DeleteObject(Object* object) override;

	void UpdateObjectIsGround();
	void CheckObjectIsGround(Object* object);

	void PrepareGroundChecking();	//�� �ٴ�üũ�� ���� ���� �۾�

	void CheckPlayerHitWall(const XMFLOAT3& velocity);

	//void CheckPlayerHitWall();	// �� �浹üũ

private:
	//TODO: Player ��ü ����
	Object* player_ = nullptr;

	static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����
	bool is_prepare_ground_checking_ = false;
	std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//�� �ٴ�üũ�� ���� �޽� �ݶ��̴� ����Ʈ �迭
	int stage_clear_num_{ 0 };	// �÷��̾��� �������� ���൵
	//TODO: ������ �浹���� ����Ʈ�� �߰��ȴٸ�(�׷� �ʿ伺�� �־) ������Ʈ �Ŵ��� Ŭ������ �����ϴ� ���� ����� ��.
	std::list<Object*> ground_check_object_list_;	//���� üũ�� �ʿ��� ��ü���� ����Ʈ(�÷��̾�, monster, NPC)

	bool is_render_debug_mesh_ = false;	//����׿� ���̾������� obb�� �����ϴ��� ����

};

