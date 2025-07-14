#pragma once
#include "Component.h"

enum class UiType
{
	kButton,
	kImage,
	kSliderX,		// ���� �����̴�
	kSliderY,		// ���� �����̴�
	kProgressBarX,	// ���� ����� ǥ����
	kProgressBarY,	// ���� ����� ǥ����
	kPanel,
	kText,			// �ؽ�Ʈ
	kCustom
};

class UiComponent :
    public Component
{
public:
	UiComponent() {};
	UiComponent(Object* owner);
	UiComponent(Object* owner, UiType type);
	UiComponent(const UiComponent& other);
	virtual ~UiComponent() {}

	virtual Component* GetCopy();
	virtual void Update(float elapsed_time) {}

	void set_type(UiType type) { type_ = type; }
	void set_view(Object* view) { view_ = view; }

protected:
	UiType type_{ UiType::kImage }; // UI Ÿ��
	Object* view_{ nullptr }; // ui�� Ư�� ������Ʈ�� �ֽ��ϱ� ���� ������

};

