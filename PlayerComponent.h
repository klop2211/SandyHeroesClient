#pragma once
#include "Component.h"
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

	void Heal(float amount);	//�÷��̾ ȸ���ϴ� �Լ�
	void HealHp(float heal_amount);	//�÷��̾��� ü���� ȸ���ϴ� �Լ�
	void HealShield(float heal_amount);	//�÷��̾��� ���и� ȸ���ϴ� �Լ�

	void HitDamage(float damage);	//�÷��̾ �������� ������ �Լ�

	//getter
	float max_hp() const { return max_hp_; }	//�÷��̾� �ִ� ü��
	float hp() const { return hp_; }		//�÷��̾� ���� ü��
	float max_shield() const { return max_shield_; }	//�÷��̾� �ִ� ����
	float shield() const { return shield_; }	//�÷��̾� ���� ����

private:
	float max_hp_ = 100.f;	//�÷��̾� �ִ� ü��
	float hp_ = 100.f;		//�÷��̾� ���� ü��
	float max_shield_ = 100.f;	//�÷��̾� �ִ� ����
	float shield_ = 100.f;	//�÷��̾� ���� ����

};

