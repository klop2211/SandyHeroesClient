#include "stdafx.h"
#include "MonsterComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "AnimationState.h"
#include "MovementComponent.h"
#include "UiMeshComponent.h"
#include "ShotDragonAnimationState.h"

MonsterComponent::MonsterComponent(Object* owner) : Component(owner)
{
}

MonsterComponent::MonsterComponent(const MonsterComponent& other) : Component(other),
shield_(other.shield_), hp_(other.hp_), attack_force_(other.attack_force_), monster_type_(other.monster_type_), target_(other.target_)
{
}

Component* MonsterComponent::GetCopy()
{
    return new MonsterComponent(*this);
}

void MonsterComponent::Update(float elapsed_time)
{
    if (is_dead_animationing_)
    {
        return;
    }

    if (hp_ <= 0)
    {
        auto animator = Object::GetComponentInChildren<AnimatorComponent>(owner_);
        if (!animator)
        {
            std::string temp = owner_->name() + "의 MonsterComponent 죽음 애니메이션 출력 과정에서 문제가 생겼습니다.";
            std::wstring debug_str;
            debug_str.assign(temp.begin(), temp.end());

            OutputDebugString(debug_str.c_str());
            return;
        }
		auto animation_state = animator->animation_state();
        if (animation_state)
        {
            if (animation_state->GetDeadAnimationTrack() == -1)
            {
				// 죽는 애니메이션이 없으면 그냥 죽는다.
                owner_->set_is_dead(true);
                return;
            }
			// 죽는 애니메이션으로 전환
			animation_state->ChangeAnimationTrack(animation_state->GetDeadAnimationTrack(), owner_, animator);
			animation_state->set_animation_loop_type(1); // Once
            is_dead_animationing_ = true;
            return;
        }
    }

	//TODO: 몬스터의 행동을 결정하는 AI 추가
	//EX) ai->Update(owner_, elapsed_time);
    if (target_)
    {
		auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
		if (movement)
		{
			XMFLOAT3 look = owner_->look_vector();
            look.y = 0.f;
			look = xmath_util_float3::Normalize(look);
			XMFLOAT3 direction = target_->world_position_vector() - owner_->world_position_vector();
			direction.y = 0.f;
			direction = xmath_util_float3::Normalize(direction);
			float angle = xmath_util_float3::AngleBetween(look, direction);
            if (angle > XM_PI / 180.f * 5.f)
            {
                //회전 방향 연산
				XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
				if (cross.y < 0)
				{
					angle = -angle;
				}
				angle = XMConvertToDegrees(angle);
				owner_->Rotate(0.f, angle, 0.f);
            }
            if (owner_->tag() == "Shot_Dragon")
            {
                auto animator = Object::GetComponent<AnimatorComponent>(owner_);
                auto animation_state = animator->animation_state();
                animation_state->ChangeAnimationTrack((int)ShotDragonAnimationTrack::kAttack, owner_, animator);
                animation_state->set_animation_loop_type(0); // Loop

                return;
            }
            if (owner_->tag() == "Strong_Dragon")
            {
                return;
            }

			movement->MoveXZ(direction.x, direction.z, 5.f);
            if (!electric_slow_applied_)
            {
                movement->set_max_speed_xz(3.5f);
            }
		}
	}

    // 상태이상 처리
    for (auto& [type, effect] : status_effects_)
    {
        if (!effect.IsActive()) continue;

        effect.elapsed += elapsed_time;

        if (type == StatusEffectType::Fire) //화염
        {
            float dps = effect.fire_damage * 0.1f;  //10%
            if (effect.flame_frenzy)
            {
                dps *= 2.0f;
            }
            HitDamage(dps * elapsed_time);

            if (hp_ <= 0 )
            {
                if (scene_)
                {
                    BaseScene* base_scene = dynamic_cast<BaseScene*>(scene_);
                    if (base_scene)
                    {
                        base_scene->add_catch_monster_num(); // 함수 만들어주면 됨
                    }
                }
            }
        }
        else if (type == StatusEffectType::Electric)    //전기
        {
            auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
            if (movement)
            {
                if (!electric_slow_applied_)
                {
                    original_speed_ = movement->max_speed_xz();
                    float electric_frenzy = 0.0f;
                    if (effect.electric_frenzy)
                    {
                        electric_frenzy = 0.3f; // 60% 감소 시키기 위한 변수
                    }
                    movement->set_max_speed_xz(original_speed_ * (0.70f - electric_frenzy));    // 기본 30% 감소
                    electric_slow_applied_ = true;
                }

                //movement->set_max_speed_xz(3.5f * 0.85f);
            }
        }
    }

    // 상태이상 종료 후 복원
    auto& electric = status_effects_[StatusEffectType::Electric];
    if (!electric.IsActive() && electric_slow_applied_)
    {
        auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
        if (movement)
        {
            movement->set_max_speed_xz(original_speed_); // 복구
            electric_slow_applied_ = false;
        }
    }
}

void MonsterComponent::HitDamage(float damage)
{
    // 산성 효과
    // 산성 상태일 경우 모든 피해 증가
    auto it = status_effects_.find(StatusEffectType::Poison);
    if (it != status_effects_.end() && it->second.IsActive())
    {
        if (it->second.acid_frenzy)
        {
            damage *= 1.3f;
        }
        else
        {
            damage *= 1.15f;
        }
    }

	if (shield_ > 0)
	{
		shield_ -= damage;
		if (shield_ < 0)
		{
			hp_ += shield_; // shield가 음수면 hp에 더해줌
			shield_ = 0;
		}
	}
	else
	{
		hp_ -= damage;
	}
	if (hp_ < 0)
	{
		hp_ = 0;
	}
}

void MonsterComponent::ApplyStatusEffect(StatusEffectType type, float duration, float damage,
    bool flame_frenzy, bool acid_frenzy, bool electric_frenzy)
{
    status_effects_[type] = { duration, 0.f, damage , flame_frenzy, acid_frenzy, electric_frenzy };
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

void MonsterComponent::set_target(Object* target)
{
	target_ = target;
}

void MonsterComponent::set_is_pushed(bool is_pushed)
{
    is_pushed_ = is_pushed;
}

void MonsterComponent::set_push_timer(float value)
{
    push_timer_ = value;
}

float MonsterComponent::shield() const
{
    return shield_;
}

float MonsterComponent::hp() const
{
    return hp_;
}

float MonsterComponent::max_hp() const
{
    return max_hp_;
}

float MonsterComponent::max_shield() const
{
    return max_shield_;
}

float MonsterComponent::attack_force() const
{
    return attack_force_;
}

void MonsterComponent::set_scene(Scene* value)
{
    scene_ = value;
}

bool MonsterComponent::IsDead() const
{
    return hp_ <= 0;
}
