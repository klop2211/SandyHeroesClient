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
#include "GameFramework.h"
#include "GunComponent.h"
#include "SkyboxShader.h"
#include "SkyboxMesh.h"
#include "MeshColliderComponent.h"
#include "DebugMeshComponent.h"
#include "DebugShader.h"
#include "SkinnedMeshComponent.h"
#include "UIShader.h"
#include "UIMesh.h"
#include "TransparentShader.h"
#include "BreathingShader.h"
#include "MonsterComponent.h"
#include "MovementComponent.h"
#include "SpawnerComponent.h"
#include "BoxColliderComponent.h"
#include "HitDragonAnimationState.h"
#include "ShotDragonAnimationState.h"

void BaseScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, ID3D12RootSignature* root_signature, GameFramework* game_framework)
{
	Scene::Initialize(device, command_list, root_signature, game_framework);

	particle_system_ = std::make_unique<ParticleSystem>(Scene::FindMesh("green_cube", meshes_), 
		Scene::FindMaterial("green", materials_));
}

void BaseScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	constexpr int shader_count = 7;
	shaders_.reserve(shader_count);

	shaders_[(int)ShaderType::kStandardMesh] = std::make_unique<StandardMeshShader>();
	shaders_[(int)ShaderType::kStandardSkinnedMesh] = std::make_unique<StandardSkinnedMeshShader>();
	shaders_[(int)ShaderType::kSkybox] = std::make_unique<SkyboxShader>();
	shaders_[(int)ShaderType::kDebug] = std::make_unique<DebugShader>();
	shaders_[(int)ShaderType::kUI] = std::make_unique<UIShader>();
	shaders_[(int)ShaderType::kBreathing] = std::make_unique<BreathingShader>();

	//TODO: ���̴��� �����Ǵ� ���׸��� ���� Reserve

	for (const auto& [type, shader] : shaders_)
	{
		shader->CreateShader(device, root_signature);
	}
}

using namespace file_load_util;
void BaseScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{	
	//������ �̸� �ε��Ǵ� �������� ������ �� �׽�Ʈ �������� �̸� �˼� ����
	constexpr UINT kMeshCount{ 60 };
	meshes_.reserve(kMeshCount);
	meshes_.push_back(std::make_unique<CubeMesh>());
	meshes_.back().get()->set_name("green_cube");

	//CrossHair
	constexpr float cross_hair_size = 64.f;
	float ui_width = cross_hair_size;
	float ui_height = cross_hair_size;
	float ui_x = kDefaultFrameBufferWidth / 2.f - ui_width / 2.f;
	float ui_y = kDefaultFrameBufferHeight / 2.f - ui_height / 2.f;
	meshes_.push_back(std::make_unique<UIMesh>(ui_x, ui_y, ui_width, ui_height));
	meshes_.back().get()->set_name("CrossHair");

	//skybox
	meshes_.push_back(std::make_unique<SkyboxMesh>(meshes_[0].get()));

	//debug mesh
	Mesh* debug_mesh = new CubeMesh();
	debug_mesh->ClearNormals();
	debug_mesh->ClearNormals();
	debug_mesh->ClearTangents();
	debug_mesh->set_name("Debug_Mesh");
	meshes_.emplace_back();
	meshes_.back().reset(debug_mesh);

	constexpr UINT kModelInfoCount{ 40 };
	model_infos_.reserve(kModelInfoCount);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Dog00.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/classic.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/SM_Bullet_01.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Hit_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Shot_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Bomb_Dragon.bin", meshes_, materials_, textures_));

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

			model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/World/" + object_name + ".bin", meshes_, materials_, textures_));

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

void BaseScene::BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	Material* material = new Material{ "green", (int)ShaderType::kStandardMesh, {0, 1, 0, 1} };
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "CrossHair", (int)ShaderType::kUI };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "CrossHair";
	textures_.back()->type = TextureType::kAlbedoMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "Skybox_Cube2", (int)ShaderType::kSkybox };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Skybox_Cube2";
	textures_.back()->type = TextureType::kCubeMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "debug", (int)ShaderType::kDebug };
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "Face27", (int)ShaderType::kStandardSkinnedMesh };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Face27";
	textures_.back()->type = TextureType::kAlbedoMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	//Create Breathing Material
	{
		material = new Material{ "Breathing", (int)ShaderType::kBreathing };
		Material* mat = Scene::FindMaterial("Desert_(Instance)", materials_);
		textures_.push_back(std::make_unique<Texture>());
		textures_.back()->name = "Desert";
		textures_.back()->type = TextureType::kAlbedoMap;
		material->CopyMaterialData(mat);
		material->AddTexture(textures_.back().get());
		materials_.emplace_back();
		materials_.back().reset(material);
	}


	Scene::BuildMaterial(device, command_list);
}

void BaseScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	//TODO: 각 메쉬의 컴포넌트 연결 개수를 파악하면 아래 수치를 디테일하게 설정할 수 있을것 같다..
	cb_object_capacity_ = 15000;
	cb_skinned_mesh_object_capacity_ = 10000;

	ShowCursor(false);

	//플레이어 생성
	Object* player = model_infos_[0]->GetInstance();
	player->set_name("Player");
	player->set_position_vector(XMFLOAT3{ -15, 30, 0 });
	player->set_collide_type(true, true);
	player->AddComponent(new MovementComponent(player));
	AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(player);
	animator->set_animation_state(new PlayerAnimationState);
	player_ = player;

	//player's mesh is invisible
	auto& mesh_list = Object::GetComponentsInChildren<SkinnedMeshComponent>(player_);
	for (auto& mesh : mesh_list)
	{
		mesh->set_is_visible(!mesh->IsVisible());
	}

	//Set FPS Controller with player
	FPSControllerComponent* fps_controller = new FPSControllerComponent(player);
	fps_controller->set_client_wnd(game_framework_->main_wnd());
	fps_controller->set_scene(this);
	player->AddComponent(fps_controller);

	//Set Scene's main input controller
	main_input_controller_ = fps_controller;

	//Load All GunInfos
	GunComponent::LoadGunInfosFromFile("./Resource/GunInfos.txt");

	//Set player's gun
	//TODO: 총기 메쉬 장착 구현
	Object* player_gun_frame = player->FindFrame("WeaponR_locator");
	player_gun_frame->AddChild(model_infos_[1]->GetInstance());
	player_gun_frame = player_gun_frame->child();
	GunComponent* player_gun = new GunComponent(player_gun_frame);
	player_gun->LoadGunInfo("classic");
	player_gun_frame->AddComponent(player_gun);
	player_gun_frame->Rotate(0, 170, -17);

	//Set player's camera
	Object* camera_object = new Object();
	player->AddChild(camera_object);
	fps_controller->set_camera_object(camera_object);
	camera_object->set_position_vector(0, 0.4f, 0); // 플레이어 캐릭터의 키가 150인것을 고려하여 머리위치에 배치
	camera_object->set_name("CAMERA_1");
	CameraComponent* camera_component =
		new CameraComponent(camera_object, 0.01, 10000,
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	camera_object->AddComponent(camera_component);
	main_camera_ = camera_component;

	//Add player to scene
	AddObject(player);

	//Create Monster spawner
	CreateMonsterSpawner();

	//Create CroosHair
	Object* ui = new Object();
	ui->AddComponent(new MeshComponent(ui, 
		Scene::FindMesh("CrossHair", meshes_), Scene::FindMaterial("CrossHair", materials_)));
	AddObject(ui);

	//Create Skybox
	Object* skybox = new Object();
	skybox->AddComponent(new MeshComponent(skybox, 
		Scene::FindMesh("Skybox", meshes_), Scene::FindMaterial("Skybox_Cube2", materials_)));
	AddObject(skybox);

	//Create sub camera (free view)
	camera_object = new Object;
	camera_object->set_name("CAMERA_2");
	camera_component =
		new CameraComponent(camera_object, 0.3, 10000,
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	TestControllerComponent* controller = new TestControllerComponent(camera_object);
	controller->set_client_wnd(game_framework_->main_wnd());
	MovementComponent* movement = new MovementComponent(camera_object);
	movement->DisableGarvity();
	camera_object->AddComponent(movement);
	camera_object->AddComponent(camera_component);
	camera_object->AddComponent(controller);

	object_list_.emplace_back();
	object_list_.back().reset(camera_object);

	//모든 메쉬 있는 객체에 메쉬 콜라이더 추가(주의사항: 새롭게 만들어지는 메쉬있는 객체는 메쉬콜라이더가 없음)
	//+ 디버그용 메쉬 추가
	Mesh* debug_mesh = Scene::FindMesh("Debug_Mesh", meshes_);
	const auto& const debug_material = Scene::FindMaterial("debug", materials_);
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
				auto debug_mesh_component = new DebugMeshComponent(object, debug_mesh, mesh->bounds());
				debug_mesh_component->AddMaterial(debug_material);
				object->AddComponent(debug_mesh_component);
			}
		}
	}
	PrepareGroundChecking();
	for (const auto& collider : checking_maps_mesh_collider_list_[3])
	{
		auto object = collider->owner();

		auto mesh_components = Object::GetComponentsInChildren<MeshComponent>(object);
		for (auto& mesh_comp : mesh_components)
		{
			Mesh* mesh = mesh_comp->GetMesh();
			if (!mesh) continue;

			const std::string& name = mesh->name();
			if (name == "Hex_01A" || name == "Hex_01B")
			{
				mesh_comp->ChangeMaterial(0, Scene::FindMaterial("Breathing", materials_));
			}
		}
	}

	Scene::UpdateObjectWorldMatrix();
}

