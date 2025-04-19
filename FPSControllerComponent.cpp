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
	{
		//총알이 총구에서 피킹 지점으로 발사되게 구현
		//2가지 정보 필요 1. 총구 위치 2. 피킹 지점

		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(owner_);
		if (!gun)
		{
			break;
		}

		// 1. 총구 위치
		XMFLOAT3 gun_shoting_point{};

		// 2. 피킹 지점(월드 좌표계)
		XMVECTOR picking_point_w = XMLoadFloat3(&(camera_object_->world_position_vector() + (camera_object_->world_look_vector() * 100.f)));
		float picking_length_min = std::numeric_limits<float>::max();

		//picking ray 계산
		int sx = LOWORD(l_param);
		int sy = HIWORD(l_param);
		CameraComponent* camera = Object::GetComponentInChildren<CameraComponent>(camera_object_);
		if (!camera)
		{
			break;
		}
		XMFLOAT4X4 proj = camera->projection_matrix();

		float vx = (2.f * sx / kDefaultFrameBufferWidth - 1.f) / proj(0, 0);
		float vy = (-2.f * sy / kDefaultFrameBufferHeight + 1.f) / proj(1, 1);

		XMMATRIX view = XMLoadFloat4x4(&camera->view_matrix());
		XMMATRIX inverse_view = XMMatrixInverse(&XMMatrixDeterminant(view), view);

		//월드좌표계에서 피킹광선
		XMVECTOR ray_origin{ XMVectorSet(0, 0, 0, 1.f) };
		XMVECTOR ray_direction{ XMVectorSet(vx, vy, 1.f, 0) };

		ray_origin = XMVector3TransformCoord(ray_origin, inverse_view);
		ray_direction = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_view));

		for (const auto& mesh : scene_->meshes())
		{
			const auto& mesh_component_list = mesh->mesh_component_list();
			for (const auto& mesh_component : mesh_component_list)
			{

				XMMATRIX world = XMLoadFloat4x4(&mesh_component->owner()->world_matrix());
				XMMATRIX inverse_world = XMMatrixInverse(&XMMatrixDeterminant(world), world);
				XMMATRIX to_local = inverse_view * inverse_world;

				// 메쉬 로컬좌표의 피킹반직선
				XMVECTOR ray_origin_local = XMVector3TransformCoord(ray_origin, to_local);
				XMVECTOR ray_direction_local = XMVector3Normalize(XMVector3Transform(ray_direction, to_local));

				float t_min{ 0 }; // 반직선 교점 매개변수 최저값, 즉 가장 가까운 교점의 매개변수
				if (mesh->bounds().Intersects(ray_origin_local, ray_direction_local, t_min))
				{
					auto& positions = mesh->positions();
					auto& indices_array = mesh->indices_array();

					t_min = std::numeric_limits<float>::max();
					for (auto& indices : indices_array)
					{
						//TODO: mesh가 triangle list 인지 strip 인지 판정해야함
						float t{ 0 };
						for (int i = 0; i < indices.size(); i += 3)
						{
							UINT i0 = indices[i + 0];
							UINT i1 = indices[i + 1];
							UINT i2 = indices[i + 2];

							XMVECTOR v0 = XMLoadFloat3(&positions[i0]);
							XMVECTOR v1 = XMLoadFloat3(&positions[i1]);
							XMVECTOR v2 = XMLoadFloat3(&positions[i2]);

							if (TriangleTests::Intersects(ray_origin_local, ray_direction_local, v0, v1, v2, t))
							{
								if (t < t_min) // 가장 반직선에 가까운 삼각형과의 교점 매개변수
								{
									t_min = t;
								}
							}
						}
					}	//for indices_array
					if (t_min < std::numeric_limits<float>::max()) // t_min 유효한지 실제 메쉬와 부딪힌건지 체크
					{
						// 피킹지점을 월드 좌표계와 비교
						XMVECTOR picking_point = ray_origin_local + (ray_direction_local * t_min);
						picking_point = XMVector3TransformCoord(picking_point, world);
						float length = XMVectorGetX(XMVector3Length(picking_point - ray_origin));
						if (length < picking_length_min)
						{
							picking_length_min = length;
							picking_point_w = picking_point;
						}
					}
				}	//if mesh->bounds().Intersects
			}
		}

		// 3. 1번에서 2번을 향하는 총알 발사
		gun_shoting_point = gun->owner()->world_position_vector();
		XMFLOAT3 bullet_dir{};
		XMStoreFloat3(&bullet_dir, XMVector3Normalize(picking_point_w - XMLoadFloat3(&gun_shoting_point)));
		Mesh* bullet_mesh = Scene::FindMesh("green_cube", scene_->meshes());
		gun->FireBullet(bullet_dir, bullet_mesh);
 		
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

void FPSControllerComponent::set_scene(Scene* value)
{
	scene_ = value;
}

