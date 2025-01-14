#pragma once

struct FrameResource;

//다중 프레임을 위한 프레임리소스를 관리하는 클래스
//싱글톤으로 사용한다.
class FrameResourceManager
{
private:
	FrameResourceManager() {}
	FrameResourceManager(const FrameResourceManager& other) = delete;
	FrameResourceManager& operator=(const FrameResourceManager& rhs) = delete;
	~FrameResourceManager() {}

public:
	static FrameResourceManager& GetInstance()
	{
		static FrameResourceManager instance;
		return instance;
	}

	// 프레임리소스들을 초기화한다.
	void ResetFrameResources(ID3D12Device* device, UINT cb_pass_count, 
		UINT cb_object_count, UINT cb_skinned_mesh_object_count);

private:
	//다중프레임을 위한 리소스들을 담은 벡터
	std::vector<std::unique_ptr<FrameResource>> frame_resources_;

	//다중프레임 개수
	static const int kFrameCount = 3;

	//오브젝트 상수 버퍼 최대 용량
	static const int kCBObjectCapacity = 1000;

	//스킨 메쉬를 사용하는 오브젝트 상수 버퍼 최대 용량
	static const int kCBSkinnedMeshObjectCapacity = 1000;

	//현재 프레임 리소스
	FrameResource* curr_frame_resource_ = nullptr;

	//현재 프레임리소스의 인덱스
	int curr_frame_resource_index_ = 0;

};

