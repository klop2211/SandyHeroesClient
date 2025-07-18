﻿#include "stdafx.h"
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
#include "SkinnedShadowShader.h"
#include "MeshColliderComponent.h"
#include "DebugMeshComponent.h"
#include "DebugShader.h"
#include "SkinnedMeshComponent.h"
#include "UIShader.h"
#include "UIMesh.h"
#include "TransparentShader.h"
#include "BreathingShader.h"
#include "ShadowShader.h"
#include "MonsterComponent.h"
#include "MovementComponent.h"
#include "SpawnerComponent.h"
#include "BoxColliderComponent.h"
#include "HitDragonAnimationState.h"
#include "ShotDragonAnimationState.h"
#include "UiMeshComponent.h"
#include "BombDragonAnimationState.h"
#include "StrongDragonAnimationState.h"
#include "TestAnimationState.h"
#include "ParticleComponent.h"
#include "ParticleShader.h"
#include "ProgressBarComponent.h"
#include "PlayerComponent.h"
#include "TextComponent.h"
#include "ParticleRenderer.h"

void BaseScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
	ID3D12RootSignature* root_signature, GameFramework* game_framework, 
	ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory)
{
	constexpr int kCutSceneTrackCount = 1;
	cut_scene_tracks_.reserve(kCutSceneTrackCount);
	cut_scene_tracks_.emplace_back("CutScene");

	text_renderer_ = std::make_unique<TextRenderer>();
	TextComponent::kTextRenderer = text_renderer_.get();

	Scene::Initialize(device, command_list, root_signature, game_framework, device_context, dwrite_factory);

	particle_system_ = std::make_unique<ParticleSystem>(Scene::FindMesh("green_cube", meshes_), 
		Scene::FindMaterial("green", materials_));
}

void BaseScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	constexpr int shader_count = 8;
	shaders_.reserve(shader_count);

	shaders_[(int)ShaderType::kStandardMesh] = std::make_unique<StandardMeshShader>();
	shaders_[(int)ShaderType::kStandardSkinnedMesh] = std::make_unique<StandardSkinnedMeshShader>();
	shaders_[(int)ShaderType::kSkybox] = std::make_unique<SkyboxShader>();
	shaders_[(int)ShaderType::kDebug] = std::make_unique<DebugShader>();
	shaders_[(int)ShaderType::kUI] = std::make_unique<UIShader>();
	shaders_[(int)ShaderType::kBreathing] = std::make_unique<BreathingShader>();
	shaders_[(int)ShaderType::kShadow] = std::make_unique<ShadowShader>();
	shaders_[(int)ShaderType::kSkinnedShadow] = std::make_unique<SkinnedShadowShader>();
	shaders_[(int)ShaderType::kParticle] = std::make_unique<ParticleShader>();

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
	XMFLOAT2 client_size = game_framework_->client_size();
	float ui_x = client_size.x / 2.f - ui_width / 2.f;
	float ui_y = client_size.y / 2.f - ui_height / 2.f;
	meshes_.push_back(std::make_unique<UIMesh>(ui_x, ui_y, ui_width, ui_height));
	meshes_.back().get()->set_name("CrossHair");

	//Hp, Shield Bar
	constexpr float hp_bar_width = 100.f;
	constexpr float hp_bar_height = 15.f;
	ui_width = hp_bar_width;
	ui_height = hp_bar_height;
	meshes_.push_back(std::make_unique<UIMesh>(ui_width, ui_height));
	meshes_.back().get()->set_name("ProgressBarBackground");
	ui_width = hp_bar_width - 5;
	ui_height = hp_bar_height - 5;
	meshes_.push_back(std::make_unique<UIMesh>(ui_width, ui_height));
	meshes_.back().get()->set_name("ProgressBar");

	//Main Skill Star Icon
	ui_width =  client_size.x / 16.f;
	ui_height = client_size.y / 9.f;
	ui_x = client_size.x / 2.f - ui_width / 2.f;
	ui_y = client_size.y - (client_size.y / 9.f) - ui_height / 2.f;
	meshes_.push_back(std::make_unique<UIMesh>(ui_x, ui_y, ui_width, ui_height));
	meshes_.back().get()->set_name("Star");

	//Player Hp, Shield Bar
	ui_width = client_size.x / 16.f * 3.f;
	ui_height = client_size.y / 9.f * 0.8f;
	ui_x = client_size.x / 16.f;
	ui_y = client_size.y - (client_size.y / 9.f * 1.5f);
	meshes_.push_back(std::make_unique<UIMesh>(ui_x, ui_y, ui_width, ui_height));
	meshes_.back().get()->set_name("PlayerHpBar");

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
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/classic.bin", meshes_, materials_, textures_)); //1
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/SM_Bullet_01.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Hit_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Shot_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Bomb_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Strong_Dragon.bin", meshes_, materials_, textures_));
	
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/vandal.bin", meshes_, materials_, textures_));	//7 밴달
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/Odin.bin", meshes_, materials_, textures_));	//8 오딘
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/flamethrower.bin", meshes_, materials_, textures_));	//9 화염방사기

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
			std::string object_name = load_token;
			object_list_.emplace_back();
			object_list_.back().reset(FindModelInfo(object_name)->GetInstance());

			ReadStringFromFile(scene_file, load_token); 
			XMFLOAT4X4 transfrom;
			if (load_token == "<SectorBounds>:")
			{
				auto bounds_center = ReadFromFile<XMFLOAT3>(scene_file);
				auto bounds_extents = ReadFromFile<XMFLOAT3>(scene_file);
				BoundingBox bounds{ bounds_center, bounds_extents };
				sectors_.emplace_back(object_name, bounds);
				ReadStringFromFile(scene_file, load_token); //<Transform>
				transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
				bounds.Transform(bounds, XMLoadFloat4x4(&transfrom));
				sectors_.emplace_back(object_name, bounds);
			}
			else
			{
				transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			}
			object_list_.back()->set_transform_matrix(transfrom);
		}
		else
		{
			std::string object_name = load_token;
			XMFLOAT4X4 transfrom;
			ReadStringFromFile(scene_file, load_token);
			bool is_sector = false;
			if (load_token == "<SectorBounds>:")
			{
				is_sector = true;
				auto bounds_center = ReadFromFile<XMFLOAT3>(scene_file);
				auto bounds_extents = ReadFromFile<XMFLOAT3>(scene_file);
				BoundingBox bounds{ bounds_center, bounds_extents };
				ReadStringFromFile(scene_file, load_token); //<Transform>
				transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
				bounds.Transform(bounds, XMLoadFloat4x4(&transfrom));
				sectors_.emplace_back(object_name, bounds);
			}
			else
			{
				transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			}

			model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/World/" + object_name + ".bin", meshes_, materials_, textures_));

			object_list_.emplace_back();
			object_list_.back().reset(model_infos_.back()->GetInstance());

			object_list_.back()->set_transform_matrix(transfrom);
			if (is_sector)
			{
				sectors_.back().object_list().push_back(object_list_.back().get());
			}
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

	material = new Material{ "ProgressBarBackground", (int)ShaderType::kUI };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Progress_Bar_Background";
	textures_.back()->type = TextureType::kAlbedoMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "HpBar", (int)ShaderType::kUI };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Hp_Bar_Red";
	textures_.back()->type = TextureType::kAlbedoMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "ShieldBar", (int)ShaderType::kUI };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Progress_Bar_Blue";
	textures_.back()->type = TextureType::kAlbedoMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);
	
	material = new Material{ "Star_Dark", (int)ShaderType::kUI };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Star_Dark";
	textures_.back()->type = TextureType::kAlbedoMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "Star", (int)ShaderType::kUI };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Star";
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

	//Create Particle Material
	{
		material = new Material{ "Trail_1", (int)ShaderType::kParticle };
		textures_.push_back(std::make_unique<Texture>());
		textures_.back()->name = "Trail_1";
		textures_.back()->type = TextureType::kAlbedoMap;
		material->AddTexture(textures_.back().get());
		materials_.emplace_back();
		materials_.back().reset(material);
	}

	//Create Breathing Material
	{
		material = new Material{ "Breathing", (int)ShaderType::kBreathing };
		Material* mat = Scene::FindMaterial("Desert_(Instance)", materials_);
		//textures_.push_back(std::make_unique<Texture>());
		//textures_.back()->name = "Desert";
		//textures_.back()->type = TextureType::kAlbedoMap;
		material->CopyMaterialData(mat);
		material->AddTexture(FindTexture("Desert",textures_));
		materials_.emplace_back();
		materials_.back().reset(material);
	}


	Scene::BuildMaterial(device, command_list);
}

void BaseScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	//TODO: 각 메쉬의 컴포넌트 연결 개수를 파악하면 아래 수치를 디테일하게 설정할 수 있을것 같다..
	cb_object_capacity_ = 10000;
	cb_skinned_mesh_object_capacity_ = 1000;
	cb_ui_mesh_capacity_ = 100;

	ShowCursor(false);

	//플레이어 생성
	Object* player = model_infos_[0]->GetInstance();
	player->set_name("Player");
	player->set_position_vector(XMFLOAT3{ -15, 6, 0 });
	player->set_collide_type(true, true);
	player->set_is_movable(true);
	player->AddComponent(new MovementComponent(player));
	AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(player);
	animator->set_animation_state(new PlayerAnimationState);
	auto player_component = new PlayerComponent(player);
	player->AddComponent(player_component);
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

	//player_gun_frame->AddChild(model_infos_[9]->GetInstance());	//1 권총, 7 밴달, 8 오딘, 9 화염방사기
	//player_gun_frame = player_gun_frame->child();
	//GunComponent* player_gun = new GunComponent(player_gun_frame);
	//player_gun->LoadGunInfo("flamethrower");
	//player_gun_frame->AddComponent(player_gun);
	//player_gun_frame->Rotate(0, 170, -17);

	//Object* player_gun_particle_pivot = new Object("gun_particle_pivot");
	//player_gun_frame->AddChild(player_gun_particle_pivot);
	////player_gun_particle_pivot->set_local_position(XMFLOAT3(0.0f, 0.2f, 0.3f));		//1
	////player_gun_particle_pivot->set_local_position(XMFLOAT3(-0.018f, 0.2f, 0.58f));	//7
	////player_gun_particle_pivot->set_local_position(XMFLOAT3(0.013f, 0.123f, 0.81f));	//8
	//player_gun_particle_pivot->set_local_position(XMFLOAT3(0.0f, 0.143f, 1.24f));		//9

	//// 화염방사기에 충돌 박스 달기
	//Object* flamethrower_tip = player->FindFrame("gun_particle_pivot");
	//GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player);
	//auto flamethrow_box_component = new BoxColliderComponent(flamethrower_tip, gun->flamethrow_box());
	//flamethrower_tip->AddComponent(flamethrow_box_component);

	//Set player's camera
	Object* camera_object = new Object();
	player->AddChild(camera_object);
	fps_controller->set_camera_object(camera_object);
	camera_object->set_position_vector(0, 0.3f, 0); // 플레이어 캐릭터의 키가 150인것을 고려하여 머리위치에 배치
	camera_object->set_name("CAMERA_1");
	CameraComponent* camera_component =
		new CameraComponent(camera_object, 0.01, 70,
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	camera_object->AddComponent(camera_component);
	main_camera_ = camera_component;

	// 몬스터 HIT 파티클 생성
	{
		monster_hit_particles_.emplace_back(new Object("monster_hit_particle"));
		Object* monster_particle = monster_hit_particles_.back();
		object_list_.emplace_back(monster_particle);
		monster_particle->set_local_position({ 0, 1, 0 });
		Material* monster_particle_material = std::find_if(materials_.begin(), materials_.end(), [&](const auto& material) {
			return material->name() == "Trail_1";
			})->get();
		ParticleComponent* monster_particle_component = new ParticleComponent(monster_particle, device, 1000, ParticleComponent::Sphere, monster_particle_material);
		monster_particle_component->set_scene(this);
		monster_particle->AddComponent(monster_particle_component);
	}

	// 총 발사 파티클 생성
	//{
	//	//particle_ = new Object();
	//	Material* particleMaterial = std::find_if(materials_.begin(), materials_.end(), [&](const auto& material) {
	//		return material->name() == "Trail_1";
	//		})->get();
	//	//ParticleComponent* particleComponent = new ParticleComponent(player_gun_particle_pivot, device, 50, ParticleComponent::Cone, particleMaterial);
	//	ParticleComponent* particleComponent = new ParticleComponent(player_gun_particle_pivot, device, 1000, ParticleComponent::BigCone, particleMaterial);
	//	particleComponent->set_scene(this);
	//	particleComponent->set_color({ 0.9f,0.1f,0.1f,0.5f });
	//	particleComponent->set_direction_pivot_object(camera_object);
	//	player_gun_particle_pivot->AddComponent(particleComponent);
	//}

	BuildModelInfo(device);

	//Set player's gun
	Object* player_gun_frame = player->FindFrame("WeaponR_locator");
	player_gun_frame->AddChild(FindModelInfo("Flamethrower")->GetInstance());

	//Add player to scene
	AddObject(player);

	CreateMonsterSpawner();

	CreatePlayerUI();

	//Set Spawn Boxs
	constexpr int kSpawnBoxCount = 5;
	spawn_boxs_.reserve(kSpawnBoxCount);
	for (int i = 0; i < kSpawnBoxCount; ++i)
	{
		const auto& box_object = FindObject("SpawnBox" + std::to_string(i + 1));
		const auto& box = Object::GetComponent<BoxColliderComponent>(box_object);
		spawn_boxs_.push_back(box);
	}

	//Create Skybox
	Object* skybox = new Object();
	skybox->AddComponent(new MeshComponent(skybox, 
		Scene::FindMesh("Skybox", meshes_), Scene::FindMaterial("Skybox_Cube2", materials_)));
	AddObject(skybox);


	//Create sub camera (free view)
	camera_object = new Object;
	camera_object->set_name("CAMERA_2");
	camera_component =
		new CameraComponent(camera_object, 0.01, 120,
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

	//컷씬 적용 카메라
	camera_object = new Object{};
	camera_object->set_name("CutSceneCamera");

	camera_component = new CameraComponent(camera_object, 0.01, 120,
		(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	camera_object->AddComponent(camera_component);

	cut_scene_tracks_.back().set_camera(camera_object);

	AddObject(camera_object);

	//모든 메쉬 있는 객체에 메쉬 콜라이더 추가(주의사항: 새롭게 만들어지는 메쉬있는 객체는 메쉬콜라이더가 없음)
	//+ 디버그용 메쉬 추가
	Mesh* debug_mesh = Scene::FindMesh("Debug_Mesh", meshes_);
	const auto& const debug_material = Scene::FindMaterial("debug", materials_);
	for (auto& object : object_list_)
	{
		auto& mesh_component_list = Object::GetComponentsInChildren<MeshComponent>(object.get());
		for (auto& mesh_component : mesh_component_list)
		{
			auto mesh = mesh_component->GetMesh();
			Object* object = mesh_component->owner();
			MeshColliderComponent* mesh_collider = new MeshColliderComponent(object);
			mesh_collider->set_mesh(mesh);
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

void BaseScene::BuildTextBrushAndFormat(ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory)
{
	device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &d2d_text_brush_);

	text_formats_["Default"] = std::make_unique<TextFormat>(dwrite_factory, L"Arial", 50.f);
	text_formats_["BulletCount"] = std::make_unique<TextFormat>(dwrite_factory, 
		L"Bahnschrift", 
		40.f,
		DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_ITALIC);
	text_formats_["HpCount"] = std::make_unique<TextFormat>(dwrite_factory,
		L"Bahnschrift",
		40.f,
		DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void BaseScene::BuildModelInfo(ID3D12Device* device)
{
	//Create Monster Hp, Shield UI
	{
		ModelInfo* monster_hp_ui = new ModelInfo();
		monster_hp_ui->set_model_name("Monster_Hp_UI");
		auto progress_bar_background = new Object();
		auto hp_bar = new Object();
		auto shield_bar = new Object();
		progress_bar_background->set_name("ProgressBarBackground");
		hp_bar->set_name("HpBar");
		shield_bar->set_name("ShieldBar");
		progress_bar_background->AddChild(hp_bar);
		progress_bar_background->AddChild(shield_bar);
		monster_hp_ui->set_hierarchy_root(progress_bar_background);
		model_infos_.emplace_back();
		model_infos_.back().reset(monster_hp_ui);

		auto ui_background_material = Scene::FindMaterial("ProgressBarBackground", materials_);
		auto ui_hpbar_material = Scene::FindMaterial("HpBar", materials_);
		auto ui_shieldbar_material = Scene::FindMaterial("ShieldBar", materials_);

		auto ui_background_component = new UiMeshComponent(progress_bar_background,
			Scene::FindMesh("ProgressBarBackground", meshes_), ui_background_material, this);
		// ModelInfo를 수정할 때 MeshComponent를 추가하였다면 material에서 delete 해야함.(씬 렌더에 포함되기 때문)
		ui_background_material->DeleteMeshComponent(ui_background_component); 
		progress_bar_background->AddComponent(ui_background_component);
		ui_background_component->set_ui_layer(UiLayer::kOne);

		auto ui_hpbar_component = new UiMeshComponent(hp_bar,
			Scene::FindMesh("ProgressBar", meshes_), ui_hpbar_material, this);
		// ModelInfo를 수정할 때 MeshComponent를 추가하였다면 material에서 delete 해야함.(씬 렌더에 포함되기 때문)
		ui_hpbar_material->DeleteMeshComponent(ui_hpbar_component);
		ui_hpbar_component->set_ui_ratio(XMFLOAT2{ 1.f, 0.5f });
		ui_hpbar_component->set_position_offset(XMFLOAT2{ 0.f, 5.f });
		hp_bar->AddComponent(ui_hpbar_component);
		ProgressBarComponent* progress_bar = new ProgressBarComponent(hp_bar);
		hp_bar->AddComponent(progress_bar);
		progress_bar->set_get_current_value_func([](Object* object) -> float
			{
				auto root = object->GetHierarchyRoot();
				auto monster_component = Object::GetComponent<MonsterComponent>(root);
				return monster_component->hp();
			});
		progress_bar->set_get_max_value_func([](Object* object) -> float
			{
				auto root = object->GetHierarchyRoot();
				auto monster_component = Object::GetComponent<MonsterComponent>(root);
				return monster_component->max_hp();
			});

		auto ui_shieldbar_component = new UiMeshComponent(shield_bar,
			Scene::FindMesh("ProgressBar", meshes_), ui_shieldbar_material, this);
		// ModelInfo를 수정할 때 MeshComponent를 추가하였다면 material에서 delete 해야함.(씬 렌더에 포함되기 때문)
		ui_shieldbar_material->DeleteMeshComponent(ui_shieldbar_component);
		ui_shieldbar_component->set_ui_ratio(XMFLOAT2{ 1.f, 0.5f });
		shield_bar->AddComponent(ui_shieldbar_component);
		progress_bar = new ProgressBarComponent(shield_bar);
		shield_bar->AddComponent(progress_bar);
		progress_bar->set_get_current_value_func([](Object* object) -> float
			{
				auto root = object->GetHierarchyRoot();
				auto monster_component = Object::GetComponent<MonsterComponent>(root);
				return monster_component->shield();
			});
		progress_bar->set_get_max_value_func([](Object* object) -> float
			{
				auto root = object->GetHierarchyRoot();
				auto monster_component = Object::GetComponent<MonsterComponent>(root);
				return monster_component->max_shield();
			});
	}

	//Hit Dragon Fix(Add Hp UI, Set CollisionType)
	ModelInfo* hit_dragon = FindModelInfo("Hit_Dragon");
	hit_dragon->hierarchy_root()->set_collide_type(true, true);
	hit_dragon->hierarchy_root()->set_is_movable(true);
	hit_dragon->hierarchy_root()->set_tag("Hit_Dragon");
	auto ui_head_socket = hit_dragon->hierarchy_root()->FindFrame("Ui_Head");
	auto monster_hp_ui = FindModelInfo("Monster_Hp_UI");
	ui_head_socket->AddChild(monster_hp_ui->GetInstance());

	auto animator = Object::GetComponentInChildren<AnimatorComponent>(hit_dragon->hierarchy_root());
	animator->set_animation_state(new HitDragonAnimationState);

	//Shot Dragon Fix(Add Hp UI, Set CollisionType)
	ModelInfo* shot_dragon = FindModelInfo("Shot_Dragon");
	shot_dragon->hierarchy_root()->set_collide_type(true, true);
	shot_dragon->hierarchy_root()->set_is_movable(true);
	shot_dragon->hierarchy_root()->set_tag("Shot_Dragon");
	ui_head_socket = shot_dragon->hierarchy_root()->FindFrame("Ui_Head");
	ui_head_socket->AddChild(monster_hp_ui->GetInstance());

	animator = Object::GetComponentInChildren<AnimatorComponent>(shot_dragon->hierarchy_root());
	animator->set_animation_state(new ShotDragonAnimationState);

	//Bomb Dragon Fix(Add Hp UI, Set CollisionType)
	ModelInfo* bomb_dragon = FindModelInfo("Bomb_Dragon");
	bomb_dragon->hierarchy_root()->set_collide_type(true, true);
	bomb_dragon->hierarchy_root()->set_is_movable(true);
	bomb_dragon->hierarchy_root()->set_tag("Bomb_Dragon");
	ui_head_socket = bomb_dragon->hierarchy_root()->FindFrame("Ui_Head");
	ui_head_socket->AddChild(monster_hp_ui->GetInstance());

	animator = Object::GetComponentInChildren<AnimatorComponent>(bomb_dragon->hierarchy_root());
	animator->set_animation_state(new BombDragonAnimationState);

	//Strong Dragon Fix(Set CollisionType)
	ModelInfo* strong_dragon = FindModelInfo("Strong_Dragon");
	strong_dragon->hierarchy_root()->set_collide_type(true, true);
	strong_dragon->hierarchy_root()->set_is_movable(true);
	strong_dragon->hierarchy_root()->set_tag("Strong_Dragon");
	animator = Object::GetComponentInChildren<AnimatorComponent>(strong_dragon->hierarchy_root());
	animator->set_animation_state(new StrongDragonAnimationState);

	//Create Hit Dragon Spawner
	ModelInfo* hit_dragon_spawner = new ModelInfo();
	hit_dragon_spawner->set_model_name("Hit_Dragon_Spawner");
	Object* spawner = new Object();
	auto monster_component = new MonsterComponent(nullptr);
	monster_component->set_target(player_);
	auto spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
	spawner_component->AddComponent(monster_component);
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner->AddComponent(spawner_component);
	hit_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(hit_dragon_spawner);	
	
	//Create Shot Dragon Spawner
	ModelInfo* shot_dragon_spawner = new ModelInfo();
	shot_dragon_spawner->set_model_name("Shot_Dragon_Spawner");
	spawner = new Object();
	spawner_component = new SpawnerComponent(spawner, this, shot_dragon);
	spawner_component->AddComponent(monster_component->GetCopy());
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner->AddComponent(spawner_component);
	shot_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(shot_dragon_spawner);

	//Create Bomb Dragon Spawner
	ModelInfo* bomb_dragon_spawner = new ModelInfo();
	bomb_dragon_spawner->set_model_name("Bomb_Dragon_Spawner");
	spawner = new Object();
	spawner_component = new SpawnerComponent(spawner, this, bomb_dragon);
	spawner_component->AddComponent(monster_component->GetCopy());
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner->AddComponent(spawner_component);
	bomb_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(bomb_dragon_spawner);

	//Create Strong Dragon Spawner
	ModelInfo* strong_dragon_spawner = new ModelInfo();
	strong_dragon_spawner->set_model_name("Strong_Dragon_Spawner");
	spawner = new Object();
	spawner_component = new SpawnerComponent(spawner, this, strong_dragon);
	spawner_component->AddComponent(monster_component->GetCopy());
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner->AddComponent(spawner_component);
	strong_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(strong_dragon_spawner);

	//Create Gun Models
	{
		//Create flamethrower model
		{
			ModelInfo* flamethrower_model = new ModelInfo();
			auto flamethrower_object = model_infos_[9]->GetInstance();
			flamethrower_object->set_name("Flamethrower");
			flamethrower_model->set_hierarchy_root(flamethrower_object);
			flamethrower_model->set_model_name("Flamethrower");

			GunComponent* gun_component = new GunComponent(flamethrower_object);
			gun_component->LoadGunInfo("flamethrower");
			flamethrower_object->AddComponent(gun_component);
			flamethrower_object->Rotate(0, 170, -17);

			Object* player_gun_particle_pivot = new Object("gun_particle_pivot");
			flamethrower_object->AddChild(player_gun_particle_pivot);
			player_gun_particle_pivot->set_local_position(XMFLOAT3(0.0f, 0.143f, 1.24f));		//9

			// 화염방사기에 충돌 박스 달기
			auto flamethrow_box_component = new BoxColliderComponent(player_gun_particle_pivot, gun_component->flamethrow_box());
			player_gun_particle_pivot->AddComponent(flamethrow_box_component);

			// 총 발사 파티클 생성
			Material* particleMaterial = std::find_if(materials_.begin(), materials_.end(), [&](const auto& material) {
				return material->name() == "Trail_1";
				})->get();
			ParticleComponent* particleComponent = new ParticleComponent(player_gun_particle_pivot, device, 1000, ParticleComponent::BigCone, particleMaterial);
			particleComponent->set_scene(this);
			particleComponent->set_color({ 0.9f,0.1f,0.1f,0.5f });
			player_gun_particle_pivot->AddComponent(particleComponent);

			model_infos_.emplace_back();
			model_infos_.back().reset(flamethrower_model);
		}
		//Create Classic Model
		{
			ModelInfo* classic_model = new ModelInfo();
			auto classic_object = model_infos_[1]->GetInstance();
			classic_object->set_name("Classic");
			classic_model->set_hierarchy_root(classic_object);
			classic_model->set_model_name("Classic");

			GunComponent* gun_component = new GunComponent(classic_object);
			gun_component->LoadGunInfo("classic");
			classic_object->AddComponent(gun_component);
			classic_object->Rotate(0, 170, -17);

			Object* player_gun_particle_pivot = new Object("gun_particle_pivot");
			classic_object->AddChild(player_gun_particle_pivot);
			player_gun_particle_pivot->set_local_position(XMFLOAT3(0.0f, 0.2f, 0.3f));		//1

			// 총 발사 파티클 생성
			Material* particleMaterial = std::find_if(materials_.begin(), materials_.end(), [&](const auto& material) {
				return material->name() == "Trail_1";
				})->get();
			ParticleComponent* particleComponent = new ParticleComponent(player_gun_particle_pivot, device, 50, ParticleComponent::Cone, particleMaterial);
			particleComponent->set_scene(this);
			particleComponent->set_color({ 0.9f,0.1f,0.1f,0.5f });
			player_gun_particle_pivot->AddComponent(particleComponent);

			model_infos_.emplace_back();
			model_infos_.back().reset(classic_model);
		}
	}

}

void BaseScene::CreatePlayerUI()
{
	UiMeshComponent* ui_mesh_component;
	//Create CroosHair
	{
		Object* ui = new Object();
		ui_mesh_component = new UiMeshComponent(ui,
			Scene::FindMesh("CrossHair", meshes_), Scene::FindMaterial("CrossHair", materials_), this);
		ui_mesh_component->set_is_static(true);
		ui->AddComponent(ui_mesh_component);
		AddObject(ui);
	}

	//Create Main Skill Star Icon
	{
		Object* star_icon = new Object();
		Object* star_icon_background = new Object();
		star_icon->AddChild(star_icon_background);

		star_icon_background->set_name("Star_Icon_Background");
		ui_mesh_component = new UiMeshComponent(star_icon_background,
			Scene::FindMesh("Star", meshes_), Scene::FindMaterial("Star_Dark", materials_), this);
		ui_mesh_component->set_is_static(true);
		ui_mesh_component->set_ui_layer(UiLayer::kOne);
		star_icon_background->AddComponent(ui_mesh_component);

		star_icon->set_name("Star_Icon");
		ui_mesh_component = new UiMeshComponent(star_icon,
			Scene::FindMesh("Star", meshes_), Scene::FindMaterial("Star", materials_), this);
		ui_mesh_component->set_is_static(true);
		star_icon->AddComponent(ui_mesh_component);
		ProgressBarComponent* progress_bar = new ProgressBarComponent(star_icon);
		progress_bar->set_max_value(10.f);
		progress_bar->set_type(UiType::kProgressBarY);
		progress_bar->set_view(player_);
		progress_bar->set_get_current_value_func([](Object* object) -> float
			{
				return object->position_vector().y;
			});
		star_icon->AddComponent(progress_bar);
		AddObject(star_icon);
	}

	//Create Player Hp, Shield Bar
	{
		Object* player_shield_bar = new Object();
		Object* player_hp_bar = new Object();
		Object* player_hp_bar_background = new Object();
		player_shield_bar->set_name("PlayerShieldBar");
		player_hp_bar->set_name("PlayerHpBar");
		player_hp_bar_background->set_name("PlayerHpBarBackground");
		player_hp_bar_background->AddChild(player_hp_bar);
		player_hp_bar_background->AddChild(player_shield_bar);

		auto ui_mesh = Scene::FindMesh("PlayerHpBar", meshes_);
		auto ui_size = static_cast<UIMesh*>(ui_mesh)->ui_size();

		auto ui_background_material = Scene::FindMaterial("ProgressBarBackground", materials_);
		auto ui_hpbar_material = Scene::FindMaterial("HpBar", materials_);
		auto ui_shieldbar_material = Scene::FindMaterial("ShieldBar", materials_);

		auto ui_background_component = new UiMeshComponent(player_hp_bar_background,
			ui_mesh, ui_background_material, this);
		player_hp_bar_background->AddComponent(ui_background_component);
		ui_background_component->set_ui_layer(UiLayer::kOne);
		ui_background_component->set_is_static(true);

		auto player_component = Object::GetComponent<PlayerComponent>(player_);

		auto ui_hpbar_component = new UiMeshComponent(player_hp_bar,
			ui_mesh, ui_hpbar_material, this);
		player_hp_bar->AddComponent(ui_hpbar_component);
		ui_hpbar_component->set_is_static(true);
		ui_hpbar_component->set_ui_ratio({0.9f, 0.4f});
		ui_hpbar_component->set_position_offset({ ui_size.x * 0.05f, ui_size.y * 0.5f });
		auto hp_progress_bar = new ProgressBarComponent(player_hp_bar);
		hp_progress_bar->set_view(player_);
		hp_progress_bar->set_max_value(player_component->max_hp());
		hp_progress_bar->set_get_current_value_func([](Object* object) -> float
			{
				auto player_component = Object::GetComponent<PlayerComponent>(object->GetHierarchyRoot());
				return player_component->hp();
			});
		player_hp_bar->AddComponent(hp_progress_bar);

		auto ui_shieldbar_component = new UiMeshComponent(player_shield_bar,
			ui_mesh, ui_shieldbar_material, this);
		player_shield_bar->AddComponent(ui_shieldbar_component);
		ui_shieldbar_component->set_is_static(true);
		ui_shieldbar_component->set_ui_ratio({ 0.9f, 0.4f });
		ui_shieldbar_component->set_position_offset({ ui_size.x * 0.05f, ui_size.y * 0.1f });
		auto shield_progress_bar = new ProgressBarComponent(player_shield_bar);
		shield_progress_bar->set_view(player_);
		shield_progress_bar->set_max_value(player_component->max_shield());
		shield_progress_bar->set_get_current_value_func([](Object* object) -> float
			{
				auto player_component = Object::GetComponent<PlayerComponent>(object->GetHierarchyRoot());
				return player_component->shield();
			});
		player_shield_bar->AddComponent(shield_progress_bar);

		AddObject(player_hp_bar_background);
	}

	//Create Player's Bullet Count Text
	{
		Object* bullet_count_text = new Object();
		bullet_count_text->set_name("BulletCountText");
		const auto client_size = game_framework_->client_size();
		const float l = client_size.x - (client_size.x / 16.f * 2.f);
		const float t = client_size.y - (client_size.y / 9.f * 1.5f);
		const float r = client_size.x;
		const float b = client_size.y - (client_size.y / 9.f);
		auto d2d1_rect = D2D1::RectF(l, t, r, b);
		std::function<std::wstring (Object*)> get_bullet_count_func =
			[](Object* object)
			{
				auto player_component = Object::GetComponentInChildren<GunComponent>(object);
				if (player_component)
				{
					return std::to_wstring(player_component->loaded_bullets());
				}
				return std::wstring(L"None");
			};
		auto text_component = new TextComponent(
			bullet_count_text,
			text_formats_["BulletCount"].get(),
			d2d1_rect,
			get_bullet_count_func
		);
		text_component->set_view(player_);
		text_component->set_color(D2D1::ColorF(D2D1::ColorF::Green));
		bullet_count_text->AddComponent(text_component);
		player_->AddChild(bullet_count_text);
	}

	//Create Player's hp, shield text
	{
		Object* player_hp_text = new Object();
		player_hp_text->set_name("PlayerHpText");
		const auto client_size = game_framework_->client_size();
		const float l = client_size.x / 16.f * 4.1f;
		const float t = client_size.y - (client_size.y / 9.f * 1.15f);
		const float r = client_size.x / 16.f * 6.f;
		const float b = client_size.y - (client_size.y / 9.f * 0.5f);
		auto d2d1_rect = D2D1::RectF(l, t, r, b);
		std::function<std::wstring (Object*)> get_hp_func =
			[](Object* object)
			{
				auto player_component = Object::GetComponentInChildren<PlayerComponent>(object);
				return std::to_wstring((int)player_component->hp());
			};
		auto text_component = new TextComponent(
			player_hp_text,
			text_formats_["HpCount"].get(),
			d2d1_rect,
			get_hp_func
		);
		text_component->set_view(player_);
		text_component->set_color(D2D1::ColorF(D2D1::ColorF::Red));
		player_hp_text->AddComponent(text_component);
		player_->AddChild(player_hp_text);

		Object* player_shield_text = new Object();
		player_shield_text->set_name("PlayerHpText");
		const float l2 = client_size.x / 16.f * 4.1f;
		const float t2 = client_size.y - (client_size.y / 9.f * 1.5f);
		const float r2 = client_size.x / 16.f * 6.f;
		const float b2 = client_size.y - (client_size.y / 9.f * 0.5f);
		d2d1_rect = D2D1::RectF(l2, t2, r2, b2);
		std::function<std::wstring(Object*)> get_shield_func =
			[](Object* object)
			{
				auto player_component = Object::GetComponentInChildren<PlayerComponent>(object);
				return std::to_wstring((int)player_component->shield());
			};
		text_component = new TextComponent(
			player_shield_text,
			text_formats_["HpCount"].get(),
			d2d1_rect,
			get_shield_func
		);
		text_component->set_view(player_);
		text_component->set_color(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		player_shield_text->AddComponent(text_component);
		player_->AddChild(player_shield_text);

	}
}

void BaseScene::CreateMonsterSpawner()
{
	std::function<Object* (ModelInfo*, int&, XMFLOAT3, int, float, float)> create_spawner =
		[this]
		(ModelInfo* spawner_model, int& spawner_id, XMFLOAT3 spawn_position, int spawn_count, float spawn_time, float spawn_cool_time)
		{
			Object* spawner = spawner_model->GetInstance();
			spawner->set_name(spawner_model->model_name() + "_" + std::to_string(++spawner_id));
			spawner->set_position_vector(spawn_position);
			auto spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
			spawner_component->SetSpawnerInfo(spawn_count, spawn_time, spawn_cool_time);

			return spawner;
		};

	//hit dragon
	int hit_spawner_id = 0;
	ModelInfo* hit_dragon_spawner = FindModelInfo("Hit_Dragon_Spawner");

	//shot dragon
	int shot_spawner_id = 0;
	ModelInfo* shot_dragon_spawner = FindModelInfo("Shot_Dragon_Spawner");

	//bomb dragon
	int bomb_spawner_id = 0;
	ModelInfo* bomb_dragon_spawner = FindModelInfo("Bomb_Dragon_Spawner");

	//strong dragon
	int strong_spawner_id = 0;
	ModelInfo* strong_dragon_spawner = FindModelInfo("Strong_Dragon_Spawner");

	Object* spawner;
	SpawnerComponent* spawner_component;
	//Stage 1
	{
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 17.38f, 0.61f, -0.92f }, 3, 3.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 16.f, 2.6f, 11.74f }, 3, 4.f, 4.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 16.84f, 1.24f, -9.07f }, 3, 5.f, 3.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 27.85f, 6.73f, -8.07f }, 1, 9.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);

		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 24.53f, 5.31f, 10.05f }, 1, 11.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);
	}

	//Stage 2
	{
		//hit 1
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 58.91f, 2.97f, 0.28f }, 3, 0.5f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);

		//hit 2		
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 58.91f, 2.97f, 9.13f }, 3, 0.5f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);

		//shot 1
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 63.63f, 7.66f, -3.49f }, 1, 3.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
		
		//shot 2
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 63.63f, 7.66f, 11.94f }, 1, 3.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);

		//shot 3
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 80.41f, 7.66f, 11.94f }, 1, 8.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
		
		//shot 4
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 80.41f, 7.66f, -3.45f }, 1, 8.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
		
		//bomb 1
		spawner = create_spawner(bomb_dragon_spawner, bomb_spawner_id, XMFLOAT3{ 50.f, 0.47f, 24.14f }, 2, 14.f, 4.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
		
		//bomb 2
		spawner = create_spawner(bomb_dragon_spawner, bomb_spawner_id, XMFLOAT3{ 49.43f, 0.47f, -15.51f }, 2, 14.f, 4.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
		}

	//Stage 4
	{
		spawner = create_spawner(strong_dragon_spawner, strong_spawner_id, XMFLOAT3{ 55.36f, 1.2f, -156.52f }, 1, 0.f, 4.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[3].push_back(spawner_component);
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
		//컷씬 실행중에는 인풋처리를 끈다!
		if (!is_play_cutscene_)
		{
			if (main_input_controller_->ProcessInput(id, w_param, l_param, time))
				return true;
		}
	}
	switch (id)
	{
	case WM_KEYDOWN:
		if (w_param == '1')
		{
			//particle_renderers_.back()->set_color({0.9f,0.1f,0.1f,0.5f});
			Object* player_gun_frame = player_->FindFrame("WeaponR_locator");
			player_gun_frame->ChangeChild(FindModelInfo("Classic")->GetInstance(), "Flamethrower", false);
		}
		if (w_param == '2')
		{
			//particle_renderers_.back()->set_color({ 0.1f,0.9f,0.1f,0.5f });
			Object* player_gun_frame = player_->FindFrame("WeaponR_locator");
			player_gun_frame->ChangeChild(FindModelInfo("Flamethrower")->GetInstance(), "Classic", false);

		}
		if (w_param == '3')
		{
			//particle_renderers_.back()->set_color({ 0.9f,0.9f,0.1f,0.5f });
		}
		if (w_param == '4')
		{
			// 총기 변경
		}
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
			cut_scene_tracks_[0].Play(this);
			auto movement = Object::GetComponent<MovementComponent>(player_);
			movement->Stop();
			return true;
		}
		if (w_param == 'P')
		{
			catch_monster_num_ = 1;
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
		if (w_param == 'B')
		{
			auto pos = player_->position_vector();
			player_->set_position_vector(pos.x, pos.y + 10.f, pos.z);
			return true;
		}
		if (w_param == VK_OEM_COMMA) // ,
		{
			auto player_component = Object::GetComponent<PlayerComponent>(player_);
			if (player_component)
			{
				player_component->HitDamage(10.f);
			}
			return true;
		}
		if (w_param == VK_OEM_PERIOD) // .
		{
			auto player_component = Object::GetComponent<PlayerComponent>(player_);
			if (player_component)
			{
				player_component->Heal(10.f);
			}
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
	for (auto& cut_scene_track : cut_scene_tracks_)
	{
		cut_scene_track.Run(elapsed_time);
	}

	Scene::Update(elapsed_time);

	//particle_system_->Update(elapsed_time);
	//particle_->Update(elapsed_time);

	UpdateObjectHitWall();
	
	UpdateObjectWorldMatrix();

	UpdateObjectIsGround();

	UpdateObjectHitBullet();

	UpdateObjectHitObject();

	DeleteDeadObjects();

	UpdateStageClear();

	CheckSpawnBoxHitPlayer();


}

void BaseScene::RenderText(ID2D1Bitmap1* d2d_render_target, ID2D1DeviceContext2* d2d_device_context)
{
	text_renderer_->Render(d2d_render_target, d2d_device_context, d2d_text_brush_.Get());
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
		CheckObjectHitFlamethrow(object);
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
	switch (stage_clear_num_)
	{
	case 0:
		if (catch_monster_num_ <= 0)
			return;
		break;
	case 1:
		if (catch_monster_num_ < 11)
			return;
		break;
	case 2:
		if (catch_monster_num_ < 14)
			return;
		break;
	case 3:		
		if (catch_monster_num_ < 1)
			return;
		//TODO: 스테이지 3번은 투명발판을 밟아 다음 스테이지로 진행해야 클리어
	case 4:
		if (catch_monster_num_ < 1)
			return;
		break;
	case 5:
		if (catch_monster_num_ < 10)
			return;
		break;
	case 6:
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
		break;
	case 7:
		// TODO: 게임클리어!
		if (catch_monster_num_ < 1)
			return;
		break;
	default:
		break;
	}
	// 현재 스테이지에서 "Cube" 메쉬 제거
	auto& mesh_list = checking_maps_mesh_collider_list_[stage_clear_num_];
	mesh_list.remove_if([](MeshColliderComponent* collider) {
		return collider->mesh() && collider->mesh()->name() == "Cube";
		});

	// 스테이지 넘버 증가
	++stage_clear_num_;
	catch_monster_num_ = 0;

	is_activate_spawner_ = false;

}

void BaseScene::CheckObjectIsGround(Object* object)
{
	XMFLOAT3 position = object->world_position_vector();
	constexpr float kGroundYOffset = 0.75f;
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
	constexpr float kGroundYOffset = 0.75f;
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
				constexpr float kGroundYOffset = 0.75f;
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
					//object->set_position_vector(object_pos + dir * 0.1f);

					// TODO : 몬스터 AI완성 이후 충돌시에 밀리는 기능 추가

					/*auto monster = Object::GetComponent<MonsterComponent>(object);
					if (monster)
					{
						monster->set_is_pushed(true);
						monster->set_push_timer(5.0f);
					}*/
				}
				return;
			}
		}
	}
	//else
	//{
	//	auto box1 = Object::GetComponentInChildren<BoxColliderComponent>(object);
	//	if (!box1) return;

	//	BoundingOrientedBox obb1 = box1->animated_box();

	//	for (auto& other : ground_check_object_list_)
	//	{
	//		if (!other || other == object || other->is_dead()) continue;

	//		auto box2 = Object::GetComponentInChildren<BoxColliderComponent>(other);
	//		if (!box2) continue;

	//		if (obb1.Intersects(box2->animated_box()))
	//		{
	//			XMFLOAT3 position = object->world_position_vector();
	//			constexpr float kGroundYOffset = 0.75f;
	//			position.y += kGroundYOffset;
	//			XMVECTOR ray_origin = XMLoadFloat3(&position);
	//			position.y -= kGroundYOffset;

	//			XMFLOAT3 other_pos = other->world_position_vector();
	//			XMFLOAT3 dir = xmath_util_float3::Normalize(object_pos - other_pos);
	//			XMVECTOR ray_direction = XMLoadFloat3(&dir);
	//			ray_direction = XMVectorSetY(ray_direction, 0);
	//			ray_direction = XMVector3Normalize(ray_direction);

	//			if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
	//				return;

	//			bool is_collide = false;
	//			float distance{ std::numeric_limits<float>::max() };
	//			for (auto& mesh_collider : checking_maps_mesh_collider_list_[stage_clear_num_])
	//			{
	//				float t{};
	//				if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
	//				{
	//					if (t < distance)
	//					{
	//						distance = t;
	//					}
	//				}
	//			}

	//			constexpr float kMinSafeDistance = 1.5f; // 살짝 밀려도 충돌 안나도록 여유
	//			if (distance > kMinSafeDistance) // 벽에 안 부딪힌다면 밀기
	//			{
	//				object->set_position_vector(object_pos + dir * 0.1f);
	//			}
	//			return;
	//		}
	//	}
	//}
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
				MonsterComponent* monster = Object::GetComponent<MonsterComponent>(object);
				if (monster && monster->IsDead())
					continue;
				particle_system_->SpawnParticle(this, box_collider->animated_box().Center, 10, 0.5f);
				bullet->set_is_dead(true);
				if (monster && !monster->IsDead())
				{
					ParticleComponent* gun_particle = nullptr;
					{
						Object* flame_tip = player_->FindFrame("gun_particle_pivot");
						if (flame_tip)
							gun_particle = Object::GetComponent<ParticleComponent>(flame_tip);
					}


					// 몬스터 HIT 파티클 출력
					ParticleComponent* particle_component = Object::GetComponent<ParticleComponent>(monster_hit_particles_.front());
					particle_component->set_hit_position(monster->owner()->world_position_vector());
					particle_component->set_color(gun_particle->color());
					particle_component->Play(50);
					monster->HitDamage(gun->damage());

					if(monster->IsDead())
						catch_monster_num_++;
				}
			}
		}
	}

}

