#pragma once
#include "Mesh.h"
class CubeMesh :
    public Mesh
{
public:
    CubeMesh(const XMFLOAT4& color);
    ~CubeMesh() {}
};

