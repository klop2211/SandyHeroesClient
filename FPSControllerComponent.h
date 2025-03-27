#pragma once
#include "InputControllerComponent.h"

class FPSControllerComponent :
    public InputControllerComponent
{
public:
    FPSControllerComponent(Object* owner);
    virtual ~FPSControllerComponent() {}

    virtual Component* GetCopy();

    virtual bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) override;

    virtual void Update(float elapsed_time) override;

    void set_head_bone(Object* head_bone);

private:
    Object* head_bone_ = nullptr;

};

