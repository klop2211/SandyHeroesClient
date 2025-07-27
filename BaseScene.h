#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "CutSceneTrack.h"
#include "TextRenderer.h"
#include "TextFormat.h"
#include "RazerComponent.h"

class MeshColliderComponent;
class SpawnerComponent;
class BoxColliderComponent;
class GroundColliderComponent;
class WallColliderComponent;
class MovementComponent;
class MonsterComponent;

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
	virtual const std::list<MeshComponent*>& GetShadowMeshList(int index) override;


	virtual void Update(float elapsed_time) override;

	virtual void RenderText(ID2D1DeviceContext2* d2d_device_context) override;

	virtual void AddObject(Object* object) override;
	virtual void DeleteObject(Object* object) override;
	virtual void DeleteDeadObjects() override;

	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateStageClear();
	void UpdateRazerHitEnemy();

	void PrepareGroundChecking();	//맵 바닥체크를 위한 사전 작업

	void CheckObjectIsGround(Object* object);
	void CheckPlayerHitWall(Object* object, MovementComponent* movement);
	void CheckObjectHitObject(Object* object);
	void CheckRayHitEnemy(const XMFLOAT3& ray_origin, const XMFLOAT3& ray_direction);  // 변경된 총알 충돌체크
	void CheckObjectHitFlamethrow(Object* object);
	void CheckPlayerHitGun(Object* object);
	void CheckPlayerHitPyramid(Object* object);
	void CheckPlayerHitChest(Object* object);
	void CheckSpawnBoxHitPlayer();
	void CheckRazerHitEnemy(RazerComponent* razer_component, MonsterComponent* monster_component);

	void ShowSandyHeroesUI();
	void ShowIntroUI();

	//getter
	int stage_clear_num();
	void add_catch_monster_num();
	int catch_monster_num() const;

	std::list<MonsterComponent*> monster_list() const;

private:
	//static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수

	ID3D12Device* device_{ nullptr };	//Direct3D 12 디바이스

	//스테이지별 몬스터 스포너 리스트
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// 몬스터 잡은 횟수
	int catch_monster_num_{ 0 };
	// 열쇠를 먹은 횟수
	int get_key_num_{ 0 };
	// F키 누름
	bool f_key_{};
	bool scroll_f_key_{};
	
	//현재 스테이지의 스포터를 활성화 했는가?
	bool is_activate_spawner_ = false;

	// 인트로 사라지고 나서 CreatePlayerUI 호출용 변수
	bool intro_ui_ready_{ false };
	bool has_created_player_ui_{ false };

	struct WallCheckObject
	{
		Object* object{ nullptr };
		MovementComponent* movement{ nullptr };

		WallCheckObject(Object* obj, MovementComponent* move)
			: object(obj), movement(move) {
		}
	};

	BoundingOrientedBox stage3_clear_box_;

	std::list<WallCheckObject> wall_check_object_list_;	//벽 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	std::array<std::list<GroundColliderComponent*>, 8> stage_ground_collider_list_;	//스테이지 바닥 콜라이더 리스트

	std::list<MonsterComponent*> monster_list_;	//몬스터 리스트

	std::list<RazerComponent*> razer_list_;	//레이저 리스트

	std::array<std::list<MeshComponent*>, 8> stage_mesh_list_;	//스테이지별 메쉬 리스트

	std::unique_ptr<ParticleSystem> particle_system_{ nullptr };	//파티클 시스템

	std::vector<Object*> monster_hit_particles_;

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;

	std::vector<Object*> sounds_;

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // 스테이지 몬스터 생성 체크를 위한 박스들

	std::vector<CutSceneTrack> cut_scene_tracks_{};

	std::unique_ptr<TextRenderer> text_renderer_{ nullptr }; // 텍스트 렌더러
	std::unordered_map<std::string, std::unique_ptr<TextFormat>> text_formats_;
};

