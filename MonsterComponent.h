#pragma once
#include "Component.h"
#include "BaseScene.h"

enum class MonsterType { kNormal, kBoss, kMiniBoss };

//Object�� ������ ����� �߰��ϴ� ������Ʈ
class MonsterComponent :
    public Component
{
public:
	MonsterComponent(Object* owner);
	MonsterComponent(const MonsterComponent& other);
	virtual ~MonsterComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	//setter
	void set_shield(float value);
	void set_hp(float value);
	void set_attack_force(float value);
	void set_target(Object* target); //���Ͱ� ������ Ÿ�� ����

	//getter
	float shield() const;
	float hp()const;
	float attack_force()const;

	void set_scene(Scene* value);

private:
	float max_hp_{ 120.f };

	float shield_{};
	float hp_{ 120.f };
	float attack_force_{};

	MonsterType monster_type_{ MonsterType::kNormal };

	Object* target_{ nullptr };	//���Ͱ� ������ Ÿ��

	bool is_dead_animationing_{ false };	//�״� �ִϸ��̼��� ��������� ����

	//TODO: ���͸� ������ AI �߰�
};

