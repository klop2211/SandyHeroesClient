#include "stdafx.h"
#include "TextRenderer.h"
#include "TextComponent.h"

void TextRenderer::Render(ID2D1DeviceContext2* d2d_device_context, ID2D1SolidColorBrush* brush)
{
	if (!is_render_ || text_list_.empty())
	{
		return; // �������� �ؽ�Ʈ�� ������ ��ȯ
	}

	for (auto& text_component : text_list_)
	{
		text_component->Render(d2d_device_context, brush); // �ؽ�Ʈ ������
	}	

	text_list_.clear(); // ������ �� �ؽ�Ʈ ����Ʈ �ʱ�ȭ

}
