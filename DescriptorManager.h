#pragma once

// ��ũ���͸� �����ϴ� Ŭ����
// ����� CBV�� ������ ���� �ؽ�ó�� �߰��ϴ� �������� SRV�� ���� ������ �߰��� ����
class DescriptorManager
{
private:
	DescriptorManager(const DescriptorManager& other) = delete;
	DescriptorManager& operator=(const DescriptorManager& rhs) = delete;

public:
	DescriptorManager();
	~DescriptorManager() {}


	//getter
	UINT cbv_bone_transform_offset() const;
	UINT cbv_pass_offset() const;
	UINT srv_offset() const;

	void ResetDescriptorHeap(ID3D12Device* device, int texture_count);

	ID3D12DescriptorHeap* GetDescriptorHeap() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(int index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(int index) const;

	static UINT kCbvSrvUavDescriptorSize;

private:
	static DescriptorManager* kDescriptorManager;

	ComPtr<ID3D12DescriptorHeap> d3d_cbv_srv_uav_heap_ = nullptr;

	// �� ������ۺ��� ������
	UINT cbv_bone_transform_offset_ = 0;
	UINT cbv_pass_offset_ = 0; 

	UINT srv_offset_ = 0;
	

};

