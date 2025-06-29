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
	static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수
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

	void PrepareGroundChecking();	//맵 바닥체크를 위한 사전 작업

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
	//static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수

	//스테이지별 몬스터 스포너 리스트
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// 몬스터 잡은 횟수
	int catch_monster_num_{ 0 };
	// 열쇠를 먹은 횟수
	int get_key_num_{ 0 };

	//현재 스테이지의 스포터를 활성화 했는가?
	bool is_activate_spawner_ = false;

	//TODO: 앞으로 충돌관련 리스트가 추가된다면(그럴 필요성이 있어서) 오브젝트 매니저 클래스를 구현하는 것을 고려할 것.
	std::list<Object*> wall_check_object_list_;	//벽 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	std::unique_ptr<ParticleSystem> particle_system_{ nullptr };	//파티클 시스템

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // 스테이지 몬스터 생성 체크를 위한 박스들


	std::vector<CutSceneTrack> cut_scene_tracks_{};

};

