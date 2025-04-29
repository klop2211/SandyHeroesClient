#pragma once

class Scene;

struct CutSceneData
{
	XMFLOAT4X4 world_matrix{};
	float key_time{};

	CutSceneData(const XMFLOAT4X4& world, float time) : world_matrix(world), key_time(time) {}
};

//�ƽ��� ��ȭ�ϴ� Ŭ����
// ��ȭ�� ������ ����ϴ� ���� ī�޶��� ��������� ����
class CutSceneRecorder
{
public:
	CutSceneRecorder(const std::string& name);
	~CutSceneRecorder();

	void Start();
	void Pause();
	void Stop();

	void CaptureCameraTransform(float elapsed_time);
	
	void WriteCutScene(const std::string& cut_scene_name);

private:
	Scene* scene_{ nullptr };
	std::string name_{ "None" };

	std::list<CutSceneData> cut_scene_data_list_;
	float key_time_{ 0.f };

	bool is_recording_{ false };

};

