#include "stdafx.h"
#include "Scene.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "CameraComponent.h"
#include "InputManager.h"
#include "InputControllerComponent.h"
#include "GameFramework.h"
#include "MeshComponent.h"
#include "SkinnedMesh.h"


XMVECTOR Scene::GetPickingPointAtWorld(float sx, float sy, Object* picked_object)
{
	//picking ray 계산
	Object* camera_object = main_camera_->owner();

	XMVECTOR picking_point_w = XMLoadFloat3(&(camera_object->world_position_vector() + (camera_object->world_look_vector() * 100.f)));
	float picking_length_min = std::numeric_limits<float>::max();

	XMFLOAT4X4 proj = main_camera_->projection_matrix();

	float vx = (2.f * sx / kDefaultFrameBufferWidth - 1.f) / proj(0, 0);
	float vy = (-2.f * sy / kDefaultFrameBufferHeight + 1.f) / proj(1, 1);

	XMMATRIX view = XMLoadFloat4x4(&main_camera_->view_matrix());
	XMMATRIX inverse_view = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	//월드좌표계에서 피킹광선
	XMVECTOR ray_origin{ XMVectorSet(0, 0, 0, 1.f) };
	XMVECTOR ray_direction{ XMVectorSet(vx, vy, 1.f, 0) };

	ray_origin = XMVector3TransformCoord(ray_origin, inverse_view);
	ray_direction = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_view));

	//TODO: 피킹 처리 리펙토링
	for (const auto& mesh : meshes_)
	{
		if (mesh->name() == "Debug_Mesh")
			continue;
		const auto& mesh_component_list = mesh->mesh_component_list();
		for (const auto& mesh_component : mesh_component_list)
		{
			XMMATRIX world = XMLoadFloat4x4(&mesh_component->owner()->world_matrix());
			XMMATRIX inverse_world = XMMatrixInverse(&XMMatrixDeterminant(world), world);

			// 메쉬 로컬좌표의 피킹반직선
			XMVECTOR ray_origin_local = XMVector3TransformCoord(ray_origin, inverse_world);
			XMVECTOR ray_direction_local = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_world));

			float t_min{ 0 }; // 반직선 교점 매개변수 최저값, 즉 가장 가까운 교점의 매개변수
			if (mesh->bounds().Intersects(ray_origin_local, ray_direction_local, t_min))
			{
				auto& positions = mesh->positions();
				auto& indices_array = mesh->indices_array();

				t_min = std::numeric_limits<float>::max();
				for (auto& indices : indices_array)
				{
					//TODO: mesh가 triangle list 인지 strip 인지 판정해야함
					float t{ 0 };
					for (int i = 0; i < indices.size(); i += 3)
					{
						UINT i0 = indices[i + 0];
						UINT i1 = indices[i + 1];
						UINT i2 = indices[i + 2];

						XMVECTOR v0 = XMLoadFloat3(&positions[i0]);
						XMVECTOR v1 = XMLoadFloat3(&positions[i1]);
						XMVECTOR v2 = XMLoadFloat3(&positions[i2]);

						if (TriangleTests::Intersects(ray_origin_local, ray_direction_local, v0, v1, v2, t))
						{
							if (t < t_min) // 가장 반직선에 가까운 삼각형과의 교점 매개변수
							{
								t_min = t;
							}
						}
					}
				}	//forend indices_array
				if (t_min < std::numeric_limits<float>::max()) // t_min 유효한지 실제 메쉬와 부딪힌건지 체크
				{
					// 피킹지점을 월드 좌표계와 비교
					XMVECTOR picking_point = ray_origin_local + (ray_direction_local * t_min);
					picking_point = XMVector3TransformCoord(picking_point, world);
					float length = XMVectorGetX(XMVector3Length(picking_point - ray_origin));
					if (length < picking_length_min)
					{
						picking_length_min = length;
						picking_point_w = picking_point;
						picked_object = mesh_component->owner();
					}
				}
			}	//ifend mesh->bounds().Intersects
		}
	}

	return picking_point_w;
}

void Scene::Update(float elapsed_time)
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->Update(elapsed_time);
	}
}

void Scene::DeleteDeadObjects()
{
	object_list_.remove_if([](const std::unique_ptr<Object>& object) {
		return object->is_dead();
		});
}

void Scene::UpdateObjectWorldMatrix()
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->UpdateWorldMatrix(nullptr);
	}
}

Object* Scene::FindObject(const std::string& object_name)
{
	auto it = std::find_if(object_list_.begin(), object_list_.end(), [&object_name](const std::unique_ptr<Object>& object) {
		return object.get()->name() == object_name;
		});

	if (it != object_list_.end())
	{
		return (*it).get();
	}

	return nullptr;
}

ModelInfo* Scene::FindModelInfo(const std::string& name)
{
	auto it = std::find_if(model_infos_.begin(), model_infos_.end(), [&name](const std::unique_ptr<ModelInfo>& object) {
		return object.get()->model_name() == name;
		});

	if (it != model_infos_.end())
	{
		return (*it).get();
	}
	return nullptr;
}


Mesh* Scene::FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes)
{
	auto it = std::find_if(meshes.begin(), meshes.end(), [&mesh_name](const std::unique_ptr<Mesh>& mesh) {
		return mesh.get()->name() == mesh_name;
		});

	if (it != meshes.end())
	{
		return (*it).get();
	}

	return nullptr;
}

