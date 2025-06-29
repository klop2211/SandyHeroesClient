#pragma once
#include "AnimationState.h"

enum class PlayerAnimationTrack{ kIdle, kRun, kJump, kDash };

class PlayerAnimationState :
    public AnimationState
{
public:
	virtual ~PlayerAnimationState() {}

	//�ִϸ��̼� Ʈ�� ����� �ش� Ʈ���� 1���� �ʿ��� ������ �ϴ� �Լ�
	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) override;
	virtual int Run(float elapsed_time, Object* object, bool is_end, AnimatorComponent* animator) override;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) override;

	virtual AnimationState* GetCopy() override;
	
private:
	//�ѱ� �� ��ǿ� ���� �ٸ� �ִϸ��̼��� �����ϱ� ���� offset
	static const int kGunTypeOffset = 3;
};

