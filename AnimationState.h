#pragma once

class Object;

//Ư�� ��ü�� �ִϸ��̼� ���� ���̸� �����ϴ� Ŭ����, Run �Լ��� ���� ��ü�� �ִϸ��̼� Ʈ�� index�� �����Ѵ�.
class AnimationState
{
public:
	virtual ~AnimationState() {}

	virtual int Run(Object* object, bool is_end) = 0;
	virtual AnimationState* GetCopy() = 0;

	virtual int GetDeadAnimationTrack() const;

	void set_animation_track(int value);
	void set_animation_loop_type(int value); 

	int animation_loop_type() const;

protected:
	int animation_track_ = 0;
	int animation_loop_type_ = 0; // 0: Loop, 1: Once, 2: Repeat
	int repeat_count_ = 0; // �ݺ� Ƚ��
};

