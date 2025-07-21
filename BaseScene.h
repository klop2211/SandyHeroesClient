#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "CutSceneTrack.h"
#include "TextRenderer.h"
#include "TextFormat.h"

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
		ID3D12RootSignature* root_signature, GameFramework* game_framework, ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildTextBrushAndFormat(ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	void BuildModelInfo(ID3D12Device* device); // 오브젝트 재사용을 위해 모델 정보를 새로 만들거나 기존 모델 정보를 수정하는 함수

	void CreatePlayerUI();	//플레이어 UI 생성

	//Create spawner each stage
	void CreateMonsterSpawner();

	void ActivateStageMonsterSpawner(int stage_num);

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	virtual void Update(float elapsed_time) override;

	virtual void RenderText(ID2D1Bitmap1* d2d_render_target, ID2D1DeviceContext2* d2d_device_context) override;

	virtual void AddObject(Object* object) override;
	virtual void DeleteObject(Object* object) override;
	virtual void DeleteDeadObjects() override;

	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateScroll(float elapsed_time);
	void UpdateStageClear();

	void PrepareGroundChecking();	//맵 바닥체크를 위한 사전 작업

	void CheckObjectIsGround(Object* object);
	void CheckPlayerHitWall(Object* object, const XMFLOAT3& velocity);
	void CheckObjectHitObject(Object* object);
	void CheckObjectHitBullet(Object* object);
	void CheckObjectHitFlamethrow(Object* object);
	void CheckPlayerHitGun(Object* object);
	void CheckPlayerHitPyramid(Object* object);
	void CheckPlayerHitChest(Object* object);
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
	// F키 누름
	bool f_key_{};

	//현재 스테이지의 스포터를 활성화 했는가?
	bool is_activate_spawner_ = false;

	ID3D12Device* device_;

	//TODO: 앞으로 충돌관련 리스트가 추가된다면(그럴 필요성이 있어서) 오브젝트 매니저 클래스를 구현하는 것을 고려할 것.
	std::list<Object*> wall_check_object_list_;	//벽 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	std::unique_ptr<ParticleSystem> particle_system_{ nullptr };	//파티클 시스템

	std::vector<Object*> monster_hit_particles_;

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;
	std::vector<bool> chests_open_;
	std::vector<bool> scroll_open_;

	struct ScrollData
	{
		Object* scroll = nullptr;
		XMFLOAT3 direction = {};  // 정규화된 이동 방향
		float moved_distance = 0.f;
		bool is_active = true;
	};
	std::vector<ScrollData> scrolls_;
	
	std::vector<BoxColliderComponent*> spawn_boxs_{}; // 스테이지 몬스터 생성 체크를 위한 박스들

	std::vector<CutSceneTrack> cut_scene_tracks_{};

	std::unique_ptr<TextRenderer> text_renderer_{ nullptr }; // 텍스트 렌더러

	std::unordered_map<std::string, std::unique_ptr<TextFormat>> text_formats_;
};

