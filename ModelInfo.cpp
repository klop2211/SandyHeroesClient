#include "stdafx.h"
#include "ModelInfo.h"
#include "Object.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "MeshComponent.h"

ModelInfo::ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes)
{
	LoadModelInfoFromFile(file_name, meshes);
}

ModelInfo::~ModelInfo()
{
	if (hierarchy_root_)
		delete hierarchy_root_;
}

using namespace file_load_util;
void ModelInfo::LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes)
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

	hierarchy_root_ = LoadFrameInfoFromFile(model_file, meshes);

	ReadStringFromFile(model_file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Hierarchy>");
#endif //_DEBUG

}

Object* ModelInfo::LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes)
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
		meshes.emplace_back();
		meshes.back().reset(mesh);

		MeshComponent* mesh_component = new MeshComponent(frame, mesh);
		frame->AddComponent(mesh_component);

		ReadStringFromFile(file, load_token);
	}
	else if (load_token == "<SkinningInfo>:")
	{
		SkinnedMesh* skinned_mesh = new SkinnedMesh;
		skinned_mesh->LoadSkinnedMeshFromFile(file);
		meshes.emplace_back();
		meshes.back().reset(skinned_mesh);

		//TODO: SkinnedMeshComponent 연결이 필요

		ReadStringFromFile(file, load_token);
	}
	
	if (load_token == "<Children>:")
	{
		int child_count = ReadFromFile<int>(file);
		for (int i = 0; i < child_count; ++i)
		{
			frame->AddChild(LoadFrameInfoFromFile(file, meshes));
		}

		ReadStringFromFile(file, load_token);
	}

#ifdef _DEBUG
	PrintDebugStringLoadTokenError(model_name_, load_token, "</Frame>");
#endif //_DEBUG


	return frame;
}
