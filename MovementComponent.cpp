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
    if (is_gravity_)
    {
        velocity_.y -= gravity_acceleration_ * elapsed_time;
    }

    //지면에 서있으면 더 이상 떨어지지 않음.
    if (owner_->is_ground() && velocity_.y < 0.f)
    {
        velocity_.y = 0.f;
    }

    //점프 높이 제한
    if (velocity_.y > 0 && (jump_before_y_ + jump_max_height_) <= owner_->position_vector().y)
    {
         velocity_.y = 0.f;
    }

    //xz 평면 이동에 필요한 변수들
    XMFLOAT3 velocity_xz{ velocity_.x, 0.f, velocity_.z };
    XMFLOAT3 direction = xmath_util_float3::Normalize(velocity_xz);
    float speed = xmath_util_float3::Length(velocity_xz);
    const float kMaxSpeedXz = elapsed_time * max_speed_xz_;
    constexpr float kFrictionAcceleration{ 70.f };

    //xz 평면 최대 속도 제한
    if (speed > max_speed_xz_)
    {
        velocity_.x *= (max_speed_xz_ / speed);
        velocity_.z *= (max_speed_xz_ / speed);
    }

    //마찰 구현
    if (speed < elapsed_time * kFrictionAcceleration)
    {
        velocity_ = { 0, velocity_.y, 0 };
    }
    else
    {
        velocity_ -= direction * elapsed_time * kFrictionAcceleration;
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

void MovementComponent::set_max_speed_xz_(float value)
{
    max_speed_xz_ = value;
}

XMFLOAT3 MovementComponent::velocity() const
{
    return velocity_;
}
