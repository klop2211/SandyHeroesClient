#pragma once
#include "Component.h"

class Scene;
class ModelInfo;

// ������Ʈ�� ���� ����� �߰��ϴ� ������Ʈ
// owner_�� ��ġ�� ������ ������Ʈ���� ���� ������Ʈ�� �����Ѵ�.
class SpawnerComponent :
    public Component
{
public:
	SpawnerComponent(Object* owner, Scene* scene);
	SpawnerComponent(Object* owner, Scene* scene, ModelInfo* model_info);

	SpawnerComponent(const SpawnerComponent& other);
	SpawnerComponent(const SpawnerComponent&& other);

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	// ����Ƚ��, ���� ���ð�, ���� ��Ÿ���� �����ϴ� �Լ�
	void SetSpawnerInfo(int spawn_count, float spawn_time, float spawn_cool_time);

	void ActivateSpawn(); // ���� ����

	void AddComponent(std::unique_ptr<Component> component);
	void AddComponent(Component* component);

	void ForceSpawn(); // ������ ���� ������Ʈ�� �����ϴ� �Լ� ���� ������ ��Ÿ���� �����ϰ� ������ �����Ѵ�.

private:
	std::list<std::unique_ptr<Component>> component_list_; // ������ ������Ʈ�� ������Ʈ ����Ʈ

	ModelInfo* model_info_{ nullptr }; // ������ �� ����

	Scene* scene_{ nullptr }; // ������ ��

	int spawn_count_{ 0 }; // ���� Ƚ��
	float spawn_time_{ 0.f }; // ���� �ʱ� ��� �ð�
	float spawn_cool_time_{ 0.f }; // ������ ��Ÿ��
	bool is_spawn_{ false }; // ���� ���� ����
};

