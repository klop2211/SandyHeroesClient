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


void Scene::UpdateObjectWorldMatrix()
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->UpdateWorldMatrix(nullptr);
	}
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
