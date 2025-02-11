#include "GlobalValues.hlsl"


struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    int4 indices : BONEINDEX;
    float4 weights : BONEWEIGHT;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn v_in)
{
    VertexOut v_out;
    
    //모델 좌표계에서 월드 좌표계로 변환하는 행렬
    float4x4 vertex_to_world_matrix = (float4x4) 0.0f;
    for (int i = 0; i < MAX_BONE_PER_VERTEX; i++)
    {
        vertex_to_world_matrix += v_in.weights[i] * mul(g_bone_offset_matrix[v_in.indices[i]], g_bone_transform_matrix[v_in.indices[i]]);
    }
    
    v_out.position = mul(mul(mul(float4(v_in.position, 1.f), vertex_to_world_matrix), g_view_matrix), g_projection_matrix);
    //TODO: 조명 처리 및 텍스처 추가시 normal에 대한 변환 필요
    
    v_out.uv = v_in.uv;
    
    return v_out;
}

float4 PS(VertexOut p_in) : SV_Target
{
    return float4(0, 1, 0, 1);
}