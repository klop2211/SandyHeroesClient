#pragma once
#include "Component.h"

class UiMeshComponent;

class FadeOutUIComponent : public Component
{
public:
	FadeOutUIComponent(Object* owner, float duration = 5.0f);
	FadeOutUIComponent(const FadeOutUIComponent& other);

	virtual Component* GetCopy() override;
	virtual void Update(float elapsed_time) override;

private:
	float duration_{ 5.0f };
	float elapsed_{ 0.0f };
	UiMeshComponent* ui_mesh_{ nullptr };
};