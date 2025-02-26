#pragma once

class Object;
class Mesh;
class Material;

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

	Object* GetInstance();

private:
	std::string model_name_ = "None";

	Object* hierarchy_root_ = nullptr;

	//TODO: �ִϸ��̼� ���� �ε� ���� Ŭ���� �߰�

};

