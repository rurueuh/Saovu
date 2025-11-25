#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <DirectXMath.h>
#include "Utils.h"
#include <memory>
#include <cstring>

/**
 * @struct SceneCB
 * @brief Represents the constant buffer data for the scene.
 * This struct must be 16-byte aligned.
 */
struct alignas(16) SceneCB
{
    DirectX::XMFLOAT4X4 uModel;
    DirectX::XMFLOAT4X4 uViewProj;
    DirectX::XMFLOAT4X4 uNormalMatrix;

    DirectX::XMFLOAT3 uCameraPos;
    float uShininess;

    DirectX::XMFLOAT4X4 uLightViewProj;

    DirectX::XMFLOAT3 uLightDir;
    float _pad0;

    DirectX::XMFLOAT3 uKs;
    float uOpacity;

    DirectX::XMFLOAT3 uKe;
    float _pad1;
};

static_assert(sizeof(SceneCB) % 16 == 0, "SceneCB must be 16-byte aligned");

/**
 * @class ConstantBuffer
 * @brief Manages a constant buffer resource.
 * This class handles the creation, mapping, and uploading of constant buffer data.
 */
class ConstantBuffer
{
public:
    /**
     * @brief Creates the constant buffer.
     * @param device The D3D12 device.
     * @param sliceCount The number of slices in the buffer.
     */
    void Create(ID3D12Device* device, UINT sliceCount)
    {
        m_sliceSize = Align256(sizeof(SceneCB));
        m_totalSize = m_sliceSize * sliceCount;

        D3D12_HEAP_PROPERTIES heap{};
        heap.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC buf{};
        buf.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        buf.Width = m_totalSize;
        buf.Height = 1;
        buf.DepthOrArraySize = 1;
        buf.MipLevels = 1;
        buf.SampleDesc = { 1, 0 };
        buf.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        DXThrow(device->CreateCommittedResource(
            &heap, D3D12_HEAP_FLAG_NONE,
            &buf, D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr, IID_PPV_ARGS(&m_resource)));

        D3D12_RANGE r{ 0, 0 };
        DXThrow(m_resource->Map(0, &r, reinterpret_cast<void**>(&m_mapped)));
    }

    /**
     * @brief Uploads a slice of data to the constant buffer.
     * @param sliceIndex The index of the slice to upload.
     * @param data The data to upload.
     * @return The GPU virtual address of the uploaded slice.
     */
    D3D12_GPU_VIRTUAL_ADDRESS UploadSlice(UINT sliceIndex, const SceneCB& data)
    {
        std::memcpy(m_mapped + sliceIndex * m_sliceSize, &data, sizeof(data));
        return m_resource->GetGPUVirtualAddress() + sliceIndex * m_sliceSize;
    }

    /**
     * @brief Gets the size of a single slice in the constant buffer.
     * @return The size of a slice in bytes.
     */
    UINT SliceSize() const { return m_sliceSize; }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
    UINT m_sliceSize = 0;
    UINT m_totalSize = 0;
    uint8_t* m_mapped = nullptr;
};
