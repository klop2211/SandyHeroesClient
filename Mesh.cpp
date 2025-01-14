#include "stdafx.h"
#include "Mesh.h"
#include "MeshComponent.h"

void Mesh::AddMeshComponent(MeshComponent* mesh_component)
{
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

void Mesh::Render(ID3D12GraphicsCommandList* command_list)
{
	command_list->IASetPrimitiveTopology(primitive_topology_);

	//정점 버퍼 set
	command_list->IASetVertexBuffers(0,
		vertex_buffer_views_.size(), vertex_buffer_views_.data());

	if (indices_array_.size())
	{
		for (MeshComponent* mesh_component : mesh_component_list_)
		{
			//메쉬 컴포넌트를 이용하여 월드행렬 set
			mesh_component->UpdateShaderVariables(command_list);

			for (int i = 0; i < indices_array_.size(); ++i)
			{
				command_list->IASetIndexBuffer(&index_buffer_views_[i]);
				command_list->DrawIndexedInstanced(indices_array_[i].size(), 1, 0, 0, 0);
			}
		}
	}
	else
	{
		for (MeshComponent* mesh_component : mesh_component_list_)
		{
			//메쉬 컴포넌트를 이용하여 월드행렬 set
			mesh_component->UpdateShaderVariables(command_list);

			command_list->DrawInstanced(positions_.size(), 1, 0, 0);
		}
	}

}
