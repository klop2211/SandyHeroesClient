#include "stdafx.h"
#include "Scene.h"
#include "Object.h"
#include "Shader.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "Mesh.h"
#include "CameraComponent.h"
#include "InputManager.h"
#include "InputControllerComponent.h"
#include "ModelInfo.h"
#include "Material.h"
#include "AnimationSet.h"
#include "GameFramework.h"


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

void Scene::ReleaseMeshUploadBuffer()
{
	for (std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->ReleaseUploadBuffer();
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


