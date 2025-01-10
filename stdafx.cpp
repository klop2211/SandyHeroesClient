#include "stdafx.h"

ComPtr<ID3D12Resource> d3d_util::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* command_list,
	const void* init_data,
	UINT64 byte_size,
	ComPtr<ID3D12Resource>& upload_buffer)
{
	ComPtr<ID3D12Resource> default_buffer;

	// ����Ʈ ���� ����
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(default_buffer.GetAddressOf()));

	// ����Ʈ ���۷� �����ϱ� ���� ���ε� ���� ����
	device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(upload_buffer.GetAddressOf()));

	// ����Ʈ ���۷� ������ ������ ����
	D3D12_SUBRESOURCE_DATA subresource_data;
	subresource_data.pData = init_data;
	subresource_data.RowPitch = byte_size;
	subresource_data.SlicePitch = subresource_data.RowPitch;

	// ����Ʈ ���۷� ������ ����
	command_list->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			default_buffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources(command_list, 
		default_buffer.Get(), upload_buffer.Get(), 
		0, 0, 1, &subresource_data);
	command_list->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			default_buffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ));

	return default_buffer;
}