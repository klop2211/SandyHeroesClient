#include "stdafx.h"
#include "SkinnedMesh.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "MeshComponent.h"
#include "SkinnedMeshComponent.h"

int SkinnedMesh::kCBSkinnedMeshObjectCurrentIndex = 0;

void SkinnedMesh::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	Mesh::CreateShaderVariables(device, command_list);

	if (bone_offsets_.size())
	{
		d3d_bone_offset_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			bone_offsets_.data(), sizeof(XMFLOAT4X4) * bone_offsets_.size(),
			d3d_bone_offset_upload_buffer_);
	}

	if (bone_indices_.size())
	{
		d3d_bone_index_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			bone_indices_.data(), sizeof(XMINT4) * bone_indices_.size(),
			d3d_bone_index_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_bone_index_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMINT4) * bone_indices_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMINT4);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}

	if (bone_weights_.size())
	{
		d3d_bone_weight_buffer_ = d3d_util::CreateDefaultBuffer(
			device, command_list,
			bone_weights_.data(), sizeof(XMFLOAT4) * bone_weights_.size(),
			d3d_bone_weight_upload_buffer_);

		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = d3d_bone_weight_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view.SizeInBytes = sizeof(XMFLOAT4) * bone_weights_.size();
		vertex_buffer_view.StrideInBytes = sizeof(XMFLOAT4);

		vertex_buffer_views_.push_back(vertex_buffer_view);
	}
}

void SkinnedMesh::UpdateConstantBuffer(FrameResource* curr_frame_resource)
{
	//�޽� ������Ʈ�� Ȱ���Ͽ� ������Ʈ CB�� ������Ʈ�Ѵ�.
	for (MeshComponent* mesh_component : mesh_component_list_)
	{
		// �׸� �ʿ� ���� ��� ���ؼ��� ������Ʈ�� �� �ʿ� ����
		if (!mesh_component->IsVisible())
			continue;

		//��Ų�޽��� ������Ʈ�� bone frame�� ����Ǿ����� üũ �� ����
		SkinnedMeshComponent* skinned_mesh_component =
			static_cast<SkinnedMeshComponent*>(mesh_component);
		skinned_mesh_component->AttachBoneFrames(bone_names_);

		mesh_component->UpdateConstantBuffer(curr_frame_resource, kCBSkinnedMeshObjectCurrentIndex);

		kCBSkinnedMeshObjectCurrentIndex++;
	}
}

