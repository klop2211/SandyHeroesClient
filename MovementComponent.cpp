#include "stdafx.h"
#include "MovementComponent.h"
#include "Object.h"

MovementComponent::MovementComponent(Object* owner) : Component(owner)
{
}

MovementComponent::MovementComponent(const MovementComponent& other) : Component(other)
{
}

Component* MovementComponent::GetCopy()
{
    return new MovementComponent(*this);
}

void MovementComponent::Update(float elapsed_time)
{
	old_position_ = owner_->position_vector();  

    if (is_gravity_)
    {
        velocity_.y -= gravity_acceleration_ * elapsed_time;
    }

    //���鿡 �������� �� �̻� �������� ����.
    if (owner_->is_ground() && velocity_.y < 0.f)
    {
        velocity_.y = 0.f;
    }

    //���� ���� ����
    if (velocity_.y > 0 && (jump_before_y_ + jump_max_height_) <= owner_->position_vector().y)
    {
         velocity_.y = 0.f;
    }

    //�ִ� �ӷ� ����
	float speed = xmath_util_float3::Length(velocity_);
	XMFLOAT3 direction = xmath_util_float3::Normalize(velocity_);
	if (speed > max_speed_)
	{
		velocity_ = xmath_util_float3::ScalarProduct(direction, max_speed_);
	}

    //xz ��� �̵��� �ʿ��� ������
    XMFLOAT3 velocity_xz{ velocity_.x, 0.f, velocity_.z };
    XMFLOAT3 direction_xz = xmath_util_float3::Normalize(velocity_xz);
    float speed_xz = xmath_util_float3::Length(velocity_xz);
    constexpr float kFrictionAcceleration{ 70.f };

    //xz ��� �ִ� �ӵ� ����
    if (speed_xz > max_speed_xz_)
    {
        velocity_.x *= (max_speed_xz_ / speed_xz);
        velocity_.z *= (max_speed_xz_ / speed_xz);
    }

    //���� ����
    if (is_friction_)
    {
        if (is_gravity_)
        {
            if (speed_xz < elapsed_time * kFrictionAcceleration)
            {
                velocity_ = { 0, velocity_.y, 0 };
            }
            else
            {
                velocity_ -= direction_xz * elapsed_time * kFrictionAcceleration;
            }
        }
        else
        {
            if (speed < elapsed_time * kFrictionAcceleration)
            {
                velocity_ = { 0, 0, 0 };
            }
            else
            {
                velocity_ -= direction * elapsed_time * kFrictionAcceleration;
            }
        }
    }
    owner_->set_position_vector(owner_->position_vector() + (velocity_ * elapsed_time));
}

void MovementComponent::EnableGravity()
{
    is_gravity_ = true;
}

void MovementComponent::DisableGarvity()
{
    is_gravity_ = false;

}

void MovementComponent::EnableFriction()
{
    is_friction_ = true;
}

void MovementComponent::DisableFriction()
{
    is_friction_ = false;
}

void MovementComponent::Move(float x, float y, float z, float speed)
{
    Move(XMFLOAT3{ x, y, z }, speed);
}

void MovementComponent::Move(XMFLOAT3 direction, float speed)
{
    XMFLOAT3 direction_n = xmath_util_float3::Normalize(direction);
    velocity_ += direction_n * speed;
}

void MovementComponent::MoveXZ(float x, float z, float speed)
{
    XMFLOAT3 direction{ x, 0.f, z };
    direction = xmath_util_float3::Normalize(direction);
    velocity_ += direction * speed;
}

void MovementComponent::Jump(float speed, float max_height)
{
    jump_before_y_ = owner_->position_vector().y;
    velocity_.y = speed;
    jump_max_height_ = max_height;
}

void MovementComponent::Stop()
{
    velocity_ = XMFLOAT3{ 0.f, 0.f, 0.f };
}

void MovementComponent::set_gravity_acceleration(float value)
{
    gravity_acceleration_ = value;
}

void MovementComponent::set_max_speed_xz(float value)
{
    max_speed_xz_ = value;
}

void MovementComponent::set_max_speed(float value)
{
	max_speed_ = value;
}

XMFLOAT3 MovementComponent::velocity() const
{
    return velocity_;
}

float MovementComponent::max_speed_xz() const
{
    return max_speed_xz_;
}
