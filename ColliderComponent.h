#pragma once
#include "Component.h"
#include "MeshComponent.h"

class ColliderComponent : public Component
{
public:
	ColliderComponent() = delete;
	ColliderComponent(Object* owner, BoundingOrientedBox* bounding_box);
	ColliderComponent(const ColliderComponent& other);
	virtual ~ColliderComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	bool CheckOBBMeshCollision(const MeshComponent* other_mesh, const XMFLOAT4X4& player_world_matrix, const XMFLOAT4X4& other_world_matrix);
	bool Intersects(ColliderComponent* collider_object);
protected:
	BoundingOrientedBox* bounding_box_ = nullptr;

};