#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Utils.h"

/**
 * @class CommandContext
 * @brief Manages a command list and command allocators.
 * This class simplifies the process of recording and submitting command lists.
 */
class CommandContext
{
public:
    /**
     * @brief Initializes the command context.
     * @param device The D3D12 device.
     */
    void Initialize(ID3D12Device* device)
    {
        for (UINT i = 0; i < 2; ++i)
        {
            DXThrow(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_alloc[i])));
        }
        DXThrow(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_alloc[0].Get(), nullptr, IID_PPV_ARGS(&m_list)));
        DXThrow(m_list->Close());
    }

    /**
     * @brief Begins a new command list.
     * @param frameIndex The current frame index.
     */
    void Begin(UINT frameIndex)
    {
        DXThrow(m_alloc[frameIndex]->Reset());
        DXThrow(m_list->Reset(m_alloc[frameIndex].Get(), nullptr));
    }

    /**
     * @brief Ends the command list.
     */
    void End()
    {
        DXThrow(m_list->Close());
    }

    /**
     * @brief Gets the underlying command list.
     * @return A pointer to the ID3D12GraphicsCommandList.
     */
    ID3D12GraphicsCommandList* Get() const { return m_list.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_alloc[2];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_list;
};