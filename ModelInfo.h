#pragma once

class Object;
class Mesh;

class ModelInfo
{
public:
	ModelInfo() {}
	ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes);
	~ModelInfo();

	void LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes);

	Object* LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes);

	Object* GetInstance();

private:
	std::string model_name_ = "None";

	Object* hierarchy_root_ = nullptr;

	//TODO: 애니메이션 정보 로드 관련 클래스 추가

};

