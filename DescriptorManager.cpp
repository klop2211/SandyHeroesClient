#include "stdafx.h"
#include "DescriptorManager.h"

UINT DescriptorManager::kCbvSrvUavDescriptorSize = 0;
DescriptorManager* DescriptorManager::kDescriptorManager = nullptr;

DescriptorManager::DescriptorManager()
{
	assert(!kDescriptorManager);
	kDescriptorManager = this;
}

UINT DescriptorManager::cbv_bone_transform_offset() const
{
	return cbv_bone_transform_offset_;
}

UINT DescriptorManager::cbv_pass_offset() const
{
	return cbv_pass_offset_;
}

void DescriptorManager::ResetDescriptorHeap(ID3D12Device* device, int object_count,
	int skinned_mesh_object_count, int frame_resource_count)
{
	d3d_cbv_heap_.Reset();

	// �� ������ ���ҽ����� �� ������Ʈ�� CBV�� �ʿ� + �����н� 1��
	UINT num_descriptors = (object_count + skinned_mesh_object_count + 1) * frame_resource_count;

	cbv_bone_transform_offset_ = object_count * frame_resource_count;
	cbv_pass_offset_ = (object_count + skinned_mesh_object_count) * frame_resource_count;

	D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc{};
	cbv_heap_desc.NumDescriptors = num_descriptors;
	cbv_heap_desc.NodeMask = 0;
	cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	device->CreateDescriptorHeap(&cbv_heap_desc, IID_PPV_ARGS(d3d_cbv_heap_.GetAddressOf()));

}

ID3D12DescriptorHeap* DescriptorManager::GetDescriptorHeap() const
{
	return d3d_cbv_heap_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorManager::GetCpuHandle(int index) const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(d3d_cbv_heap_->GetCPUDescriptorHandleForHeapStart()).Offset(index, kCbvSrvUavDescriptorSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorManager::GetGpuHandle(int index) const
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(d3d_cbv_heap_->GetGPUDescriptorHandleForHeapStart()).Offset(index, kCbvSrvUavDescriptorSize);
}
