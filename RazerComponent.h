#pragma once
#include "Component.h"
class RazerComponent :
    public Component
{
public:
    RazerComponent(Object* owner);
    RazerComponent(Object* owner, XMFLOAT3 start, XMFLOAT3 end);
    RazerComponent(const RazerComponent& other);
    virtual ~RazerComponent() {}
    virtual Component* GetCopy() override;
    virtual void Update(float elapsed_time) override;

    void InitRazer(XMFLOAT3 start, XMFLOAT3 end);

	float life_time() const { return life_time_; } // �������� ���� �ð� ��ȯ

    //getter
	XMFLOAT3 start_position() const { return start_position_; } // ������ ���� ��ġ ��ȯ
    XMFLOAT3 end_position() const { return end_position_; } // ������ �� ��ġ ��ȯ
	float damage() const { return damage_; } // �������� ������ ���ط� ��ȯ
	float is_collision_active() const { return is_collision_active_; } // ������ �浹 Ȱ��ȭ ���� ��ȯ
	bool is_collided() const { return is_collided_; } // �������� �浹�ߴ��� ���� ��ȯ

	//setter
	void set_is_collided(bool value) { is_collided_ = value; }

private:
	float life_time_ = 0.0f; // �������� ���� �ð�
	float max_life_time_ = 0.8f; // �������� �ִ� ���� �ð�
	XMFLOAT3 start_position_{ 0,0,0 }; // ������ ���� ��ġ
	XMFLOAT3 end_position_{ 0,0,0 }; // ������ �� ��ġ
	float damage_ = 10.f; // �������� ������ ���ط�
	bool is_collision_active_ = false; // �������� �浹 Ȱ��ȭ �������� ����
	bool is_collided_ = false; // �������� �浹�ߴ��� ����
};

