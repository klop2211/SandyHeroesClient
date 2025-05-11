#pragma once
#include "Component.h"

//������Ʈ�� �̵� ����� �߰��ϴ� ������Ʈ
//�ӵ��� ���� �̵�, �߷� ���� ���� 2���� ��ɸ� �����Ͽ���.
//����: m/s
class MovementComponent :
    public Component
{
public:
    MovementComponent(Object* owner);
    MovementComponent(const MovementComponent& other);

    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    void EnableGravity();
    void DisableGarvity();

    void EnableFriction();
    void DisableFriction();

    void Move(float x, float y, float z, float speed);
    void Move(XMFLOAT3 direction, float speed);

    //�Է¹��� xz�������� �ӷ��� speed ��ŭ �߰��Ѵ�.
    void MoveXZ(float x, float z, float speed);

    void Jump(float speed, float max_height = 999.f);

    void Stop();

    //setter
    void set_gravity_acceleration(float value);
    void set_max_speed_xz(float value);
	void set_max_speed(float value);

    //getter
    XMFLOAT3 velocity() const;

private:
    bool is_gravity_{ true };
    float gravity_acceleration_{ 20.f };

    float jump_max_height_{ 999.f };
    float jump_before_y_{};

    float max_speed_xz_{ 10.f };    //�ִ� �ӷ�
    float max_speed_{ 999.f };

    bool is_friction_{ true };      //������ �����ϴ��� ����

    XMFLOAT3 velocity_{};

	XMFLOAT3 old_position_{};
};

