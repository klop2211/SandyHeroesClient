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

	void PrepareGroundChecking();	//맵 바닥체크를 위한 사전 작업

	void CheckPlayerHitWall(const XMFLOAT3& velocity);

	//void CheckPlayerHitWall();	// 벽 충돌체크

private:
	//TODO: Player 객체 구현
	Object* player_ = nullptr;

	static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수
	bool is_prepare_ground_checking_ = false;
	std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//맵 바닥체크를 위한 메쉬 콜라이더 리스트 배열
	int stage_clear_num_{ 0 };	// 플레이어의 스테이지 진행도
	//TODO: 앞으로 충돌관련 리스트가 추가된다면(그럴 필요성이 있어서) 오브젝트 매니저 클래스를 구현하는 것을 고려할 것.
	std::list<Object*> ground_check_object_list_;	//지면 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	bool is_render_debug_mesh_ = false;	//디버그용 와이어프레임 obb를 렌더하는지 여부

};

