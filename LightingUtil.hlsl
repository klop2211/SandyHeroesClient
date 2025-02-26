
#define MAX_LIGHTS 16
#define DIRECTION_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
    float3 strength;
    float falloff_start;    // ��/���� ����
    float3 direction;       // ����/���� ����
    float falloff_end;      // ��/���� ����
    float3 position;        // ��/���� ����
    float spot_power;       // ���� ����
    bool enable;            // ������ ��뿩��(�Ϻ������� ���� ���࿡ ���� ���� ų�� ����)
    int type;               // 0: ����, 1: ��, 2: ����
    float2 pad; 
};

struct Material
{
    float4 albedo_color;    
    float3 fresnel_r0;      // �Ի簢 0������ ������ �ݻ���
    float glossiness;       // ����
    float4 emission_color; // �߻걤
};

//������� �������� ����ϴ� ������ ����Լ�
float CalcAttenuation(float d, float falloff_start, float falloff_end)
{
    return saturate((falloff_end - d) / (falloff_end - falloff_start));
}

//������ �ݻ� ���� ���ϱ����� �����ٻ縦 ���� ����ϴ� �Լ�
//Rf = R0 + (1 - R0) * ((1 - cos(seta))^5)
float3 SchlickFresnel(float3 r0, float3 normal, float3 light_vector)
{
    float cos_seta = saturate(dot(normal, light_vector));
    float f0 = 1 - cos_seta;
    float3 rf = r0 + (1 - r0) * pow((1 - cos_seta), 5);
    return rf;
}

//���� ����, ������, ������ ����, �������� ī�޶�� ���ϴ� to eye ����, ������ �޾� �������� �ϴ� �Լ�
float3 BlinnPhongLighting(float3 light_strength, float3 light_vector, float3 normal, float3 to_eye_vector, Material mat)
{
    const float m = mat.glossiness * 256.f;
    float3 half_vector = normalize(to_eye_vector + light_vector);
    
    //ǥ�� ��ĥ�⿡ ���� ���
    float roughness_factor = (m + 8.0f) * pow(max(dot(half_vector, normal), 0.0f), m) / 8.0f;
    //������ ȿ���� ���� ���
    float3 fresnel_factor = SchlickFresnel(mat.fresnel_r0, normal, light_vector);
    
    //�� �� ����� ������ �ݿ��ݻ� ����
    float3 specular_albedo = roughness_factor * fresnel_factor;
    
    //�ݿ� �ݻ����� 1 �̸����� ����(LDR ������)
    specular_albedo = specular_albedo / (1.f + specular_albedo);
    
    // ambient ���� ������ �� �Լ��� �ƴ϶� ps�Լ� ���ο��� ó���Ѵ�.
    return (mat.albedo_color.rgb + specular_albedo) * light_strength;
}

//���� ���� ���
float3 ComputeDirectionalLight(Light l, Material mat, float3 normal, float3 to_eye_vector)
{
    float3 light_vector = -l.direction;
    
    float3 light_strength = l.strength * max(dot(light_vector, normal), 0.f);
    
    return BlinnPhongLighting(light_strength, light_vector, normal, to_eye_vector, mat);
}

//�� ���� ���
float3 ComputePointLight(Light l, Material mat, float3 position, float3 normal, float3 to_eye_vector)
{
    float3 light_vector = l.position - position;
    
    float d = length(light_vector);
    
    if(d > l.falloff_end)
        return 0.f;
    
    light_vector /= d;
    
    float3 light_strength = l.strength * max(dot(light_vector, normal), 0.f);

    float attenuation = CalcAttenuation(d, l.falloff_start, l.falloff_end);
    light_strength *= attenuation;
    
    return BlinnPhongLighting(light_strength, light_vector, normal, to_eye_vector, mat);
}

//���� ���� ���
float3 ComputeSpotLight(Light l, Material mat, float3 position, float3 normal, float3 to_eye_vector)
{
    float3 light_vector = l.position - position;
    
    float d = length(light_vector);
    
    if (d > l.falloff_end)
        return 0.f;
    
    light_vector /= d;
    
    float3 light_strength = l.strength * max(dot(light_vector, normal), 0.f);

    float attenuation = CalcAttenuation(d, l.falloff_start, l.falloff_end);
    light_strength *= attenuation;

    float spot_factor = pow(max(dot(-light_vector, l.direction), 0.0f), l.spot_power);
    light_strength *= spot_factor;

    return BlinnPhongLighting(light_strength, light_vector, normal, to_eye_vector, mat);
}

//TODO: ���� �׸��� �߰��� shadow ���� ��� �߰�
float4 ComputeLighting(Light lights[MAX_LIGHTS], Material mat, 
                        float3 position, float3 normal, float3 to_eye_vector)
{
    float3 result = 0.f;
    
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if(!lights[i].enable)
            continue;
        if(lights[i].type == DIRECTION_LIGHT)
        {
            result += ComputeDirectionalLight(lights[i], mat, normal, to_eye_vector);
        }
        else if(lights[i].type == POINT_LIGHT)
        {
            result += ComputePointLight(lights[i], mat, position, normal, to_eye_vector);
        }
        else if(lights[i].type == SPOT_LIGHT)
        {
            result += ComputeSpotLight(lights[i], mat, position, normal, to_eye_vector);
        }
    }

    return float4(result, 0.f);
}