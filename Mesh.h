#pragma once

class MeshComponent;
class FrameResourceManager;
class DescriptorManager;

// 메쉬 정보를 가지는 클래스
class Mesh
{
public:
	// 메쉬 정보는 복사될 이유가 없음
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void AddMeshComponent(MeshComponent* mesh_component);

	void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);

	virtual void Render(ID3D12GraphicsCommandList* command_list,
		FrameResourceManager* frame_resource_manager, DescriptorManager* descriptor_manager);

	//getter
	int shader_type() const;

	//업데이트 되어야 할 cb의 인덱스를 갖는 변수 Scene 렌더시 0으로 초기화 해야한다.
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

	//대부분의 메쉬는 1개의 인덱스 버퍼를 사용하지만 
	//일부 메쉬의 경우 정점을 공유한채로 인덱스 버퍼가 여러개인 경우가 있음
	std::vector<std::vector<UINT>> indices_array_;
	std::vector<ComPtr<ID3D12Resource>> d3d_index_buffers_;
	std::vector<ComPtr<ID3D12Resource>> d3d_index_upload_buffers_;
	std::vector<D3D12_INDEX_BUFFER_VIEW> index_buffer_views_;

	std::string name_ = "None";

	// 이 메쉬를 참조중인 컴포넌트 리스트
	std::list<MeshComponent*> mesh_component_list_;

	// 메쉬가 사용하는 쉐이더의 타입 Shader.h 참고
	int shader_type_ = 0;
};

