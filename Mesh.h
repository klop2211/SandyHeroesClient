#pragma once

class MeshComponent;
class FrameResourceManager;
class DescriptorManager;

// �޽� ������ ������ Ŭ����
class Mesh
{
public:
	// �޽� ������ ����� ������ ����
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void AddMeshComponent(MeshComponent* mesh_component);

	void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);

	virtual void Render(ID3D12GraphicsCommandList* command_list,
		FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager);

	//getter
	int shader_type() const;

	//������Ʈ �Ǿ�� �� cb�� �ε����� ���� ���� Scene ������ 0���� �ʱ�ȭ �ؾ��Ѵ�.
	static int kCBObjectCurrentIndex;

protected:

	D3D12_PRIMITIVE_TOPOLOGY primitive_topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<XMFLOAT3> positions_;
	ComPtr<ID3D12Resource> d3d_position_buffer_;
	ComPtr<ID3D12Resource> d3d_position_upload_buffer_;

	std::vector<XMFLOAT4> colors_;
	ComPtr<ID3D12Resource> d3d_color_buffer_;
	ComPtr<ID3D12Resource> d3d_color_upload_buffer_;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffer_views_;

	//��κ��� �޽��� 1���� �ε��� ���۸� ��������� 
	//�Ϻ� �޽��� ��� ������ ������ä�� �ε��� ���۰� �������� ��찡 ����
	std::vector<std::vector<UINT>> indices_array_;
	std::vector<ComPtr<ID3D12Resource>> d3d_index_buffers_;
	std::vector<ComPtr<ID3D12Resource>> d3d_index_upload_buffers_;
	std::vector<D3D12_INDEX_BUFFER_VIEW> index_buffer_views_;

	std::string name_ = "None";

	// �� �޽��� �������� ������Ʈ ����Ʈ
	std::list<MeshComponent*> mesh_component_list_;

	// �޽��� ����ϴ� ���̴��� Ÿ�� Shader.h ����
	int shader_type_ = 0;
};

