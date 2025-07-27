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
		ui_mesh_->set_alpha(1.0f); // 시작은 완전 불투명
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

	// 다 사라지면 숨기기 or 삭제
	if (elapsed_ >= duration_)
	{
		ui_mesh_->set_is_visible(false);
		owner_->set_is_dead(true); // 또는 Destroy() 등 처리
	}
}