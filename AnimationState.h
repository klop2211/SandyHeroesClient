#pragma once

class Object;

//Ư�� ��ü�� �ִϸ��̼� ���� ���̸� �����ϴ� Ŭ����, Run �Լ��� ���� ��ü�� �ִϸ��̼� Ʈ�� index�� �����Ѵ�.
class AnimationState
{
public:
	virtual ~AnimationState() {}

	virtual int Run(Object* object, bool is_end) = 0;
	virtual AnimationState* GetCopy() = 0;

	void set_animation_track(int value);

protected:
	int animation_track_ = 0;

};

