#pragma once
#include "InputControllerComponent.h"

//��ǲ�� �޾Ƽ� �����¿���� �̵� �� ���� �¿� ȸ���� �����ϴ� �ܼ��� ������Ʈ
class TestControllerComponent :
    public InputControllerComponent
{
public:
    TestControllerComponent(Object* owner);
    virtual ~TestControllerComponent() {}

    virtual Component* GetCopy();

    virtual bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) override;

};

