#pragma once
#include "UiComponent.h"
class ProgressBarComponent :
    public UiComponent
{
public:
	ProgressBarComponent() = default;
	ProgressBarComponent(Object* owner);
	ProgressBarComponent(const ProgressBarComponent& other);
	~ProgressBarComponent() = default;

	virtual Component* GetCopy() override
	{
		return new ProgressBarComponent(*this);
	}

	virtual void Update(float elapsed_time) override;

	void set_max_value(float value) { max_value_ = value; is_correct_max_ = true; }
	void set_current_value(float value) { current_value_ = value; }
	void set_view(Object* view) { view_ = view; }
	void set_get_current_value_func(const std::function<float(Object*)>& func)
	{
		get_current_value_func_ = func;
	}
	void set_get_max_value_func(const std::function<float(Object*)>& func)
	{
		get_max_value_func_ = func;
	}
private:
	float max_value_{ 100.f }; // �ִ밪
	float current_value_{ 0.f }; // ���簪
	bool is_correct_max_{ false };
	std::function<float(Object*)> get_max_value_func_{ nullptr }; // �ִ밪�� �������� �Լ�
	std::function<float(Object*)> get_current_value_func_{ nullptr }; // ���簪�� �������� �Լ�
	Object* view_{ nullptr }; // ���簪�� �������� �Լ��� ����ϴ� ������Ʈ
};

