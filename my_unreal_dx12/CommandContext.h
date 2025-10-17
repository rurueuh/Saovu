#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Utils.h"

class CommandContext
{
public:
    void Initialize(ID3D12Device* device)
    {
        for (UINT i = 0; i < 2; ++i)
        {
            DXThrow(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_alloc[i])));
        }
        DXThrow(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_alloc[0].Get(), nullptr, IID_PPV_ARGS(&m_list)));
        DXThrow(m_list->Close());
    }

    void Begin(UINT frameIndex)
    {
        DXThrow(m_alloc[frameIndex]->Reset());
        DXThrow(m_list->Reset(m_alloc[frameIndex].Get(), nullptr));
    }

    void End()
    {
        DXThrow(m_list->Close());
    }

    ID3D12GraphicsCommandList* Get() const { return m_list.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_alloc[2];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_list;
};