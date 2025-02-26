#include "stdafx.h"
#include "ModelInfo.h"
#include "Object.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "MeshComponent.h"
#include "SkinnedMeshComponent.h"
#include "Shader.h"
#include "Material.h"

ModelInfo::ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
	std::vector<std::unique_ptr<Material>>& materials)
{
	LoadModelInfoFromFile(file_name, meshes, materials);
}

ModelInfo::~ModelInfo()
{
	if (hierarchy_root_)
		delete hierarchy_root_;
}

using namespace file_load_util;
void ModelInfo::LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
	std::vector<std::unique_ptr<Material>>& materials)
{
	size_t last_slash = file_name.find_last_of("/\\");
	size_t last_dot = file_name.find_last_of('.');
	model_name_ = file_name.substr(last_slash + 1, last_dot - last_slash - 1);

	std::ifstream model_file{ file_name, std::ios::binary };

	std::string load_token; 

	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<Hierarchy>:");
#endif //_DEBUG

	hierarchy_root_ = LoadFrameInfoFromFile(model_file, meshes, materials);

	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Hierarchy>");
#endif //_DEBUG

}

Object* ModelInfo::LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes,
	std::vector<std::unique_ptr<Material>>& materials)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<Frame>:");
#endif //_DEBUG

	Object* frame = new Object;

	//object name
	ReadStringFromFile(file, load_token);
	frame->set_name(load_token);

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "<TransformMatrix>:");
#endif //_DEBUG
	XMFLOAT4X4 transform = ReadFromFile<XMFLOAT4X4>(file);
	frame->set_transform_matrix(transform);

	ReadStringFromFile(file, load_token);
	if (load_token == "<Mesh>:")
	{
		Mesh* mesh = new Mesh;
		mesh->LoadMeshFromFile(file);
		mesh->set_shader_type((int)ShaderType::kStandardMesh);

		meshes.emplace_back();
		meshes.back().reset(mesh);

		MeshComponent* mesh_component = new MeshComponent(frame, mesh);
		mesh->DeleteMeshComponent(mesh_component);	//모델 정보는 그려지면 안되기 때문에 메쉬의 컴포넌트 리스트에서 제외함
		frame->AddComponent(mesh_component);

		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "<Materials>:");
#endif //_DEBUG
		int material_count = ReadFromFile<int>(file);
		for (int i = 0; i < material_count; ++i)
		{
			Material* material = new Material();
			material->LoadMaterialFromFile(file);
			materials.emplace_back();
			materials.back().reset(material);

			mesh->AddMaterial(material);
		}
		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "</Materials>");
#endif //_DEBUG

		ReadStringFromFile(file, load_token);

	}
	else if (load_token == "<SkinningInfo>:")
	{
		SkinnedMesh* skinned_mesh = new SkinnedMesh;
		skinned_mesh->LoadSkinnedMeshFromFile(file);
		skinned_mesh->set_shader_type((int)ShaderType::kStandardSkinnedMesh);
		meshes.emplace_back();
		meshes.back().reset(skinned_mesh);

		SkinnedMeshComponent* skinned_mesh_component = new SkinnedMeshComponent(frame, skinned_mesh);
		skinned_mesh->DeleteMeshComponent(skinned_mesh_component);
		frame->AddComponent(skinned_mesh_component);

		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "<Materials>:");
#endif //_DEBUG
		int material_count = ReadFromFile<int>(file);
		for (int i = 0; i < material_count; ++i)
		{
			Material* material = new Material();
			material->LoadMaterialFromFile(file);
			materials.emplace_back();
			materials.back().reset(material);

			skinned_mesh->AddMaterial(material);
		}
		ReadStringFromFile(file, load_token);

#ifdef _DEBUG
		PrintDebugStringLoadTokenError(model_name_, load_token, "</Materials>");
#endif //_DEBUG

		ReadStringFromFile(file, load_token);

	}
	
	if (load_token == "<Children>:")
	{
		int child_count = ReadFromFile<int>(file);
		for (int i = 0; i < child_count; ++i)
		{
			frame->AddChild(LoadFrameInfoFromFile(file, meshes, materials));
		}

		ReadStringFromFile(file, load_token);
	}

#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Frame>");
#endif //_DEBUG


	return frame;
}

Object* ModelInfo::GetInstance()
{
	return Object::DeepCopy(hierarchy_root_);
}
