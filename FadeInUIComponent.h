#pragma once
#include "Component.h"

class UiMeshComponent;

// UI ���� ���� ������Ű�� ������Ʈ
class FadeInUIComponent : public Component
{
public:
	FadeInUIComponent(Object* owner, float duration = 5.0f);
	FadeInUIComponent(const FadeInUIComponent& other);

	virtual Component* GetCopy() override;
	virtual void Update(float elapsed_time) override;

	// �������� ������ �� ����
	void Reset() { elapsed_ = 0.0f; }

private:
	float duration_{ 5.0f };	// ������ ���̴� �� �ɸ��� �� �ð�
	float elapsed_{ 0.0f };		// ���� �ð�

	UiMeshComponent* ui_mesh_{ nullptr }; // ���ĸ� ������ UI ������Ʈ
};