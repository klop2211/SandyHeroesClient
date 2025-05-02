#pragma once

// Ŭ���̾�Ʈ���� �ð��� �����ϴ� Ŭ����
// ��ü ����ð�, �Ͻ�����, �����Ӱ� �ð�(elapsed time)�� �ð��� ������ ����� ����
class Timer
{
public:
	Timer();

	float TotalTime() const;	// ��ü ����ð�
	float PlayTime() const;		// �Ͻ������� �ð��� ������ ����ð�
	float ElapsedTime() const;	// ���� ������ �ð����� ���� ������ �ð��� �� ���� �ð�

	int Fps();

	void Reset();		// Ÿ�̸� �ʱ�ȭ
	void Start();		// Ÿ�̸� ����
	void Stop();		// Ÿ�̸� �Ͻ�����
	void Tick();		// elapsed_time ����

private:
	double seconds_per_count_ = 0;
	double elapsed_time_ = -1;

	static constexpr int kFpsBufferSize{ 60 };
	std::array<double, kFpsBufferSize> fps_buffer_{};
	UINT fps_buffer_index_{ 0 };

	__int64 base_time_ = 0;		// ����ð�
	__int64 paused_time_ = 0;	// ������ �ð�
	__int64 stop_time_ = 0;		// ������ ������ �ð�
	__int64 prev_time_ = 0;		// ���� ������ �ð�
	__int64 curr_time_ = 0;		// ���� �ð�

	bool is_stop_ = false;
};

