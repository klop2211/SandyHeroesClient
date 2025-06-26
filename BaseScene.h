#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "CutSceneTrack.h"

class MeshColliderComponent;
class SpawnerComponent;
class BoxColliderComponent;

class BaseScene :
    public Scene
{
private:
	static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����
public:
	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, GameFramework* game_framework) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;

	//Create spawner each stage
	void CreateMonsterSpawner();

	void ActivateStageMonsterSpawner(int stage_num);

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	virtual void Update(float elapsed_time) override;

	virtual void AddObject(Object* object) override;
	virtual void DeleteObject(Object* object) override;
	virtual void DeleteDeadObjects() override;

	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateStageClear();

	void PrepareGroundChecking();	//�� �ٴ�üũ�� ���� ���� �۾�

	void CheckObjectIsGround(Object* object);
	void CheckPlayerHitWall(Object* object, const XMFLOAT3& velocity);
	void CheckObjectHitObject(Object* object);
	void CheckObjectHitBullet(Object* object);
	void CheckPlayerHitPyramid(Object* object);
	void CheckSpawnBoxHitPlayer();

	//getter
	std::list<MeshColliderComponent*> checking_maps_mesh_collider_list(int index);
	int stage_clear_num();
	void add_catch_monster_num();
private:
	//static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����

	//���������� ���� ������ ����Ʈ
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// ���� ���� Ƚ��
	int catch_monster_num_{ 0 };
	// ���踦 ���� Ƚ��
	int get_key_num_{ 0 };

	//���� ���������� �����͸� Ȱ��ȭ �ߴ°�?
	bool is_activate_spawner_ = false;

	//TODO: ������ �浹���� ����Ʈ�� �߰��ȴٸ�(�׷� �ʿ伺�� �־) ������Ʈ �Ŵ��� Ŭ������ �����ϴ� ���� ����� ��.
	std::list<Object*> wall_check_object_list_;	//�� üũ�� �ʿ��� ��ü���� ����Ʈ(�÷��̾�, monster, NPC)

	std::unique_ptr<ParticleSystem> particle_system_{ nullptr };	//��ƼŬ �ý���

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // �������� ���� ���� üũ�� ���� �ڽ���


	std::vector<CutSceneTrack> cut_scene_tracks_{};

};

