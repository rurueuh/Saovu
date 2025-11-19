#include "ShaderPipeline.h"

D3D12_BLEND_DESC ShaderPipeline::CreateBlend(bool enableBlend)
{
    D3D12_BLEND_DESC b{};
    b.AlphaToCoverageEnable = FALSE;
    b.IndependentBlendEnable = FALSE;
    auto& rt = b.RenderTarget[0];

    rt.BlendEnable = enableBlend ? TRUE : FALSE;
    rt.LogicOpEnable = FALSE;

    rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    rt.BlendOp = D3D12_BLEND_OP_ADD;

    rt.SrcBlendAlpha = D3D12_BLEND_ONE;
    rt.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;

    rt.LogicOp = D3D12_LOGIC_OP_NOOP;
    rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    return b;
}

D3D12_DEPTH_STENCIL_DESC ShaderPipeline::CreateDepth(bool depthWrite)
{
    D3D12_DEPTH_STENCIL_DESC d{};
    d.DepthEnable = TRUE;
    d.DepthWriteMask = depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    d.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    d.StencilEnable = FALSE;
    d.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    d.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    d.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP,
                    D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    d.BackFace = d.FrontFace;
    return d;
}

D3D12_RASTERIZER_DESC ShaderPipeline::CreateRast(bool wireframed, D3D12_CULL_MODE cull)
{
    D3D12_RASTERIZER_DESC r{};
    r.FillMode = wireframed ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
    r.CullMode = cull;
    r.FrontCounterClockwise = FALSE;
    r.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    r.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    r.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    r.DepthClipEnable = TRUE;
    r.MultisampleEnable = FALSE;
    r.AntialiasedLineEnable = FALSE;
    r.ForcedSampleCount = 0;
    r.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    return r;
}
