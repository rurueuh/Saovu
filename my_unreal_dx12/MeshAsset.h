#pragma once
#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "Texture.h"

/**
 * @struct Vertex
 * @brief Represents a vertex in a mesh.
 */
struct Vertex {
    float px, py, pz;
    float nx, ny, nz;
    float r, g, b;
    float u, v;
    float tx, ty, tz;
    float bx, by, bz;
};

/**
 * @struct Submesh
 * @brief Represents a submesh of a mesh asset.
 */
struct Submesh
{
    uint32_t indexStart = 0;
    uint32_t indexCount = 0;

    DirectX::XMFLOAT3 kd{ 1.f, 1.f, 1.f };
    DirectX::XMFLOAT3 ks{ 1.f, 1.f, 1.f };
    DirectX::XMFLOAT3 ke{ 0.f, 0.f, 0.f };
    float shininess = 128.f;
    float opacity = 1.f;

    std::shared_ptr<Texture> texture;
    std::shared_ptr<Texture> normalMap;
    bool hasNormalMap = false;

    std::shared_ptr<Texture> metalRoughMap;
    bool hasMetalRoughMap = false;
};

/**
 * @class MeshAsset
 * @brief Represents the raw data of a mesh.
 * This class stores the vertices, indices, and material properties of a mesh.
 */
class MeshAsset
{
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float shininess = 128.f;

    Microsoft::WRL::ComPtr<ID3D12Resource> vb, ib;
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    D3D12_INDEX_BUFFER_VIEW ibv{};
    UINT indexCount = 0;

    std::shared_ptr<Texture> texture;

    std::vector<Submesh> submeshes;

	/**
	 * @brief Sets the shininess of the mesh asset.
	 * @param s The new shininess value.
	 */
	void setShininess(float s) { shininess = s; }

    /**
     * @brief Uploads the mesh data to the GPU.
     * @param device The D3D12 device.
     */
    void Upload(ID3D12Device* device);
};
