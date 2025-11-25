#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Utils.h"
#include "GraphicsDevice.h"

/**
 * @class DepthBuffer
 * @brief Manages a depth buffer resource.
 * This class handles the creation, resizing, and descriptor heap for a depth buffer.
 */
class DepthBuffer
{
public:
	/**
	 * @brief Creates the depth buffer.
	 * @param gd The graphics device.
	 * @param width The width of the depth buffer.
	 * @param height The height of the depth buffer.
	 */
	void Create(GraphicsDevice& gd, UINT width, UINT height)
	{
		D3D12_RESOURCE_DESC d{};
		d.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		d.Width = width; d.Height = height;
		d.DepthOrArraySize = 1; d.MipLevels = 1;
		d.Format = DXGI_FORMAT_D32_FLOAT;
		d.SampleDesc = { 1, 0 };
		d.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		d.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


		D3D12_HEAP_PROPERTIES heap{}; heap.Type = D3D12_HEAP_TYPE_DEFAULT;
		D3D12_CLEAR_VALUE clear{}; clear.Format = DXGI_FORMAT_D32_FLOAT; clear.DepthStencil.Depth = 1.0f;


		DXThrow(gd.Device()->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &d,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear, IID_PPV_ARGS(&m_tex)));


		D3D12_DESCRIPTOR_HEAP_DESC desc{}; desc.NumDescriptors = 1; desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DXThrow(gd.Device()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)));


		D3D12_DEPTH_STENCIL_VIEW_DESC dsv{};
		dsv.Format = DXGI_FORMAT_D32_FLOAT; dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		gd.Device()->CreateDepthStencilView(m_tex.Get(), &dsv, m_heap->GetCPUDescriptorHandleForHeapStart());
	}

	/**
	 * @brief Resizes the depth buffer.
	 * @param gd The graphics device.
	 * @param width The new width.
	 * @param height The new height.
	 */
	void Resize(GraphicsDevice& gd, UINT width, UINT height)
	{
		m_tex.Reset();
		Create(gd, width, height);
	}

	/**
	 * @brief Gets the depth stencil view descriptor handle.
	 * @return The CPU descriptor handle for the depth stencil view.
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE DSV() const { return m_heap->GetCPUDescriptorHandleForHeapStart(); }


private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_tex;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
};