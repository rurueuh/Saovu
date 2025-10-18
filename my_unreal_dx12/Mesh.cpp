#include "Mesh.h"
#include "WindowDX12.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices) {
	this->Upload(WindowDX12::Get().GetDevice(), vertices, indices);
	std::cout << "Mesh created with " << indices.size() / 3 << " triangles.\n";
}

Mesh::Mesh(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur: impossible d’ouvrir " << filename << std::endl;
    }

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normals;
    std::vector<DirectX::XMFLOAT2> texcoords;

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    std::string line;
    std::unordered_map<std::string, uint16_t> vertexMap;
    uint16_t nextIndex = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            DirectX::XMFLOAT3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (type == "vt") {
            DirectX::XMFLOAT2 tex;
            iss >> tex.x >> tex.y;
            texcoords.push_back(tex);
        }
        else if (type == "vn") {
            DirectX::XMFLOAT3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (type == "f") {
            std::string vtx[3];
            iss >> vtx[0] >> vtx[1] >> vtx[2];
            for (int i = 0; i < 3; ++i) {
                if (vertexMap.find(vtx[i]) == vertexMap.end()) {
                    std::replace(vtx[i].begin(), vtx[i].end(), '/', ' ');
                    std::istringstream vss(vtx[i]);
                    int vi = 0, ti = 0, ni = 0;
                    vss >> vi >> ti >> ni;

                    Vertex vert{};
                    auto& p = positions[vi - 1];
                    vert.px = p.x;
                    vert.py = p.y;
                    vert.pz = p.z;

                    vert.r = 1.0f;
                    vert.g = 1.0f;
                    vert.b = 1.0f;

                    if (ti > 0) {
                        auto& t = texcoords[ti - 1];
                        vert.u = t.x;
                        vert.v = 1.0f - t.y;
                    }

                    m_vertices.push_back(vert);
                    vertexMap[vtx[i]] = nextIndex++;
                }
                m_indices.push_back(vertexMap[vtx[i]]);
            }
        }
    }

    this->Upload(nullptr, m_vertices, m_indices);
    std::cout << "OBJ chargé: " << m_vertices.size() << " sommets, " << m_indices.size() / 3 << " faces." << std::endl;
}

void Mesh::Upload(ID3D12Device* device,
    const std::vector<Vertex>& vertices,
    const std::vector<uint16_t>& indices,
    ID3D12Fence* fence, UINT64 fenceValue)
{
    if (!device) device = WindowDX12::Get().GetDevice();

	m_vertices = vertices;
	m_indices = indices;

    if (fence && fence->GetCompletedValue() < fenceValue) {
        HANDLE e = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->SetEventOnCompletion(fenceValue, e);
    }

    const UINT vbBytesReal = UINT(vertices.size() * sizeof(Vertex));
    const UINT ibBytesReal = UINT(indices.size() * sizeof(uint16_t));
    const UINT vbBytes = std::max<UINT>(vbBytesReal, 1u);
    const UINT ibBytes = std::max<UINT>(ibBytesReal, 1u);

    auto makeOrResize = [&](ComPtr<ID3D12Resource>& res, UINT bytes) {
        if (res && res->GetDesc().Width >= bytes) return;
        D3D12_HEAP_PROPERTIES hp{}; hp.Type = D3D12_HEAP_TYPE_UPLOAD;
        D3D12_RESOURCE_DESC rd{}; rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        rd.Width = bytes; rd.Height = 1; rd.DepthOrArraySize = 1; rd.MipLevels = 1;
        rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; rd.SampleDesc = { 1,0 };
        DXThrow(device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&res)));
        };

    makeOrResize(m_vb, vbBytes);
    makeOrResize(m_ib, ibBytes);

    if (vbBytesReal) {
        void* p = nullptr; D3D12_RANGE rr{ 0,0 };
        m_vb->Map(0, &rr, &p);
        memcpy(p, vertices.data(), vbBytesReal);
        D3D12_RANGE wr{ 0, vbBytesReal }; m_vb->Unmap(0, &wr);
    }
    if (ibBytesReal) {
        void* p = nullptr; D3D12_RANGE rr{ 0,0 };
        m_ib->Map(0, &rr, &p);
        memcpy(p, indices.data(), ibBytesReal);
        D3D12_RANGE wr{ 0, ibBytesReal }; m_ib->Unmap(0, &wr);
    }

    m_vbv.BufferLocation = m_vb->GetGPUVirtualAddress();
    m_vbv.StrideInBytes = sizeof(Vertex);
    m_vbv.SizeInBytes = vbBytesReal;

    m_ibv.BufferLocation = m_ib->GetGPUVirtualAddress();
    m_ibv.Format = DXGI_FORMAT_R16_UINT;
    m_ibv.SizeInBytes = ibBytesReal;

    m_indexCount = UINT(indices.size());
}

void Mesh::BindTexture(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex) const {
    Texture* tx = m_texture ? m_texture : m_defaultWhite;
    std::wcout << L"Binding texture with GPU handle: " << m_defaultWhite << L"\n";
    if (m_texture == nullptr)
        printf("%p\n", m_texture);
    if (m_defaultWhite == nullptr) {
        auto window = WindowDX12::Get();
        m_defaultWhite = &window.getDefaultTexture();
    }
    if (!tx) return;
    ID3D12DescriptorHeap* heaps[] = { tx->SRVHeap() };
    cmdList->SetDescriptorHeaps(1, heaps);
    cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, tx->GPUHandle());
}
