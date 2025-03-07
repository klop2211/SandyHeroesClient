#pragma once

class Object;

//Ư�� ��ü�� �ִϸ��̼� ���� ���̸� �����ϴ� Ŭ����, Run �Լ��� ���� ��ü�� �ִϸ��̼� Ʈ�� index�� �����Ѵ�.
//��ӽ� AS ���λ縦 ���δ�.
class AnimationState
{
public:
	virtual ~AnimationState() {}

	virtual int Run(Object* object, bool is_end) = 0;
	virtual AnimationState* GetCopy() = 0;
};

