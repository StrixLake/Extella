Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

cbuffer Inverse : register(b0)
{
    matrix inverseMat;
    matrix persp;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float4 p : POSITION;
};

inline bool is_zero(float4 col)
{
    return col.x == 0 || col.y == 0 || col.z == 0 || col.y == 0;
}

float4 main(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.p.xz;
    float2 lineWidth = 0.01;
    float4 uvDDXY = float4(ddx(uv), ddy(uv)); //
    float2 uvDeriv = float2(length(uvDDXY.xz), length(uvDDXY.yw)); //
    bool2 invertLine = lineWidth > 0.5;
    float2 targetWidth = invertLine ? 1.0 - lineWidth : lineWidth;
    float2 drawWidth = clamp(targetWidth, uvDeriv, 0.5);
    float2 lineAA = uvDeriv * 1.5;
    float2 gridUV = abs(frac(uv) * 2.0 - 1.0);
    gridUV = invertLine ? gridUV : 1.0 - gridUV;
    float2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
    grid2 *= saturate(targetWidth / drawWidth);
    grid2 = lerp(grid2, targetWidth, saturate(uvDeriv * 2.0 - 1.0));
    grid2 = invertLine ? 1.0 - grid2 : grid2;
    float grid = lerp(grid2.x, 1.0, grid2.y);
    return grid;
}