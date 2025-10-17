#pragma once
static const char* kVertexShaderSrc = R"(
cbuffer Scene : register(b0) { float4x4 uMVP; }
struct VSIn { float3 pos : POSITION; float3 col : COLOR0; };
struct VSOut { float4 pos : SV_Position; float3 col : COLOR0; };
VSOut main(VSIn v)
{
VSOut o;
o.pos = mul(float4(v.pos, 1.0), uMVP);
o.col = v.col;
return o;
}
)";


static const char* kPixelShaderSrc = R"(
float4 main(float4 pos : SV_Position, float3 col : COLOR0) : SV_Target
{
return float4(col,1);
}
)";