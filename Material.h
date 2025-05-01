#pragma once

class DescriptorManager;
struct FrameResource;

constexpr int kTextureMaskAlbedo(1 << 0);
constexpr int kTextureMaskSpecGlos(1 << 1);
constexpr int kTextureMaskMetalGlos(1 << 2);
constexpr int kTextureMaskEmission(1 << 3);
constexpr int kTextureMaskNormal(1 << 4);
constexpr int kTextureMaskCube(1 << 5);

enum class TextureType{ kAlbedoMap, kSpecGlossMap, kMetallicGlossMap, kEmissionMap, kNormalMap, kCubeMap };

struct Texture
{
    std::string name;
    int heap_index = -1;
    TextureType type;

    ComPtr<ID3D12Resource> resource = nullptr;
    ComPtr<ID3D12Resource> upload_heap = nullptr;
};

class Material
{
public:
    Material() {}
    ~Material() {}

    //getter
    std::string name() const;

    //setter
    void set_frame_resource_index(int value);
    void set_albedo_color(float r, float g, float b, float a);
    void set_albedo_color(XMFLOAT4 value);
    void set_name(const std::string& value);

    // ���͸����� ����ϴ� �� �ʿ��� �ؽ�ó �ε�
    void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);
    
    // ���͸��� ���� ������� ������Ʈ �� ���̴��� �����Ѵ�
    void UpdateShaderVariables(ID3D12GraphicsCommandList* command_list, 
        FrameResource* curr_frame_resource, DescriptorManager* descriptor_manager);

    // SRV�� �����ϰ� ���� ���� �ε����� ����(���ϰ��� �޾Ƽ� ���� ���͸����� srv ������ ����ϸ� ��)
    int CreateShaderResourceViews(ID3D12Device* device, DescriptorManager* descriptor_manager, int start_index);

    void LoadMaterialFromFile(std::ifstream& file);

    void AddTexture(Texture* texture);

    static int GetTextureCount();

private:
    static const std::string kTextureFilePath;
    static int kTextureCount;

    // ���� ������ ����� frame resource�� ���� �ε���
    int frame_resource_index_ = -1;

    std::list<std::unique_ptr<Texture>> texture_list_;

    //��� ���۷� �ԷµǴ� ���� ��
    XMFLOAT4 albedo_color_{ 1,1,1,1 };
    XMFLOAT3 fresnel_r0_{ 0.04, 0.04, 0.04 };
    float glossiness_{ 0.2 };
    XMFLOAT4 emission_color_{ 0,0,0,1 };
    
    std::string name_{ "None" };

};

