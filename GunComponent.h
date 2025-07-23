#pragma once
#include "Component.h"

class Mesh;
class Scene;

enum class GunFireType { kAuto, kSemiAuto, kBoltAction, kBurst };
enum class BulletType { kNormal, kBig, kSpecial };
enum class ElementType { kFire, kElectric, kPoison };

struct GunInfo
{
    int damage{ 0 };
    float critical_damage_rate{ 2.0 };                 // ġ��Ÿ ����
    int rpm{ 300 };                                    // �ѱ� RPM (rounds per minute)
    int magazine_cacity{ 12 };                         // ��ź ��
    float reload_time{ 1.75 };                          // ������ �ð�
    GunFireType fire_type{ GunFireType::kSemiAuto };   // �߻� ���
    int burst_num{ 0 };                                // ���� �Ѿ� ����(���� ����� �Ѹ� ���)
    BulletType bullet_type{ BulletType::kNormal };     // ����ϴ� �Ѿ� Ÿ��
    float bullet_speed{ 350.f };                       // ź��

};

//������Ʈ�� �ѱ����� �߰��ϴ� ������Ʈ
//�Ѿ� �߻� �� ����, ���������� �� �ѱ⿡ ���� ����� ������.
class GunComponent :
    public Component
{
public:
    GunComponent(Object* owner);
    GunComponent(const GunComponent& other);
    
    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    void ReloadBullets();

    bool FireBullet(XMFLOAT3 direction, Object* bullet_mesh, Scene* scene);

    void LoadGunInfo(const std::string& gun_name);

    //setter
    void set_gun_name(std::string& value);
    void set_upgrade(int value);
    void set_element(ElementType value);
    void set_scene(Scene* value);

    //getter
    GunFireType fire_type() const;
    BulletType bullet_type() const;
    std::list<Object*> fired_bullet_list() const;
	int damage() const;
	float critical_damage_rate() const;
	int loaded_bullets() const;
    BoundingBox flamethrow_box() const;
    const std::string& gun_name() const { return gun_name_; }
    int upgrade() const;
    ElementType element() const;

    //�ѱ� ������ �ε��ϴ� �Լ�
    static void LoadGunInfosFromFile(const std::string& file_name); 

private:
    Scene* scene_{ nullptr };
    Object* player_{ nullptr };

    static std::unordered_map<std::string, GunInfo> kGunInfos;

    std::list<Object*> fired_bullet_list_;
    int loaded_bullets_{};      //���� �Ѿ� ��
    float loading_time_{ 0.f }; //���� �� �ð�
    bool is_reload_ = false;    //���� ��?
    float cooling_time_{ 0.f }; //������ �߻�� ���� ����� �ð�

    BoundingBox flamethrow_box_{
    { 0.0f, 0.0f, 0.0f },    // Center
    { 1.5f, 2.0f, 2.5f }     // Extents
    };

    std::string gun_name_;
    int upgrade_{};
    ElementType element_{ ElementType::kFire };

    // �ѱ� ����
    int damage_{ 0 };                   
    float critical_damage_rate_{ 2.0 };                 // ġ��Ÿ ����
    int rpm_{ 300 };                                    // �ѱ� RPM (rounds per minute)
    int magazine_cacity_{ 12 };                         // ��ź ��
    float reload_time_{ 1.75 };                          // ������ �ð�
    GunFireType fire_type_{ GunFireType::kSemiAuto };   // �߻� ���
    int burst_num_{ 0 };                                // ���� �Ѿ� ����(���� ����� �Ѹ� ���)
    BulletType bullet_type_{ BulletType::kNormal };     // ����ϴ� �Ѿ� Ÿ��
    float bullet_speed_{ 200.f };                       // ź�� (�⺻ : 350)
};

