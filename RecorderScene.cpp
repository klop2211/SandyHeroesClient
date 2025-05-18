#include "stdafx.h"
#include "RecorderScene.h"
#include "StandardMeshShader.h"
#include "StandardSkinnedMeshShader.h"
#include "SkyboxShader.h"
#include "CubeMesh.h"
#include "SkyboxMesh.h"
#include "ModelInfo.h"
#include "GameFramework.h"
#include "CameraComponent.h"
#include "TestControllerComponent.h"
#include "MovementComponent.h"

void RecorderScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, ID3D12RootSignature* root_signature, GameFramework* game_framework)
{
	Scene::Initialize(device, command_list, root_signature, game_framework);

	cut_scene_recorder_ = std::make_unique<CutSceneRecorder>("CutScene");
}

void RecorderScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	constexpr int shader_count = 5;
	shaders_.reserve(shader_count);
	shaders_.push_back(std::make_unique<StandardMeshShader>());
	shaders_.push_back(std::make_unique<StandardSkinnedMeshShader>());
	shaders_.push_back(std::make_unique<SkyboxShader>());

	for (int i = 0; i < shaders_.size(); ++i)
	{
		shaders_[i]->CreateShader(device, root_signature);
	}
}

using namespace file_load_util;
void RecorderScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	meshes_.reserve(50);
	meshes_.push_back(std::make_unique<CubeMesh>());
	Material* material = new Material{};
	material->set_albedo_color(0, 1, 0, 1);
	meshes_.back().get()->AddMaterial(material);
	meshes_.back().get()->set_name("green_cube");
	materials_.emplace_back();
	materials_.back().reset(material);

	//skybox
	material = SkyboxMesh::CreateSkyboxMaterial("Skybox_Cube");
	meshes_.push_back(std::make_unique<SkyboxMesh>(meshes_[0].get(), material));
	materials_.emplace_back();
	materials_.back().reset(material);

	model_infos_.reserve(40);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Dog00.bin", meshes_, materials_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/classic.bin", meshes_, materials_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/SM_Bullet_01.bin", meshes_, materials_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Hit_Dragon.bin", meshes_, materials_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Shot_Dragon.bin", meshes_, materials_));

	BuildScene();

	for (const std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->CreateShaderVariables(device, command_list);
	}
}

void RecorderScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	cb_object_capacity_ = 5000;
	cb_skinned_mesh_object_capacity_ = 5000;

	//카메라 배치
	Object* camera = new Object{};
	camera->set_name("Camera");
	camera->set_position_vector(XMFLOAT3{ 0.f, 5.f, -10.f });

	CameraComponent* camera_component = new CameraComponent{ camera, 0.1, 300, 
		(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58.f };
	camera->AddComponent(camera_component);
	main_camera_ = camera_component;

	TestControllerComponent* controller = new TestControllerComponent(camera);
	camera->AddComponent(controller);
	main_input_controller_ = controller;

	MovementComponent* movement = new MovementComponent(camera);
	movement->DisableGarvity();
	camera->AddComponent(movement);

}

bool RecorderScene::ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)
{
	return false;
}



