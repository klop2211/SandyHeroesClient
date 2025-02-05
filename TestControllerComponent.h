#pragma once
#include "InputControllerComponent.h"

//인풋을 받아서 전후좌우 이동 및 상하 좌우 회전을 수행하는 단순한 컴포넌트
class TestControllerComponent :
    public InputControllerComponent
{
public:
    TestControllerComponent(Object* owner);
    virtual ~TestControllerComponent() {}

    virtual Component* GetCopy();

    virtual void ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) override;

};

