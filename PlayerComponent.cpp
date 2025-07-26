#include "stdafx.h"
#include "PlayerComponent.h"
#include "ScrollComponent.h"
#include "FMODSoundManager.h"
#include "Scene.h"
#include "RazerComponent.h"
#include "BaseScene.h"
#include "MonsterComponent.h"
#include "ModelInfo.h"

PlayerComponent::PlayerComponent(Object* owner)
	: Component(owner)
{
}

PlayerComponent::PlayerComponent(Object* owner, float max_hp, float hp, float max_shield, float shield)
	: Component(owner), max_hp_(max_hp), hp_(hp), max_shield_(max_shield), shield_(shield)
{
}

Component* PlayerComponent::GetCopy()
{
    return new PlayerComponent(*this);
}

void PlayerComponent::Update(float elapsed_time)
{
	main_skill_gage_ += elapsed_time;

	if (main_skill_activation_time_ < main_skill_max_activation_time_)
	{
		main_skill_activation_time_ += elapsed_time;
		main_skill_razer_shot_time_ += elapsed_time;
		if(main_skill_razer_cool_time_ < main_skill_razer_shot_time_){
			main_skill_razer_shot_time_ = 0.f; // ������ �߻� �� ���� �ð��� �ʱ�ȭ
			auto base_scene = dynamic_cast<BaseScene*>(scene_);
			if (!base_scene) return;
			auto monster_list = base_scene->monster_list();
			for (auto& monster : monster_list)
			{
				float length = xmath_util_float3::Length(
					monster->owner()->world_position_vector() - owner_->world_position_vector());
				if (length <= main_skill_range_)
				{
					XMFLOAT3 monster_pos = monster->owner()->world_position_vector();
					std::uniform_real_distribution<float> random_position(-5.f, 5.f);
					XMFLOAT3 start_pos = { monster_pos.x,
										   monster_pos.y,
										   monster_pos.z };
					XMFLOAT3 end_pos = { monster_pos.x + random_position(kRandomGenerator),
										 monster_pos.y + 50.f,
										 monster_pos.z + random_position(kRandomGenerator) };

					auto razer = razer_model_info_->GetInstance();
					auto razer_component = Object::GetComponent<RazerComponent>(razer);
					razer_component->InitRazer(start_pos, end_pos);
					scene_->AddObject(razer);
				}
			}

		}
	}
}

void PlayerComponent::Heal(float amount)
{
	if (amount <= 0.f)
	{
		return; // ȸ������ 0 ������ ��� �ƹ� �۾��� ���� ����
	}
	float rest{};
	if (hp_ < max_hp_)
	{
		hp_ += amount;
		if (hp_ > max_hp_)
		{
			rest = hp_ - max_hp_; // �ִ� ü���� �Ѵ� �κ��� rest�� ����
			hp_ = max_hp_; // hp�� �ִ� ü���� ���� �ʵ���
		}
	}
	else
	{
		rest = amount; // hp�� �̹� �ִ� ü���� ���, rest�� ��ü ȸ���� ����
		hp_ = max_hp_; // �̹� �ִ� ü���� ���
	}

	if (rest < 0.f)
		return;

	if(shield_ < max_shield_)
	{
		shield_ += rest; // ���� ȸ������ ���п� �߰�
		if (shield_ > max_shield_)
		{
			shield_ = max_shield_; // shield�� �ִ� ���и� ���� �ʵ���
		}
	}
	else
	{
		shield_ = max_shield_; // �̹� �ִ� ������ ���
	}
}

void PlayerComponent::HealHp(float heal_amount)
{
	if (hp_ < max_hp_)
	{
		hp_ += heal_amount;
		if (hp_ > max_hp_)
		{
			hp_ = max_hp_; // hp�� �ִ� ü���� ���� �ʵ���
		}
	}
	else
	{
		hp_ = max_hp_; // �̹� �ִ� ü���� ���
	}
}

void PlayerComponent::HealShield(float heal_amount)
{
	if (shield_ < max_shield_)
	{
		shield_ += heal_amount;
		if (shield_ > max_shield_)
		{
			shield_ = max_shield_; // shield�� �ִ� ���и� ���� �ʵ���
		}
	}
	else
	{
		shield_ = max_shield_; // �̹� �ִ� ������ ���
	}
}

void PlayerComponent::HitDamage(float damage)
{

	FMODSoundManager::Instance().PlaySound("grunt", false, 0.3f);

	if (HasScroll(ScrollType::kHardenedSkin))
	{
		damage *= 0.9f;
	}
	if (shield_ > 0.f)
	{
		shield_ -= damage;
		if (shield_ < 0.f)
		{
			hp_ += shield_; // shield_�� ������ hp�� ������
			shield_ = 0.f; // shield�� 0���� ����
		}
	}
	else
	{
		hp_ -= damage;
	}
	if (hp_ < 0.f)
	{
		hp_ = 0.f; // hp�� ������ �������� �ʵ���
	}
}

void PlayerComponent::AddScroll(ScrollType type)
{
	if (type == ScrollType::None) return;

	acquired_scrolls_.insert(type);
}

bool PlayerComponent::HasScroll(ScrollType type) const
{
	return acquired_scrolls_.count(type) > 0;
}

const std::unordered_set<ScrollType>& PlayerComponent::acquired_scrolls() const
{
	return acquired_scrolls_;
}

void PlayerComponent::ActivateMainSkill()
{
	if (!razer_model_info_)
	{
		razer_model_info_ = scene_->FindModelInfo("Razer");
	}
	if (main_skill_gage_ < main_skill_max_gage_)
		return;

	main_skill_gage_ = 0.f;
	main_skill_activation_time_ = 0.f;
}
