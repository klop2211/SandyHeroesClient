#pragma once

class Object;

// 오브젝트에 기능을 추가해주는 클래스의 기초 클래스
// 
class Component
{
public:
	// 컴포넌트는 반드시 owner 즉, 주인 오브젝트가 있어야함
	Component() = delete;
	Component(Object* owner);
	Component(const Component& other);
	virtual ~Component() {}

	/*
	컴포넌트의 복사본을 리턴(이 클래스를 상속받는 클래스에서 반드시 작성해야한다.)
	작성 예시
	Component* GetCopy() override 
	{ 
		return new MeshComponent(*this); 
	}
	*/
	virtual Component* GetCopy() = 0;

protected:
	Object* owner_ = nullptr;

};

