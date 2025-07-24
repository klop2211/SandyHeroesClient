#pragma once
#include "Component.h"
#include "BaseScene.h"

enum class MonsterType { kNormal, kBoss, kMiniBoss };
enum class StatusEffectType { None, Fire, Poison, Electric };
struct StatusEffect
{
	//StatusEffectType  type = StatusEffectType::None;
	float duration = 0.f;
	float elapsed = 0.f;
	float fire_damage = 0.0f;

	bool IsActive() const { return elapsed < duration; }
};

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

	void HitDamage(float damage); //���Ϳ� �������� ����

	void ApplyStatusEffect(StatusEffectType type, float duration, float base_damage);	// �Ӽ� ȿ�� ����

	//setter
	void set_shield(float value);
	void set_hp(float value);
	void set_attack_force(float value);
	void set_target(Object* target); //���Ͱ� ������ Ÿ�� ����
	void set_is_pushed(bool is_pushed);
	void set_push_timer(float value);

	//getter
	float shield() const;
	float hp()const;
	float max_hp()const;
	float max_shield()const;
	float attack_force()const;

	void set_scene(Scene* value);
	
	bool IsDead() const;

private:
	float max_hp_{ 90.f };

	float shield_{ 50.f };
	float max_shield_{ 50.f };	
	float hp_{ 90.f };
	float attack_force_{};

	MonsterType monster_type_{ MonsterType::kNormal };

	Object* target_{ nullptr };	//���Ͱ� ������ Ÿ��

	bool is_dead_animationing_{ false };	//�״� �ִϸ��̼��� ��������� ����

	Scene* scene_{ nullptr };

	// ���� ��ħ ���� ���ֱ� ���� ����
	bool is_pushed_{ false };
	float push_timer_{ 0.0f }; // �и� �� ���� �ð� �̵� ����

	bool electric_slow_applied_{ false };
	float original_speed_{3.5};

	bool dead_by_fire_{ false };

	//StatusEffect status_effect_; //���� �Ӽ� ȿ�� ����
	std::unordered_map<StatusEffectType, StatusEffect> status_effects_; //���� �Ӽ� ȿ�� ����

	//TODO: ���͸� ������ AI �߰�
};

