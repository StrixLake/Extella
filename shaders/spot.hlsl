#include <phong.hlsli>

cbuffer Transformation : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float4 p : POSITION;
    float4 light : POSITION1;
};

VS_OUTPUT VS_MAIN( float4 pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL, uint ins : SV_InstanceID)
{
    VS_OUTPUT output;

    output.norm = norm;
    output.uv = tex;
    output.pos = pos;

    output.pos.y += 1;

    output.light = float4(5,5,-5, 1);

    output.pos = mul(output.pos, World);
    output.p = pos;
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    output.norm = mul(output.norm, (float3x4)World).xyz;
    output.light = mul(output.light, World);
    output.light /= output.light.w;

    return output;
}


Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

float4 PS_MAIN(VS_OUTPUT input) : SV_Target
{
    float light = bl_phong(input.p, input.norm, input.light.xyz);
    float4 o = image.Sample(samLinear, input.uv);
    return light*o;
}