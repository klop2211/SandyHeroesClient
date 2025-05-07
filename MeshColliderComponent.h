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

    // 월드 좌표계의 광선과 mesh_ 간에 충돌체크
    // 매개변수: 광선 원점, 방향, OUT 충돌된 거리(월드 좌표계)
    // return: 충돌 여부
    bool CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance);

    bool CollisionCheckByObb(BoundingOrientedBox obb);

    bool CheckWallCollisionByObb(const BoundingOrientedBox& curr_obb, const XMVECTOR& move_dir);

    //bool CheckWallCollisionByObb(BoundingOrientedBox& obb);

    //bool CheckWallCollisionByObb(BoundingOrientedBox& prev_obb, BoundingOrientedBox& curr_obb);

    BoundingOrientedBox GetWorldOBB() const;

    //setter
    void set_mesh(Mesh* mesh);


private:
    Mesh* mesh_{ nullptr };

};

