#pragma once
#include "Component.h"

// 인풋을 처리하는 컨트롤러의 상위 클래스
// 이 클래스를 상속받아 인풋매니저에 메인 컨트롤러로 등록하면 
// ProcessInput 함수에서 인풋을 처리할 수 있다.
class InputControllerComponent :
    public Component
{
public:
    // 인풋컨트롤러는 다른 컨트롤러와 다르게 Update함수를 통해 오브젝트를 업데이트 하지 않고
    // 인풋 매니저에 의해 ProcessInput 함수로 인풋처리를 하기 때문에 owner 포인터가 필요하다.
    InputControllerComponent(Object* owner);
    virtual ~InputControllerComponent() {}

    virtual Component* GetCopy() = 0;

    virtual void ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) = 0;

protected:
    std::unordered_map<char, bool> is_key_down_;
    POINT mouse_xy_{ 0,0 };
};

