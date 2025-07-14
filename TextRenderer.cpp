#include "stdafx.h"
#include "TextRenderer.h"
#include "TextComponent.h"

void TextRenderer::Render(ID2D1Bitmap1* d2d_render_target, ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush)
{
	d2d_device_context->SetTarget(d2d_render_target);
	d2d_device_context->BeginDraw();

	for (auto& text_component : text_list_)
	{
		text_component->Render(d2d_device_context, brush); // �ؽ�Ʈ ������
	}	

	d2d_device_context->EndDraw();

	text_list_.clear(); // ������ �� �ؽ�Ʈ ����Ʈ �ʱ�ȭ

}
