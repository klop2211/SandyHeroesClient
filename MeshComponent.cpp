#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Object.h"

MeshComponent::MeshComponent(Object* owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{

}

MeshComponent::MeshComponent(const MeshComponent& other) : Component(other), mesh_(other.mesh_)
{
}

Component* MeshComponent::GetCopy()
{
	return new MeshComponent(*this);
}

void MeshComponent::UpdateShaderVariables(ID3D12GraphicsCommandList* command_list)
{
	XMFLOAT4X4 owner_world_matrix;
	XMStoreFloat4x4(&owner_world_matrix, 
		XMMatrixTranspose(XMLoadFloat4x4(&owner_->world_matrix())));
	//TODO: ��Ʈ�ñ׳�ó �ۼ��� �ε��� 0�� enum class �� ����� �����ϱ�
	command_list->SetGraphicsRoot32BitConstants(0, 16, &owner_world_matrix, 0);
}
