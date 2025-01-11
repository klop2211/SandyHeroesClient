#pragma once

class Object;

// ������Ʈ�� ����� �߰����ִ� Ŭ������ ���� Ŭ����
// 
class Component
{
public:
	// ������Ʈ�� �ݵ�� owner ��, ���� ������Ʈ�� �־����
	Component() = delete;
	Component(Object* owner);
	Component(const Component& other);
	virtual ~Component() {}

	/*
	������Ʈ�� ���纻�� ����(�� Ŭ������ ��ӹ޴� Ŭ�������� �ݵ�� �ۼ��ؾ��Ѵ�.)
	�ۼ� ����
	Component* GetCopy() override 
	{ 
		return new MeshComponent(*this); 
	}
	*/
	virtual Component* GetCopy() = 0;

protected:
	Object* owner_ = nullptr;

};

