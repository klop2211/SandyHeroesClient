#include "stdafx.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"

int Mesh::kCBObjectCurrentIndex = 0;

void Mesh::AddMeshComponent(MeshComponent* mesh_component)
{
	mesh_component_list_.push_back(mesh_component);
}

void Mesh::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	if (positions_.size())
	{
		d3d_position_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			positions_.data(), sizeof(XMFLOAT3) * positions_.size(),
			d3d_position_upload_buffer_);


		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_position_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT3) * positions_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT3);
		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (colors_.size())
	{
		d3d_color_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			colors_.data(), sizeof(XMFLOAT4) * colors_.size(),
			d3d_color_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_color_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT4) * colors_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT4);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* command_list, 
	FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager)
{
	FrameResource* curr_frame_resource = frame_resource_manager->curr_frame_resource();
	int curr_frame_resource_index = frame_resource_manager->curr_frame_resource_index();
	int cb_object_count = frame_resource_manager->object_count();

	//�� �޽��� ����ϴ� ������Ʈ�� CB ���� �ε����� �����Ѵ�.
	int cb_object_start_index = kCBObjectCurrentIndex;

	//�޽� ������Ʈ�� Ȱ���Ͽ� ������Ʈ CB�� ������Ʈ�Ѵ�.
	for (MeshComponent* mesh_component : mesh_component_list_)
	{
		// �׸� �ʿ� ���� ��� ���ؼ��� ������Ʈ�� �� �ʿ� ����
		if (!mesh_component->IsVisible())
			continue;
		CBObject object_buffer{};
		XMStoreFloat4x4(&object_buffer.world_matrix,
			XMMatrixTranspose(XMLoadFloat4x4(&mesh_component->GetOwnerWorld())));

		UploadBuffer<CBObject>* object_cb = curr_frame_resource->cb_object.get();
		object_cb->CopyData(kCBObjectCurrentIndex, object_buffer);

		kCBObjectCurrentIndex++;
	}

	command_list->IASetPrimitiveTopology(primitive_topology_);

	//���� ���� set
	command_list->IASetVertexBuffers(0,
		vertex_buffer_views_.size(), vertex_buffer_views_.data());

	if (indices_array_.size())
	{
		for (int i = 0; i < indices_array_.size(); ++i)
		{
			command_list->IASetIndexBuffer(&index_buffer_views_[i]);
			for (int object_index = cb_object_start_index; object_index < kCBObjectCurrentIndex; ++object_index)
			{		
				// �� ������ ���ҽ��� �ش��ϴ� ������Ʈ�� CBV�� �������� ����մϴ�.
				UINT cbv_index = curr_frame_resource_index * cb_object_count + object_index;
				D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = descriptor_manager->GetGpuHandle(cbv_index);

				//TODO: ��Ʈ�ñ׳�ó �ۼ��� �ε��� 0�� enum class �� ����� �����ϱ�
				command_list->SetGraphicsRootDescriptorTable(0, gpu_handle);
				command_list->DrawIndexedInstanced(indices_array_[i].size(), 1, 0, 0, 0);
			}
		}
	}
	else
	{
		for (int object_index = cb_object_start_index; object_index < kCBObjectCurrentIndex; ++object_index)
		{
			// �� ������ ���ҽ��� �ش��ϴ� ������Ʈ�� CBV�� �������� ����մϴ�.
			UINT cbv_index = curr_frame_resource_index * cb_object_count + object_index;
			D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = descriptor_manager->GetGpuHandle(cbv_index);

			//TODO: ��Ʈ�ñ׳�ó �ۼ��� �ε��� 0�� enum class �� ����� �����ϱ�
			command_list->SetGraphicsRootDescriptorTable(0, gpu_handle);
			command_list->DrawInstanced(positions_.size(), 1, 0, 0);
		}
	}

}

int Mesh::shader_type() const
{
	return shader_type_;
}
