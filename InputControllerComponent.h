#pragma once
#include "Component.h"

// ��ǲ�� ó���ϴ� ��Ʈ�ѷ��� ���� Ŭ����
// �� Ŭ������ ��ӹ޾� ��ǲ�Ŵ����� ���� ��Ʈ�ѷ��� ����ϸ� 
// ProcessInput �Լ����� ��ǲ�� ó���� �� �ִ�.
class InputControllerComponent :
    public Component
{
public:
    // ��ǲ��Ʈ�ѷ��� �ٸ� ��Ʈ�ѷ��� �ٸ��� Update�Լ��� ���� ������Ʈ�� ������Ʈ ���� �ʰ�
    // ��ǲ �Ŵ����� ���� ProcessInput �Լ��� ��ǲó���� �ϱ� ������ owner �����Ͱ� �ʿ��ϴ�.
    InputControllerComponent(Object* owner);
    virtual ~InputControllerComponent() {}

    virtual Component* GetCopy() = 0;

    virtual void ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) = 0;

protected:
    std::unordered_map<char, bool> is_key_down_;
    POINT mouse_xy_{ 0,0 };
};

