#include "stdafx.h"
#include "GunComponent.h"
#include "Object.h"
#include "Mesh.h"
#include "MeshComponent.h"

std::unordered_map<std::string, GunInfo> GunComponent::kGunInfos{};

GunComponent::GunComponent(Object* owner) : Component(owner)
{
}

GunComponent::GunComponent(const GunComponent& other) : Component(other.owner_)
{
}

GunComponent::~GunComponent()
{
    for (const auto& bullet : fired_bullet_list_)
    {
        delete bullet;
    }
}

Component* GunComponent::GetCopy()
{
    return new GunComponent(*this);
}

void GunComponent::Update(float elapsed_time)
{
    loading_time_ -= elapsed_time;
    cooling_time_ += elapsed_time;

    if (is_reload_ && loading_time_ <= 0.f)
    {
        loaded_bullets_ = magazine_cacity_;
    }

    for (const auto& bullet : fired_bullet_list_)
    {
        bullet->Update(elapsed_time);
        bullet->UpdateWorldMatrix(nullptr);
    }

}


void GunComponent::ReloadBullets()
{
    //TODO: ������ �ִϸ��̼� ����
    loading_time_ = reload_time_;
    is_reload_ = true;
}

bool GunComponent::FireBullet(XMFLOAT3 direction, Mesh* bullet_mesh)
{
    if (loaded_bullets_ > 0)
    {
        const float rps = rpm_ / 60.f;
        if (rps * cooling_time_ >= 1.f)
        {
            cooling_time_ = 0.f;
            Object* bullet = new Object();
            bullet->AddComponent(new MeshComponent(bullet, bullet_mesh));
            bullet->set_position_vector(owner_->world_position_vector());
            bullet->set_velocity(direction * bullet_speed_);
            bullet->Scale(0.5);
            fired_bullet_list_.push_back(bullet);
        }
    }
    else
    {
        ReloadBullets();
    }

    return false;
}

void GunComponent::LoadGunInfo(const std::string& gun_name)
{
    GunInfo gun_info{ kGunInfos[gun_name] };

    damage_ = gun_info.damage;
    critical_damage_rate_ = gun_info.critical_damage_rate;
    rpm_ = gun_info.rpm;
    magazine_cacity_ = gun_info.magazine_cacity;
    reload_time_ = gun_info.reload_time;
    fire_type_ = gun_info.fire_type;
    burst_num_ = gun_info.burst_num;
    bullet_type_ = gun_info.bullet_type;
    bullet_speed_ = gun_info.bullet_speed;

    loaded_bullets_ = magazine_cacity_;

}

GunFireType GunComponent::fire_type() const
{
    return fire_type_;
}


void GunComponent::LoadGunInfosFromFile(const std::string& file_name)
{
    std::ifstream file(file_name);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] != '@') continue;

        std::istringstream ss(line);
        std::string at_sign, name, bullet_type_str, fire_type_str;
        GunInfo info;

        ss >> at_sign >> name >> bullet_type_str
            >> info.damage >> info.critical_damage_rate >> info.rpm
            >> info.magazine_cacity >> info.reload_time >> fire_type_str >> info.burst_num >> info.bullet_speed;

        if (bullet_type_str == "�Ϲ�")
        {
            info.bullet_type = BulletType::kNormal;
        }
        if (bullet_type_str == "����")
        {
            info.bullet_type = BulletType::kBig;
        }
        if (bullet_type_str == "Ư��")
        {
            info.bullet_type = BulletType::kSpecial;
        }
        if (fire_type_str == "�ڵ�")
        {
            info.fire_type = GunFireType::kAuto;
        }
        if (fire_type_str == "���ڵ�")
        {
            info.fire_type = GunFireType::kSemiAuto;
        }
        if (fire_type_str == "��Ʈ�׼�")
        {
            info.fire_type = GunFireType::kBoltAction;
        }
        if (fire_type_str == "����")
        {
            info.fire_type = GunFireType::kBurst;
        }
        kGunInfos[name] = info;
    }
}

