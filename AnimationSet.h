#pragma once

class Object;

class AnimationSet
{
public:
	AnimationSet() {}
	~AnimationSet() {}

	float total_time() const;

	//frame_count: �� �ִϸ��̼��� ����ϴ� ���� root ��带 ������ ������ ����
	void LoadAnimationSetFromFile(std::ifstream& file, int frame_count); 

	void AnimateBoneFrame(const std::vector<Object*>& bone_frames, float animation_time);

private:
	std::string name_;

	float total_time_; 	//�ִϸ��̼� �ð�(����: ��)
	int frame_per_sec_;
	int key_frame_count_; //�ִϸ��̼� Ű ������ ����

	std::vector<float> key_frame_times_; //�ִϸ��̼��� �� Ű �������� �ð�(����: ��)
	std::vector<std::vector<XMFLOAT4X4>> key_frame_transforms_; //�� Ű �����ӿ��� transform ��ĵ�

};