void BaseScene::CreateMonsterSpawner()
{
	Object* spawner{ nullptr };
	SpawnerComponent* spawner_component{ nullptr };
	MonsterComponent* monster_component{ nullptr };

	//hit dragon
	ModelInfo* hit_dragon = FindModelInfo("Hit_Dragon");
	hit_dragon->hierarchy_root()->set_collide_type(true, true);
	auto animator = Object::GetComponentInChildren<AnimatorComponent>(hit_dragon->hierarchy_root());
	animator->set_animation_state(new HitDragonAnimationState);
	int hit_spawner_id = 0;


	//shot dragon
	ModelInfo* shot_dragon = FindModelInfo("Shot_Dragon");
	shot_dragon->hierarchy_root()->set_collide_type(true, true);
	animator = Object::GetComponentInChildren<AnimatorComponent>(shot_dragon->hierarchy_root());
	animator->set_animation_state(new ShotDragonAnimationState);
	int shot_spawner_id = 0;

	//Stage 1
	{
		spawner = new Object();
		spawner->set_name("Hit_Dragon_Spawner_" + std::to_string(++hit_spawner_id));
		spawner->set_position_vector(59.f, 9.3f, 42.8f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
		spawner_component->SetSpawnerInfo(3, 13.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = new Object();
		spawner->set_name("Hit_Dragon_Spawner_" + std::to_string(++hit_spawner_id));
		spawner->set_position_vector(67.8f, 2.6f, -1.5f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
		spawner_component->SetSpawnerInfo(3, 5.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = new Object();
		spawner->set_name("Hit_Dragon_Spawner_" + std::to_string(++hit_spawner_id));
		spawner->set_position_vector(88.2f, 10.f, -36.1f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
		spawner_component->SetSpawnerInfo(3, 8.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = new Object();
		spawner->set_name("Shot_Dragon_Spawner_" + std::to_string(++shot_spawner_id));
		spawner->set_position_vector(97.1f, 20.8f, 40.f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, shot_dragon);
		spawner_component->SetSpawnerInfo(3, 9.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = new Object();
		spawner->set_name("Shot_Dragon_Spawner_" + std::to_string(++shot_spawner_id));
		spawner->set_position_vector(111.6f, 26.9f, -32.6f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, shot_dragon);
		spawner_component->SetSpawnerInfo(3, 11.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

	}

	//Stage 2
	{
		spawner = new Object();
		spawner->set_name("Hit_Dragon_Spawner_" + std::to_string(++hit_spawner_id));
		spawner->set_position_vector(235.8f, 10.6f, 20.5f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
		spawner_component->SetSpawnerInfo(3, 0.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);

		spawner = new Object();
		spawner->set_name("Hit_Dragon_Spawner_" + std::to_string(++hit_spawner_id));
		spawner->set_position_vector(235.8f, 1.62f, 95.3f);
		monster_component = new MonsterComponent(nullptr);
		monster_component->set_target(player_);
		spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
		spawner_component->SetSpawnerInfo(3, 4.f, 5.f);
		spawner_component->AddComponent(monster_component);
		spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		spawner->AddComponent(spawner_component);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);

		//spawner = new Object();
		//spawner->set_name("Shot_Dragon_Spawner_" + std::to_string(++shot_spawner_id));
		//spawner->set_position_vector(97.1f, 20.8f, 40.f);
		//monster_component = new MonsterComponent(nullptr);
		//monster_component->set_target(player_);
		//spawner_component = new SpawnerComponent(spawner, this, shot_dragon);
		//spawner_component->SetSpawnerInfo(3, 9.f, 5.f);
		//spawner_component->AddComponent(monster_component);
		//spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
		//spawner->AddComponent(spawner_component);
		//AddObject(spawner);
		//stage_monster_spawner_list_[1].push_back(spawner_component);

	}
}

void BaseScene::ActivateStageMonsterSpawner(int stage_num)
{
	if (stage_num < 0 || stage_num >= kStageMaxCount)
	{
		return;
	}
	for (auto& spawner : stage_monster_spawner_list_[stage_num])
	{
		spawner->ActivateSpawn();
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
		// 카메라 전환 테스트
		if (w_param == 'K')
		{
			ShowCursor(true);
			//바꿀 카메라 오브젝트를 찾고
			Object* camera = FindObject("CAMERA_2");

			//그 오브젝트의 카메라와 컨트롤러를 씬으로 가져온다
			CameraComponent* new_camera = Object::GetComponent<CameraComponent>(camera);
			if (new_camera) // nullptr 방지
			{
				main_camera_ = new_camera;
			}
			main_input_controller_ = Object::GetComponent<TestControllerComponent>(camera);
			return true;
		}
		if (w_param == 'L')
		{
			ShowCursor(false);
			//바꿀 카메라 오브젝트를 찾고
			//그 오브젝트의 카메라와 컨트롤러를 씬으로 가져온다
			CameraComponent* new_camera = Object::GetComponentInChildren<CameraComponent>(player_);
			if (new_camera) // nullptr 방지
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
			ActivateStageMonsterSpawner(stage_clear_num_);
			catch_monster_num_ = 8;
			//++stage_clear_num_;
			return true;
		}
		if (w_param == 'N')
		{
			auto& mesh_list = Object::GetComponentsInChildren<SkinnedMeshComponent>(player_);
			for (auto& mesh : mesh_list)
			{
				mesh->set_is_visible(!mesh->IsVisible());
			}
			return true;
		}
		if (w_param == '0')
		{
			auto& mesh_list = Object::GetComponentsInChildren<SkinnedMeshComponent>(player_);
			for (auto& mesh : mesh_list)
			{
				if (mesh->owner()->name() == "Face01")
				{
					mesh->ChangeMaterial(0, Scene::FindMaterial("Face27", materials_));
				}
			}
			return true;
		}
		if (w_param == 'M')
		{
			is_render_debug_mesh_ = !is_render_debug_mesh_;
			return true;
		}
		break;
	default:
		return false;
		break;
	}
}

void BaseScene::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);

	particle_system_->Update(elapsed_time);

	UpdateObjectHitWall();
	
	UpdateObjectWorldMatrix();

	UpdateObjectIsGround();

	UpdateObjectHitBullet();

	UpdateObjectHitObject();

	DeleteDeadObjects();

	UpdateStageClear();

}

void BaseScene::AddObject(Object* object)
{
	Scene::AddObject(object);

	CollideType collide_type = object->collide_type();
	if (collide_type.ground_check)
	{
		ground_check_object_list_.push_back(object);
	}
	if (collide_type.wall_check)
	{
		wall_check_object_list_.push_back(object);
	}
}

void BaseScene::DeleteObject(Object* object)
{
	CollideType collide_type = object->collide_type();
	if (collide_type.ground_check)
	{
		ground_check_object_list_.remove(object);
	}
	if (collide_type.wall_check)
	{
		wall_check_object_list_.remove(object);
	}

	Scene::DeleteObject(object);

}

void BaseScene::DeleteDeadObjects()
{
	ground_check_object_list_.remove_if([](const Object* object) {
		return object->is_dead();
		});
	wall_check_object_list_.remove_if([](const Object* object) {
		return object->is_dead();
		});
	Scene::DeleteDeadObjects();
}

void BaseScene::UpdateObjectIsGround()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}

	for (auto& object : ground_check_object_list_)
	{
		CheckObjectIsGround(object);
	}
}

void BaseScene::UpdateObjectHitWall()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}

	for (auto& object : wall_check_object_list_)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		XMFLOAT3 velocity = movement->velocity();
		CheckPlayerHitWall(object, velocity);
	}
}

void BaseScene::UpdateObjectHitBullet()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	for (auto& object : ground_check_object_list_)
	{
		if (object == player_)
			continue;
		CheckObjectHitBullet(object);
	}
}

