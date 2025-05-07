#pragma once
#include "Object.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "ModelInfo.h"

class FrameResourceManager;
class DescriptorManager;
class CameraComponent;
class InputManager;
class InputControllerComponent;
class Material;
class GameFramework;
class ColliderComponent;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {};

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
		ID3D12RootSignature* root_signature, GameFramework* game_framework);
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) = 0;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildFrameResources(ID3D12Device* device);
	virtual void BuildDescriptorHeap(ID3D12Device* device);
	virtual void BuildConstantBufferViews(ID3D12Device* device) {};
	virtual void BuildShaderResourceViews(ID3D12Device* device);
	
	void BuildScene(const std::string& scene_name);

	virtual bool CheckObjectByObjectCollisions() { return false; };

	//���� ������Ʈ �߰� �� ����(�ΰ����� ������Ʈ ����Ʈ�� �ִٸ� �� �Լ��� �������̵��Ͽ�	���)
	virtual void AddObject(Object* object);
	virtual void DeleteObject(Object* object);

	void ReleaseMeshUploadBuffer();

	virtual void Render(ID3D12GraphicsCommandList* command_list) = 0;

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) = 0;

	//��ȯ ��: ���� ��ǥ�迡�� ��ŷ�� ����
	//����: ��ũ�� x, y��ǥ�� �޾� ��ŷ ������ ������Ʈ�鰣 �浹�˻縦 ����
	XMVECTOR GetPickingPointAtWorld(float sx, float sy, Object* picked_object);

	virtual void Update(float elapsed_time);

	void UpdateObjectWorldMatrix();

	Object* FindObject(const std::string& object_name);
	ModelInfo* FindModelInfo(const std::string& name);

	static Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes);
	static Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials);

	//getter
	const std::vector<std::unique_ptr<Mesh>>& meshes() const;
	CameraComponent* main_camera() const;

protected:
	std::list<std::unique_ptr<Object>> object_list_;
	std::vector<std::unique_ptr<Shader>> shaders_;
	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::vector<std::unique_ptr<ModelInfo>> model_infos_;
	std::vector<std::unique_ptr<Material>> materials_;

	GameFramework* game_framework_{ nullptr };

	// �Ϲ� �޽��� ����ϴ� ������Ʈ�� �ִ� �뷮
	// �� �뷮��ŭ ������۰� gpu�� ���������.
	int cb_object_capacity_{ 0 };
	int cb_skinned_mesh_object_capacity_{ 0 };

	CameraComponent* main_camera_{ nullptr };
	InputControllerComponent* main_input_controller_{ nullptr };

};

