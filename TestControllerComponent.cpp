#include "stdafx.h"
#include "Object.h"
#include "TestControllerComponent.h"

TestControllerComponent::TestControllerComponent(Object* owner)
	: InputControllerComponent(owner)
{
	is_key_down_['W'] = false;
	is_key_down_['A'] = false;
	is_key_down_['S'] = false;
	is_key_down_['D'] = false;
	is_key_down_['Q'] = false;
	is_key_down_['E'] = false;
	is_key_down_[VK_LBUTTON] = false;

}

Component* TestControllerComponent::GetCopy()
{
	return new TestControllerComponent(*this);
}

void TestControllerComponent::ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	switch (message_id)
	{
	case WM_MOUSEMOVE:
		if (is_key_down_[VK_LBUTTON])
		{
			int x = LOWORD(l_param), y = HIWORD(l_param);
			owner_->Rotate((y - mouse_xy_.y) * 0.1, (x - mouse_xy_.x) * 0.1, 0.f);
			mouse_xy_.x = x;
			mouse_xy_.y = y;
		}
		break;
	case WM_LBUTTONDOWN:
		if (!is_key_down_[VK_LBUTTON])
		{
			is_key_down_[VK_LBUTTON] = true;
			mouse_xy_.x = LOWORD(l_param);
			mouse_xy_.y = HIWORD(l_param);
		}
		break;
	case WM_LBUTTONUP:
		if (is_key_down_[VK_LBUTTON])
		{
			is_key_down_[VK_LBUTTON] = false;
		}
		break;
	case WM_KEYDOWN:
		switch (w_param)
		{
		case 'W':
			if (!is_key_down_['W'])
			{
				is_key_down_['W'] = true;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 0, 1));
					
			}
			break;
		case 'A':
			if (!is_key_down_['A'])
			{
				is_key_down_['A'] = true;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(-1, 0, 0));
			}
			break;
		case 'S':
			if (!is_key_down_['S'])
			{
				is_key_down_['S'] = true;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 0, -1));
			}
			break;
		case 'D':
			if (!is_key_down_['D'])
			{
				owner_->set_velocity(owner_->velocity() - XMFLOAT3(0, 0, 1));
			}
			break;
		case 'Q':
			if (!is_key_down_['Q'])
			{
				is_key_down_['Q'] = true;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, -1, 0));
			}
			break;
		case 'E':
			if (!is_key_down_['E'])
			{
				is_key_down_['E'] = true;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 1, 0));
			}
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (w_param)
		{
		case 'W':
			if (is_key_down_['W'])
			{
				is_key_down_['W'] = false;
				owner_->set_velocity(owner_->velocity() - XMFLOAT3(0, 0, 1));
			}
			break;
		case 'A':
			if (is_key_down_['A'])
			{
				is_key_down_['A'] = false;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(1, 0, 0));
			}
			break;
		case 'S':
			if (is_key_down_['S'])
			{
				is_key_down_['S'] = false;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 0, 1));
			}
			break;
		case 'D':
			if (is_key_down_['D'])
			{
				is_key_down_['D'] = false;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 0, 1));
			}
			break;
		case 'Q':
			if (is_key_down_['Q'])
			{
				is_key_down_['Q'] = false;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 1, 0));
			}
			break;
		case 'E':
			if (is_key_down_['E'])
			{
				is_key_down_['E'] = false;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, -1, 0));
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
