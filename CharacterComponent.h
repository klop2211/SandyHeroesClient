#pragma once
#include "Component.h"

class AnimatorComponent;
class MovementComponent;

class CharacterComponent :
    public Component
{
public:
    CharacterComponent() = default;
	CharacterComponent(Object* owner) : Component(owner) {}
    virtual ~CharacterComponent() = default;

    virtual Component* GetCopy() = 0;

    virtual void Update(float elapsed_time) = 0;

	virtual void HendleCollision(Object* other_object) = 0;

protected:
	AnimatorComponent* animator_{ nullptr }; // �ִϸ����� ������Ʈ ������
	MovementComponent* movement_{ nullptr }; // �̵� ������Ʈ ������

};

