#pragma once
#include "Component.h"

class Mesh;
struct FrameResource;

// 오브젝트에 메쉬 기능을 추가해 주는 클래스
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

