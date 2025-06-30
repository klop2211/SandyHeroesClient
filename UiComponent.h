#pragma once
#include "Component.h"

enum class UiType
{
	kButton,
	kImage,
	kSliderX,		// 가로 슬라이더
	kSliderY,		// 세로 슬라이더
	kProgressBarX,	// 가로 진행률 표시줄
	kProgressBarY,	// 세로 진행률 표시줄
	kPanel,
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

protected:
	UiType type_{ UiType::kImage }; // UI 타입
};

