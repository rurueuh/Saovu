#include "MeshAsset.h"
#include "WindowDX12.h"
#include "Utils.h"

void MeshAsset::Upload(ID3D12Device* device) {
    if (!device) device = WindowDX12::Get().GetDevice();

    const UINT vbBytes = UINT(vertices.size() * sizeof(Vertex));
    const UINT ibBytes = UINT(indices.size() * sizeof(uint16_t));

    auto makeBuf = [&](Microsoft::WRL::ComPtr<ID3D12Resource>& res, UINT bytes) {
        if (res && res->GetDesc().Width >= bytes) return;
        D3D12_HEAP_PROPERTIES hp{}; hp.Type = D3D12_HEAP_TYPE_UPLOAD;
        D3D12_RESOURCE_DESC rd{}; rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        rd.Width = bytes ? bytes : 1; rd.Height = 1; rd.DepthOrArraySize = 1;
        rd.MipLevels = 1; rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; rd.SampleDesc = { 1,0 };
        DXThrow(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&res)));
        };

    makeBuf(vb, vbBytes);
    makeBuf(ib, ibBytes);

    if (vbBytes) {
        void* p = nullptr; D3D12_RANGE r{ 0,0 };
        vb->Map(0, &r, &p);
        memcpy(p, vertices.data(), vbBytes);
        D3D12_RANGE w{ 0, vbBytes }; vb->Unmap(0, &w);
    }
    if (ibBytes) {
        void* p = nullptr; D3D12_RANGE r{ 0,0 };
        ib->Map(0, &r, &p);
        memcpy(p, indices.data(), ibBytes);
        D3D12_RANGE w{ 0, ibBytes }; ib->Unmap(0, &w);
    }

    vbv.BufferLocation = vb->GetGPUVirtualAddress();
    vbv.StrideInBytes = sizeof(Vertex);
    vbv.SizeInBytes = vbBytes;

    ibv.BufferLocation = ib->GetGPUVirtualAddress();
    ibv.Format = DXGI_FORMAT_R16_UINT;
    ibv.SizeInBytes = ibBytes;

    indexCount = UINT(indices.size());
}