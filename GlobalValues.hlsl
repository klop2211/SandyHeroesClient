
cbuffer ObjectInfo : register(b0)
{
    matrix g_world_matrix;
}

#define SKINNED_ANIMATION_BONES 128
#define MAX_BONE_PER_VERTEX 4

cbuffer BoneTransforms : register(b1)
{
    matrix g_bone_transform_matrix[SKINNED_ANIMATION_BONES];
}

cbuffer BoneOffsets : register(b2)
{
    matrix g_bone_offset_matrix[SKINNED_ANIMATION_BONES];
}

// 프레임 당 1번 갱신되는 상수 버퍼
cbuffer RenderPass : register(b3)
{
    matrix g_view_matrix : packoffset(c0);
    matrix g_projection_matrix : packoffset(c4);
    float3 g_camera_position : packoffset(c8);
}

