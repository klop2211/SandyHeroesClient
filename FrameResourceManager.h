#pragma once

struct FrameResource;

//���� �������� ���� �����Ӹ��ҽ��� �����ϴ� Ŭ����
//�̱������� ����Ѵ�.
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

	// �����Ӹ��ҽ����� �ʱ�ȭ�Ѵ�.
	void ResetFrameResources(ID3D12Device* device, UINT cb_pass_count, 
		UINT cb_object_count, UINT cb_skinned_mesh_object_count);

private:
	//������������ ���� ���ҽ����� ���� ����
	std::vector<std::unique_ptr<FrameResource>> frame_resources_;

	//���������� ����
	static const int kFrameCount = 3;

	//������Ʈ ��� ���� �ִ� �뷮
	static const int kCBObjectCapacity = 1000;

	//��Ų �޽��� ����ϴ� ������Ʈ ��� ���� �ִ� �뷮
	static const int kCBSkinnedMeshObjectCapacity = 1000;

	//���� ������ ���ҽ�
	FrameResource* curr_frame_resource_ = nullptr;

	//���� �����Ӹ��ҽ��� �ε���
	int curr_frame_resource_index_ = 0;

};

