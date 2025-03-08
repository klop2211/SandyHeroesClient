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

	void EnQueueInputMessage(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time);
	InputMessage DeQueueInputMessage(float play_time);

	bool IsEmpty();
	bool IsFull();

private:
	static InputManager* kInputManager;

	static constexpr UINT kMaxInputMessage = 1000;
	static constexpr float kExpirationDateInputMessage = 2.f; // ��ǲ �޽����� ��ȿ�Ⱓ(����: ��)

	std::array<InputMessage, kMaxInputMessage> message_buffer_;

	//ó���ؾ��� ù��° �޽��� �ε���
	int head_ = 0;
	//�޽����� �߰��Ǵ� ��ġ �ε���
	int tail_ = 0;

};

