#include "stdafx.h"
#include "MonsterComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "AnimationState.h"

MonsterComponent::MonsterComponent(Object* owner) : Component(owner)
{
}

MonsterComponent::MonsterComponent(const MonsterComponent& other) : Component(other),
shield_(other.shield_), hp_(other.hp_), attack_force_(other.attack_force_), monster_type_(other.monster_type_)
{
}

Component* MonsterComponent::GetCopy()
{
    return new MonsterComponent(*this);
}

void MonsterComponent::Update(float elapsed_time)
{
    if (hp_ <= 0)
    {
		auto animation_state = Object::GetComponentInChildren<AnimatorComponent>(owner_)->animation_state();
        if (animation_state)
        {
            if (animation_state->GetDeadAnimationTrack() == -1)
            {
				// 죽는 애니메이션이 없으면 그냥 죽는다.
                owner_->set_is_dead(true);
                return;
            }
			// 죽는 애니메이션으로 전환
			animation_state->set_animation_track(animation_state->GetDeadAnimationTrack());
			animation_state->set_animation_loop_type(1); // Once
        }
    }


	//TODO: 몬스터의 행동을 결정하는 AI 추가
	//EX) ai->Update(owner_, elapsed_time);
}

void MonsterComponent::set_shield(float value)
{
    shield_ = value;
}

void MonsterComponent::set_hp(float value)
{
    hp_ = value;
}

void MonsterComponent::set_attack_force(float value)
{
    attack_force_ = value;
}

float MonsterComponent::shield() const
{
    return shield_;
}

float MonsterComponent::hp() const
{
    return hp_;
}

float MonsterComponent::attack_force() const
{
    return attack_force_;
}


