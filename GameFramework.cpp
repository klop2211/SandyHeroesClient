#include "stdafx.h"
#include "GameFramework.h"
#include "Timer.h"
#include "Object.h"
#include "Shader.h"
#include "TestScene.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "Mesh.h"
#include "InputManager.h"
#include "InputControllerComponent.h"
#include "ModelInfo.h"
#include "Material.h"
#include "AnimationSet.h"
#include "BaseScene.h"
#include "RecorderScene.h"

GameFramework* GameFramework::kGameFramework = nullptr;


GameFramework::GameFramework() 
{
	assert(kGameFramework == nullptr);
	kGameFramework = this;
}

GameFramework::~GameFramework()
{
    if (d3d_device_)
    {
        FlushCommandQueue();
        //TODO: ��üȭ�� ��忡�� ����� ���� �߻� �ذ�
		if (client_full_screen_state_)
		{
            ChangeWindowMode();
		}
    }
}

void GameFramework::Initialize(HINSTANCE hinstance, HWND hwnd)
{
    app_instance_ = hinstance;
    main_wnd_ = hwnd;

    InitDirect3D();


    d3d_command_list_->Reset(d3d_command_allocator_.Get(), nullptr);

    BuildRootSignature();

    frame_resource_manager_ = std::make_unique<FrameResourceManager>();
    descriptor_manager_ = std::make_unique<DescriptorManager>();
    input_manager_ = std::make_unique<InputManager>();

    //�� ���� �� �ʱ�ȭ
    scene_ = std::make_unique<BaseScene>();
    scene_->Initialize(d3d_device_.Get(), d3d_command_list_.Get(), d3d_root_signature_.Get(), 
        this, d2d_device_context_.Get(), dwrite_factory_.Get());

    OnResize();

    d3d_command_list_->Close();
    ID3D12CommandList* cmd_list[] = { d3d_command_list_.Get() };
    d3d_command_queue_->ExecuteCommandLists(1, cmd_list);

    FlushCommandQueue();

    scene_->ReleaseMeshUploadBuffer();

    client_timer_.reset(new Timer);
    client_timer_->Reset();
}

void GameFramework::InitDirect3D()
{
    UINT dxgi_farctory_flags = 0;

#if defined(DEBUG) | defined(_DEBUG)
    // D3D12 ����� ���̾ Ȱ��ȭ �մϴ�.
    {
        ComPtr<ID3D12Debug> debeg_controller;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debeg_controller))))
        {
            debeg_controller->EnableDebugLayer();

            dxgi_farctory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    CreateDXGIFactory2(dxgi_farctory_flags, IID_PPV_ARGS(&dxgi_factory_));

    HRESULT hardwareResult = D3D12CreateDevice(
        nullptr, // �⺻ ��� ����մϴ�.
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&d3d_device_));

    if (FAILED(hardwareResult))
    {
        ComPtr<IDXGIAdapter> pWrapAdapter;
        dxgi_factory_->EnumWarpAdapter(IID_PPV_ARGS(&pWrapAdapter));

        D3D12CreateDevice(
            pWrapAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&d3d_device_));
    }

    if (!d3d_device_)
    {
        MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
        ::PostQuitMessage(0);
        return;
    }

    d3d_device_->CreateFence(current_fence_value_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d_fence_));


    rtv_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsv_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    cbv_srv_uav_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DescriptorManager::kCbvSrvUavDescriptorSize = cbv_srv_uav_descriptor_size_;

    //4x msaa �˻�
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS quality_levels = {};
    quality_levels.Format = back_buffer_format_;
    quality_levels.SampleCount = 4;
    quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    quality_levels.NumQualityLevels = 0;
    d3d_device_->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &quality_levels,
        sizeof(quality_levels));

    msaa_quality_ = quality_levels.NumQualityLevels;
    assert(msaa_quality_ > 0 && "Unexpected MSAA quality level.");

    CreateCommandObject();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();

    // 11on12
    UINT d3d11_device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2d_factory_options = {};

    ComPtr<ID3D11Device> d3d11Device;
    D3D11On12CreateDevice(
        d3d_device_.Get(),
        d3d11_device_flags,
        nullptr,
        0,
        reinterpret_cast<IUnknown**>(d3d_command_queue_.GetAddressOf()),
        1,
        0,
        &d3d11Device,
        &d3d11_device_context_,
        nullptr
    );

    d3d11Device.As(&d3d11on12_device_);

    // Create D2D/DWrite components.
    {
        D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2d_factory_options, &d2d_factory_);
        ComPtr<IDXGIDevice> dxgiDevice;
        d3d11on12_device_.As(&dxgiDevice);
        d2d_factory_->CreateDevice(dxgiDevice.Get(), &d2d_device_);
        d2d_device_->CreateDeviceContext(deviceOptions, &d2d_device_context_);
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &dwrite_factory_);
    }

    // Query the desktop's dpi settings, which will be used to create
