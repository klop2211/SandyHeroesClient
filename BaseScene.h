#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "CutSceneTrack.h"
#include "TextRenderer.h"
#include "TextFormat.h"

class MeshColliderComponent;
class SpawnerComponent;
class BoxColliderComponent;
class GroundColliderComponent;
class WallColliderComponent;
class MovementComponent;

class BaseScene :
	public Scene
{
private:
	static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����
public:
	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, GameFramework* game_framework, ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildTextBrushAndFormat(ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	void BuildModelInfo(ID3D12Device* device); // ������Ʈ ������ ���� �� ������ ���� ����ų� ���� �� ������ �����ϴ� �Լ�

	void CreatePlayerUI();	//�÷��̾� UI ����

	//Create spawner each stage
	void CreateMonsterSpawner();

	void ActivateStageMonsterSpawner(int stage_num);

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	virtual void Update(float elapsed_time) override;

	virtual void RenderText(ID2D1DeviceContext2* d2d_device_context) override;

	virtual void AddObject(Object* object) override;
	virtual void DeleteObject(Object* object) override;
	virtual void DeleteDeadObjects() override;

	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateScroll(float elapsed_time);
	void UpdateStageClear();

	void PrepareGroundChecking();	//�� �ٴ�üũ�� ���� ���� �۾�

	void CheckObjectIsGround(Object* object);
	void CheckPlayerHitWall(Object* object, MovementComponent* movement);
	void CheckObjectHitObject(Object* object);
	void CheckRayHitEnemy(const XMFLOAT3& ray_origin, const XMFLOAT3& ray_direction);  // ����� �Ѿ� �浹üũ
	void CheckObjectHitFlamethrow(Object* object);
	void CheckPlayerHitGun(Object* object);
	void CheckPlayerHitPyramid(Object* object);
	void CheckPlayerHitChest(Object* object);
	void CheckSpawnBoxHitPlayer();

	//getter
	std::list<MeshColliderComponent*> checking_maps_mesh_collider_list(int index);
	int stage_clear_num();
	void add_catch_monster_num();
	int catch_monster_num() const;
private:
	//static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����

	ID3D12Device* device_{ nullptr };	//Direct3D 12 ����̽�

	//���������� ���� ������ ����Ʈ
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// ���� ���� Ƚ��
	int catch_monster_num_{ 0 };
	// ���踦 ���� Ƚ��
	int get_key_num_{ 0 };
	// FŰ ����
	bool f_key_{};
	bool scroll_f_key_{};
	
	//���� ���������� �����͸� Ȱ��ȭ �ߴ°�?
	bool is_activate_spawner_ = false;

	struct WallCheckObject
	{
		Object* object{ nullptr };
		MovementComponent* movement{ nullptr };

		WallCheckObject(Object* obj, MovementComponent* move)
			: object(obj), movement(move) {
		}
	};

	std::list<WallCheckObject> wall_check_object_list_;	//�� üũ�� �ʿ��� ��ü���� ����Ʈ(�÷��̾�, monster, NPC)

	std::array<std::list<GroundColliderComponent*>, 8> stage_ground_collider_list_;	//�������� �ٴ� �ݶ��̴� ����Ʈ
	std::array<std::list<WallColliderComponent*>, 8> stage_wall_collider_list_;	//�������� �� �ݶ��̴� ����Ʈ

	std::unique_ptr<ParticleSystem> particle_system_{ nullptr };	//��ƼŬ �ý���

	std::vector<Object*> monster_hit_particles_;

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;

	std::vector<Object*> sounds_;

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // �������� ���� ���� üũ�� ���� �ڽ���

	std::vector<CutSceneTrack> cut_scene_tracks_{};

	std::unique_ptr<TextRenderer> text_renderer_{ nullptr }; // �ؽ�Ʈ ������
	std::unordered_map<std::string, std::unique_ptr<TextFormat>> text_formats_;
};

