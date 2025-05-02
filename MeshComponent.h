#pragma once
#include "Component.h"
#include "Mesh.h"

struct FrameResource;

// ������Ʈ�� �޽� ����� �߰��� �ִ� Ŭ����
// 
class MeshComponent :
    public Component
{
public:
    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& rhs);

    virtual Component* GetCopy() override;

    virtual void UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index);

    bool IsVisible() const;

    Mesh* GetMesh() const;
protected:
    Mesh* mesh_ = nullptr;

    bool is_visible_ = true;
};