// D2D's render targets.
    float dpiX;
    float dpiY;
#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
    d2d_factory_->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        dpiX,
        dpiY
    );

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(d3d_rtv_heap_->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV, D2D render target, and a command allocator for each frame.
        for (UINT n = 0; n < kSwapChainBufferCount; n++)
        {
            dxgi_swap_chain_->GetBuffer(n, IID_PPV_ARGS(&d3d_swap_chain_buffers_[n]));
            d3d_device_->CreateRenderTargetView(d3d_swap_chain_buffers_[n].Get(), nullptr, rtvHandle);

            //NAME_D3D12_OBJECT_INDEXED(d3d_swap_chain_buffers_, n);

            // Create a wrapped 11On12 resource of this back buffer. Since we are 
            // rendering all D3D12 content first and then all D2D content, we specify 
            // the In resource state as RENDER_TARGET - because D3D12 will have last 
            // used it in this state - and the Out resource state as PRESENT. When 
            // ReleaseWrappedResources() is called on the 11On12 device, the resource 
            // will be transitioned to the PRESENT state.
            D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
            d3d11on12_device_->CreateWrappedResource(
                d3d_swap_chain_buffers_[n].Get(),
                &d3d11Flags,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT,
                IID_PPV_ARGS(&d3d11_wrapped_swap_chain_buffers_[n])
            );

            // Create a render target for D2D to draw directly to this back buffer.
            ComPtr<IDXGISurface> surface;
            d3d11_wrapped_swap_chain_buffers_[n].As(&surface);
            d2d_device_context_->CreateBitmapFromDxgiSurface(
                surface.Get(),
                &bitmapProperties,
                &d2d_render_targets_[n]
            );

            rtvHandle.Offset(1, rtv_descriptor_size_);
        }
    }
}

void GameFramework::CreateCommandObject()
{
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    HRESULT hresult = d3d_device_->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&d3d_command_queue_));

    d3d_device_->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        IID_PPV_ARGS(&d3d_command_allocator_));

    d3d_device_->CreateCommandList(
        0, 
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        d3d_command_allocator_.Get(),
        nullptr, 
        IID_PPV_ARGS(&d3d_command_list_));

    // ������ �ϱ⿡ �ռ��� Ŀ�ǵ� ����Ʈ�� �����ϰ� Ŀ�ǵ带 ����ϴµ�,
    // ������ �ϱ� ���ؼ� Ŀ�ǵ� ����Ʈ�� �����ִ� ���¿��� �մϴ�.
    d3d_command_list_->Close();
}

void GameFramework::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.BufferDesc.Width = client_width_;
    swap_chain_desc.BufferDesc.Height = client_height_;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = client_refresh_rate_;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.BufferDesc.Format = back_buffer_format_;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SampleDesc.Count = msaa_state_ ? 4 : 1;
    swap_chain_desc.SampleDesc.Quality = msaa_state_ ? (msaa_quality_ - 1) : 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = kSwapChainBufferCount;
    swap_chain_desc.OutputWindow = main_wnd_;
    swap_chain_desc.Windowed = !client_full_screen_state_;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hresult = dxgi_factory_->CreateSwapChain(
        d3d_command_queue_.Get(),
        &swap_chain_desc,
        (IDXGISwapChain**)dxgi_swap_chain_.GetAddressOf());

    dxgi_factory_->MakeWindowAssociation(main_wnd_, DXGI_MWA_NO_ALT_ENTER);

    int i = 0;
    ++i;
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.NumDescriptors = kSwapChainBufferCount;
    rtv_heap_desc.NodeMask = 0;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3d_device_->CreateDescriptorHeap(&rtv_heap_desc, 
        IID_PPV_ARGS(d3d_rtv_heap_.GetAddressOf()));

    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.NumDescriptors = kDepthStencilBufferCount;
    dsv_heap_desc.NodeMask = 0;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3d_device_->CreateDescriptorHeap(&dsv_heap_desc,
        IID_PPV_ARGS(d3d_dsv_heap_.GetAddressOf()));

}

