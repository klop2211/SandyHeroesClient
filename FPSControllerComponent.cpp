#include "stdafx.h"
#include "FPSControllerComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "PlayerAnimationState.h"

FPSControllerComponent::FPSControllerComponent(Object* owner) : InputControllerComponent(owner)
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;

	mouse_xy_.x = kDefaultFrameBufferWidth / 2;
	mouse_xy_.y = kDefaultFrameBufferHeight / 2;
}

Component* FPSControllerComponent::GetCopy()
{
    return new FPSControllerComponent(*this);
}

bool FPSControllerComponent::ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	switch (message_id)
	{
	case WM_MOUSEMOVE:
	{
		POINT mouse_cursor_pos;
		GetCursorPos(&mouse_cursor_pos);
		owner_->Rotate(0, (mouse_cursor_pos.x - mouse_xy_.x) * 0.1, 0.f);
		if (camera_object_)
		{
			camera_object_->Rotate((mouse_cursor_pos.y - mouse_xy_.y) * 0.1, 0, 0);
		}
		SetCursorPos(mouse_xy_.x, mouse_xy_.y);
	}
		break;
	case WM_KEYDOWN:
		switch (w_param)
		{
		case 'W':
			is_key_down_['W'] = true;
			break;
		case 'A':
			is_key_down_['A'] = true;
			break;
		case 'S':
			is_key_down_['S'] = true;
			break;
		case 'D':
			is_key_down_['D'] = true;
			break;
		case VK_SPACE:
			if (owner_->position_vector().y < 0.001) //임시 지면 판정
			{
				is_jumpkey_pressed_ = true;
			}
			break; 
		case VK_SHIFT:
			if (dash_cool_delta_time_ <= 0)
			{
				is_dash_pressed_ = true;
				dash_cool_delta_time_ = dash_cool_time_;
				dash_velocity_ = { 0,0,0 };
				dash_before_position_ = owner_->position_vector();
				XMFLOAT3 look = owner_->look_vector();
				XMFLOAT3 right = owner_->right_vector();
				look.y = 0.f;
				right.y = 0.f;
				if (is_key_down_['W'])
				{
					dash_velocity_ += look;
				}
				if (is_key_down_['S'])
				{
					dash_velocity_ -= look;
				}
				if (is_key_down_['D'])
				{
					dash_velocity_ += right;
				}
				if (is_key_down_['A'])
				{
					dash_velocity_ -= right;
				}
				if (xmath_util_float3::Length(dash_velocity_) == 0)
				{
					dash_velocity_ += look;
				}
			}
			break;
		default:
			return false;
			break;
		}
		break;
	case WM_KEYUP:
		switch (w_param)
		{
		case 'W':
			is_key_down_['W'] = false;
			break;
		case 'A':
			is_key_down_['A'] = false;
			break;
		case 'S':
			is_key_down_['S'] = false;
			break;
		case 'D':
			is_key_down_['D'] = false;
			break;

		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}
	return true;
}

void FPSControllerComponent::Update(float elapsed_time)
{
	XMFLOAT3 velocity{ 0,0,0 };
	float speed = 10;
	XMFLOAT3 look = owner_->look_vector();
	XMFLOAT3 right = owner_->right_vector();
	look.y = 0.f; // xz 평면을 따라 이동
	right.y = 0.f; 
	look = xmath_util_float3::Normalize(look);
	right = xmath_util_float3::Normalize(right);

	if (is_key_down_['W']) velocity += look * speed;
	if (is_key_down_['S']) velocity -= look * speed;
	if (is_key_down_['A']) velocity -= right * speed;
	if (is_key_down_['D']) velocity += right * speed;


	//TODO: 지면 판정 처리 정상화
	if (owner_->position_vector().y < 0.001) //임시 지면 판정
	{
		y_axis_velocity_ = 0.f;
		if (is_jumpkey_pressed_)
		{
			y_axis_velocity_ = jump_speed_;
			is_jumpkey_pressed_ = false;
		}
	}
	else
	{
		y_axis_velocity_ -= gravity_ * elapsed_time;
	}
	XMFLOAT3 position = owner_->position_vector();
	if (position.y < 0)
	{
		owner_->set_position_vector(position.x, 0, position.z);
	}
	velocity.y += y_axis_velocity_;

	// 대쉬
	constexpr float kDashSpeed = 70.f;
	constexpr float kDashLength = 10.f;
	if (is_dash_pressed_)
	{
		is_dash_pressed_ = false;
		dash_velocity_ = (xmath_util_float3::Normalize(dash_velocity_) * kDashSpeed);
	}
	if (xmath_util_float3::Length(owner_->position_vector() - dash_before_position_) >= kDashLength)
	{
		dash_velocity_ = { 0,0,0 };
		AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(owner_);
		if (animator)
		{
			animator->animation_state()->set_animation_track((int)PlayerAnimationTrack::kIdle);
		}
	}
	dash_cool_delta_time_ -= elapsed_time;
	velocity += dash_velocity_;

	owner_->set_velocity(velocity);

}

void FPSControllerComponent::set_camera_object(Object* value)
{
	camera_object_ = value;
}

