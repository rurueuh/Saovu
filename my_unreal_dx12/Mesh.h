#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <cstdint>
#include <DirectXMath.h>
#include "Utils.h"

struct Vertex { float px, py, pz; float r, g, b; };

class Mesh
{
public:
	void Upload(ID3D12Device* device, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
	{
		// Vertex buffer on UPLOAD heap (simpler for sample)
		D3D12_HEAP_PROPERTIES uploadHeap{}; uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
		D3D12_RESOURCE_DESC vbDesc{}; vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; vbDesc.Width = UINT(vertices.size() * sizeof(Vertex)); vbDesc.Height = 1; vbDesc.DepthOrArraySize = 1; vbDesc.MipLevels = 1; vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; vbDesc.SampleDesc = { 1,0 };
		DXThrow(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vb)));
		{
			void* map = nullptr; D3D12_RANGE r{ 0,0 }; m_vb->Map(0, &r, &map); memcpy(map, vertices.data(), vertices.size() * sizeof(Vertex)); m_vb->Unmap(0, nullptr);
		}
		m_vbv.BufferLocation = m_vb->GetGPUVirtualAddress();
		m_vbv.StrideInBytes = sizeof(Vertex);
		m_vbv.SizeInBytes = UINT(vertices.size() * sizeof(Vertex));


		// Index buffer
		D3D12_RESOURCE_DESC ibDesc = vbDesc; ibDesc.Width = UINT(indices.size() * sizeof(uint16_t));
		DXThrow(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ib)));
		{
			void* map = nullptr; D3D12_RANGE r{ 0,0 }; m_ib->Map(0, &r, &map); memcpy(map, indices.data(), indices.size() * sizeof(uint16_t)); m_ib->Unmap(0, nullptr);
		}
		m_ibv.BufferLocation = m_ib->GetGPUVirtualAddress();
		m_ibv.Format = DXGI_FORMAT_R16_UINT;
		m_ibv.SizeInBytes = UINT(indices.size() * sizeof(uint16_t));


		m_indexCount = UINT(indices.size());
	}


	const D3D12_VERTEX_BUFFER_VIEW& VBV() const { return m_vbv; }
	const D3D12_INDEX_BUFFER_VIEW& IBV() const { return m_ibv; }
	UINT IndexCount() const { return m_indexCount; }


private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vb, m_ib;
	D3D12_VERTEX_BUFFER_VIEW m_vbv{};
	D3D12_INDEX_BUFFER_VIEW m_ibv{};
	UINT m_indexCount = 0;
};