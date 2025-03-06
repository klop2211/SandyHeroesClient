#pragma once

class Object;
class Mesh;
class Material;
class AnimationSet;

class ModelInfo
{
public:
	ModelInfo() {}
	ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials);
	~ModelInfo();

	void LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials);

	Object* LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials);

	void LoadAnimationInfoFromFile(std::ifstream& file);

	Object* GetInstance();

private:
	std::string model_name_ = "None";

	Object* hierarchy_root_ = nullptr;

	//�� �� ������ �𵨿� �´� �ִϸ��̼� �������� ������ ������ �ִ�. 
	//�̰͵��� AnimatorComponent�� ����Ǿ� �۵��Ѵ�.
	std::vector<std::unique_ptr<AnimationSet>> animation_sets_;
	//�� �ִϸ��̼� set�� ����� �ִϸ��̼� ������ ������ �̸� ������. 
	//�̸� ������ �ִϸ��̼��� transform matrix ������ ��ġ�ϱ� ������ �̸� ���� �ִϸ��̼��� �����Ѵ�.
	std::vector<std::string> frame_names_;
	//�ִϸ��̼��� ����Ǵ� ������ ��Ʈ�� �̸�(��Ʈ��� �ִϸ��̼� �� ���ڸ� �ִϸ��̼��� ���� �ʿ���)
	std::string root_bone_name_;
};

