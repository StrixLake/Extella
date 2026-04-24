struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer Transformation
{
    matrix transform;
    matrix WorldProj;
}

VS_OUT VS_MAIN(float4 position : POSITION, float2 uv : TEXCOORD0)
{
    VS_OUT output;

    output.pos = mul(position, WorldProj);
    output.uv = uv;
    return output;
}

SamplerState samLinear : register(s0);
Texture2D skybox : register(t0);

float4 PS_MAIN(VS_OUT input) : SV_Target0
{
    return skybox.Sample(samLinear, input.uv);
}