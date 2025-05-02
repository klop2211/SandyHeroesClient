#include "stdafx.h"
#include "MeshColliderComponent.h"
#include "Object.h"
#include "Mesh.h"

MeshColliderComponent::MeshColliderComponent(Object* owner) : Component(owner)
{
}

MeshColliderComponent::MeshColliderComponent(const MeshColliderComponent& other) : Component(other.owner_)
{
}

Component* MeshColliderComponent::GetCopy()
{
	return new MeshColliderComponent(*this);
}

bool MeshColliderComponent::CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance)
{
	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());
	XMMATRIX inverse_world = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	XMVECTOR ray_origin_local = XMVector3TransformCoord(ray_origin, inverse_world);
	XMVECTOR ray_direction_local = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_world));

	float t{};
	float t_min{ std::numeric_limits<float>::max() };
	bool is_collide{ false };
	if (mesh_->bounds().Intersects(ray_origin_local, ray_direction_local, t))
	{
		auto& positions = mesh_->positions();
		auto& indices_array = mesh_->indices_array();

		//TODO: 트라이앵글 스트립에 대한 처리 구현
		if (mesh_->primitive_topology() == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			for (auto& indices : indices_array)
			{
				for (int i = 0; i < indices.size(); i += 3)
				{
					UINT i0 = indices[i + 0];
					UINT i1 = indices[i + 1];
					UINT i2 = indices[i + 2];

					XMVECTOR v0 = XMLoadFloat3(&positions[i0]);
					XMVECTOR v1 = XMLoadFloat3(&positions[i1]);
					XMVECTOR v2 = XMLoadFloat3(&positions[i2]);

					if (TriangleTests::Intersects(ray_origin_local, ray_direction_local, v0, v1, v2, t))
					{
						is_collide = true;
						if (t < t_min) // 가장 반직선에 가까운 삼각형과의 교점 매개변수
						{
							t_min = t;
						}
					}
				}
			}
		}
	}
	if (is_collide)
	{
		ray_direction_local = ray_direction_local* t_min;
		XMVector3Transform(ray_direction, world);
		out_distance = XMVectorGetX(XMVector3Length(ray_direction_local));
	}

	return is_collide;
}

bool MeshColliderComponent::CollisionCheckByObb(BoundingOrientedBox obb)
{
	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());
	XMMATRIX inverse_world = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	BoundingOrientedBox obb_local{};
	obb.Transform(obb_local, inverse_world);

	bool is_collide{ false };
	if (mesh_->bounds().Intersects(obb_local))
	{
		auto& positions = mesh_->positions();
		auto& indices_array = mesh_->indices_array();

		//TODO: 트라이앵글 스트립에 대한 처리 구현
		if (mesh_->primitive_topology() == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			for (auto& indices : indices_array)
			{
				for (int i = 0; i < indices.size(); i += 3)
				{
					UINT i0 = indices[i + 0];
					UINT i1 = indices[i + 1];
					UINT i2 = indices[i + 2];

					XMVECTOR v0 = XMLoadFloat3(&positions[i0]);
					XMVECTOR v1 = XMLoadFloat3(&positions[i1]);
					XMVECTOR v2 = XMLoadFloat3(&positions[i2]);

					if (obb_local.Intersects(v0, v1, v2))
					{
						is_collide = true;
					}
				}
			}
		}
	}

	return is_collide;
}

void MeshColliderComponent::set_mesh(Mesh* mesh)
{
	mesh_ = mesh;
}