void BaseScene::UpdateObjectHitObject()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	for (auto& object : ground_check_object_list_)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		CheckObjectHitObject(object);
	}
}

void BaseScene::UpdateStageClear()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	if (catch_monster_num_ > 7)
	{
		// 현재 스테이지에서 "Cube" 메쉬 제거
		auto& mesh_list = checking_maps_mesh_collider_list_[stage_clear_num_];
		mesh_list.remove_if([](MeshColliderComponent* collider) {
			return collider->mesh() && collider->mesh()->name() == "Cube";
			});

		// 스테이지 넘버 증가
		++stage_clear_num_;
		catch_monster_num_ = 0;
	}
	for (auto& object : ground_check_object_list_)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		CheckPlayerHitPyramid(object);
	}
	if (get_key_num_ == 3)
	{
		auto& mesh_list = checking_maps_mesh_collider_list_[stage_clear_num_];
		mesh_list.remove_if([](MeshColliderComponent* collider) {
			return collider->mesh() && collider->mesh()->name() == "Cube";
			});
		++stage_clear_num_;
		get_key_num_ = 0;
	}
}

void BaseScene::CheckObjectIsGround(Object* object)
{
	XMFLOAT3 position = object->world_position_vector();
	constexpr float kGroundYOffset = 1.5f;
	position.y += kGroundYOffset;
	XMVECTOR ray_origin = XMLoadFloat3(&position);
	position.y -= kGroundYOffset;
	XMVECTOR ray_direction = XMVectorSet(0, -1, 0, 0);

	bool is_collide = false;
	float distance{ std::numeric_limits<float>::max() };
	for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_])
	{
		float t{};
		if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
		{
			is_collide = true;
			if (t < distance)
			{
				distance = t;
			}
		}
	}
	if (stage_clear_num_ - 1 >= 0)
	{
		for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_ - 1])
		{
			float t{};
			if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
			{
				is_collide = true;
				if (t < distance)
				{
					distance = t;
				}
			}
		}
	}
	if (is_collide)
	{
		float distance_on_ground = distance - kGroundYOffset; //지면까지의 거리
		if (distance_on_ground > 0.005f)
		{
			object->set_is_ground(false);
			return;
		}
		position.y -= distance_on_ground;
		object->set_is_ground(true);
		object->set_position_vector(position);
		return;
	}

	object->set_is_ground(false);
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

