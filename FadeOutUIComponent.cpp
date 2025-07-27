#include "stdafx.h"
#include "FadeOutUIComponent.h"
#include "UiMeshComponent.h"
#include "Object.h"

FadeOutUIComponent::FadeOutUIComponent(Object* owner, float duration)
	: Component(owner), duration_(duration)
{
	ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_);
	if (ui_mesh_)
	{
		ui_mesh_->set_alpha(1.0f); // ������ ���� ������
	}
}

FadeOutUIComponent::FadeOutUIComponent(const FadeOutUIComponent& other)
	: Component(other), duration_(other.duration_), elapsed_(other.elapsed_)
{
}

Component* FadeOutUIComponent::GetCopy()
{
	return new FadeOutUIComponent(*this);
}

void FadeOutUIComponent::Update(float elapsed_time)
{
	if (!ui_mesh_)
	{
		ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_);
		if (!ui_mesh_) return;
	}

	elapsed_ += elapsed_time;
	float alpha = std::max(0.0f, 1.0f - (elapsed_ / duration_));
	ui_mesh_->set_alpha(alpha);

	// �� ������� ����� or ����
	if (elapsed_ >= duration_)
	{
		ui_mesh_->set_is_visible(false);
		owner_->set_is_dead(true); // �Ǵ� Destroy() �� ó��
	}
}