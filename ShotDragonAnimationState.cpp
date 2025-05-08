#include "stdafx.h"
#include "ShotDragonAnimationState.h"
#include "Object.h"
#include "MovementComponent.h"

int ShotDragonAnimationState::Run(Object* object, bool is_end)
{
	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	auto velocity_xz = movement->velocity();
	velocity_xz.y = 0.f;
	float speed = xmath_util_float3::Length(velocity_xz);

	switch ((ShotDragonAnimationTrack)animation_track_)
	{
	case ShotDragonAnimationTrack::kIdle:
		if (speed > 0.f)
		{
			animation_track_ = (int)ShotDragonAnimationTrack::kRun;
		}
		break;
	case ShotDragonAnimationTrack::kRun:
		if (IsZero(speed))
		{
			animation_track_ = (int)ShotDragonAnimationTrack::kIdle;
		}
	case ShotDragonAnimationTrack::kDie:
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

AnimationState* ShotDragonAnimationState::GetCopy()
{
    return new ShotDragonAnimationState(*this);
}

int ShotDragonAnimationState::GetDeadAnimationTrack() const
{
    return (int)ShotDragonAnimationTrack::kDie;
}
