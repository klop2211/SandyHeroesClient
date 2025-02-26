#pragma once

#include "FrameResource.h"

//���� �������� ���� �����Ӹ��ҽ��� �����ϴ� Ŭ����
//�̱������� ����Ѵ�.
class FrameResourceManager
{
private:
	FrameResourceManager(const FrameResourceManager& other) = delete;
	FrameResourceManager& operator=(const FrameResourceManager& rhs) = delete;

public:
	FrameResourceManager();
	~FrameResourceManager() {}

	//getter
	FrameResource* curr_frame_resource() const;
	int curr_frame_resource_index() const;
	int pass_count() const;
	int object_count() const;
	int skinned_mesh_object_count() const;

	FrameResource* GetResource(int index) const;

	// �����Ӹ��ҽ����� �ʱ�ȭ�Ѵ�.
	void ResetFrameResources(ID3D12Device* device, UINT cb_pass_count, 
		UINT cb_object_count, UINT cb_skinned_mesh_object_count, UINT cb_material_count);

	//�����Ӹ��ҵ���� ��ȯ ��Ų�� (��, ���� �����Ӹ��ҽ��� ���� ������Ʈ ����)
	void CirculateFrameResource(ID3D12Fence* fence);

	//���������� ����
	static const int kFrameCount = 3;

private:
	static FrameResourceManager* kFrameResourceManager;

	//������������ ���� ���ҽ����� ���� ����
	std::vector<std::unique_ptr<FrameResource>> frame_resources_;

	//���� ������ ���ҽ�
	FrameResource* curr_frame_resource_ = nullptr;

	//���� �����Ӹ��ҽ��� �ε���
	int curr_frame_resource_index_ = 0;

	int pass_count_ = 0;
	int object_count_ = 0;
	int skinned_mesh_object_count_ = 0;
	int material_count_ = 0;

};

