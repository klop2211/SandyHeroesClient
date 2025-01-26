#pragma once
#include "Component.h"

class Mesh;
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

    XMFLOAT4X4 GetOwnerWorld() const;

    bool IsVisible() const;

protected:
    Mesh* mesh_ = nullptr;

    bool is_visible_ = true;
};

