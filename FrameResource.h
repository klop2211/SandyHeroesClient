#pragma once

#include "UploadBuffer.h"

// �����н� 1ȸ�� ���Ǵ� ��� ����
struct CBPass
{
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 proj_matrix;
	XMFLOAT3 camera_position;
};

// �Ϲ� �޽��� ����ϴ� ������Ʈ�� ��� ����
struct CBObject
{
	XMFLOAT4X4 world_matrix;
};

// ��Ų �޽��� ����ϴ� ������Ʈ�� bone transform ���
struct CBBoneTransform
{
	XMFLOAT4X4 bone_transform_matrix[128];
};


// ���ӿ��� �� �����ӿ� ����ϴ� ���ҽ��� ���� ����ü
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

