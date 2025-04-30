#pragma once
#include "Mesh.h"

//ȭ��� 2d ui�� �׸��� �޽�
class UIMesh :
    public Mesh
{
public:
    //pivot �� �簢���� ���� ���̴�.
    UIMesh(float screen_x, float screen_y, float screen_width, float screen_height);
    ~UIMesh() {}

    XMFLOAT3 ScreenToNDC(float x, float y, float width, float height);

private:
    XMFLOAT2 screen_position_{};    //ui�� ȭ�� ��ġ
    XMFLOAT2 screen_size_{};        //ui�� ȭ�鿡�� ũ��


};

