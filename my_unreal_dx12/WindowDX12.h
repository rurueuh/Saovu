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

/**
 * @class WindowDX12
 * @brief Manages the DirectX 12 window, rendering, and scene.
 * This class integrates the Win32 window with the D3D12 rendering pipeline,
 * handles the main loop, and manages scene objects.
 */
class WindowDX12 {
public:
    /**
     * @brief Constructs a new WindowDX12 object.
     * @param w The width of the window.
     * @param h The height of the window.
     * @param title The title of the window.
     */
    WindowDX12(UINT w, UINT h, const std::wstring& title);

    /**
     * @brief Creates the shader pipelines.
     */
    void CreateShader(void);

    /**
     * @brief Allocates a shader resource view descriptor.
     * @return A pair of CPU and GPU descriptor handles.
     */
    SrvHandlePair AllocateSrv();

    /**
     * @brief Gets the singleton instance of the WindowDX12 class.
     * @return A reference to the singleton instance.
     */
    static WindowDX12& Get() { static WindowDX12 instance(800, 600, L"DX12 Window"); return instance; }

    /**
     * @brief Gets the default white texture.
     * @return A reference to the default texture.
     */
    Texture& getDefaultTexture() { return *m_whitePtr; }

    /**
     * @brief Gets a shared pointer to the default white texture.
     * @return A shared pointer to the default texture.
     */
    std::shared_ptr<Texture> getDefaultTextureShared() { return m_whitePtr; }

    /**
     * @brief Gets a const shared pointer to the default white texture.
     * @return A const shared pointer to the default texture.
     */
    std::shared_ptr<const Texture> getDefaultTextureShared() const { return m_whitePtr; }

    /**
     * @brief Gets the ImGuiDx12 instance.
     * @return A reference to the ImGuiDx12 object.
     */
    ImGuiDx12& getImGui() { return m_imgui; }

    /**
     * @brief Sets the window title.
     * @param title The new title.
     */
    static void setWindowTitle(const std::wstring& title) { Get().m_window.SetTitle(title); }

    /**
     * @brief Sets the window size.
     * @param w The new width.
     * @param h The new height.
     */
    static void setWindowSize(UINT w, UINT h) { Get().m_window.SetSize(w, h); }

    /**
     * @brief Enables or disables wireframe rendering.
     * @param enable True to enable wireframe, false to disable.
     */
    void setWireframe(bool enable) { m_pipeline.setWireframe(enable);}

    /**
     * @brief Checks if the window is still open.
     * @return True if the window is open, false otherwise.
     */
    bool IsOpen() { return m_window.PumpMessages(); }

    /**
     * @brief Clears the back buffer and depth stencil view.
     * @return The number of triangles rendered in the previous frame.
     */
    uint32_t Clear();

    /**
     * @brief Renders the shadow pass.
     * @param meshes The list of meshes to render into the shadow map.
     */
    void RenderShadowPass(const std::vector<Mesh*>& meshes);

    /**
     * @brief Adds a mesh to the draw list for the current frame.
     * @param mesh The mesh to draw.
     */
    void Draw(const Mesh& mesh);

    /**
     * @brief Presents the back buffer to the screen.
     */
    void Display();

    /**
     * @brief Sets the camera's view matrix.
     * @param eye The position of the camera.
     * @param at The point the camera is looking at.
     * @param up The up vector.
     */
    void SetCameraLookAt(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up);

    /**
     * @brief Sets the camera's projection matrix.
     * @param fov The field of view in radians.
     * @param aspect The aspect ratio.
     * @param zn The near clipping plane.
     * @param zf The far clipping plane.
     */
    void SetCameraPerspective(float fov, float aspect, float zn, float zf);

    /**
     * @brief Gets the camera's position.
     * @return The position of the camera.
     */
    DirectX::XMFLOAT3 GetCameraPosition() const {
        return m_camera.getPosition();
    }

    /**
     * @brief Gets the D3D12 device.
     * @return A pointer to the ID3D12Device.
     */
    ID3D12Device* GetDevice() const { return m_gfx.Device(); }

    /**
     * @brief Gets the graphics device.
     * @return A reference to the GraphicsDevice object.
     */
    GraphicsDevice& GetGraphicsDevice() { return m_gfx; }

    /**
     * @brief Activates the console window.
     */
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
    ShaderPipeline  m_alphaPipeline;
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
