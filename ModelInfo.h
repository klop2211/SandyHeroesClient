#pragma once
#include "AnimationSet.h"

class Object;
class Mesh;
class Material;
struct Texture;

class ModelInfo
{
public:
	ModelInfo() {}
	ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures);
	~ModelInfo();

	std::string model_name() const;

	void LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures);

	Object* LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures);

	void LoadAnimationInfoFromFile(std::ifstream& file);

	Object* GetInstance() const;

	Object* hierarchy_root() const;

	void set_hierarchy_root(Object* root);
	void set_model_name(const std::string& value) { model_name_ = value; }

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

