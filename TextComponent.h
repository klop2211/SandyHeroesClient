#pragma once
#include "UiComponent.h"
#include "TextRenderer.h"

class TextFormat;

class TextComponent :
    public UiComponent
{
public:	
	TextComponent();
	TextComponent(Object* owner);
	TextComponent(
		Object* owner,
		TextFormat* text_format = nullptr,
		D2D1_RECT_F text_rect = D2D1::RectF(0.f, 0.f, 1920.f, 1080.f),
		std::function<std::wstring(Object*)> get_text_func = nullptr);
	TextComponent(
		Object* owner,
		TextFormat* text_format = nullptr,
		D2D1_RECT_F text_rect = D2D1::RectF(0.f, 0.f, 1920.f, 1080.f),
		const std::wstring& text = L"None");

	TextComponent(const TextComponent& other) = default; //���� ������
	
	virtual Component* GetCopy() override;
	virtual void Update(float elapsed_time);

	void Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush);

	void set_text(const std::wstring& text);
	void set_get_text_func(const std::function<std::wstring(Object*)>& func);
	void set_is_static(bool value) { is_static_ = value; }
	void set_color(const D2D1_COLOR_F& color) { color_ = color; }

	static TextRenderer* kTextRenderer;

private:
	std::wstring text_ = L"None"; //�ؽ�Ʈ ����
	D2D1_COLOR_F color_ = D2D1::ColorF(D2D1::ColorF::White); //��Ʈ ����
	D2D1::Matrix3x2F transform_ = D2D1::Matrix3x2F::Identity(); //�ؽ�Ʈ ��ȯ ���
	TextFormat* text_format_ = nullptr; //�ؽ�Ʈ ����
	bool is_static_{ true }; //�ؽ�Ʈ�� ����(static)���� ����
	std::function<std::wstring(Object*)> get_text_func_{ nullptr }; //�ؽ�Ʈ�� �������� �Լ�

	D2D1_RECT_F text_rect_{ 0.f, 0.f, 1920.f, 1080.f }; //�ؽ�Ʈ ������ ����
};

