#pragma once
#include "Component.h"
#include "Mesh.h"

class Scene;
struct FrameResource;

// ������Ʈ�� �޽� ����� �߰��� �ִ� Ŭ����
// 
class MeshComponent :
    public Component
{
public:
    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(Object* owner, Mesh* mesh, Material* material);
    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& rhs);

    virtual ~MeshComponent();

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index);

    void AddMaterial(Material* material);

    bool ChangeMaterial(int index, Material* material);

    virtual void Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

    bool IsVisible() const;

    void set_is_visible(bool value);

    Mesh* GetMesh() const;

    void set_mesh(Mesh* mesh);

    UINT constant_buffer_index() const;
protected:
    UINT constant_buffer_index_{};

    Mesh* mesh_ = nullptr;

    std::vector<Material*> materials_;

    bool is_visible_ = true;
};