void BaseScene::CheckPlayerHitWall(Object* object, const XMFLOAT3& velocity)
{
	XMFLOAT3 position = object->world_position_vector();
	constexpr float kGroundYOffset = 1.5f;
	position.y += kGroundYOffset;
	XMVECTOR ray_origin = XMLoadFloat3(&position);
	position.y -= kGroundYOffset;

	XMVECTOR ray_direction = XMLoadFloat3(&velocity);
	ray_direction = XMVectorSetY(ray_direction, 0);
	ray_direction = XMVector3Normalize(ray_direction);

	if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
		return;

	bool is_collide = false;
	float distance{ std::numeric_limits<float>::max() };
	for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_])
	{
		float t{};
		if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
		{
			if (t < distance)
			{
				distance = t;
			}
		}
	}

	constexpr float MAX_DISTANCE = 0.5f;
	if (distance < MAX_DISTANCE)
		is_collide = true;


	if (stage_clear_num_ - 1 >= 0)
	{
		for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_ - 1])
		{
			float t{};
			if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
			{
				if (t < distance)
				{
					distance = t;
				}
			}
		}
		if (distance < MAX_DISTANCE)
			is_collide = true;
	}


	if (is_collide)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		movement->Stop();
		return;
	}
}

void BaseScene::CheckObjectHitObject(Object* object)
{
	if (!object || object->is_dead()) return;

	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	if (!movement) return;

	XMFLOAT3 object_pos = object->world_position_vector();

	auto mesh_collider = Object::GetComponentInChildren<MeshColliderComponent>(object);
	if (mesh_collider)
	{
		BoundingOrientedBox obb1 = mesh_collider->GetWorldOBB();

		for (auto& other : ground_check_object_list_)
		{
			if (!other || other == object || other->is_dead()) continue;

			auto other_box = Object::GetComponentInChildren<BoxColliderComponent>(other);
			if (!other_box) continue;

			if (obb1.Intersects(other_box->animated_box()))
			{
				XMFLOAT3 position = object->world_position_vector();
				constexpr float kGroundYOffset = 1.5f;
				position.y += kGroundYOffset;
				XMVECTOR ray_origin = XMLoadFloat3(&position);
				position.y -= kGroundYOffset;

				XMFLOAT3 other_pos = other->world_position_vector();
				XMFLOAT3 dir = xmath_util_float3::Normalize(object_pos - other_pos);

				XMVECTOR ray_direction = XMLoadFloat3(&dir);
				ray_direction = XMVectorSetY(ray_direction, 0);
				ray_direction = XMVector3Normalize(ray_direction);

				if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
					return;

				bool is_collide = false;
				float distance{ std::numeric_limits<float>::max() };
				for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_])
				{
					float t{};
					if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
					{
						if (t < distance)
						{
							distance = t;
						}
					}
				}

				constexpr float kMinSafeDistance = 1.5f; // 살짝 밀려도 충돌 안나도록 여유
				if (distance > kMinSafeDistance) // 벽에 안 부딪힌다면 밀기
				{
					object->set_position_vector(object_pos + dir * 0.1f);
				}
				return;
			}
		}
	}
	else
	{
		auto box1 = Object::GetComponentInChildren<BoxColliderComponent>(object);
		if (!box1) return;

		BoundingOrientedBox obb1 = box1->animated_box();

		for (auto& other : ground_check_object_list_)
		{
			if (!other || other == object || other->is_dead()) continue;

			auto box2 = Object::GetComponentInChildren<BoxColliderComponent>(other);
			if (!box2) continue;

			if (obb1.Intersects(box2->animated_box()))
			{
				XMFLOAT3 position = object->world_position_vector();
				constexpr float kGroundYOffset = 1.5f;
				position.y += kGroundYOffset;
				XMVECTOR ray_origin = XMLoadFloat3(&position);
				position.y -= kGroundYOffset;

				XMFLOAT3 other_pos = other->world_position_vector();
				XMFLOAT3 dir = xmath_util_float3::Normalize(object_pos - other_pos);
				XMVECTOR ray_direction = XMLoadFloat3(&dir);
				ray_direction = XMVectorSetY(ray_direction, 0);
				ray_direction = XMVector3Normalize(ray_direction);

				if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
					return;

				bool is_collide = false;
				float distance{ std::numeric_limits<float>::max() };
				for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_])
				{
					float t{};
					if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
					{
						if (t < distance)
						{
							distance = t;
						}
					}
				}

				constexpr float kMinSafeDistance = 1.5f; // 살짝 밀려도 충돌 안나도록 여유
				if (distance > kMinSafeDistance) // 벽에 안 부딪힌다면 밀기
				{
					object->set_position_vector(object_pos + dir * 0.1f);
				}
				return;
			}
		}
	}
}

