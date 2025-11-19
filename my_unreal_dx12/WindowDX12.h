#pragma once
#include "GraphicsDevice.h"
#include "SwapChain.h"
#include "DepthBuffer.h"
#include "CommandContext.h"
#include "ShaderPipeline.h"
#include "ShadowMap.h"
#include "Renderer.h"
#include "Shaders.h"
#include "Camera.h"
#include "Window.h"
#include "CameraController.h"
#include <chrono>
#include <wrl.h>
#include "ImGuiDx12.h"
#include <fstream>

struct SrvHandlePair {
    D3D12_CPU_DESCRIPTOR_HANDLE cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu;
};

class WindowDX12 {
public:
    WindowDX12(UINT w, UINT h, const std::wstring& title);

    void CreateShader(void);

    SrvHandlePair AllocateSrv();

    static WindowDX12& Get() { static WindowDX12 instance(800, 600, L"DX12 Window"); return instance; }

    Texture& getDefaultTexture() { return *m_whitePtr; }
    std::shared_ptr<Texture> getDefaultTextureShared() { return m_whitePtr; }
    std::shared_ptr<const Texture> getDefaultTextureShared() const { return m_whitePtr; }

    ImGuiDx12& getImGui() { return m_imgui; }

    static void setWindowTitle(const std::wstring& title) { Get().m_window.SetTitle(title); }

    static void setWindowSize(UINT w, UINT h) { Get().m_window.SetSize(w, h); }

    void setWireframe(bool enable) { m_pipeline.setWireframe(enable);}

    bool IsOpen() { return m_window.PumpMessages(); }

    uint32_t Clear();

    void RenderShadowPass(const std::vector<Mesh*>& meshes);


    void Draw(const Mesh& mesh);

    void Display();

    void SetCameraLookAt(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up);

    void SetCameraPerspective(float fov, float aspect, float zn, float zf);

    DirectX::XMFLOAT3 GetCameraPosition() const {
        return m_camera.getPosition();
    }

    ID3D12Device* GetDevice() const { return m_gfx.Device(); }
    GraphicsDevice& GetGraphicsDevice() { return m_gfx; }

    static void ActivateConsole();

private:
    inline static std::shared_ptr<Texture> m_whitePtr;
    static constexpr UINT kMaxDrawsPerFrame = 19000;

    Window          m_window;
    GraphicsDevice  m_gfx;
    SwapChain       m_swap;
    DepthBuffer     m_depth;
    Renderer        m_renderer;
    ShaderPipeline  m_pipeline;
    ShaderPipeline  m_shadowPipeline;
    ShadowMap       m_shadowMap;

    ConstantBuffer  m_cb{};
    UINT            m_drawCursor = 0;

    Camera           m_camera;
    CameraController m_camController;

    ImGuiDx12 m_imgui;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    UINT  m_srvDescriptorSize = 0;
    UINT  m_nextSrvIndex = 0;
    bool m_reloadShadersRequested = false;

    DirectX::XMMATRIX  m_view = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX  m_proj = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT4X4 m_lightViewProj{};
    DirectX::XMFLOAT3   m_lightDir{};

	std::vector<Mesh*> m_DrawList;

    std::chrono::steady_clock::time_point m_t0 = std::chrono::steady_clock::now();
    float dt = 0.0f;
    mutable uint32_t m_trianglesCount = 0;

    void DrawScene();
};
