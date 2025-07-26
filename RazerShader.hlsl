#include "GlobalValues.hlsl"

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 position_w : POSITION0;
    float3 normal_w : NORMAL;
    float3 tangent_w : TANGENT;
    float2 uv : TEXCOORD;
    float4 position_s : POSITION1;
};


float4 PS(VertexOut input) : SV_Target
{
    float3 color = float3(1.0, 0.f, 0.f); // ���� ������
    float3 white = float3(0.1, 0.1, 0.1); 
    
// Input: UV.y�� �������� ���� ����, UV.x�� �ʺ� ����
    float2 uv = input.uv; // uv.x: 0~1 (��), uv.y: 0~1 (����)

// �߾ӿ��� �־������� ��������
    float distanceFromCenter = abs(uv.x - 0.5f); // 0~0.5
    float alpha = saturate(1.0 - distanceFromCenter * 2.0); // ���� falloff
    
    float speed = 1.5f; // �������� �ӵ� (����: UV.y/sec)
    float bandPos = frac(1.0 - (g_object_time * speed)); // 0~1 ���� �ݺ�
    
    // �ȼ��� �߽ɼ����� �Ʒ��� ������ �ٷ� ���� (����)
    if (uv.y < bandPos)
        discard;
    
    // �ð� ��� �� �̵� ȿ��
    float scroll = abs(uv.y - bandPos); // �Ʒ��� �̵�
    float glow = exp(-pow(scroll * 8.0f, 2)); // Gaussian pulse
    alpha *= glow;

// ���� threshold ��
    if (alpha < 0.05)
        discard;

// ���� ���
    return float4(color - white * alpha, alpha);
}