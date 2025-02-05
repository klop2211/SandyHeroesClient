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
}

Component* TestControllerComponent::GetCopy()
{
	return new TestControllerComponent(*this);
}

void TestControllerComponent::ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	switch (message_id)
	{
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
				is_key_down_['D'] = true;
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(1, 0, 0));
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
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(0, 0, -1));
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
				owner_->set_velocity(owner_->velocity() + XMFLOAT3(-1, 0, 0));
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