void BaseScene::CheckObjectHitFlamethrow(Object* object)
{
	FPSControllerComponent* controller = Object::GetComponent<FPSControllerComponent>(player_);
	if (!controller || !controller->is_firekey_down())
		return;

	GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_);
	auto& bullet_list = gun->fired_bullet_list();

	auto& box_collider_list = Object::GetComponentsInChildren<BoxColliderComponent>(object);
	if (!box_collider_list.size())
		return;

	if (gun->gun_name() == "flamethrower") // 화염방사기 조건
	{
		Object* flame_tip = player_->FindFrame("gun_particle_pivot");
		auto flame_collider = Object::GetComponent<BoxColliderComponent>(flame_tip);
		if (!flame_collider) return;

		for (auto& monster_box : Object::GetComponentsInChildren<BoxColliderComponent>(object))
		{
			if (flame_collider->animated_box().Intersects(monster_box->animated_box()))
			{
				ParticleComponent* gun_particle = nullptr;
				{
					Object* flame_tip = player_->FindFrame("gun_particle_pivot");
					if (flame_tip)
						gun_particle = Object::GetComponent<ParticleComponent>(flame_tip);
				}

				// 데미지 적용
				MonsterComponent* monster = Object::GetComponent<MonsterComponent>(object);
				if (monster && !monster->IsDead())
				{
					ParticleComponent* gun_particle = nullptr;
					{
						Object* flame_tip = player_->FindFrame("gun_particle_pivot");
						if (flame_tip)
							gun_particle = Object::GetComponent<ParticleComponent>(flame_tip);
					}

					// 몬스터 HIT 파티클 출력
					ParticleComponent* particle_component = Object::GetComponent<ParticleComponent>(monster_hit_particles_.front());
					particle_component->set_hit_position(monster->owner()->world_position_vector());
					particle_component->set_color(gun_particle->color());
					particle_component->Play(50);

					monster->HitDamage(gun->damage());
					if (monster->IsDead())
						catch_monster_num_++;
				}
			}
		}
		return;
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

void BaseScene::CheckSpawnBoxHitPlayer()
{
	if (stage_clear_num_ < 1)
		return;
	const auto& const spawn_box = spawn_boxs_[stage_clear_num_ - 1];
	auto& player_meshes = Object::GetComponentsInChildren<MeshComponent>(player_);
	for (const auto& mesh : player_meshes)
	{
		BoundingBox mesh_animated_aabb;
		mesh->GetMesh()->bounds().Transform(mesh_animated_aabb, XMLoadFloat4x4(&mesh->owner()->world_matrix()));
		if (spawn_box->animated_box().Intersects(mesh_animated_aabb) && !is_activate_spawner_)
		{
			if (stage_clear_num_ == 4)
			{
				auto& mesh_list = Object::GetComponentsInChildren<SkinnedMeshComponent>(player_);
				for (auto& mesh : mesh_list)
				{
					mesh->set_is_visible(!mesh->IsVisible());
				}
				cut_scene_tracks_[0].Play(this);
				auto controller = Object::GetComponent<FPSControllerComponent>(player_);
				controller->Stop();
			}
			ActivateStageMonsterSpawner(stage_clear_num_ - 1);
			is_activate_spawner_ = true;
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