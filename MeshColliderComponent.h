#pragma once
#include "Component.h"

class Mesh;

class MeshColliderComponent :
    public Component
{
public:
    MeshColliderComponent(Object* owner);
    MeshColliderComponent(const MeshColliderComponent& other);

    virtual Component* GetCopy() override;

    // ���� ��ǥ���� ������ mesh_ ���� �浹üũ
    // �Ű�����: ���� ����, ����, OUT �浹�� �Ÿ�(���� ��ǥ��)
    // return: �浹 ����
    bool CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance);

    bool CollisionCheckByObb(BoundingOrientedBox obb);

    BoundingOrientedBox GetWorldOBB() const;

    //getter
    Mesh* mesh() const;

    //setter
    void set_mesh(Mesh* mesh);


private:
    Mesh* mesh_{ nullptr };

};

