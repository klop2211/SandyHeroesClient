#include "stdafx.h"
#include "ColliderComponent.h"
#include "Object.h"

ColliderComponent::ColliderComponent(Object* owner, BoundingOrientedBox* bounding_box) : Component(owner)
{
	bounding_box_ = bounding_box;
}

ColliderComponent::ColliderComponent(const ColliderComponent& other) : Component(nullptr)
{

}

Component* ColliderComponent::GetCopy()
{
	return new ColliderComponent(*this);
}

void ColliderComponent::Update(float elapsed_time)
{
	bounding_box_->Transform(*bounding_box_, XMLoadFloat4x4(&owner_->world_matrix()));
	XMStoreFloat4(&bounding_box_->Orientation, XMQuaternionNormalize(XMLoadFloat4(&bounding_box_->Orientation)));
}

bool ColliderComponent::CheckOBBMeshCollision(const MeshComponent* other_mesh, const XMFLOAT4X4& player_world_matrix, const XMFLOAT4X4& other_world_matrix)
{
	if (nullptr == other_mesh) return false; // 메쉬 데이터가 없는 경우

	auto indices = other_mesh->GetMesh()->GetIndicesArray();
	auto positions = other_mesh->GetMesh()->GetPositions();

	for (int i = 0; i < indices.size(); ++i)
	{
		for (int j = 0; j < indices[i].size(); ++j) 
		{
			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[j]), XMLoadFloat4x4(&other_world_matrix));
			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[j + 1]), XMLoadFloat4x4(&other_world_matrix));
			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[j + 2]), XMLoadFloat4x4(&other_world_matrix));

			if (bounding_box_->Intersects(v0, v1, v2))
			{
				return true;
			}
		}
	}

	return false; // 충돌 없음
}

bool ColliderComponent::Intersects(ColliderComponent* collider_object)
{
	return (bounding_box_->Intersects(*collider_object->bounding_box_));
}
