#pragma once

class Timer;

class GameFramework
{
public:
	GameFramework(HINSTANCE hinstance, HWND hwnd);
	~GameFramework();

	void Initialize();

	void InitDirect3D();
	void CreateCommandObject();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	void OnResize();

	void FrameAdvance();

	// gpu, cpu ����ȭ
	void FlushCommandQueue(); 

	//���� �ĸ���� ���� CPU�ڵ��� ����
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

private:
	static GameFramework* kGameFramework;

	static const UINT kInitFenceValue = 0;			// �潺 �ʱⰪ
	static const UINT kSwapChainBufferCount = 2;	// ����ü�� ���� ��
	static const UINT kDepthStencilBufferCount = 1;

	HINSTANCE app_instance_ = nullptr;	// ���ø����̼� �ν��Ͻ� �ڵ�
	HWND main_wnd_ = nullptr;			// ���� ������ �ڵ�

	ComPtr<IDXGIFactory4> dxgi_factory_;

	ComPtr<ID3D12Device> d3d_device_;
	ComPtr<ID3D12Fence> d3d_fence_;
	UINT64 current_fence_value_ = kInitFenceValue;

	ComPtr<ID3D12CommandQueue> d3d_command_queue_;
	ComPtr<ID3D12CommandAllocator> d3d_command_allocator_;
	ComPtr<ID3D12GraphicsCommandList> d3d_command_list_;

	ComPtr<IDXGISwapChain3> dxgi_swap_chain_;
	std::array<ComPtr<ID3D12Resource>, kSwapChainBufferCount> d3d_swap_chain_buffers_;

	ComPtr<ID3D12DescriptorHeap> d3d_rtv_heap_;
	ComPtr<ID3D12DescriptorHeap> d3d_dsv_heap_;

	ComPtr<ID3D12Resource> d3d_depth_stencil_buffer_;

	UINT rtv_descriptor_size_;
	UINT dsv_descriptor_size_;
	UINT cbv_srv_uav_descriptor_size_;

	UINT msaa_quality_; // 4x msaa ����Ƽ ����
	bool msaa_state_ = false; // 4x msaa�� ����ϴ��� ����

	DXGI_FORMAT back_buffer_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT current_back_buffer_ = 0; // ���� �ĸ������ �ε���

	DXGI_FORMAT depth_stencil_buffer_format_ = DXGI_FORMAT_D24_UNORM_S8_UINT;

	int client_width_ = kDefaultFrmaeBufferWidth;
	int client_height_ = kDefaultFrameBufferHeight;
	int client_refresh_rate_ = kDefaultRefreshRate;

	D3D12_VIEWPORT client_viewport_;

	D3D12_RECT scissor_rect_;

	std::unique_ptr<Timer> client_timer_;

};

