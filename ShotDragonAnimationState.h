#pragma once
#include "AnimationState.h"

enum class ShotDragonAnimationTrack { kIdle, kRun, kDie };

class ShotDragonAnimationState :
    public AnimationState
{
public:
	ShotDragonAnimationState() {}
	virtual ~ShotDragonAnimationState() {}

	virtual int Run(Object* object, bool is_end) override;
	virtual AnimationState* GetCopy() override;

	virtual int GetDeadAnimationTrack() const override;

};

