#pragma once
#include "AnimationState.h"

enum class HitDragonAnimationTrack { kBreathAttack, kCastSpell, kDashAttack, kDie, kIdle, kJumpSmashAttack, kRun, kSlashLeftAttack, kSlashRightAttack, kTakeDamage };


class HitDragonAnimationState :
    public AnimationState
{
public:
	HitDragonAnimationState();
	virtual ~HitDragonAnimationState() {}

	virtual int Run(Object* object, bool is_end) override;
	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;

};

