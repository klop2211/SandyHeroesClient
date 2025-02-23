#pragma once

#include "UploadBuffer.h"

// 쉐이더에서 사용하는 조명정보를 저장하는 구조체
struct LightInfo
{
	XMFLOAT3 strength;
	float falloff_start;
	XMFLOAT3 direction;
	float falloff_end;
	XMFLOAT3 position;
	float spot_power;
	bool enable;
	int type;
	XMFLOAT2 pad;
};

// 렌더패스 1회에 사용되는 상수 버퍼
struct CBPass
{
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 proj_matrix;
	XMFLOAT3 camera_position;
	float pad;
	XMFLOAT4 ambient_light;
	LightInfo lights[kMaxLights];
};

// 일반 메쉬를 사용하는 오브젝트의 상수 버퍼
struct CBObject
{
	XMFLOAT4X4 world_matrix;
};

// 스킨 메쉬를 사용하는 오브젝트의 bone transform 행렬
struct CBBoneTransform
{
	XMFLOAT4X4 bone_transform_matrix[kMaxBoneCount];
};


// 게임에서 한 프레임에 사용하는 리소스에 대한 구조체
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT pass_count,
		UINT object_count, UINT skinned_mesh_object_count)
	{
		device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&d3d_allocator));

		cb_pass = std::make_unique<UploadBuffer<CBPass>>(device, pass_count, true);
		cb_object = std::make_unique<UploadBuffer<CBObject>>(device, object_count, true);
		cb_bone_transform = std::make_unique<UploadBuffer<CBBoneTransform>>(device, skinned_mesh_object_count, true);
	}
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource() {}

	ComPtr<ID3D12CommandAllocator> d3d_allocator;

	std::unique_ptr<UploadBuffer<CBPass>> cb_pass;
	std::unique_ptr<UploadBuffer<CBObject>> cb_object;
	std::unique_ptr<UploadBuffer<CBBoneTransform>> cb_bone_transform;

	UINT fence = 0;
};

