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

	bool flame_frenzy = false;
	bool acid_frenzy = false;
	bool electric_frenzy = false;

	bool IsActive() const { return elapsed < duration; }
};

//Object를 몬스터의 기능을 추가하는 컴포넌트
class MonsterComponent :
    public Component
{
public:
	MonsterComponent(Object* owner);
	MonsterComponent(const MonsterComponent& other);
	virtual ~MonsterComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	void HitDamage(float damage); //몬스터에 데미지를 입힘

	void ApplyStatusEffect(StatusEffectType type, float duration, float base_damage, bool flame_frenzy, bool acid_frenzy, bool electric_frenzy);	// 속성 효과 적용

	//setter
	void set_shield(float value);
	void set_hp(float value);
	void set_attack_force(float value);
	void set_target(Object* target); //몬스터가 공격할 타겟 설정
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

	Object* target_{ nullptr };	//몬스터가 공격할 타겟

	bool is_dead_animationing_{ false };	//죽는 애니메이션이 재생중인지 여부

	Scene* scene_{ nullptr };

	// 몬스터 겹침 현상 없애기 위한 변수
	bool is_pushed_{ false };
	float push_timer_{ 0.0f }; // 밀린 후 일정 시간 이동 금지

	bool electric_slow_applied_{ false };
	float original_speed_{3.5};

	bool dead_by_fire_{ false };

	//StatusEffect status_effect_; //단일 속성 효과 적용
	std::unordered_map<StatusEffectType, StatusEffect> status_effects_; //다중 속성 효과 적용

	//TODO: 몬스터를 움직일 AI 추가
};

