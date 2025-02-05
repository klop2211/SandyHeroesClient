#include "stdafx.h"
#include "InputManager.h"
#include "InputControllerComponent.h"

InputManager* InputManager::kInputManager = nullptr;

InputManager::InputManager()
{
	assert(!kInputManager);
	kInputManager = nullptr;
}

void InputManager::AddInputMessage(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time)
{
	//추가하더라도 아직 버퍼에 공간이 남았는지 체크
	assert((tail_ + 1) % kMaxInputMessage != head_);

	message_buffer_[tail_].id = message_id;
	message_buffer_[tail_].w_param = w_param;
	message_buffer_[tail_].l_param = l_param;
	message_buffer_[tail_].time = message_time;
	tail_ = (tail_ + 1) % kMaxInputMessage;
}

void InputManager::Update()
{
	if (!main_controller_ || head_ == tail_)
		return;
	// 누적된 인풋 메시지 처리
	for (int i = head_; i != tail_; i = (i + 1) % kMaxInputMessage)
	{
		const InputMessage& m = message_buffer_[i];
		main_controller_->ProcessInput(m.id, m.w_param, m.l_param, m.time);
	}
	head_ = tail_;
}

void InputManager::set_main_controller(InputControllerComponent* controller)
{
	main_controller_ = controller;
}
