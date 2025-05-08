#include "stdafx.h"
#include "HitDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"

HitDragonAnimationState::HitDragonAnimationState()
{
	animation_track_ = (int)HitDragonAnimationTrack::kIdle;
}

int HitDragonAnimationState::Run(Object* object, bool is_end)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);

	switch ((HitDragonAnimationTrack)animation_track_)
	{
	case HitDragonAnimationTrack::kIdle:
		if (speed > 0.f)
		{
			animation_track_ = (int)HitDragonAnimationTrack::kRun;
		}
		break;
	case HitDragonAnimationTrack::kRun:
		if (IsZero(speed))
		{
			animation_track_ = (int)HitDragonAnimationTrack::kIdle;
		}
	case HitDragonAnimationTrack::kDie:
		if (is_end)
		{
			object->set_is_dead(true);
		}
		break;
	default:
		break;
	}
    return animation_track_;
}

AnimationState* HitDragonAnimationState::GetCopy()
{
    return new HitDragonAnimationState(*this);
}

int HitDragonAnimationState::GetDeadAnimationTrack() const
{
	return (int)HitDragonAnimationTrack::kDie;
}