void SkinnedMesh::Render(ID3D12GraphicsCommandList* command_list,
	FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager)
{
	command_list->SetGraphicsRootConstantBufferView(
		(int)CBShaderRegisterNum::kBoneOffset, d3d_bone_offset_buffer_->GetGPUVirtualAddress());

	FrameResource* curr_frame_resource = frame_resource_manager->curr_frame_resource();
	UINT cb_bone_transform_size = d3d_util::CalculateConstantBufferSize(sizeof(CBBoneTransform));

	//�� �޽��� ����ϴ� ������Ʈ�� CB ���� �ε����� �����Ѵ�. 
	int cb_object_start_index = kCBSkinnedMeshObjectCurrentIndex;

	UpdateConstantBuffer(curr_frame_resource);

	command_list->IASetPrimitiveTopology(primitive_topology_);

	//���� ���� set
	command_list->IASetVertexBuffers(0,
		vertex_buffer_views_.size(), vertex_buffer_views_.data());

	if (indices_array_.size())
	{
		for (int i = 0; i < indices_array_.size(); ++i)
		{
			if (indices_array_[i].size())
			{
				command_list->IASetIndexBuffer(&index_buffer_views_[i]);
				for (int object_index = cb_object_start_index; object_index < kCBSkinnedMeshObjectCurrentIndex; ++object_index)
				{
					//25.02.23 ����
					//���� ��Ʈ ��ũ���� ���̺��� ��Ʈ CBV�� ����
					D3D12_GPU_VIRTUAL_ADDRESS cb_bone_transform_address =
						curr_frame_resource->cb_bone_transform.get()->Resource()->GetGPUVirtualAddress() + cb_bone_transform_size * object_index;

					command_list->SetGraphicsRootConstantBufferView((int)CBShaderRegisterNum::kBoneTransform, cb_bone_transform_address);
					command_list->DrawIndexedInstanced(indices_array_[i].size(), 1, 0, 0, 0);
				}
			}
			else
			{
				for (int object_index = cb_object_start_index; object_index < kCBSkinnedMeshObjectCurrentIndex; ++object_index)
				{
					//25.02.23 ����
					//���� ��Ʈ ��ũ���� ���̺��� ��Ʈ CBV�� ����
					D3D12_GPU_VIRTUAL_ADDRESS cb_bone_transform_address =
						curr_frame_resource->cb_bone_transform.get()->Resource()->GetGPUVirtualAddress() + cb_bone_transform_size * object_index;

					command_list->SetGraphicsRootConstantBufferView((int)CBShaderRegisterNum::kBoneTransform, cb_bone_transform_address);
					command_list->DrawInstanced(positions_.size(), 1, 0, 0);
				}
			}
		}
	}
	else
	{
		for (int object_index = cb_object_start_index; object_index < kCBSkinnedMeshObjectCurrentIndex; ++object_index)
		{				
			//25.02.23 ����
			//���� ��Ʈ ��ũ���� ���̺��� ��Ʈ CBV�� ����
			D3D12_GPU_VIRTUAL_ADDRESS cb_bone_transform_address =
				curr_frame_resource->cb_bone_transform.get()->Resource()->GetGPUVirtualAddress() + cb_bone_transform_size * object_index;

			command_list->SetGraphicsRootConstantBufferView((int)CBShaderRegisterNum::kBoneTransform, cb_bone_transform_address);
			command_list->DrawInstanced(positions_.size(), 1, 0, 0);
		}
	}
}

using namespace file_load_util;
void SkinnedMesh::LoadSkinnedMeshFromFile(std::ifstream& file)
{
	std::string load_token;

	ReadStringFromFile(file, load_token);
	name_ = load_token;

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BonesPerVertex>:");
#endif // _DEBUG
	bones_per_vertex_ = ReadFromFile<int>(file);

	// bone name ����
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneNames>:");
#endif // _DEBUG

	int bone_count = ReadFromFile<int>(file);
	bone_names_.reserve(bone_count);
	for (int i = 0; i < bone_count; ++i)
	{
		ReadStringFromFile(file, load_token);
		bone_names_.emplace_back(load_token);
	}

	// bone offset ����
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneOffsets>:");
#endif // _DEBUG

	bone_offsets_.resize(ReadFromFile<int>(file));
	ReadFromFile<XMFLOAT4X4>(file, bone_offsets_.data(), bone_offsets_.size());
	//�� �켱 ��ķ� ��ȯ
	for (XMFLOAT4X4& bone_offset : bone_offsets_)
	{
		XMStoreFloat4x4(&bone_offset, XMMatrixTranspose(XMLoadFloat4x4(&bone_offset)));
	}

	// bone index ����
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneIndices>:");
#endif // _DEBUG

	bone_indices_.resize(ReadFromFile<int>(file));
	ReadFromFile<XMINT4>(file, bone_indices_.data(), bone_indices_.size());

	// bone weight ����
	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<BoneWeights>:");
#endif // _DEBUG

	bone_weights_.resize(ReadFromFile<int>(file));
	ReadFromFile<XMFLOAT4>(file, bone_weights_.data(), bone_weights_.size());

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "</SkinningInfo>");
#endif // _DEBUG

	ReadStringFromFile(file, load_token);
#ifdef _DEBUG
	PrintDebugStringLoadTokenError(name_, load_token, "<Mesh>:");
#endif // _DEBUG

	LoadMeshFromFile(file);
}

void SkinnedMesh::ResetCBSkinnedMeshObjectCurrentIndex()
{
	kCBSkinnedMeshObjectCurrentIndex = 0;
}

