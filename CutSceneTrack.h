#pragma once
#include "CutSceneRecorder.h"

//�ƽ��� �����ϰ� ����ϴ� Ŭ����
class CutSceneTrack
{
public:
	CutSceneTrack() = default;
	CutSceneTrack(const std::string& cut_scene_name);
	~CutSceneTrack() = default;

	void LoadCutSceneFromFile(const std::string& cut_scene_name);

	void Play(Scene* scene);
	void Pause();
	void Stop();

	void Run(float elapsed_time);

	void set_camera(Object* value);

	Object* camera() const;

private:
	std::string name_{};
	bool is_playing_{ false };
	float track_position_{};	// ���� Ʈ�� ��� �ð�
	std::vector<CutSceneData> cut_scene_data_buffer_{};

	Object* camera_;
	Object* scene_camera_;	//�ƾ� ����� ���� ī�޶�

	Scene* scene_{};		//�ƾ� ������� ��
};

