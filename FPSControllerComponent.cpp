#include "stdafx.h"
#include "FPSControllerComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "PlayerAnimationState.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "Scene.h"
#include "Mesh.h"
#include "Shader.h"
#include "ModelInfo.h"
#include "Material.h"
#include "AnimationSet.h"
#include "GunComponent.h"
#include "MovementComponent.h"

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

		RECT client_rect;
		GetClientRect(client_wnd_, &client_rect);

		POINT center;
		center.x = (client_rect.right - client_rect.left) / 2;
		center.y = (client_rect.bottom - client_rect.top) / 2;
		mouse_xy_.x = center.x;
		mouse_xy_.y = center.y;

		ClientToScreen(client_wnd_, &center);

		owner_->Rotate(0, (mouse_cursor_pos.x - center.x) * 0.1, 0.f);
		if (camera_object_)
		{
			camera_object_->Rotate((mouse_cursor_pos.y - center.y) * 0.1, 0, 0);
		}
		SetCursorPos(center.x, center.y);


	}
		break;
	case WM_LBUTTONDOWN:
		is_firekey_down_ = true;
		mouse_xy_.x = LOWORD(l_param);
		mouse_xy_.y = HIWORD(l_param);
		{		
			GunComponent* gun = Object::GetComponentInChildren<GunComponent>(owner_);
		if (!gun)
		{
			break;
		}
			// 1. 총구 위치
			XMFLOAT3 gun_shoting_point{ gun->owner()->world_position_vector() };

			// 2. 피킹 지점(월드 좌표계)
			int sx = mouse_xy_.x;
			int sy = mouse_xy_.y;
			Object* picked_object = nullptr;
			XMVECTOR picking_point_w = scene_->GetPickingPointAtWorld(sx, sy, picked_object);

			// 3. 1번에서 2번을 향하는 총알 발사
			XMFLOAT3 bullet_dir{};
			XMStoreFloat3(&bullet_dir, XMVector3Normalize(picking_point_w - XMLoadFloat3(&gun_shoting_point)));
			Mesh* bullet_mesh = Scene::FindMesh("green_cube", scene_->meshes());
			gun->FireBullet(bullet_dir, bullet_mesh);
		}
		break;
	case WM_LBUTTONUP:
		is_firekey_down_ = false;
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
		{
			constexpr float kFallCheckVelocity = -0.5;
			if (owner_->is_ground())
			{
				is_jumpkey_pressed_ = true;
			}
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
	const auto& movement = Object::GetComponent<MovementComponent>(owner_);

	XMFLOAT3 velocity{ 0,0,0 };
	float speed = 10;
	XMFLOAT3 look = owner_->look_vector();
	XMFLOAT3 right = owner_->right_vector();
	look.y = right.y = 0.f;

	look = xmath_util_float3::Normalize(look);
	right = xmath_util_float3::Normalize(right);


	// if (is_key_down_['W']) velocity += look * speed;
	// if (is_key_down_['S']) velocity -= look * speed;
	// if (is_key_down_['A']) velocity -= right * speed;
	// if (is_key_down_['D']) velocity += right * speed;

	// // 중력
	// if (owner_->is_ground())
	// 	y_axis_velocity_ = 0.f;
	// else
	// 	y_axis_velocity_ -= gravity_ * elapsed_time;
	if (is_key_down_['W']) movement->MoveXZ(look.x, look.z, speed);
	if (is_key_down_['S']) movement->MoveXZ(-look.x, -look.z, speed);
	if (is_key_down_['A']) movement->MoveXZ(-right.x, -right.z, speed);
	if (is_key_down_['D']) movement->MoveXZ(right.x, right.z, speed);

	if (is_jumpkey_pressed_)
	{
		movement->Jump(jump_speed_);
		is_jumpkey_pressed_ = false;
	}

	//velocity.y = y_axis_velocity_;

	// 대쉬
	// �뽬
	constexpr float kDashSpeed = 70.f;
	constexpr float kDashLength = 10.f;
	if (is_dash_pressed_)
	{
		is_dash_pressed_ = false;
// 		dash_velocity_ = xmath_util_float3::Normalize(dash_velocity_) * kDashSpeed;
// 	}
// 	if (xmath_util_float3::Length(owner_->position_vector() - dash_before_position_) >= kDashLength)
// 	{
// 		dash_velocity_ = { 0,0,0 };
// 		if (auto* animator = Object::GetComponent<AnimatorComponent>(owner_))
		movement->set_max_speed_xz_(kDashSpeed);
		movement->MoveXZ(dash_velocity_.x, dash_velocity_.z, kDashSpeed);
	}
	if (xmath_util_float3::Length(owner_->position_vector() - dash_before_position_) >= kDashLength)
	{
		movement->set_max_speed_xz_(speed);
		AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(owner_);
		if (animator)
		{
			animator->animation_state()->set_animation_track((int)PlayerAnimationTrack::kIdle);
		}
	}

	dash_cool_delta_time_ -= elapsed_time;

	//velocity += dash_velocity_;

	// 위치 이동은 여기서 하지 않음!
	//owner_->set_velocity(velocity);
	if (is_firekey_down_)
	{
		//�Ѿ��� �ѱ����� ��ŷ �������� �߻��ǰ� ����
		//2���� ���� �ʿ� 1. �ѱ� ��ġ 2. ��ŷ ����
		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(owner_);
		if (gun && (gun->fire_type() == GunFireType::kAuto))
		{
			// 1. �ѱ� ��ġ
			XMFLOAT3 gun_shoting_point{ gun->owner()->world_position_vector() };

			// 2. ��ŷ ����(���� ��ǥ��)
			int sx = mouse_xy_.x;
			int sy = mouse_xy_.y;
			Object* picked_object = nullptr;
			XMVECTOR picking_point_w = scene_->GetPickingPointAtWorld(sx, sy, picked_object);

			// 3. 1������ 2���� ���ϴ� �Ѿ� �߻�
			XMFLOAT3 bullet_dir{};
			XMStoreFloat3(&bullet_dir, XMVector3Normalize(picking_point_w - XMLoadFloat3(&gun_shoting_point)));
			Mesh* bullet_mesh = Scene::FindMesh("green_cube", scene_->meshes());
			gun->FireBullet(bullet_dir, bullet_mesh);
		}
	}
}


void FPSControllerComponent::set_camera_object(Object* value)
{
	camera_object_ = value;
}

void FPSControllerComponent::set_scene(Scene* value)
{
	scene_ = value;
}

