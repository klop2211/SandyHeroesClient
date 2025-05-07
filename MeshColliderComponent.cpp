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

bool MeshColliderComponent::CheckWallCollisionByObb(const BoundingOrientedBox& curr_obb, const XMVECTOR& move_dir)
{
	if (!mesh_) return false;

	const auto& indices_array = mesh_->indices_array();
	const auto& positions = mesh_->positions();

	//Map오브젝트의 메쉬를 기준으로해서 obb만들고 실제 위치로 옮겨주기
	
	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());

	// 기본 AABB 충돌 먼저 확인
	BoundingOrientedBox mesh_obb;
	BoundingOrientedBox::CreateFromBoundingBox(mesh_obb, mesh_->bounds());
	mesh_obb.Transform(mesh_obb, world);
	

	//{
	//	XMMATRIX playerWorld = XMLoadFloat4x4(&object->world_matrix());

	//	// 기본 AABB 충돌 먼저 확인
	//	BoundingOrientedBox playerObb;
	//	BoundingOrientedBox::CreateFromBoundingBox(playerObb, object.get mesh_->bounds());
	//	mesh_obb.Transform(mesh_obb, playerWorld);
	//}
	if (!mesh_obb.Intersects(curr_obb)) return false;

	// 무릎 높이 계산
	float knee_height = curr_obb.Center.y - curr_obb.Extents.y * 0.5f;

	// 선분 시작점: 무릎 위치 + 약간 앞
	XMVECTOR start = XMLoadFloat3(&curr_obb.Center);
	start = XMVectorSetY(start, knee_height);
	XMVECTOR short_forward = XMVector3Normalize(move_dir) * 0.4f;
	XMVECTOR probe_pos = start + short_forward;

	// 수직 레이 캐스팅 (위에서 아래로)
	XMVECTOR ray_origin = probe_pos + XMVectorSet(0, 1000.0f, 0, 0);
	XMVECTOR ray_dir = XMVectorSet(0, -1, 0, 0);

	
	float closest_y = FLT_MAX;
	bool is_hit = false;
	float best_upness = -1.0f;

	for (const auto& indices : indices_array)
	{
		for (int i = 0; i + 2 < indices.size(); i += 3)
		{
			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 0]]), world);
			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 1]]), world);
			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 2]]), world);
			
			float t = 0.0f;
			if (TriangleTests::Intersects(ray_origin, ray_dir, v0, v1, v2, t))
			{
				XMVECTOR hit_point = ray_origin + t * ray_dir;
				float y = XMVectorGetY(hit_point);

				if (y < closest_y)
				{
					closest_y = y;
					is_hit = true;

					// 평면의 기울기 확인 (법선 벡터) 추가
					XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0));
					best_upness = XMVectorGetX(XMVector3Dot(normal, XMVectorSet(0, 1, 0, 0)));

				}
			}
		}
	}

	if (is_hit)
	{
		float step_height = closest_y - (curr_obb.Center.y - curr_obb.Extents.y);
		if (step_height >= 0.0f && step_height <= 4.0f && best_upness >= 0.5f)
		{
			// 계단으로 간주 → 충돌 무시
			return false;
		}
	}

	// 계단이 아니면 벽 충돌로 간주
	OutputDebugStringA("[WallCollision] 충돌 발생\n");
	return true;
}





//bool MeshColliderComponent::CheckWallCollisionByObb(BoundingOrientedBox& curr_obb)
//{
//	if (!mesh_) return false;
//
//	const auto& indices_array = mesh_->indices_array();
//	const auto& positions = mesh_->positions();
//	XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());
//
//	// 기본 AABB 충돌 먼저 확인
//	BoundingOrientedBox mesh_obb;
//	BoundingOrientedBox::CreateFromBoundingBox(mesh_obb, mesh_->bounds());
//	mesh_obb.Transform(mesh_obb, world);
//	if (!mesh_obb.Intersects(curr_obb)) return false;
//
//	// 진행 방향 계산 (curr_obb.Velocity 또는 전역 제공 필요)
//	// 여기서는 오른쪽 방향으로 고정 테스트
//	XMVECTOR move_dir = XMVectorSet(1, 0, 0, 0); // 실제 구현 시 외부에서 받아야 함
//
//	// 무릎 높이 (기준 높이에서 약간 위)
//	float knee_height = curr_obb.Center.y - curr_obb.Extents.y * 0.5f;
//
//	// 선분 시작점: 무릎 위치 + 약간 앞
//	XMVECTOR start = XMLoadFloat3(&curr_obb.Center);
//	start = XMVectorSetY(start, knee_height);
//	XMVECTOR short_forward = move_dir * 0.4f;
//	XMVECTOR probe_pos = start + short_forward;
//
//	// 수직 레이 캐스팅 (위에서 아래로)
//	XMVECTOR ray_origin = probe_pos + XMVectorSet(0, 3.0f, 0, 0); // 천장 위치부터
//	XMVECTOR ray_dir = XMVectorSet(0, -1, 0, 0);
//
//	float closest_y = FLT_MAX;
//	bool is_hit = false;
//
//	for (const auto& indices : indices_array)
//	{
//		for (int i = 0; i + 2 < indices.size(); i += 3)
//		{
//			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 0]]), world);
//			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 1]]), world);
//			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[indices[i + 2]]), world);
//
//			float t = 0.0f;
//			if (TriangleTests::Intersects(ray_origin, ray_dir, v0, v1, v2, t))
//			{
//				XMVECTOR hit_point = ray_origin + t * ray_dir;
//				float y = XMVectorGetY(hit_point);
//
//				if (y < closest_y)
//				{
//					closest_y = y;
//					is_hit = true;
//				}
//			}
//		}
//	}
//
//	if (is_hit)
//	{
//		float step_height = closest_y - (curr_obb.Center.y - curr_obb.Extents.y);
//		if (step_height >= 0.0f && step_height <= 1.5f)
//		{
//			// 계단으로 간주 → 충돌 무시
//			return false;
//		}
//	}
//
//	// 계단이 아니면 벽 충돌로 간주
//	OutputDebugStringA("[WallCollision] 충돌 발생\n");
//	return true;
//}
//

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
