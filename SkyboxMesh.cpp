#include "stdafx.h"
#include "SkyboxMesh.h"
#include "Material.h"
#include "Shader.h"

SkyboxMesh::SkyboxMesh()
{
}

SkyboxMesh::SkyboxMesh(Mesh* source_mesh, Material* material)
{
	shader_type_ = (int)ShaderType::kSkybox;
	name_ = "Skybox";

	positions_.assign(source_mesh->positions().begin(), source_mesh->positions().end());
	indices_array_.resize(1);		// ��ī�̹ڽ� �޽��� ���� �޽����� �Ѵ�.(������ 1���� ���� ������)
	indices_array_[0].assign(source_mesh->indices_array()[0].begin(), source_mesh->indices_array()[0].end());
	
	if (material)
	{
		AddMaterial(material);
	}
}

Material* SkyboxMesh::CreateSkyboxMaterial(const std::string& texture_name)
{
	Material* material = new Material();
	Texture* skybox_texture{ new Texture };
	skybox_texture->name = texture_name;
	skybox_texture->type = TextureType::kCubeMap;
	material->AddTexture(skybox_texture);

	return material;
}
