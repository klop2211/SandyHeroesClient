#pragma once

class InputControllerComponent;

struct InputMessage
{
	UINT id;
	WPARAM w_param;
	LPARAM l_param;
	float time;
};

//인풋 메시지를 관리하는 클래스
class InputManager
{
public:
	InputManager();
	~InputManager() {}

	void AddInputMessage(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time);

	void Update();

	void set_main_controller(InputControllerComponent* controller);

private:
	static InputManager* kInputManager;

	static const UINT kMaxInputMessage = 1000;

	std::array<InputMessage, kMaxInputMessage> message_buffer_;

	//처리해야할 첫번째 메시지 인덱스
	int head_ = 0;
	//메시지가 추가되는 위치 인덱스
	int tail_ = 0;

	// 현재 사용중인 컨트롤러
	InputControllerComponent* main_controller_ = nullptr;

};

