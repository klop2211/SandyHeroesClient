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

	//���� ������ ������Ʈ�� owner�� �������ؾ���(�ǵ� �������� ������Ʈ�� �������)
	Component(const Component& other);
	virtual ~Component() {}

	void set_owner(Object* owner);
	Object* owner() const;

	/*
	������Ʈ�� ���纻�� ����(�� Ŭ������ ��ӹ޴� Ŭ�������� �ݵ�� �ۼ��ؾ��Ѵ�.)
	������Ʈ ����� ���� ������Ʈ�� ���� ������Ʈ�� �����ϱ� ���� �ʿ�
	�ۼ� ����
	Component* GetCopy() override 
	{ 
		return new MeshComponent(*this); 
	}
	*/
	virtual Component* GetCopy() = 0;
	virtual void Update(float elapsed_time) {};

protected:
	Object* owner_ = nullptr;

};

