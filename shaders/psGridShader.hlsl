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

float4 main(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.p.xz;
    float2 duv = fwidth(uv);
    uv = abs(frac(uv / 2) - 0.5) / duv;
    uv = smoothstep(0, 1 , 1 - uv);

    return max(uv.x,uv.y)/ sqrt(length(input.p.xz));
}