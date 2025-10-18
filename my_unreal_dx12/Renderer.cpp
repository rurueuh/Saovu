#include "Renderer.h"
#include "WindowDX12.h"

void Renderer::DrawMesh(const Mesh& mesh, D3D12_GPU_VIRTUAL_ADDRESS cbAddr)
{
    {
        auto* tex = mesh.GetTexture();
        if (tex == nullptr)
            tex = &WindowDX12::Get().getDefaultTexture();

        ID3D12DescriptorHeap* heaps[] = { const_cast<ID3D12DescriptorHeap*>(tex->SRVHeap()) };
        m_cmd.Get()->SetDescriptorHeaps(1, heaps);
        m_cmd.Get()->SetGraphicsRootDescriptorTable(1, tex->GPUHandle());
    }
    m_cmd.Get()->SetGraphicsRootConstantBufferView(0, cbAddr);
    m_cmd.Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_cmd.Get()->IASetVertexBuffers(0, 1, &mesh.VBV());
    m_cmd.Get()->IASetIndexBuffer(&mesh.IBV());
    m_cmd.Get()->DrawIndexedInstanced(mesh.IndexCount(), 1, 0, 0, 0);
}
