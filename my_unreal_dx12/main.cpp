// main.cpp — Deux cubes en orbite (DX12, classes du projet)
#include <DirectXMath.h>
#include <vector>
#include <chrono>

#include "Utils.h"
#include "Window.h"
#include "GraphicsDevice.h"
#include "SwapChain.h"
#include "DepthBuffer.h"
#include "CommandContext.h"
#include "ShaderPipeline.h"
#include "ConstantBuffer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Renderer.h"
#include "Shaders.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    // 1) Fenêtre
    Window window;
    if (!window.Create(L"DX12 – Deux cubes en orbite", 1280, 720)) return -1;

    // 2) Device / factory / queue / fence
    GraphicsDevice gfx;
    gfx.Initialize();

    // 3) Swapchain + Depth
    SwapChain swap;
    swap.Create(gfx, window.GetHwnd(), window.GetWidth(), window.GetHeight());

    DepthBuffer depth;
    depth.Create(gfx, window.GetWidth(), window.GetHeight());

    // 4) Renderer
    Renderer renderer;
    renderer.Initialize(gfx, swap, depth);

    // 5) Pipeline (input layout + shaders unicolores)
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    ShaderPipeline pipeline;
    pipeline.Create(gfx.Device(), inputLayout, _countof(inputLayout),
        kVertexShaderSrc, kPixelShaderSrc,
        DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
    renderer.SetPipeline(pipeline);

    // 6) Mesh (cube 8 sommets / 36 indices)
    std::vector<Vertex> vertices = {
        {-1,-1,-1, 1,0,0}, {-1, 1,-1, 0,1,0}, { 1, 1,-1, 0,0,1}, { 1,-1,-1, 1,1,0},
        {-1,-1, 1, 1,0,1}, {-1, 1, 1, 0,1,1}, { 1, 1, 1, 1,1,1}, { 1,-1, 1, 0.2f,0.7f,1},
    };
    std::vector<uint16_t> indices = {
        0,1,2, 0,2,3,  4,6,5, 4,7,6,  4,5,1, 4,1,0,
        3,2,6, 3,6,7,  1,5,6, 1,6,2,  4,0,3, 4,3,7
    };
    Mesh cube;
    cube.Upload(gfx.Device(), vertices, indices);

    // 7) Constant buffer : 2 objets × frames (double-buffering)
    //    On réserve 2 slices par frame, un pour chaque cube.
    ConstantBuffer cbMVP;
    cbMVP.Create(gfx.Device(), kSwapBufferCount * 2);

    // 8) Caméra
    Camera cam;
    float aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
    cam.SetPerspective(DirectX::XM_PIDIV4, aspect, 0.1f, 100.0f);

    auto t0 = std::chrono::steady_clock::now();

    // 9) Boucle principale
    while (window.PumpMessages())
    {
        // Resize (recreate swapchain & depth + update viewport/proj)
        if (window.WasResized())
        {
            gfx.WaitGPU();
            swap.Resize(gfx, window.GetWidth(), window.GetHeight());
            depth.Resize(gfx, window.GetWidth(), window.GetHeight());
            renderer.OnResize(window.GetWidth(), window.GetHeight());
            aspect = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
            cam.SetPerspective(DirectX::XM_PIDIV4, aspect, 0.1f, 1000.0f);
        }

        // Temps
        float t = std::chrono::duration<float>(std::chrono::steady_clock::now() - t0).count();

        using namespace DirectX;

        // Caméra vue/proj
        cam.LookAt(XMVectorSet(0, 0, -20, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
        XMMATRIX V = cam.View();
        XMMATRIX P = cam.Proj();

        // --- Orbite de deux cubes autour de l'origine ---
        float r = 2.0f;                        // rayon d’orbite
        XMMATRIX Rorbit = XMMatrixRotationY(t * 0.8f);

        // Cube A : décalé +r sur X, orbite + spin propre
        XMMATRIX TA = XMMatrixTranslation(r, 0, 0);
        XMMATRIX SpinA = XMMatrixRotationX(t * 0.9f) * XMMatrixRotationY(t * 1.3f);
        XMMATRIX WA = SpinA * Rorbit * TA;

        // Cube B : décalé -r sur X, orbite + spin propre (différente)
        XMMATRIX TB = XMMatrixTranslation(-r, 0, 0);
        XMMATRIX SpinB = XMMatrixRotationY(-t * 1.1f) * XMMatrixRotationZ(t * 0.7f);
        XMMATRIX WB = SpinB * Rorbit * TB;

        // MVP transposées (HLSL par défaut column-major)
        XMFLOAT4X4 mvpA; XMStoreFloat4x4(&mvpA, XMMatrixTranspose(WA * V * P));
        XMFLOAT4X4 mvpB; XMStoreFloat4x4(&mvpB, XMMatrixTranspose(WB * V * P));

        // Rendu
        UINT frame = swap.FrameIndex();
        renderer.BeginFrame(frame);

        // Slice 0 pour cube A, slice 1 pour cube B (par frame)
        auto cbAddrA = cbMVP.UploadSlice(frame * 2 + 0, mvpA);
        renderer.DrawMesh(cube, cbAddrA);

        auto cbAddrB = cbMVP.UploadSlice(frame * 2 + 1, mvpB);
        renderer.DrawMesh(cube, cbAddrB);

        renderer.EndFrame(frame);
    }

    return 0;
}
