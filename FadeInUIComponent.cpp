#include "stdafx.h"
#include "FadeInUIComponent.h"
#include "UiMeshComponent.h"
#include "Object.h"

FadeInUIComponent::FadeInUIComponent(Object* owner, float duration)
	: Component(owner), duration_(duration)
{
	ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_);
	if (ui_mesh_)
	{
		ui_mesh_->set_alpha(0.0f); // ó���� ����
		ui_mesh_->set_is_visible(true); // Ȥ�ö� ��Ȱ��ȭ�� ������ �ѱ�
	}
}

FadeInUIComponent::FadeInUIComponent(const FadeInUIComponent& other)
	: Component(other), duration_(other.duration_), elapsed_(other.elapsed_)
{
	// ���� �����ڿ����� owner_�� ���� ���� ������ Init�� ���� ����� ��
}

Component* FadeInUIComponent::GetCopy()
{
	return new FadeInUIComponent(*this);
}

void FadeInUIComponent::Update(float elapsed_time)
{
	if (!ui_mesh_)
	{
		ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_);
		if (!ui_mesh_) return;
	}

	elapsed_ += elapsed_time;
	float alpha = std::min(1.0f, elapsed_ / duration_);
	ui_mesh_->set_alpha(alpha);
}