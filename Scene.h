#pragma once

class Object;
class Shader;
class FrameResourceManager;
class DescriptorManager;
class Mesh;
class CameraComponent;
class InputManager;
class InputControllerComponent;
class ModelInfo;
class Material;
class GameFramework;

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

	void ReleaseMeshUploadBuffer();

	virtual void Render(ID3D12GraphicsCommandList* command_list) = 0;

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) = 0;

	virtual void Update(float elapsed_time);

	void UpdateObjectWorldMatrix();

	Object* FindObject(const std::string& object_name);
	ModelInfo* FindModelInfo(const std::string& name);

	static Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes);
	static Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials);

	//getter
	const std::vector<std::unique_ptr<Mesh>>& meshes() const;

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

