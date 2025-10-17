#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <string>
#include "Utils.h"
using Microsoft::WRL::ComPtr;

class ShaderPipeline
{
public:
	void Create(ID3D12Device* device,
		const D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT inputCount,
		const char* vsSource, const char* psSource,
		DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
	{
		// Root signature: single CBV at b0 for MVP
		D3D12_ROOT_PARAMETER rootParam{};
		rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParam.Descriptor.ShaderRegister = 0;
		rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;


		D3D12_ROOT_SIGNATURE_DESC rsDesc{};
		rsDesc.NumParameters = 1; rsDesc.pParameters = &rootParam;
		rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> sig, err;
		DXThrow(D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err));
		DXThrow(device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&m_root)));


		// Compile shaders
		ComPtr<ID3DBlob> VS, PS, errs;
		DXThrow(D3DCompile(vsSource, strlen(vsSource), nullptr, nullptr, nullptr, "main", "vs_5_1", 0, 0, &VS, &errs));
		DXThrow(D3DCompile(psSource, strlen(psSource), nullptr, nullptr, nullptr, "main", "ps_5_1", 0, 0, &PS, &errs));


		// Fixed-function state
		D3D12_RASTERIZER_DESC rast{};
		rast.FillMode = D3D12_FILL_MODE_SOLID;
		rast.CullMode = D3D12_CULL_MODE_BACK;
		rast.FrontCounterClockwise = FALSE;
		rast.DepthClipEnable = TRUE;
		D3D12_BLEND_DESC blend{}; blend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		D3D12_DEPTH_STENCIL_DESC ds{}; ds.DepthEnable = TRUE; ds.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; ds.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
		pso.pRootSignature = m_root.Get();
		pso.VS = { VS->GetBufferPointer(), VS->GetBufferSize() };
		pso.PS = { PS->GetBufferPointer(), PS->GetBufferSize() };
		pso.InputLayout = { inputLayout, inputCount };
		pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pso.RasterizerState = rast;
		pso.BlendState = blend;
		pso.DepthStencilState = ds;
		pso.NumRenderTargets = 1;
		pso.RTVFormats[0] = rtvFormat;
		pso.DSVFormat = dsvFormat;
		pso.SampleDesc = { 1, 0 };
		pso.SampleMask = UINT_MAX;


		DXThrow(device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&m_pso)));
	}


	ID3D12PipelineState* PSO() const { return m_pso.Get(); }
	ID3D12RootSignature* Root() const { return m_root.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
};