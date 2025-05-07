#include "stdafx.h"
#include "GunComponent.h"
#include "Object.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MovementComponent.h"
#include "BoxColliderComponent.h"

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
        is_reload_ = false;
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
    if (!is_reload_)
    {
        loading_time_ = reload_time_;
        is_reload_ = true;

    }
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
            MovementComponent* movement = new MovementComponent(bullet);

            BoundingOrientedBox obb;
            constexpr float bullet_size = 0.1f;
            obb.Extents = { bullet_size, bullet_size, bullet_size };
            BoxColliderComponent* box = new BoxColliderComponent(bullet, obb);
            bullet->AddComponent(movement);
            bullet->AddComponent(box);
            bullet->set_position_vector(owner_->world_position_vector());
            movement->DisableFriction();
            movement->set_gravity_acceleration(9.8f);
            movement->set_max_speed_xz_(bullet_speed_);
            movement->Move(direction, bullet_speed_);
            bullet->Scale(0.1);
            fired_bullet_list_.push_back(bullet);
            --loaded_bullets_;
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

std::list<Object*> GunComponent::fired_bullet_list() const
{
    return fired_bullet_list_;
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

        if (bullet_type_str == "normal")
        {
            info.bullet_type = BulletType::kNormal;
        }
        if (bullet_type_str == "big")
        {
            info.bullet_type = BulletType::kBig;
        }
        if (bullet_type_str == "special")
        {
            info.bullet_type = BulletType::kSpecial;
        }
        if (fire_type_str == "auto")
        {
            info.fire_type = GunFireType::kAuto;
        }
        if (fire_type_str == "semiauto")
        {
            info.fire_type = GunFireType::kSemiAuto;
        }
        if (fire_type_str == "boltaction")
        {
            info.fire_type = GunFireType::kBoltAction;
        }
        if (fire_type_str == "burst")
        {
            info.fire_type = GunFireType::kBurst;
        }
        kGunInfos[name] = info;
    }
}

