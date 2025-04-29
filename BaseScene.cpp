#include "stdafx.h"
#include "BaseScene.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "CubeMesh.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "InputControllerComponent.h"
#include "TestControllerComponent.h"
#include "InputManager.h"
#include "SkinnedMesh.h"
#include "StandardMeshShader.h"
#include "StandardSkinnedMeshShader.h"
#include "FPSControllerComponent.h"
#include "AnimatorComponent.h"
#include "PlayerAnimationState.h"
#include "AnimatorComponent.h"
#include "GameFramework.h"
#include "GunComponent.h"
#include "SkyboxShader.h"
#include "SkyboxMesh.h"
#include "MeshColliderComponent.h"
#include "DebugMeshComponent.h"
#include "DebugShader.h"
#include "SkinnedMeshComponent.h"

void BaseScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	int shader_count = 4;
	shaders_.reserve(shader_count);
	shaders_.push_back(std::make_unique<StandardMeshShader>());
	shaders_.push_back(std::make_unique<StandardSkinnedMeshShader>());
	shaders_.push_back(std::make_unique<SkyboxShader>());
	//shaders_.push_back(std::make_unique<DebugShader>());

	for (int i = 0; i < shaders_.size(); ++i)
	{
		shaders_[i]->CreateShader(device, root_signature);
	}
}

using namespace file_load_util;
void BaseScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	meshes_.reserve(50);
	meshes_.push_back(std::make_unique<CubeMesh>());
	Material* material = new Material{};
	material->set_albedo_color(0, 1, 0, 1);
	meshes_[0].get()->AddMaterial(material);
	meshes_[0].get()->set_name("green_cube");
	materials_.emplace_back();
	materials_.back().reset(material);

	//skybox
	material = SkyboxMesh::CreateSkyboxMaterial("Skybox_Cube");
	meshes_.push_back(std::make_unique<SkyboxMesh>(meshes_[0].get(), material));
	materials_.emplace_back();
	materials_.back().reset(material);

	//debug mesh
	material = materials_[0].get(); // �ǹ� ���� �ƹ� ���͸���
	Mesh* debug_mesh = new CubeMesh();
	debug_mesh->ClearNormals();
	debug_mesh->ClearNormals();
	debug_mesh->ClearTangents();
	debug_mesh->AddMaterial(material);
	debug_mesh->set_shader_type((int)ShaderType::kDebug);
	debug_mesh->set_name("Debug_Mesh");
	meshes_.emplace_back();
	meshes_.back().reset(debug_mesh);


	model_infos_.reserve(40);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Dog00.bin", meshes_, materials_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/classic.bin", meshes_, materials_));

	//BuildScene("Base");

	std::ifstream scene_file{ "./Resource/Model/World/Scene.bin", std::ios::binary };

	int root_object_count = ReadFromFile<int>(scene_file);

	std::string load_token;

	for (int i = 0; i < root_object_count; ++i)
	{
		ReadStringFromFile(scene_file, load_token);
		if (load_token[0] == '@')
		{
			load_token.erase(0, 1);
			object_list_.emplace_back();
			object_list_.back().reset(FindModelInfo(load_token)->GetInstance());

			ReadStringFromFile(scene_file, load_token);
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			object_list_.back()->set_transform_matrix(transfrom);
		}
		else
		{
			std::string object_name = load_token;

			ReadStringFromFile(scene_file, load_token); // <Transfrom>
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);

			model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/World/" + object_name + ".bin", meshes_, materials_));

			object_list_.emplace_back();
			object_list_.back().reset(model_infos_.back()->GetInstance());

			object_list_.back()->set_transform_matrix(transfrom);

		}
	}

	for (const std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->CreateShaderVariables(device, command_list);
	}
}

void BaseScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	//TODO: �� �޽��� ������Ʈ ���� ������ �ľ��ϸ� �Ʒ� ��ġ�� �������ϰ� ������ �� ������ ����..
	cb_object_capacity_ = 10000;
	cb_skinned_mesh_object_capacity_ = 10000;

	Object* skybox = new Object();
	skybox->AddComponent(new MeshComponent(skybox, Scene::FindMesh("Skybox", meshes_)));

	object_list_.emplace_back();
	object_list_.back().reset(skybox);

	//�� ������Ʈ ��ġ
	Object* player = model_infos_[0]->GetInstance();
	player->set_position_vector(XMFLOAT3{ 0, 30, 0 });
	AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(player);
	animator->set_animation_state(new PlayerAnimationState);

	player_ = player;

	//FPS ���ۿ� ��Ʈ�ѷ� ����
	FPSControllerComponent* fps_controller = new FPSControllerComponent(player);
	fps_controller->set_client_wnd(game_framework_->main_wnd());
	fps_controller->set_scene(this);
	player->AddComponent(fps_controller);
	//���� ��Ʈ�ѷ��� ����
	main_input_controller_ = fps_controller;

	//��� �ѱ� ���� �ε�
	GunComponent::LoadGunInfosFromFile("./Resource/GunInfos.txt");

	//�÷��̾� �ѱ� ����
	//TODO: �ѱ� �޽� ���� ����
	Object* player_gun_frame = player->FindFrame("WeaponR_locator");
	player_gun_frame->AddChild(model_infos_[1]->GetInstance());
	player_gun_frame = player_gun_frame->child();
	GunComponent* player_gun = new GunComponent(player_gun_frame);
	player_gun->LoadGunInfo("specter");
	player_gun_frame->AddComponent(player_gun);
	player_gun_frame->Rotate(0, 170, -17);
	//player_gun_frame->Scale(3);

	//ī�޶� ����
	Object* camera_object = new Object();
	player->AddChild(camera_object);
	fps_controller->set_camera_object(camera_object);
	camera_object->set_position_vector(0,0.4,0); // �÷��̾� ĳ������ Ű�� 150�ΰ��� ����Ͽ� �Ӹ���ġ�� ��ġ
	camera_object->set_name("CAMERA_1");
	CameraComponent* camera_component =
		new CameraComponent(camera_object, 0.01, 10000,
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	camera_object->AddComponent(camera_component);
	main_camera_ = camera_component;

	//�� ����Ʈ�� �߰�
	object_list_.emplace_back();
	object_list_.back().reset(player);


	camera_object = new Object;
	camera_object->set_name("CAMERA_2");
	camera_component =
		new CameraComponent(camera_object, 0.3, 10000,
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	TestControllerComponent* controller = new TestControllerComponent(camera_object);
	camera_object->AddComponent(camera_component);
	camera_object->AddComponent(controller);

	object_list_.emplace_back();
	object_list_.back().reset(camera_object);

	//��� �޽� �ִ� ��ü�� �޽� �ݶ��̴� �߰�(���ǻ���: ���Ӱ� ��������� �޽��ִ� ��ü�� �޽��ݶ��̴��� ����)
	//+ ����׿� �޽� �߰�
	Mesh* debug_mesh = Scene::FindMesh("Debug_Mesh", meshes_);
	for (auto& mesh : meshes_)
	{
		auto& mesh_component_list = mesh->mesh_component_list();
		for (auto& mesh_component : mesh_component_list)
		{
			Object* object = mesh_component->owner();
			MeshColliderComponent* mesh_collider = new MeshColliderComponent(object);
			mesh_collider->set_mesh(mesh.get());
			object->AddComponent(mesh_collider);
			if (mesh->name() != "Debug_Mesh")
			{
				object->AddComponent(new DebugMeshComponent(object, debug_mesh, mesh->bounds()));
			}
		}
	}

	Scene::UpdateObjectWorldMatrix();
}

void BaseScene::Render(ID3D12GraphicsCommandList* command_list)
{
	main_camera_->UpdateCameraInfo();

	CBPass cb_pass{};
	cb_pass.view_matrix = xmath_util_float4x4::TransPose(main_camera_->view_matrix());
	cb_pass.proj_matrix = xmath_util_float4x4::TransPose(main_camera_->projection_matrix());
	cb_pass.camera_position = main_camera_->world_position();

	//TODO: ���� ���� Ŭ������ ������ �װ��� ����Ͽ� �Ʒ� ���� ������Ʈ(����� �׽�Ʈ�� �ϵ��ڵ�)
	cb_pass.ambient_light = XMFLOAT4{ 0.01,0.01,0.01, 1 };
	cb_pass.lights[0].strength = XMFLOAT3{ 0.7, 0.7, 0.7 };
	cb_pass.lights[0].direction = XMFLOAT3{ 0, -1, 0 };
	cb_pass.lights[0].enable = true;
	cb_pass.lights[0].type = 0;

	cb_pass.lights[1].strength = XMFLOAT3{ 1, 0, 0 };
	cb_pass.lights[1].falloff_start = 0.1;
	cb_pass.lights[1].direction = xmath_util_float3::Normalize(main_camera_->owner()->world_look_vector());
	cb_pass.lights[1].falloff_end = 100.f;
	cb_pass.lights[1].position = main_camera_->owner()->world_position_vector();
	cb_pass.lights[1].spot_power = 14;
	cb_pass.lights[1].enable = true;
	cb_pass.lights[1].type = 2;

	for (int i = 3; i < 16; ++i)
		cb_pass.lights[i].enable = false;

	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	frame_resource_manager->curr_frame_resource()->cb_pass.get()->CopyData(0, cb_pass);

	//25.02.23 ����
	//���� ��Ʈ ��ũ���� ���̺��� ��Ʈ CBV�� ����
	D3D12_GPU_VIRTUAL_ADDRESS cb_pass_address =
		frame_resource_manager->curr_frame_resource()->cb_pass.get()->Resource()->GetGPUVirtualAddress();

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kRenderPass, cb_pass_address);

	Mesh::ResetCBObjectCurrentIndex();
	SkinnedMesh::ResetCBSkinnedMeshObjectCurrentIndex();

	// �ܼ��� ��ġ ó�� 
	// ������ ����ϴ� ���̴��� n���̸� SetPipelineState�� n�� ȣ��ȴ�
	for (const std::unique_ptr<Shader>& shader : shaders_)
	{
		command_list->SetPipelineState(shader->GetPipelineState());
		for (const std::unique_ptr<Mesh>& mesh : meshes_)
		{
			if (mesh->shader_type() == (int)shader->shader_type())
			{
				mesh->Render(command_list, frame_resource_manager, game_framework_->descriptor_manager());
			}
		}
	}

}

