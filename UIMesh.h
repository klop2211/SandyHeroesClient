#pragma once
#include "Mesh.h"

//ȭ��� 2d ui�� �׸��� �޽�
class UIMesh :
    public Mesh
{
public:
    //pivot �� �簢���� ���� ���̴�.
    UIMesh(float screen_x, float screen_y, float screen_width, float screen_height, float z_depth = 0.f);
    UIMesh(float screen_width, float screen_height, float z_depth = 0.f);
    ~UIMesh() {}

    XMFLOAT3 ScreenToNDC(float x, float y, float width, float height);

    //getter
    XMFLOAT2 screen_position() const;
    XMFLOAT2 ui_size() const;

private:
    XMFLOAT2 screen_position_{};    //ui�� ȭ�� ��ġ
    XMFLOAT2 ui_size_{};        //ui�� ȭ�鿡�� ũ��


};