Material* Scene::FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials)
{
	auto it = std::find_if(materials.begin(), materials.end(), [&material_name](const std::unique_ptr<Material>& mat) {
		return mat.get()->name() == material_name;
		});

	if (it != materials.end())
	{
		return (*it).get();
	}

	return nullptr;
}

const std::vector<std::unique_ptr<Mesh>>& Scene::meshes() const
{
	return meshes_;
}

CameraComponent* Scene::main_camera() const
{
	return main_camera_;
}

void Scene::set_main_camera(CameraComponent* value)
{
	main_camera_ = value;
}

void Scene::AddObject(Object* object)
{
	object_list_.emplace_back();
	object_list_.back().reset(object);
}

void Scene::DeleteObject(Object* object)
{
	object_list_.remove_if([&object](const std::unique_ptr<Object>& obj) {
		return obj.get() == object;
		});
}

void Scene::ReleaseMeshUploadBuffer()
{
	for (std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->ReleaseUploadBuffer();
	}
}

void Scene::UpdateRenderPassConstantBuffer(ID3D12GraphicsCommandList* command_list)
{
	main_camera_->UpdateCameraInfo();

	CBPass cb_pass{};
	cb_pass.view_matrix = xmath_util_float4x4::TransPose(main_camera_->view_matrix());
	cb_pass.proj_matrix = xmath_util_float4x4::TransPose(main_camera_->projection_matrix());
	cb_pass.camera_position = main_camera_->world_position();

	//TODO: 조명 관련 클래스를 생성후 그것을 사용하여 아래 정보 업데이트(현재는 테스트용 하드코딩)
	cb_pass.ambient_light = XMFLOAT4{ 0.01,0.01,0.01, 1 };
	cb_pass.lights[0].strength = XMFLOAT3{ 0.7, 0.7, 0.7 };
	cb_pass.lights[0].direction = XMFLOAT3{ 0, -1, 0.f };
	cb_pass.lights[0].enable = true;
	cb_pass.lights[0].type = 0;

	//cb_pass.lights[1].strength = XMFLOAT3{ 1, 0, 0 };
	//cb_pass.lights[1].falloff_start = 0.1;
	//cb_pass.lights[1].direction = xmath_util_float3::Normalize(main_camera_->owner()->world_look_vector());
	//cb_pass.lights[1].falloff_end = 100.f;
	//cb_pass.lights[1].position = main_camera_->owner()->world_position_vector();
	//cb_pass.lights[1].spot_power = 14;
	//cb_pass.lights[1].enable = true;
	//cb_pass.lights[1].type = 2;

	for (int i = 1; i < 16; ++i)
		cb_pass.lights[i].enable = false;

	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	frame_resource_manager->curr_frame_resource()->cb_pass.get()->CopyData(0, cb_pass);

	//25.02.23 수정
	//기존 루트 디스크립터 테이블에서 루트 CBV로 변경
	D3D12_GPU_VIRTUAL_ADDRESS cb_pass_address =
		frame_resource_manager->curr_frame_resource()->cb_pass.get()->Resource()->GetGPUVirtualAddress();

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kRenderPass, cb_pass_address);
}

void Scene::Render(ID3D12GraphicsCommandList* command_list)
{
	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();

	UpdateRenderPassConstantBuffer(command_list);

	Mesh::ResetCBObjectCurrentIndex();
	SkinnedMesh::ResetCBSkinnedMeshObjectCurrentIndex();

	// 단순한 배치 처리 
	// 씬에서 사용하는 쉐이더가 n개이면 SetPipelineState가 n번 호출된다
	for (const std::unique_ptr<Shader>& shader : shaders_)
	{
		command_list->SetPipelineState(shader->GetPipelineState());
		if (shader->shader_type() == ShaderType::kDebug && !is_render_debug_mesh_)
		{
			continue;
		}
		for (const std::unique_ptr<Mesh>& mesh : meshes_)
		{
			if (mesh->shader_type() == (int)shader->shader_type())
			{
				mesh->Render(command_list, frame_resource_manager, game_framework_->descriptor_manager());
			}
		}
	}
}

void Scene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
	ID3D12RootSignature* root_signature, GameFramework* game_framework)
{
	game_framework_ = game_framework;

	BuildShader(device, root_signature);
	BuildMesh(device, command_list);
	BuildMaterial(device, command_list);
	BuildObject(device, command_list);
	BuildFrameResources(device);
	BuildDescriptorHeap(device);
	BuildConstantBufferViews(device);
	BuildShaderResourceViews(device);
}

void Scene::BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	int frame_resource_index = 0;
	for (std::unique_ptr<Material>& material : materials_)
	{
		material->CreateShaderVariables(device, command_list);
		material->set_frame_resource_index(frame_resource_index);
		++frame_resource_index;
	}
}

void Scene::BuildFrameResources(ID3D12Device* device)
{
	game_framework_->frame_resource_manager()->
		ResetFrameResources(device, 1, cb_object_capacity_,
			cb_skinned_mesh_object_capacity_, materials_.size());
}

void Scene::BuildDescriptorHeap(ID3D12Device* device)
{
	game_framework_->descriptor_manager()->
		ResetDescriptorHeap(device,
			Material::GetTextureCount());
}

void Scene::BuildShaderResourceViews(ID3D12Device* device)
{
	int heap_index = game_framework_->descriptor_manager()->srv_offset();
	for (std::unique_ptr<Material>& material : materials_)
	{
		heap_index = material->CreateShaderResourceViews(device, game_framework_->descriptor_manager(), heap_index);
	}
}

using namespace file_load_util;
void Scene::BuildScene()
{
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
}
