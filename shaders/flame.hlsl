cbuffer Transformation : register(b0)
{
    matrix Transform;
}

struct VS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_MAIN(float4 position : POSITION, float2 uv : TEXCOORD0)
{
    VS_OUT output;
    output.position = mul(position, Transform);
    output.uv = uv;
    return output;
}


Texture2D alphaEmit : register(t0);
SamplerState samLinear : register(s0);

float4 PS_MAIN(VS_OUT input) : SV_Target0
{
    float4 cyan = {0, 1, 1, 1};
    float alpha = alphaEmit.Sample(samLinear, input.uv).x;
    return cyan*alpha;
}