void GameFramework::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE descriptor_range[7];
    descriptor_range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //albedo
    descriptor_range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //spec gloss
    descriptor_range[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //metal gloss
    descriptor_range[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3); //emission
    descriptor_range[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4); //normal
    descriptor_range[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5); //cube
    descriptor_range[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6); //shadow

    constexpr int root_parameter_size{ 14 };
    CD3DX12_ROOT_PARAMETER root_parameter[root_parameter_size];

    //25.02.23 ����
    //���� ��Ʈ ��ũ���� ���̺����� ��Ʈ CBV������� ����
    root_parameter[0].InitAsConstantBufferView(0); // world matrix
    root_parameter[1].InitAsConstantBufferView(1); // bone transform
    root_parameter[2].InitAsConstantBufferView(2); // bone offset (default buffer)
    root_parameter[3].InitAsConstantBufferView(3); // ui
    root_parameter[4].InitAsConstantBufferView(4); // render pass
    root_parameter[5].InitAsConstantBufferView(5); // material
    root_parameter[6].InitAsDescriptorTable(1, &descriptor_range[0], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[7].InitAsDescriptorTable(1, &descriptor_range[1], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[8].InitAsDescriptorTable(1, &descriptor_range[2], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[9].InitAsDescriptorTable(1, &descriptor_range[3], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[10].InitAsDescriptorTable(1, &descriptor_range[4], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[11].InitAsDescriptorTable(1, &descriptor_range[5], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[12].InitAsDescriptorTable(1, &descriptor_range[6], D3D12_SHADER_VISIBILITY_PIXEL);
    root_parameter[13].InitAsConstantBufferView(6); // shadow pass

    
    CD3DX12_STATIC_SAMPLER_DESC aniso_warp{ 0 };    //���� ���͸� warp ��� ���÷�
    CD3DX12_STATIC_SAMPLER_DESC linear_warp{ 1, D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT };
    CD3DX12_STATIC_SAMPLER_DESC shadow_sampler
    {   2,
        D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        0.0f,
        16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK
    };

    constexpr int static_sampler_size = 3;
    D3D12_STATIC_SAMPLER_DESC static_sampler[static_sampler_size]{ aniso_warp , linear_warp, shadow_sampler };

    CD3DX12_ROOT_SIGNATURE_DESC root_sig_desc(root_parameter_size, root_parameter, static_sampler_size, static_sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serialized_root_sig = nullptr;
    ComPtr<ID3DBlob> error_blob = nullptr;
    D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, 
        serialized_root_sig.GetAddressOf(), error_blob.GetAddressOf());

    if (error_blob)
    {
        OutputDebugStringA((char*)error_blob->GetBufferPointer());
    }

    d3d_device_->CreateRootSignature(
        0,
        serialized_root_sig->GetBufferPointer(),
        serialized_root_sig->GetBufferSize(),
        IID_PPV_ARGS(d3d_root_signature_.GetAddressOf()));

}


void GameFramework::OnResize()
{
    FlushCommandQueue();

    d3d_command_list_->Reset(d3d_command_allocator_.Get(), nullptr);

    //�ĸ���۸� �ٽ� ����� ���� ���� ���� ����
    for (ComPtr<ID3D12Resource>& swap_chain_buffer : d3d_swap_chain_buffers_)
        swap_chain_buffer.Reset();

    d3d_depth_stencil_buffer_.Reset();
    d3d_shadow_depth_buffer_.Reset();

    //�ĸ���� ũ�� ����
    dxgi_swap_chain_->ResizeBuffers(
        kSwapChainBufferCount, 
        client_width_, client_height_, 
        back_buffer_format_, 
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    current_back_buffer_ = 0;

    //Rtv ����
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle (
        d3d_rtv_heap_->GetCPUDescriptorHandleForHeapStart());
    for (int i = 0; i < kSwapChainBufferCount; ++i)
    {
        //���� ��������
        dxgi_swap_chain_->GetBuffer(i, 
            IID_PPV_ARGS(d3d_swap_chain_buffers_[i].GetAddressOf()));
        //�� ����
        d3d_device_->CreateRenderTargetView(
            d3d_swap_chain_buffers_[i].Get(), nullptr, rtv_heap_handle);

        //�� ������ �̵�
        rtv_heap_handle.Offset(1, rtv_descriptor_size_);
    }


    //Depth-Stencil ���� ����
    D3D12_RESOURCE_DESC ds_buffer_desc = {};
    ds_buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    ds_buffer_desc.Alignment = 0;
    ds_buffer_desc.Width = client_width_;
    ds_buffer_desc.Height = client_height_;
    ds_buffer_desc.DepthOrArraySize = 1;
    ds_buffer_desc.MipLevels = 1;
    ds_buffer_desc.Format = depth_stencil_buffer_format_;
    ds_buffer_desc.SampleDesc.Count = msaa_state_ ? 4 : 1; //msaa�� ����ϸ� 4 �ƴϸ� 1
    ds_buffer_desc.SampleDesc.Quality = msaa_state_ ? (msaa_quality_ - 1) : 0; 
    ds_buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    ds_buffer_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE ds_clear_value = {};
    ds_clear_value.Format = depth_stencil_buffer_format_;
    ds_clear_value.DepthStencil.Depth = 1.f;
    ds_clear_value.DepthStencil.Stencil = 0;

    d3d_device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &ds_buffer_desc,
        D3D12_RESOURCE_STATE_COMMON,
        &ds_clear_value,
        IID_PPV_ARGS(d3d_depth_stencil_buffer_.GetAddressOf()));

    //Dsv ����
    // ���� ds ���۸� ������ �� ������ �ٲ�ų� ���ۿ� ���� 
    // ������ desc�� ������� �ʴٸ� �Ʒ� �ּ��� ����ؼ� dsv�� �����ؾ���
    /*D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = mDepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;*/
    d3d_device_->CreateDepthStencilView(
        d3d_depth_stencil_buffer_.Get(), 
        nullptr,                
        DepthStencilView());

    //�������۸� ��� ���� ���·� ����
    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_depth_stencil_buffer_.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE));

    //Create ShadowDepth Buffer
    {
        D3D12_RESOURCE_DESC shadowDesc = {};
        shadowDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        shadowDesc.Alignment = 0;
        shadowDesc.Width = SHADOW_MAP_SIZE;
        shadowDesc.Height = SHADOW_MAP_SIZE;
        shadowDesc.DepthOrArraySize = 1;
        shadowDesc.MipLevels = 1;
        shadowDesc.Format = depth_stencil_buffer_format_;
        shadowDesc.SampleDesc.Count = msaa_state_ ? 4 : 1;
        shadowDesc.SampleDesc.Quality = msaa_state_ ? (msaa_quality_ - 1) : 0;
        shadowDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        shadowDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE shadowClear = {};
        shadowClear.Format = depth_stencil_buffer_format_;
        shadowClear.DepthStencil.Depth = 1.f;
        shadowClear.DepthStencil.Stencil = 0;

        d3d_device_->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &shadowDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            &shadowClear,
            IID_PPV_ARGS(d3d_shadow_depth_buffer_.GetAddressOf()));

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        srvDesc.Texture2D.PlaneSlice = 0;

        auto srvHandle = descriptor_manager_->GetCpuHandle(descriptor_manager_->texture_count() - 1);

        d3d_device_->CreateShaderResourceView(d3d_shadow_depth_buffer_.Get(), &srvDesc, srvHandle);

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Format = depth_stencil_buffer_format_;
        dsvDesc.Texture2D.MipSlice = 0;

        auto dsvHandle = DepthStencilView();
        dsvHandle.ptr += dsv_descriptor_size_;

        d3d_device_->CreateDepthStencilView(d3d_shadow_depth_buffer_.Get(), &dsvDesc, dsvHandle);
    }

    //���� ����
    d3d_command_list_->Close();
    ID3D12CommandList* command_list[] = { d3d_command_list_.Get() };
    d3d_command_queue_->ExecuteCommandLists(_countof(command_list), command_list);

    FlushCommandQueue();

    //����Ʈ ������Ʈ
    client_viewport_.TopLeftX = 0;
    client_viewport_.TopLeftY = 0;
    client_viewport_.Width = client_width_;
    client_viewport_.Height = client_height_;
    client_viewport_.MaxDepth = 1.f;
    client_viewport_.MinDepth = 0;

    scissor_rect_ = { 0, 0, client_width_, client_height_ };

    {
        shadow_viewport_.TopLeftX = 0;
        shadow_viewport_.TopLeftY = 0;
        shadow_viewport_.Width = SHADOW_MAP_SIZE;
        shadow_viewport_.Height = SHADOW_MAP_SIZE;
        shadow_viewport_.MaxDepth = 1.f;
        shadow_viewport_.MinDepth = 0;

        shadow_scissor_rect_ = { 0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE };
    }
}

void GameFramework::ChangeWindowMode()
{
	client_full_screen_state_ = !client_full_screen_state_;
	dxgi_swap_chain_->SetFullscreenState(client_full_screen_state_, nullptr);

    DXGI_MODE_DESC dxgiTargetParameters;
    dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiTargetParameters.Width = client_width_;
    dxgiTargetParameters.Height = client_height_;
    dxgiTargetParameters.RefreshRate.Numerator = 60;
    dxgiTargetParameters.RefreshRate.Denominator = 1;
    dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    dxgi_swap_chain_->ResizeTarget(&dxgiTargetParameters);

    OnResize();
}

void GameFramework::ProcessInput()
{
    while (!input_manager_->IsEmpty())
    {
        InputMessage message = input_manager_->DeQueueInputMessage(client_timer_->PlayTime());
        ProcessInput(message.id, message.w_param, message.l_param, message.time);
    }
}

void GameFramework::ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)
{
    //���� Scene���� ��ǲ�� ó���ϴ��� Ȯ���Ѵ�
    if (scene_)
    {
        if (scene_->ProcessInput(id, w_param, l_param, time))
            return;
    }
    switch (id)
    {
    case WM_KEYDOWN:
        if (w_param == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        if (w_param == 'P')
        {
            FlushCommandQueue();

            d3d_command_list_->Reset(d3d_command_allocator_.Get(), nullptr);

            scene_ = std::make_unique<BaseScene>();
            scene_->Initialize(d3d_device_.Get(), d3d_command_list_.Get(), d3d_root_signature_.Get(),
                this, d2d_device_context_.Get(), dwrite_factory_.Get());

            d3d_command_list_->Close();
            ID3D12CommandList* cmd_list[] = { d3d_command_list_.Get() };
            d3d_command_queue_->ExecuteCommandLists(1, cmd_list);

            FlushCommandQueue();

            scene_->ReleaseMeshUploadBuffer();

        }
		if (w_param == VK_F9)
		{
			ChangeWindowMode();
		}
        break;
    default:
        break;
    }
}

void GameFramework::FrameAdvance()
{
    client_timer_->Tick();

    //��ǲ ó��
    ProcessInput();

    //scene_->CheckObjectByObjectCollisions();

    //������Ʈ
    scene_->Update(client_timer_->ElapsedTime());
    scene_->UpdateObjectWorldMatrix();


    //����
    auto& command_allocator = frame_resource_manager_->curr_frame_resource()->d3d_allocator;

    command_allocator->Reset();

    d3d_command_list_->Reset(command_allocator.Get(), nullptr);
    d3d_command_list_->SetGraphicsRootSignature(d3d_root_signature_.Get());


    d3d_command_list_->RSSetViewports(1, &shadow_viewport_);
    d3d_command_list_->RSSetScissorRects(1, &shadow_scissor_rect_);

    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_shadow_depth_buffer_.Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            D3D12_RESOURCE_STATE_DEPTH_WRITE));

    auto dsvHandle = DepthStencilView();
    dsvHandle.ptr += dsv_descriptor_size_;

    d3d_command_list_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
    d3d_command_list_->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

    scene_->ShadowRender(d3d_command_list_.Get());

    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_shadow_depth_buffer_.Get(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_GENERIC_READ));

    ID3D12DescriptorHeap* descriptor_heaps[] = { descriptor_manager_->GetDescriptorHeap() };
    d3d_command_list_->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

    auto shadowHandle = descriptor_manager_->GetGpuHandle(descriptor_manager_->texture_count() - 1);
    d3d_command_list_->SetGraphicsRootDescriptorTable((int)RootParameterIndex::kShadowMap, shadowHandle);

    d3d_command_list_->ResourceBarrier(1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            d3d_swap_chain_buffers_[current_back_buffer_].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET));

    d3d_command_list_->RSSetViewports(1, &client_viewport_);
    d3d_command_list_->RSSetScissorRects(1, &scissor_rect_);

    d3d_command_list_->ClearRenderTargetView(
        CurrentBackBufferView(), DirectX::Colors::Purple, 0, nullptr);
    d3d_command_list_->ClearDepthStencilView(
        DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

    const UINT kNumRenderTargetDescriptors = 1; // ��������� ����
    d3d_command_list_->OMSetRenderTargets(
        kNumRenderTargetDescriptors,
        &CurrentBackBufferView(),
        true, &DepthStencilView());

    scene_->Render(d3d_command_list_.Get());
    scene_->ParticleRender(d3d_command_list_.Get());

    d3d_command_list_->Close();
    ID3D12CommandList* command_lists[] = { d3d_command_list_.Get() };
    d3d_command_queue_->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Acquire our wrapped render target resource for the current back buffer.
    d3d11on12_device_->AcquireWrappedResources(d3d11_wrapped_swap_chain_buffers_[current_back_buffer_].GetAddressOf(), 1);

    scene_->RenderText(d2d_render_targets_[current_back_buffer_].Get(), d2d_device_context_.Get());

    // Release our wrapped render target resource. Releasing 
    // transitions the back buffer resource to the state specified
    // as the OutState when the wrapped resource was created.
    d3d11on12_device_->ReleaseWrappedResources(d3d11_wrapped_swap_chain_buffers_[current_back_buffer_].GetAddressOf(), 1);

    // Flush to submit the 11 command list to the shared command queue.
    d3d11_device_context_->Flush();

    dxgi_swap_chain_->Present(0, 0);
    current_back_buffer_ = (current_back_buffer_ + 1) % kSwapChainBufferCount;

    frame_resource_manager_->curr_frame_resource()->fence = ++current_fence_value_;

    d3d_command_queue_->Signal(d3d_fence_.Get(), current_fence_value_);

    frame_resource_manager_->CirculateFrameResource(d3d_fence_.Get());

    std::wstring fps{ L"SandyHeroes(" };
    fps += std::to_wstring(client_timer_->Fps()) + L"fps)";
    SetWindowText(main_wnd_, fps.c_str());
}

