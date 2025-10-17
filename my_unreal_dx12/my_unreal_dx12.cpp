//#include <windows.h>
//#include <wrl.h>
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//#include <cstdint>
//#include <cassert>
//#include <chrono>
//#include <iostream>
//#include <io.h>
//#include <fcntl.h>
//
//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "d3dcompiler.lib")
//
//using Microsoft::WRL::ComPtr;
//using namespace DirectX;
//
//static const uint32_t kFrameCount = 2;
//static uint32_t gWidth = 1280, gHeight = 720;
//
//inline void ThrowIfFailed(HRESULT hr) { if (FAILED(hr)) { assert(false); ExitProcess((UINT)hr); } }
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    switch (msg)
//    {
//        case WM_DESTROY:
//            PostQuitMessage(0);
//            return 0;
//        case WM_KEYDOWN:
//            if (wParam == VK_ESCAPE)
//                DestroyWindow(hWnd);
//            return 0;
//        case WM_SIZE:
//            return 0;
//        default:
//            return DefWindowProc(hWnd, msg, wParam, lParam);
//    }
//}
//
//// Alignement CBV (256)
//static UINT Align256(UINT size) { return (size + 255) & ~255u; }
//
//struct Dx12App
//{
//    // Core
//    ComPtr<IDXGIFactory7> factory;
//    ComPtr<ID3D12Device> device;
//    ComPtr<ID3D12CommandQueue> queue;
//    ComPtr<IDXGISwapChain3> swapchain;
//    UINT frameIndex = 0;
//
//    // Descriptors / Back buffers
//    ComPtr<ID3D12DescriptorHeap> rtvHeap;
//    UINT rtvStride = 0;
//    ComPtr<ID3D12Resource> backBuffers[kFrameCount];
//
//    // Depth
//    ComPtr<ID3D12DescriptorHeap> dsvHeap;
//    ComPtr<ID3D12Resource> depth;
//
//    // Commands
//    ComPtr<ID3D12CommandAllocator> cmdAlloc[kFrameCount];
//    ComPtr<ID3D12GraphicsCommandList> cmdList;
//
//    // Sync
//    ComPtr<ID3D12Fence> fence;
//    UINT64 fenceValue = 0;
//    HANDLE fenceEvent = nullptr;
//
//    // Pipeline
//    ComPtr<ID3D12RootSignature> rootSig;
//    ComPtr<ID3D12PipelineState> pso;
//    D3D12_VIEWPORT viewport{};
//    D3D12_RECT scissor{};
//
//    // Geometry
//    ComPtr<ID3D12Resource> vertexBuffer;
//    D3D12_VERTEX_BUFFER_VIEW vbView{};
//    ComPtr<ID3D12Resource> indexBuffer;
//    D3D12_INDEX_BUFFER_VIEW ibView{};
//
//    // Constant buffer (MVP)
//    ComPtr<ID3D12Resource> constantBuffer; // upload heap, persistently mapped
//    UINT cbSizeAligned = 0;
//    uint8_t* cbMapped = nullptr;
//
//    // Timing
//    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
//
//    // API
//    void Init(HWND hwnd);
//    void CreateSwapchain(HWND hwnd);
//    void CreateRTVs();
//    void CreateDSV();
//    void CreatePipeline();
//    void CreateCubeBuffers();
//    void CreateConstantBuffer();
//    void UpdateConstants();
//    void Render();
//    void WaitGPU();
//    void MoveToNextFrame();
//};
//
//void Dx12App::Init(HWND hwnd)
//{
//#if defined(_DEBUG)
//    if (ComPtr<ID3D12Debug> dbg; SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dbg))))
//        dbg->EnableDebugLayer();
//    UINT flags = DXGI_CREATE_FACTORY_DEBUG;
//#else
//    UINT flags = 0;
//#endif
//    ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory)));
//
//    // Adapter + fallback WARP
//    ComPtr<IDXGIAdapter1> adapter;
//    for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
//        DXGI_ADAPTER_DESC1 d{};
//        adapter->GetDesc1(&d);
//
//        std::wcout << L"GPU détecté : " << d.Description << std::endl;
//
//        if (d.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
//        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
//            break;
//    }
//    if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) {
//        ComPtr<IDXGIAdapter> warp; ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));
//        ThrowIfFailed(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
//    }
//
//    // Queue
//    D3D12_COMMAND_QUEUE_DESC q{}; q.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//    ThrowIfFailed(device->CreateCommandQueue(&q, IID_PPV_ARGS(&queue)));
//
//    CreateSwapchain(hwnd);
//    CreateRTVs();
//    CreateDSV();
//
//    // Command allocs & list
//    for (UINT i = 0; i < kFrameCount; ++i)
//        ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc[i])));
//    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc[frameIndex].Get(), nullptr, IID_PPV_ARGS(&cmdList)));
//    ThrowIfFailed(cmdList->Close());
//
//    // Fence
//    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
//    fenceValue = 0;
//    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//
//    // Viewport / Scissor
//    viewport = { 0.0f, 0.0f, (float)gWidth, (float)gHeight, 0.0f, 1.0f };
//    scissor = { 0, 0, (LONG)gWidth, (LONG)gHeight };
//
//    CreatePipeline();
//    CreateCubeBuffers();
//    CreateConstantBuffer();
//}
//
//void Dx12App::CreateSwapchain(HWND hwnd)
//{
//    DXGI_SWAP_CHAIN_DESC1 sc{};
//    sc.BufferCount = kFrameCount;
//    sc.Width = gWidth; sc.Height = gHeight;
//    sc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//    sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    sc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//    sc.SampleDesc.Count = 1;
//    ComPtr<IDXGISwapChain1> sc1;
//    ThrowIfFailed(factory->CreateSwapChainForHwnd(queue.Get(), hwnd, &sc, nullptr, nullptr, &sc1));
//    ThrowIfFailed(sc1.As(&swapchain));
//    frameIndex = swapchain->GetCurrentBackBufferIndex();
//}
//
//void Dx12App::CreateRTVs()
//{
//    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{}; rtvDesc.NumDescriptors = kFrameCount; rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//    ThrowIfFailed(device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap)));
//    rtvStride = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//
//    auto h = rtvHeap->GetCPUDescriptorHandleForHeapStart();
//    for (UINT i = 0; i < kFrameCount; ++i) {
//        ThrowIfFailed(swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])));
//        device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, h);
//        h.ptr += rtvStride;
//    }
//}
//
//void Dx12App::CreateDSV()
//{
//    D3D12_RESOURCE_DESC tex{};
//    tex.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//    tex.Alignment = 0;
//    tex.Width = gWidth;
//    tex.Height = gHeight;
//    tex.DepthOrArraySize = 1;
//    tex.MipLevels = 1;
//    tex.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//    tex.SampleDesc = { 1,0 };
//    tex.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//    tex.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//
//    D3D12_HEAP_PROPERTIES heap{}; heap.Type = D3D12_HEAP_TYPE_DEFAULT;
//    D3D12_CLEAR_VALUE clear{}; clear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; clear.DepthStencil.Depth = 1.0f;
//
//    ThrowIfFailed(device->CreateCommittedResource(
//        &heap, D3D12_HEAP_FLAG_NONE, &tex,
//        D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear, IID_PPV_ARGS(&depth)));
//
//    D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{}; dsvDesc.NumDescriptors = 1; dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//    ThrowIfFailed(device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvHeap)));
//
//    D3D12_DEPTH_STENCIL_VIEW_DESC dsv{};
//    dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//    device->CreateDepthStencilView(depth.Get(), &dsv, dsvHeap->GetCPUDescriptorHandleForHeapStart());
//}
//
//void Dx12App::CreatePipeline()
//{
//    // Root: 1 CBV (b0) pour la matrice MVP
//    D3D12_ROOT_PARAMETER rp{};
//    rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//    rp.Descriptor.ShaderRegister = 0;
//    rp.Descriptor.RegisterSpace = 0;
//    rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//
//    D3D12_ROOT_SIGNATURE_DESC rs{};
//    rs.NumParameters = 1;
//    rs.pParameters = &rp;
//    rs.NumStaticSamplers = 0;
//    rs.pStaticSamplers = nullptr;
//    rs.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//    ComPtr<ID3DBlob> sig, err;
//    ThrowIfFailed(D3D12SerializeRootSignature(&rs, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err));
//    ThrowIfFailed(device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&rootSig)));
//
//    // Shaders
//    static const char* kVS = R"(
//cbuffer Scene : register(b0) { float4x4 uMVP; }
//struct VSIn  { float3 pos: POSITION; float3 col: COLOR0; };
//struct VSOut { float4 pos: SV_Position; float3 col: COLOR0; };
//VSOut main(VSIn v){
//    VSOut o;
//    o.pos = mul(float4(v.pos, 1.0), uMVP);
//    o.col = v.col;
//    return o;
//})";
//
//    static const char* kPS = R"(
//float4 main(float4 pos: SV_Position, float3 col: COLOR0) : SV_Target {
//    return float4(col,1);
//})";
//
//    ComPtr<ID3DBlob> VS, PS, errs;
//    ThrowIfFailed(D3DCompile(kVS, strlen(kVS), nullptr, nullptr, nullptr, "main", "vs_5_1", 0, 0, &VS, &errs));
//    ThrowIfFailed(D3DCompile(kPS, strlen(kPS), nullptr, nullptr, nullptr, "main", "ps_5_1", 0, 0, &PS, &errs));
//
//    // Input layout
//    D3D12_INPUT_ELEMENT_DESC il[] = {
//        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    };
//
//    // States
//    D3D12_RASTERIZER_DESC rast{};
//    rast.FillMode = D3D12_FILL_MODE_SOLID;
//    rast.CullMode = D3D12_CULL_MODE_FRONT;
//    rast.FrontCounterClockwise = TRUE;
//    rast.DepthClipEnable = TRUE;
//
//    D3D12_BLEND_DESC blend{};
//    blend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//
//    D3D12_DEPTH_STENCIL_DESC ds{};
//    ds.DepthEnable = TRUE;
//    ds.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//    ds.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//    ds.StencilEnable = FALSE;
//
//    // PSO
//    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
//    psoDesc.pRootSignature = rootSig.Get();
//    psoDesc.VS = { VS->GetBufferPointer(), VS->GetBufferSize() };
//    psoDesc.PS = { PS->GetBufferPointer(), PS->GetBufferSize() };
//    psoDesc.InputLayout = { il, _countof(il) };
//    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//    psoDesc.RasterizerState = rast;
//    psoDesc.BlendState = blend;
//    psoDesc.DepthStencilState = ds;
//    psoDesc.NumRenderTargets = 1;
//    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
//    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
//    psoDesc.SampleDesc.Count = 1;
//    psoDesc.SampleMask = UINT_MAX;
//
//    ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));
//}
//
//struct Vertex { float px, py, pz; float r, g, b; };
//
//void Dx12App::CreateCubeBuffers()
//{
//    // Cube 8 sommets (positions + couleurs), indices 36
//    Vertex v[8] = {
//        {-1,-1,-1, 1,0,0}, { -1, 1,-1., 0,1,0}, { 1, 1,-1, 0,0,1}, { 1,-1,-1, 1,1,0}, // back
//        {-1,-1, 1, 1,0,1}, { -1, 1, 1, 0,1,1}, { 1, 1, 1, 1,1,1}, { 1,-1, 1, 0.2f,0.7f,1}
//    };
//    uint16_t idx[36] = {
//        // back (-Z)
//        0,1,2, 0,2,3,
//        // front (+Z)
//        4,6,5, 4,7,6,
//        // left (-X)
//        4,5,1, 4,1,0,
//        // right (+X)
//        3,2,6, 3,6,7,
//        // top (+Y)
//        1,5,6, 1,6,2,
//        // bottom (-Y)
//        4,0,3, 4,3,7
//    };
//
//    // VB (upload pour la démo)
//    D3D12_HEAP_PROPERTIES heapU{}; heapU.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//    D3D12_RESOURCE_DESC vb{};
//    vb.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//    vb.Width = sizeof(v);
//    vb.Height = 1; vb.DepthOrArraySize = 1; vb.MipLevels = 1;
//    vb.SampleDesc = { 1,0 }; vb.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//    ThrowIfFailed(device->CreateCommittedResource(&heapU, D3D12_HEAP_FLAG_NONE, &vb,
//        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer)));
//
//    void* map = nullptr; D3D12_RANGE r{ 0,0 };
//    vertexBuffer->Map(0, &r, &map); memcpy(map, v, sizeof(v)); vertexBuffer->Unmap(0, nullptr);
//
//    vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
//    vbView.StrideInBytes = sizeof(Vertex);
//    vbView.SizeInBytes = sizeof(v);
//
//    // IB
//    D3D12_RESOURCE_DESC ib = vb; ib.Width = sizeof(idx);
//    ThrowIfFailed(device->CreateCommittedResource(&heapU, D3D12_HEAP_FLAG_NONE, &ib,
//        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer)));
//
//    indexBuffer->Map(0, &r, &map); memcpy(map, idx, sizeof(idx)); indexBuffer->Unmap(0, nullptr);
//
//    ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
//    ibView.Format = DXGI_FORMAT_R16_UINT;
//    ibView.SizeInBytes = sizeof(idx);
//}
//
//void Dx12App::CreateConstantBuffer()
//{
//    cbSizeAligned = Align256(sizeof(XMFLOAT4X4));
//    D3D12_HEAP_PROPERTIES heapU{}; heapU.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//    D3D12_RESOURCE_DESC buf{};
//    buf.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//    buf.Width = cbSizeAligned * kFrameCount;
//    buf.Height = 1; buf.DepthOrArraySize = 1; buf.MipLevels = 1;
//    buf.SampleDesc = { 1,0 }; buf.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//    ThrowIfFailed(device->CreateCommittedResource(&heapU, D3D12_HEAP_FLAG_NONE, &buf,
//        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constantBuffer)));
//
//    D3D12_RANGE r{ 0,0 };
//    ThrowIfFailed(constantBuffer->Map(0, &r, reinterpret_cast<void**>(&cbMapped)));
//}
//
//void Dx12App::UpdateConstants()
//{
//    // Temps écoulé
//    float t = std::chrono::duration<float>(std::chrono::steady_clock::now() - t0).count();
//
//    // Matrices : World (rotation), View (look-at), Proj (perspective)
//    XMMATRIX W = XMMatrixRotationY(t) * XMMatrixRotationX(t * 0.5f) * XMMatrixScaling(0.7f, 0.7f, 0.7f);
//    XMVECTOR eye = XMVectorSet(0, 0, -6, 0); // caméra reculée sur -Z (coord. left-handed)
//    XMVECTOR at = XMVectorSet(0, 0, 0, 0);
//    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
//    XMMATRIX V = XMMatrixLookAtLH(eye, at, up);
//    float aspect = (float)gWidth / (float)gHeight;
//    XMMATRIX P = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.5f, 100.0f);
//
//    XMMATRIX MVP = XMMatrixTranspose(W * V * P); // HLSL column-major par défaut → transpose
//
//    XMFLOAT4X4 out;
//    XMStoreFloat4x4(&out, MVP);
//    std::memcpy(cbMapped + cbSizeAligned * frameIndex, &out, sizeof(out));
//}
//
//void Dx12App::Render()
//{
//    UpdateConstants();
//
//    // Reset
//    ThrowIfFailed(cmdAlloc[frameIndex]->Reset());
//    ThrowIfFailed(cmdList->Reset(cmdAlloc[frameIndex].Get(), pso.Get()));
//
//    // Transition PRESENT -> RENDER_TARGET
//    D3D12_RESOURCE_BARRIER b{};
//    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//    b.Transition.pResource = backBuffers[frameIndex].Get();
//    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//    b.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
//    b.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
//    cmdList->ResourceBarrier(1, &b);
//
//    // Handles
//    auto rtvStart = rtvHeap->GetCPUDescriptorHandleForHeapStart();
//    D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvStart; rtv.ptr += frameIndex * rtvStride;
//    D3D12_CPU_DESCRIPTOR_HANDLE dsv = dsvHeap->GetCPUDescriptorHandleForHeapStart();
//
//    // States
//    cmdList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
//    cmdList->RSSetViewports(1, &viewport);
//    cmdList->RSSetScissorRects(1, &scissor);
//
//    // Clear
//    const float clear[4]{ 0.02f, 0.05f, 0.12f, 1.0f };
//    cmdList->ClearRenderTargetView(rtv, clear, 0, nullptr);
//    cmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//
//    // IA + root
//    cmdList->SetGraphicsRootSignature(rootSig.Get());
//    D3D12_GPU_VIRTUAL_ADDRESS cbAddr = constantBuffer->GetGPUVirtualAddress() + cbSizeAligned * frameIndex;
//    cmdList->SetGraphicsRootConstantBufferView(0, cbAddr);
//
//    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    cmdList->IASetVertexBuffers(0, 1, &vbView);
//    cmdList->IASetIndexBuffer(&ibView);
//
//    // Draw
//    cmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);
//
//    // Transition RENDER_TARGET -> PRESENT
//    std::swap(b.Transition.StateBefore, b.Transition.StateAfter);
//    cmdList->ResourceBarrier(1, &b);
//
//    ThrowIfFailed(cmdList->Close());
//    ID3D12CommandList* lists[]{ cmdList.Get() };
//    queue->ExecuteCommandLists(1, lists);
//
//    ThrowIfFailed(swapchain->Present(1, 0));
//    MoveToNextFrame();
//}
//
//void Dx12App::WaitGPU()
//{
//    const UINT64 v = ++fenceValue;
//    ThrowIfFailed(queue->Signal(fence.Get(), v));
//    if (fence->GetCompletedValue() < v) {
//        ThrowIfFailed(fence->SetEventOnCompletion(v, fenceEvent));
//        WaitForSingleObject(fenceEvent, INFINITE);
//    }
//}
//
//void Dx12App::MoveToNextFrame()
//{
//    WaitGPU();
//    frameIndex = swapchain->GetCurrentBackBufferIndex();
//}
//
//int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow)
//{
//    AllocConsole();
//    SetConsoleTitleW(L"Debug Output");
//    FILE* fp;
//    freopen_s(&fp, "CONOUT$", "w", stdout);
//    freopen_s(&fp, "CONOUT$", "w", stderr);
//    // Fenêtre Win32
//    WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
//    wc.lpfnWndProc = WndProc;
//    wc.hInstance = hInst;
//    wc.lpszClassName = L"DX12Wnd";
//    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    RegisterClassEx(&wc);
//
//    RECT rc{ 0,0,(LONG)gWidth,(LONG)gHeight };
//
//    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
//    HWND hwnd = CreateWindow(wc.lpszClassName, L"DX12 Rotating Cube",
//        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
//        rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
//    LONG s = GetWindowLong(hwnd, GWL_STYLE);
//    s &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
//    SetWindowLong(hwnd, GWL_STYLE, s);
//    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
//    ShowWindow(hwnd, nCmdShow);
//    UpdateWindow(hwnd);
//
//    // DX12
//    static Dx12App app;
//    app.Init(hwnd);
//
//    // Boucle
//    MSG msg{};
//    for (;;) {
//        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
//            if (msg.message == WM_QUIT) return (int)msg.wParam;
//            TranslateMessage(&msg); DispatchMessage(&msg);
//        }
//        app.Render();
//    }
//}
