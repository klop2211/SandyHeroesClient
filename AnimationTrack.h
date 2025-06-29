#pragma once

class AnimationSet;
class Object;

enum class AnimationLoopType{kLoop, kOnce, kRepeat};

class AnimationTrack
{
public:
	AnimationTrack(AnimationSet* animation_set);

	bool is_end() const;

	void Start();
	void Start(AnimationLoopType loop_type, int repeat_count = 0); //�ִϸ��̼��� �־��� ���� ������� ���
	void Pause();
	void Stop();

	void PlayTrack(float elapsed_time, std::vector<XMFLOAT4X4>& animated_transforms, float weight);

private:
	//Ʈ���� ����ϴ� �ִϸ��̼� ��
	AnimationSet* animation_set_;

	AnimationLoopType loop_type_ = AnimationLoopType::kLoop;
	int repeat_count_ = 1; //�ݺ� Ƚ�� Repeat ����϶��� �۵�
	int repeat_counter_ = 0;

	bool is_pause_ = false;
	bool is_end_ = false; //�ִϸ��̼� ����� ����Ǹ� true

	float animation_time_ = 0.f;

};

