#include "stdafx.h"
#include "TestScene.h"
#include "Object.h"
#include "ColorShader.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "Mesh.h"

void TestScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
	ID3D12RootSignature* root_signature, FrameResourceManager* frame_resource_manager,
	DescriptorManager* descriptor_manager)
{
	frame_resource_manager_ = frame_resource_manager;
	descriptor_manager_ = descriptor_manager;

	BuildShader(device, root_signature);
	BuildObject(device, command_list);

	BuildFrameResources(device);
	BuildDescriptorHeap(device);
}

void TestScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	//씬에서 사용하는 쉐이더 개수만큼 예약
	int shader_count = 1;
	shaders_.reserve(shader_count);

	for (int i = 0; i < shader_count; ++i)
	{
		shaders_.push_back(std::make_unique<ColorShader>());
		shaders_[i]->CreateShader(device, root_signature);
	}
	
}

void TestScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
}

void TestScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	cb_object_capacity_ = 1000;
	cb_skinned_mesh_object_capacity_ = 1000;

}

void TestScene::BuildFrameResources(ID3D12Device* device)
{
	frame_resource_manager_->ResetFrameResources(device, 1,
		cb_object_capacity_, 
		cb_skinned_mesh_object_capacity_);
}

void TestScene::BuildDescriptorHeap(ID3D12Device* device)
{
	descriptor_manager_->ResetDescriptorHeap(device, 
		cb_object_capacity_, 
		cb_skinned_mesh_object_capacity_,
		FrameResourceManager::kFrameCount);
}

void TestScene::BuildConstantBufferViews(ID3D12Device* device)
{
	UINT object_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBObject));

	UINT bone_transform_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBBoneTransform));
	UINT bone_transform_offset = descriptor_manager_->cbv_bone_transform_offset();

	UINT pass_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBPass));

	//각 프레임리소스별로 cbv가 필요하다
	for (int frame_index = 0; frame_index < FrameResourceManager::kFrameCount; ++frame_index)
	{
		//프레임리소스에서 원하는 상수버퍼의 리소스를 가져온다.
		ID3D12Resource* object_cb =
			frame_resource_manager_->GetResource(frame_index)->cb_object->Resource();

		D3D12_GPU_VIRTUAL_ADDRESS cb_adress = object_cb->GetGPUVirtualAddress();
		//모든 오브젝트에 대해 cbv가 필요하다.
		for (UINT i = 0; i < cb_object_capacity_; ++i)
		{
			// i번째 오브젝트의 상수버퍼 어드레스
			cb_adress += object_cb_size;

			// 디스크립터 힙 오프셋
			int heap_index = cb_object_capacity_ * frame_index + i;
			D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
			cbv_desc.BufferLocation = cb_adress;
			cbv_desc.SizeInBytes = object_cb_size;

			device->CreateConstantBufferView(&cbv_desc, handle);
		}

		// 위 오브젝트와 동일한 방법으로 만들어준다.
		ID3D12Resource* bone_transform_cb =
			frame_resource_manager_->GetResource(frame_index)->cb_bone_transform->Resource();

		cb_adress = bone_transform_cb->GetGPUVirtualAddress();
		for (UINT i = 0; i < cb_skinned_mesh_object_capacity_; ++i)
		{
			cb_adress += bone_transform_cb_size;

			int heap_index = descriptor_manager_->cbv_bone_transform_offset() +
				cb_skinned_mesh_object_capacity_ * frame_index + i;
			D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
			cbv_desc.BufferLocation = cb_adress;
			cbv_desc.SizeInBytes = bone_transform_cb_size;

			device->CreateConstantBufferView(&cbv_desc, handle);
		}

		ID3D12Resource* pass_cb =
			frame_resource_manager_->GetResource(frame_index)->cb_pass->Resource();

		cb_adress = pass_cb->GetGPUVirtualAddress();

		int heap_index = descriptor_manager_->cbv_pass_offset() + frame_index;
		D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
		cbv_desc.BufferLocation = cb_adress;
		cbv_desc.SizeInBytes = pass_cb_size;

		device->CreateConstantBufferView(&cbv_desc, handle);

	}

}

void TestScene::Render(ID3D12GraphicsCommandList* command_list)
{
	//TODO: 향후 스킨메쉬 클래스 추가시 kCBSkinnedMeshObjectCurrentIndex에 대한 초기화 필요
	Mesh::kCBObjectCurrentIndex = 0;

	// 단순한 배치 처리 
	// 씬에서 사용하는 쉐이더가 n개이면 SetPipelineState가 n번 호출된다
	for (const std::unique_ptr<Shader>& shader : shaders_)
	{
		command_list->SetPipelineState(shader->GetPipelineState());

		for (const std::unique_ptr<Mesh>& mesh : meshes_)
		{
			if (mesh->shader_type() == (int)shader->shader_type())
			{
				mesh->Render(command_list, frame_resource_manager_, descriptor_manager_);
			}
		}

	}
}
