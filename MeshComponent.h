#pragma once
#include "Component.h"

class Mesh;

class MeshComponent :
    public Component
{
public:
    MeshComponent(Object* owner, Mesh* mesh);
    MeshComponent(const MeshComponent& other);

    virtual Component* GetCopy() override;

    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* command_list);

protected:
    Mesh* mesh_ = nullptr;

    int object_constant_buffer_index_ = -1;

};

