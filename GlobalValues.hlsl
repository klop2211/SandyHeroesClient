#include "LightingUtil.hlsl"

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
    matrix g_view_matrix;
    matrix g_projection_matrix;
    float3 g_camera_position;
    float pad;
    float4 g_ambient_light;
    Light g_lights[MAX_LIGHTS];
}

cbuffer Material : register(b4)
{
    Material g_material;
}