void BaseScene::CheckObjectHitBullet(Object* object)
{
	GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_);
	auto& bullet_list = gun->fired_bullet_list();


	auto& box_collider_list = Object::GetComponentsInChildren<BoxColliderComponent>(object);
	if (!box_collider_list.size())
		return;

	for (auto& box_collider : box_collider_list)
	{
		for (auto& bullet : bullet_list)
		{
			if (bullet->is_dead())
				continue;
			BoxColliderComponent* bullet_collider = Object::GetComponent<BoxColliderComponent>(bullet);
			if (!bullet_collider)
				continue;


			if (bullet_collider->animated_box().Intersects(box_collider->animated_box()))
			{
				if (bullet->is_dead())
					continue;
				particle_system_->SpawnParticle(this, box_collider->animated_box().Center, 10, 0.5f);
				bullet->set_is_dead(true);
				MonsterComponent* monster = Object::GetComponent<MonsterComponent>(object);
				if (monster)
				{
					monster->set_hp(monster->hp() - gun->damage());
					catch_monster_num_++;
				}
				return;
			}
		}
	}

}

void BaseScene::CheckPlayerHitPyramid(Object* object)
{
	if (stage_clear_num_ != 6) return; // 스테이지 6에서만 체크

	auto mesh_collider = Object::GetComponentInChildren<MeshColliderComponent>(object);
	if (!mesh_collider) return;

	BoundingOrientedBox player_obb = mesh_collider->GetWorldOBB();

	for (auto& pyramid_collider : checking_maps_mesh_collider_list_[6])
	{
		if (!pyramid_collider || !pyramid_collider->mesh()) continue;

		auto name = pyramid_collider->mesh()->name();

		if (name != "Pyramid_01") continue;
		BoundingOrientedBox pyramid_obb = pyramid_collider->GetWorldOBB();

		if (player_obb.Intersects(pyramid_obb))
		{
			// 피라미드 획득 처리
			get_key_num_++;

			// 피라미드 제거 (Scene과 충돌 리스트에서 제거)
			Object* pyramid_object = pyramid_collider->owner();
			MeshComponent* mesh_component = Object::GetComponent<MeshComponent>(pyramid_object);
			MeshColliderComponent* mesh_collider_component = Object::GetComponent<MeshColliderComponent>(pyramid_object);
			mesh_component->set_is_visible(false);
			//DeleteObject(pyramid_object); // Scene::DeleteObject 호출 포함됨
			auto& mesh_list = checking_maps_mesh_collider_list_[6];
			mesh_list.remove_if([&](MeshColliderComponent* collider) {
				return collider == mesh_collider_component;
				});

			break;
		}
	}
}

std::list<MeshColliderComponent*> BaseScene::checking_maps_mesh_collider_list(int index)
{
	return checking_maps_mesh_collider_list_[index];
}

int BaseScene::stage_clear_num()
{
	return stage_clear_num_;
}

void BaseScene::add_catch_monster_num()
{
	catch_monster_num_++;
	return ;
}