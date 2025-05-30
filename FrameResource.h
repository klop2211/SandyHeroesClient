#pragma once

#include "UploadBuffer.h"

// ���̴����� ����ϴ� ���������� �����ϴ� ����ü
struct LightInfo
{
	XMFLOAT3 strength;
	float falloff_start;	// ��/���� ����
	XMFLOAT3 direction;		// ����/���� ����
	float falloff_end;		// ��/���� ����
	XMFLOAT3 position;		// ��/���� ����
	float spot_power;		// ���� ����
	bool enable;			// ������ ��뿩��(�Ϻ������� ���� ���࿡ ���� ���� ų�� ����)
	int type;				// 0: ����, 1: ��, 2: ����
	XMFLOAT2 pad;
};

// �����н� 1ȸ�� ���Ǵ� ��� ����
struct CBPass
{
	XMFLOAT4X4 view_matrix;
	XMFLOAT4X4 proj_matrix;
	XMFLOAT3 camera_position;
	float total_time;
	XMFLOAT4 ambient_light;
	LightInfo lights[kMaxLights];    
	XMFLOAT2 screen_size;
	XMFLOAT2 pad_2;
};

struct CBShadow
{
	XMFLOAT4X4 light_view;
	XMFLOAT4X4 light_proj;
	XMFLOAT4X4 shadow_transform;
	XMFLOAT3 light_dir;
	float near_z;
	XMFLOAT3 light_pos_w;
	float far_z;
};

// �Ϲ� �޽��� ����ϴ� ������Ʈ�� ��� ����
struct CBObject
{
	XMFLOAT4X4 world_matrix;
};

// ��Ų �޽��� ����ϴ� ������Ʈ�� bone transform ���
struct CBBoneTransform
{
	XMFLOAT4X4 bone_transform_matrix[kMaxBoneCount];
};

struct CBMaterial
{
	XMFLOAT4 albedo_color;
	XMFLOAT3 fresnel_r0;
	float glossiness;
	XMFLOAT4 emission_color;
	int texture_mask;
};

struct CBUi
{
	XMFLOAT2 screen_offset;	// �»�� ��ũ�� ��ǥ (�ȼ�)
	float width_ratio;		// ui ũ�� ���� 0.0 ~ 1.0
	float height_ratio;
};

// ���ӿ��� �� �����ӿ� ����ϴ� ���ҽ��� ���� ����ü
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT pass_count,
		UINT object_count, UINT skinned_mesh_object_count, UINT material_count, UINT ui_mesh_count)
	{
		device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&d3d_allocator));

		cb_pass = std::make_unique<UploadBuffer<CBPass>>(device, pass_count, true);
		cb_shadow = std::make_unique<UploadBuffer<CBShadow>>(device, pass_count, true);
		cb_object = std::make_unique<UploadBuffer<CBObject>>(device, object_count, true);
		cb_bone_transform = std::make_unique<UploadBuffer<CBBoneTransform>>(device, skinned_mesh_object_count, true);
		cb_material = std::make_unique<UploadBuffer<CBMaterial>>(device, material_count, true);
		cb_ui = std::make_unique<UploadBuffer<CBUi>>(device, ui_mesh_count, true);
	}
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource() {}

	ComPtr<ID3D12CommandAllocator> d3d_allocator;

	std::unique_ptr<UploadBuffer<CBPass>> cb_pass;
	std::unique_ptr<UploadBuffer<CBShadow>> cb_shadow;
	std::unique_ptr<UploadBuffer<CBObject>> cb_object;
	std::unique_ptr<UploadBuffer<CBBoneTransform>> cb_bone_transform;
	std::unique_ptr<UploadBuffer<CBMaterial>> cb_material;
	std::unique_ptr<UploadBuffer<CBUi>> cb_ui;

	UINT fence = 0;
};

