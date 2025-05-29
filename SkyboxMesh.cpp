#include "stdafx.h"
#include "SkyboxMesh.h"
#include "Material.h"
#include "Shader.h"

SkyboxMesh::SkyboxMesh()
{
}

SkyboxMesh::SkyboxMesh(Mesh* source_mesh)
{
	name_ = "Skybox";

	positions_.assign(source_mesh->positions().begin(), source_mesh->positions().end());
	indices_array_.resize(1);		// ��ī�̹ڽ� �޽��� ���� �޽����� �Ѵ�.(������ 1���� ���� ������)
	indices_array_[0].assign(source_mesh->indices_array()[0].begin(), source_mesh->indices_array()[0].end());
}

Material* SkyboxMesh::CreateSkyboxMaterial(const std::string& texture_name, Texture* skybox_texture)
{
	Material* material = new Material();
	skybox_texture->name = texture_name;
	skybox_texture->type = TextureType::kCubeMap;
	material->AddTexture(skybox_texture);

	return material;
}
