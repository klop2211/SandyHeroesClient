#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Object.h"
#include "FrameResource.h"
#include "DescriptorManager.h"

MeshComponent::MeshComponent(Object* owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
	mesh->AddMeshComponent(this);
}

MeshComponent::MeshComponent(const MeshComponent& other) : Component(other), mesh_(other.mesh_)
{
	other.mesh_->AddMeshComponent(this);
}

MeshComponent& MeshComponent::operator=(const MeshComponent& rhs)
{
	owner_ = rhs.owner_;
	mesh_ = rhs.mesh_;
	mesh_->AddMeshComponent(this);
	return *this;
}

Component* MeshComponent::GetCopy()
{
	return new MeshComponent(*this);
}

XMFLOAT4X4 MeshComponent::GetOwnerWorld() const
{
	return owner_->world_matrix();
}

bool MeshComponent::IsVisible() const
{
	return is_visible_;
}
