#pragma once
#include "Component.h"
#include "ScrollComponent.h"

//class ScrollComponent;


class Scene;
class ModelInfo;

class PlayerComponent :
    public Component
{
public:
	PlayerComponent() {};
	PlayerComponent(Object* owner);
	PlayerComponent(Object* owner, float max_hp, float hp, float max_shield, float shield);
	PlayerComponent(const PlayerComponent& other) = default; //���� ������
	virtual ~PlayerComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	void Heal(float amount);	//�÷��̾ ȸ���ϴ� �Լ�
	void HealHp(float heal_amount);	//�÷��̾��� ü���� ȸ���ϴ� �Լ�
	void HealShield(float heal_amount);	//�÷��̾��� ���и� ȸ���ϴ� �Լ�

	void HitDamage(float damage);	//�÷��̾ �������� ������ �Լ�

	void AddScroll(ScrollType type);
	bool HasScroll(ScrollType type) const;

	void ActivateMainSkill();	//�÷��̾� ���� ��ų Ȱ��ȭ �Լ�

	//getter
	float max_hp() const { return max_hp_; }	//�÷��̾� �ִ� ü��
	float hp() const { return hp_; }		//�÷��̾� ���� ü��
	float max_shield() const { return max_shield_; }	//�÷��̾� �ִ� ����
	float shield() const { return shield_; }	//�÷��̾� ���� ����
	const std::unordered_set<ScrollType>& acquired_scrolls() const;
	float main_skill_gage() const { return main_skill_gage_; }
	float main_skill_max_gage() const { return main_skill_max_gage_; }

	//setter
	void set_scene(Scene* scene) { scene_ = scene; }	//�÷��̾ ���� �� ����

private:
	Scene* scene_ = nullptr;	//�÷��̾ ���� ��

	float max_hp_ = 100.f;	//�÷��̾� �ִ� ü��
	float hp_ = 100.f;		//�÷��̾� ���� ü��
	float max_shield_ = 100.f;	//�÷��̾� �ִ� ����
	float shield_ = 100.f;	//�÷��̾� ���� ����

	std::unordered_set<ScrollType> acquired_scrolls_;

	ModelInfo* razer_model_info_ = nullptr;	//�÷��̾� ������ �� ����
	float main_skill_gage_ = 0.f;	//�÷��̾� ���� ��ų ������
	float main_skill_max_gage_ = 25.f;	//�÷��̾� �ִ� ���� ��ų ������
	float main_skill_activation_time_ = 0.f;	//�÷��̾� ���� ��ų Ȱ��ȭ �ð�
	float main_skill_max_activation_time_ = 5.f;	//�÷��̾� �ִ� ���� ��ų Ȱ��ȭ �ð�
	float main_skill_razer_shot_time_ = 0.f;	//�÷��̾� ���� ��ų ������ �߻� �� ���� �ð�
	float main_skill_razer_cool_time_ = 0.3f;	//�÷��̾� ���� ��ų ������ �߻� ����
	float main_skill_range_ = 20.f;	//�÷��̾� ���� ��ų ������ ��Ÿ�
};

