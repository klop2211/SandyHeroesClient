#pragma once
#include "Component.h"

class MeshComponent;

//������Ʈ�� ī�޶� ��� �߰�
class CameraComponent :
    public Component
{
public:
	CameraComponent() = delete;
	CameraComponent(Object* owner, 
		float near_plane_distance, float far_plane_distance, float aspect_ratio, float fov_angle);
	CameraComponent(const CameraComponent& other);
	virtual ~CameraComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	void CreateProjectionMatrix(float near_plane_distance, float far_plane_distance, float aspect_ratio, float fov_angle);

	bool CollisionCheckByMeshComponent(MeshComponent* mesh_component);

	//owner�� ���� ��� ������� view matrix ������Ʈ 
	//(owner�� ��������� ������Ʈ�� �ǰ� �� �Լ��� ȣ��Ǿ����)
	void UpdateCameraInfo();

	//getter
	XMFLOAT4X4 view_matrix() const;
	XMFLOAT4X4 projection_matrix() const;
	XMFLOAT3 world_position() const;
	XMFLOAT3 up_vector() const;
	BoundingFrustum view_frustum() const;

private:
	XMFLOAT4X4 view_matrix_ = xmath_util_float4x4::Identity();
	XMFLOAT4X4 projection_matrix_ = xmath_util_float4x4::Identity();

	XMFLOAT3 world_position_{ 0,0,0 };
	XMFLOAT3 up_vector_{ 0,0,0 };

	BoundingFrustum view_frustum_{};
	BoundingFrustum world_frustum_{};
};

