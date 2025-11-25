#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "GraphicsDevice.h"

/**
 * @class Texture
 * @brief Manages a texture resource.
 */
class Texture
{
public:
    Texture() = default;

    /**
     * @brief Loads a texture from a file.
     * @param gd The graphics device.
     * @param path The path to the texture file.
     * @param srvCpu The CPU descriptor handle for the shader resource view.
     * @param srvGpu The GPU descriptor handle for the shader resource view.
     */
    void LoadFromFile(GraphicsDevice& gd,
        const char* path,
        D3D12_CPU_DESCRIPTOR_HANDLE srvCpu,
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpu);

    /**
     * @brief Initializes a 1x1 white texture.
     * @param gd The graphics device.
     * @param srvCpu The CPU descriptor handle for the shader resource view.
     * @param srvGpu The GPU descriptor handle for the shader resource view.
     */
    void InitWhite1x1(GraphicsDevice& gd,
        D3D12_CPU_DESCRIPTOR_HANDLE srvCpu,
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpu);

    /**
     * @brief Gets the texture resource.
     * @return A pointer to the ID3D12Resource.
     */
    ID3D12Resource* Resource() const { return m_tex.Get(); }

    /**
     * @brief Gets the GPU descriptor handle for the shader resource view.
     * @return The D3D12_GPU_DESCRIPTOR_HANDLE.
     */
    D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle() const { return m_srvGPU; }

    /**
     * @brief Gets the CPU descriptor handle for the shader resource view.
     * @return The D3D12_CPU_DESCRIPTOR_HANDLE.
     */
    D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle() const { return m_srvCPU; }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_tex;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_upload;

    D3D12_CPU_DESCRIPTOR_HANDLE m_srvCPU{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_srvGPU{};
};
