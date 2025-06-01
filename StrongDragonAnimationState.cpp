#include "stdafx.h"
#include "StrongDragonAnimationState.h"
#include "MovementComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"

StrongDragonAnimationState::StrongDragonAnimationState()
{
	set_animation_track((int)StrongDragonAnimationTrack::kSpawn);
	animation_loop_type_ = 1; //Once

}

void StrongDragonAnimationState::Enter(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)StrongDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 1; //Once
	}

}

int StrongDragonAnimationState::Run(Object* object, bool is_end, AnimatorComponent* animator)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);

	switch ((StrongDragonAnimationTrack)animation_track())
	{
	case StrongDragonAnimationTrack::kSpawn:
		if (is_end)
		{
			ChangeAnimationTrack((int)StrongDragonAnimationTrack::kIdle, object, animator);
		}
		break;
	case StrongDragonAnimationTrack::kIdle:
		break;
	case StrongDragonAnimationTrack::kDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	default:
		break;
	}
	return animation_track();
}

void StrongDragonAnimationState::Exit(int animation_track, Object* object, AnimatorComponent* animator)
{
	if ((int)StrongDragonAnimationTrack::kSpawn == animation_track)
	{
		animation_loop_type_ = 0; 
	}

}

AnimationState* StrongDragonAnimationState::GetCopy()
{
    return new StrongDragonAnimationState(*this);
}

int StrongDragonAnimationState::GetDeadAnimationTrack() const
{
    return (int)StrongDragonAnimationTrack::kDie;
}
