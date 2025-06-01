#pragma once

class Object;
class AnimatorComponent;

//Ư�� ��ü�� �ִϸ��̼� ���� ���̸� �����ϴ� Ŭ����, Run �Լ��� ���� ��ü�� �ִϸ��̼� Ʈ�� index�� �����Ѵ�.
class AnimationState
{
public:
	virtual ~AnimationState() {}

	virtual void Enter(int animation_track, Object* object, AnimatorComponent* animator) = 0;
	virtual int Run(Object* object, bool is_end, AnimatorComponent* animator) = 0;
	virtual void Exit(int animation_track, Object* object, AnimatorComponent* animator) = 0;


	virtual AnimationState* GetCopy() = 0;

	virtual int GetDeadAnimationTrack() const;

	void ChangeAnimationTrack(int animation_track, Object* object, AnimatorComponent* animator);

	void set_animation_loop_type(int value); 

	int animation_loop_type() const;
	int animation_track() const;

protected:
	void set_animation_track(int value);

	int animation_loop_type_ = 0; // 0: Loop, 1: Once, 2: Repeat
	int repeat_count_ = 0; // �ݺ� Ƚ��

private:
	int animation_track_ = 0;

};

