#pragma once
#include "CharacterComponent.h"
#include "ScrollComponent.h"

class ModelInfo;
class Scene;

class ChestComponent :
    public CharacterComponent
{
public:
    ChestComponent() = default;
    ChestComponent(Object* owner, Scene* scene = nullptr);
    virtual ~ChestComponent() = default;

    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    virtual void HendleCollision(Object* other_object) override;

    ScrollType TakeScroll();

    void set_scroll_model(ModelInfo* scroll_model)
    {
        scroll_model_ = scroll_model;
	}

private:		
	bool is_open_{ false }; // ü��Ʈ�� �����ִ��� ����
    ModelInfo* scroll_model_{ nullptr };

	Object* scroll_object_{ nullptr }; // ��ũ�� ������Ʈ

	Scene* scene_{ nullptr }; // �� ����
};

