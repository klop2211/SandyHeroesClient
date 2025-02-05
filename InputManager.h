#pragma once

class InputControllerComponent;

struct InputMessage
{
	UINT id;
	WPARAM w_param;
	LPARAM l_param;
	float time;
};

//��ǲ �޽����� �����ϴ� Ŭ����
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

	//ó���ؾ��� ù��° �޽��� �ε���
	int head_ = 0;
	//�޽����� �߰��Ǵ� ��ġ �ε���
	int tail_ = 0;

	// ���� ������� ��Ʈ�ѷ�
	InputControllerComponent* main_controller_ = nullptr;

};

