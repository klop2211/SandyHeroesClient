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

	//XMVECTOR ray_origin_local = XMVector3TransformCoord(ray_origin, inverse_world);
	//XMVECTOR ray_direction_local = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_world));

	BoundingOrientedBox bounds;
	BoundingOrientedBox::CreateFromBoundingBox(bounds, mesh_->bounds());
	bounds.Transform(bounds, world);

	float t{};
	float t_min{ std::numeric_limits<float>::max() };
	bool is_collide{ false };
	if (bounds.Intersects(ray_origin, ray_direction, t))
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

					XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[i0]), world);
					XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[i1]), world);
					XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[i2]), world);

					if (TriangleTests::Intersects(ray_origin, ray_direction, v0, v1, v2, t))
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
		out_distance = t_min;
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

//bool MeshColliderComponent::CheckWallCollisionByObb(BoundingOrientedBox& obb)
//{
//	if (!mesh_) return false;
//
//	const auto& indices_array = mesh_->indices_array();
//	const auto& positions = mesh_->positions();
//	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());
//
//	constexpr float kStepHeightThreshold = 0.5f; // 계단 허용 최대 높이
//
//	for (const auto& indices : indices_array)
//	{
//		for (int i = 0; i + 2 < indices.size(); i += 3)
//		{
//			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 0]]), world);
//			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 1]]), world);
//			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 2]]), world);
//
//			// 삼각형 법선 벡터 계산
//			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0));
//			float upness = XMVectorGetX(XMVector3Dot(normal, XMVectorSet(0, 1, 0, 0)));
//
//			// 바닥 또는 계단 가능성 있는 면만 필터링
//			if (upness >= 0.5f)
//			{
//				if (obb.Intersects(v0, v1, v2))
//				{
//					// 삼각형 중심 높이 계산
//					float tri_y = (
//						XMVectorGetY(v0) +
//						XMVectorGetY(v1) +
//						XMVectorGetY(v2)
//						) / 3.0f;
//
//					float player_y = obb.Center.y;
//					float delta_y = player_y - tri_y;
//
//					// 너무 높은 경우 → 벽
//					if (delta_y > kStepHeightThreshold)
//					{
//						OutputDebugStringA("Wall? Collision\n");
//						return true; // 벽 충돌
//					}
//
//					// 계단이면 통과 허용 → 충돌 아님
//					continue;
//				}
//			}
//			else
//			{
//				// 수직 벽 가능성 (법선이 위를 향하지 않음)
//				if (obb.Intersects(v0, v1, v2))
//				{
//					OutputDebugStringA("Perpect Wall Collision\n");
//					return true; // 확실한 벽
//				}
//			}
//		}
//	}
//
//	return false; // 충돌 없음
//}

//bool MeshColliderComponent::CheckWallCollisionByObb(BoundingOrientedBox& obb)
//{
//	if (!mesh_)
//		return false;
//
//	const auto& indices_array = mesh_->indices_array();
//	const auto& positions = mesh_->positions();
//	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());
//
//	for (const auto& indices : indices_array)
//	{
//		for (int i = 0; i + 2 < indices.size(); i += 3)
//		{
//			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i]]), world);
//			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 1]]), world);
//			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 2]]), world);
//
//			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0));
//			float upness = XMVectorGetX(XMVector3Dot(normal, XMVectorSet(0, 1, 0, 0)));
//
//			if (upness < 0.9f) // 벽이라고 판단
//			{
//				if (obb.Intersects(v0, v1, v2))
//				{
//					OutputDebugStringA("Wall collision detected!\n");
//					return true; // 벽과 충돌
//				}
//			}
//		}
//	}
//
//	return false; // 벽과 충돌 없음
//}

bool MeshColliderComponent::CheckWallCollisionByObb(BoundingOrientedBox& obb)
{
	if (!mesh_) return false;

	const auto& indices_array = mesh_->indices_array();
	const auto& positions = mesh_->positions();
	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());

	constexpr float kStepHeightThreshold = 15.0f;      // 계단 최대 허용 높이
	constexpr float kWalkableUpness = 0.3f;            // y축을 기준으로 평평한 면 최소 기준 (0.3 ~ 0.7 추천)

	for (const auto& indices : indices_array)
	{
		for (int i = 0; i + 2 < indices.size(); i += 3)
		{
			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 0]]), world);
			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 1]]), world);
			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 2]]), world);

			// 법선 벡터 계산
			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0));
			float upness = XMVectorGetX(XMVector3Dot(normal, XMVectorSet(0, 1, 0, 0)));

			// OBB와 삼각형 충돌 검사
			if (obb.Intersects(v0, v1, v2))
			{
				// 삼각형 중심 y좌표
				float tri_y = (
					XMVectorGetY(v0) +
					XMVectorGetY(v1) +
					XMVectorGetY(v2)
					) / 3.0f;

				float player_y = obb.Center.y;
				float delta_y = player_y - tri_y;

				// ✅ 계단 조건: 어느 정도 위로 향하고 + 높이차 작음
				if (upness >= kWalkableUpness && delta_y <= kStepHeightThreshold)
				{
					// 계단 → 충돌 아님
					continue;
				}

				// ✅ 벽 조건: 너무 가파르거나 너무 높음
				OutputDebugStringA("Wall Collision\n");
				return true;
			}
		}
	}

	return false; // 충돌 없음
}


BoundingOrientedBox MeshColliderComponent::GetWorldOBB() const
{
	if (!mesh_) return BoundingOrientedBox{};

	BoundingOrientedBox obb;
	BoundingOrientedBox::CreateFromBoundingBox(obb, mesh_->bounds());

	// 월드 좌표계로 변환
	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());
	obb.Transform(obb, world);

	return obb;
}


void MeshColliderComponent::set_mesh(Mesh* mesh)
{
	mesh_ = mesh;
}
