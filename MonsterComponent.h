#pragma once
#include "Component.h"

//Object를 몬스터의 기능을 추가하는 컴포넌트
class MonsterComponent :
    public Component
{
public:
	MonsterComponent(Object* owner);
	MonsterComponent(const MonsterComponent& other);
	virtual ~MonsterComponent() {}

	virtual Component* GetCopy() override;

	//setter
	void set_shield(float value);
	void set_hp(float value);
	void set_attack_force(float value);

	//setter
	float shield() const;
	float hp()const;
	float attack_force()const;

private:
	float shield_{};
	float hp_{};
	float attack_force_{};

};

