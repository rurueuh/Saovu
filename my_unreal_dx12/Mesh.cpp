#include "Mesh.h"
#include "WindowDX12.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices) {
    m_asset = std::make_shared<MeshAsset>();
    m_asset->vertices = vertices;
    m_asset->indices = indices;
    m_asset->texture = ResourceCache::I().defaultWhite();
    m_asset->Upload(WindowDX12::Get().GetDevice());
}

Mesh::Mesh(const std::string& filename) {
    m_asset = ResourceCache::I().getMeshFromOBJ(filename);
    if (!m_asset->texture) m_asset->texture = ResourceCache::I().defaultWhite();
}

void Mesh::setColor(float r, float g, float b) {
    for (auto& v : m_asset->vertices) { v.r = r; v.g = g; v.b = b; }
    m_asset->Upload(nullptr);
}
std::tuple<float, float, float> Mesh::getColor() const {
    if (m_asset->vertices.empty()) return { 1.f,1.f,1.f };
    const auto& v = m_asset->vertices[0];
    return { v.r, v.g, v.b };
}

void Mesh::SetPosition(float x, float y, float z) { m_transform.r[3] = DirectX::XMVectorSet(x, y, z, 1.0f); }
void Mesh::AddPosition(float dx, float dy, float dz) {
    m_transform.r[3] = DirectX::XMVectorAdd(m_transform.r[3], DirectX::XMVectorSet(dx, dy, dz, 0.0f));
}
void Mesh::SetRotationYawPitchRoll(float yaw, float pitch, float roll) {
    auto rot = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    m_transform.r[0] = rot.r[0]; m_transform.r[1] = rot.r[1]; m_transform.r[2] = rot.r[2];
}
void Mesh::AddRotationYawPitchRoll(float dy, float dp, float dr) {
    DirectX::XMMATRIX cur{}; cur.r[0] = m_transform.r[0]; cur.r[1] = m_transform.r[1]; cur.r[2] = m_transform.r[2]; cur.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
    auto d = DirectX::XMMatrixRotationRollPitchYaw(dp, dy, dr);
    auto n = DirectX::XMMatrixMultiply(d, cur);
    m_transform.r[0] = n.r[0]; m_transform.r[1] = n.r[1]; m_transform.r[2] = n.r[2];
}
void Mesh::SetScale(float sx, float sy, float sz) {
    m_transform.r[0] = DirectX::XMVectorSetX(m_transform.r[0], sx);
    m_transform.r[1] = DirectX::XMVectorSetY(m_transform.r[1], sy);
    m_transform.r[2] = DirectX::XMVectorSetZ(m_transform.r[2], sz);
}
void Mesh::AddScale(float dsx, float dsy, float dsz) {
    m_transform.r[0] = DirectX::XMVectorAdd(m_transform.r[0], DirectX::XMVectorSet(dsx, 0, 0, 0));
    m_transform.r[1] = DirectX::XMVectorAdd(m_transform.r[1], DirectX::XMVectorSet(0, dsy, 0, 0));
    m_transform.r[2] = DirectX::XMVectorAdd(m_transform.r[2], DirectX::XMVectorSet(0, 0, dsz, 0));
}

void Mesh::BindTexture(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex) const {
    auto tx = m_asset->texture ? m_asset->texture : ResourceCache::I().defaultWhite();
    if (!tx) return;
    ID3D12DescriptorHeap* heaps[] = { tx->SRVHeap() };
    cmdList->SetDescriptorHeaps(1, heaps);
    cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, tx->GPUHandle());
}
