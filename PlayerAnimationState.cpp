#include "stdafx.h"
#include "PlayerAnimationState.h"
#include "Object.h"

int PlayerAnimationState::Run(Object* object, bool is_end)
{
	constexpr float kPlayerDashSpeed = 70.f;

	switch ((PlayerAnimationTrack)animation_track_)
	{
	case PlayerAnimationTrack::kIdle:
		if (xmath_util_float3::Length(object->velocity()) > 0)
		{
			animation_track_ = (int)PlayerAnimationTrack::kRun;
		}
		if (object->position_vector().y > 0.001)
		{
			animation_track_ = (int)PlayerAnimationTrack::kJump;
		}
		if (xmath_util_float3::Length(object->velocity()) >= kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kDash;
		}
		break;
	case PlayerAnimationTrack::kRun:
		//TODO: 달리기 애니메이션 좌우 구별
		if (IsZero(xmath_util_float3::Length(object->velocity())))
		{
			animation_track_ = (int)PlayerAnimationTrack::kIdle;
		}		
		if (xmath_util_float3::Length(object->velocity()) >= kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kDash;
		}
		break;
	case PlayerAnimationTrack::kJump:
		if (object->is_ground())
		{
			animation_track_ = (int)PlayerAnimationTrack::kIdle;
		}
		if (xmath_util_float3::Length(object->velocity()) >= kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kDash;
		}
		break;
	case PlayerAnimationTrack::kDash:
		if (xmath_util_float3::Length(object->velocity()) < kPlayerDashSpeed)
		{
			animation_track_ = (int)PlayerAnimationTrack::kRun;
		}
		break;
	default:
		break;
	}

	return (int)animation_track_;
}

AnimationState* PlayerAnimationState::GetCopy()
{
    return new PlayerAnimationState(*this);
}