bool BaseScene::ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)
{

	if (main_input_controller_)
	{
		if (main_input_controller_->ProcessInput(id, w_param, l_param, time))
			return true;
	}
	switch (id)
	{
	case WM_KEYDOWN:
		// ī�޶� ��ȯ �׽�Ʈ
		if (w_param == 'K')
		{
			//�ٲ� ī�޶� ������Ʈ�� ã��
			Object* camera = FindObject("CAMERA_2");

			//�� ������Ʈ�� ī�޶�� ��Ʈ�ѷ��� ������ �����´�
			CameraComponent* new_camera = Object::GetComponent<CameraComponent>(camera);
			if (new_camera) // nullptr ����
			{
				main_camera_ = new_camera;
			}
			main_input_controller_ = Object::GetComponent<InputControllerComponent>(camera);
			return true;
		}
		if (w_param == 'L')
		{
			//�ٲ� ī�޶� ������Ʈ�� ã��
			//�� ������Ʈ�� ī�޶�� ��Ʈ�ѷ��� ������ �����´�
			CameraComponent* new_camera = Object::GetComponentInChildren<CameraComponent>(player_);
			if (new_camera) // nullptr ����
			{
				main_camera_ = new_camera;
			}
			main_input_controller_ = Object::GetComponent<FPSControllerComponent>(player_);
			return true;
		}
		if (w_param == 'O')
		{
			--stage_clear_num_;
			return true;
		}
		if (w_param == 'P')
		{
			++stage_clear_num_;
			return true;
		}
		if (w_param == 'N')
		{
			auto& mesh_list = Object::GetComponentsInChildren<SkinnedMeshComponent>(player_);
			for (auto& mesh : mesh_list)
			{
				mesh->set_is_visible(!mesh->IsVisible());
			}
		}
		break;
	default:
		return false;
		break;
	}
}

void BaseScene::Update(float elapsed_time)
{
	CheckPlayerIsGround();

	Scene::Update(elapsed_time);

}

void BaseScene::CheckPlayerIsGround()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}

	XMFLOAT3 position = player_->world_position_vector();
	constexpr float kGroundYOffset = 0.68;
	XMVECTOR ray_origin = XMLoadFloat3(&position);
	XMVECTOR ray_direction = XMVectorSet(0, -1, 0, 0);

	float distance{std::numeric_limits<float>::max()};
	for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_])
	{
		if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, distance))
		{
			if (distance <= kGroundYOffset)
			{
				player_->set_is_ground(true);
				position.y += kGroundYOffset - distance;
				player_->set_position_vector(position);
				return;
			}
		}
	}
	if (stage_clear_num_ - 1 >= 0)
	{
		for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_ - 1])
		{
			if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, distance))
			{
				if (distance <= kGroundYOffset)
				{
					player_->set_is_ground(true);
					position.y += kGroundYOffset - distance;
					player_->set_position_vector(position);
					return;
				}
			}
		}
	}

	player_->set_is_ground(false);
}

void BaseScene::PrepareGroundChecking()
{
	static const std::array<std::string, kStageMaxCount>
		stage_names{ "BASE", "STAGE1", "STAGE2", "STAGE3", "STAGE4", "STAGE5", "STAGE6", "STAGE7", };
	for (int i = 0; i < stage_names.size(); ++i)
	{
		Object* object = Scene::FindObject(stage_names[i]);
		checking_maps_mesh_collider_list_[i] = Object::GetComponentsInChildren<MeshColliderComponent>(object);
	}
	is_prepare_ground_checking_ = true;
}

