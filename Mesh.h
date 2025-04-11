#pragma once

class MeshComponent;
class FrameResourceManager;
struct FrameResource;
class DescriptorManager;
class Material;

// �޽� ������ ������ Ŭ����
class Mesh
{
public:
	Mesh() {}
	virtual ~Mesh() {}
	// �޽� ������ ����� ������ ����
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void AddMeshComponent(MeshComponent* mesh_component);
	void DeleteMeshComponent(MeshComponent* mesh_component);

	void AddMaterial(Material* material);

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);

	virtual void ReleaseUploadBuffer();

	//�� �޽��� ����ϴ� object�� ������ŭ cb�� ������Ʈ
	virtual void UpdateConstantBuffer(FrameResource* curr_frame_resource);

	virtual void Render(ID3D12GraphicsCommandList* command_list,
		FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager);

	void LoadMeshFromFile(std::ifstream& file);

	static void ResetCBObjectCurrentIndex();

	//getter
	int shader_type() const;
	std::string name() const;
	const std::vector<std::vector<UINT>>& GetIndicesArray() const;
	std::vector<XMFLOAT3> GetPositions() const;

	//setter
	void set_shader_type(int value);
	void set_name(const std::string& name);
protected:
	std::vector<Material*> materials_;

	D3D12_PRIMITIVE_TOPOLOGY primitive_topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<XMFLOAT3> positions_;
	ComPtr<ID3D12Resource> d3d_position_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_position_upload_buffer_ = nullptr;

	std::vector<XMFLOAT4> colors_;
	ComPtr<ID3D12Resource> d3d_color_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_color_upload_buffer_ = nullptr;

	std::vector<XMFLOAT2> uvs_;
	ComPtr<ID3D12Resource> d3d_uv_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_uv_upload_buffer_ = nullptr;

	std::vector<XMFLOAT3> normals_;
	ComPtr<ID3D12Resource> d3d_normal_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_normal_upload_buffer_ = nullptr;

	std::vector<XMFLOAT3> tangents_;
	ComPtr<ID3D12Resource> d3d_tangent_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_tangent_upload_buffer_ = nullptr;

	std::vector<XMFLOAT3> bi_tangents_;
	ComPtr<ID3D12Resource> d3d_bi_tangent_buffer_ = nullptr;
	ComPtr<ID3D12Resource> d3d_bi_tangent_upload_buffer_ = nullptr;

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

private:
	//������Ʈ �Ǿ�� �� cb�� �ε����� ���� ���� Scene ������ 0���� �ʱ�ȭ �ؾ��Ѵ�. (Reset�Լ���)
	static int kCBObjectCurrentIndex;
};