void GameFramework::FlushCommandQueue()
{
    ++current_fence_value_;

    d3d_command_queue_->Signal(d3d_fence_.Get(), current_fence_value_);

    if (d3d_fence_->GetCompletedValue() < current_fence_value_)
    {
        HANDLE event_handle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        d3d_fence_->SetEventOnCompletion(current_fence_value_, event_handle);

        WaitForSingleObject(event_handle, INFINITE);
        CloseHandle(event_handle);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE GameFramework::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        d3d_rtv_heap_->GetCPUDescriptorHandleForHeapStart(), 
        current_back_buffer_,
        rtv_descriptor_size_);
}

D3D12_CPU_DESCRIPTOR_HANDLE GameFramework::DepthStencilView() const
{
    return d3d_dsv_heap_->GetCPUDescriptorHandleForHeapStart();
}

LRESULT GameFramework::ProcessWindowMessage(HWND h_wnd, UINT message_id, WPARAM w_param, LPARAM l_param)
{
    switch (message_id)
    {
    case WM_MOUSEWHEEL:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN:
    case WM_KEYUP:
        input_manager_->EnQueueInputMessage(message_id, w_param, l_param, client_timer_->PlayTime());
        break;
    default:
        break;
    }
    return 0;
}

FrameResourceManager* GameFramework::frame_resource_manager() const
{
    return frame_resource_manager_.get();
}

DescriptorManager* GameFramework::descriptor_manager() const
{
    return descriptor_manager_.get();
}

HWND GameFramework::main_wnd() const
{
    return main_wnd_;
}

XMFLOAT2 GameFramework::client_size() const
{
    return XMFLOAT2{ (float)client_width_, (float)client_height_ };
}
