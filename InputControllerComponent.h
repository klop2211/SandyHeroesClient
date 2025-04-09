#pragma once
#include "Component.h"

// ��ǲ�� ó���ϴ� ��Ʈ�ѷ��� ���� Ŭ����
// �� Ŭ������ ��ӹ޾� ProcessInput �Լ����� ��ǲ�� ó���� �� �ִ�.
class InputControllerComponent :
    public Component
{
public:
    InputControllerComponent(Object* owner);
    virtual ~InputControllerComponent() {}

    virtual Component* GetCopy() = 0;

    virtual bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) = 0;

    void set_client_wnd(HWND value);

protected:
    std::unordered_map<char, bool> is_key_down_;
    POINT mouse_xy_{ 0,0 };

    HWND client_wnd_{ nullptr };
};

