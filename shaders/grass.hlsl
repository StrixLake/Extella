cbuffer Transformation : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float4 light : TEXCOORD3;
    float4 p : POSITION;
    float instance : TEXCOORD2;
};

float xorshift(uint state)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return asfloat((state >> 9) | 0x3f800000) -1.0;
}

uint PCGHash(inout uint state) {
    const uint x = state;

    state = x * 747796405u + 2891336453u;

    const uint word = ((x >> ((x >> 28u) + 4u)) ^ x) * 277803737u;

    return (word >> 22u) ^ word;
}

 float randomPCG(inout uint state, float mi, float ma) {
    const uint n = (PCGHash(state) >> 9u) | 0x3F800000u;

    return (asfloat(n) - 1.0f) * (ma - mi) + mi;
}

VS_OUTPUT VS_MAIN(float4 pos : POSITION, float2 tex : TEXCOORD0, float3 norm : NORMAL, uint instance_id : SV_InstanceID)
{
    VS_OUTPUT output;
    
    output.light = float4(50,50,-50,1);
    output.uv = tex;
    output.pos = pos;
    output.p = pos;
    output.instance = instance_id;

    uint seed = instance_id+1;
    seed *= 20;

    float x = randomPCG(seed, -100, 100);
    float y = randomPCG(seed, -100, 100);
    output.pos.xz += float2(x, y);
    output.instance = x;

    output.pos = mul(output.pos, World);
    
    output.light = mul(output.light, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    return output;
}

#include <phong.hlsli>

float4 PS_MAIN(VS_OUTPUT input) : SV_TARGET
{
    float4 o = image.Sample(samLinear, input.uv);
    float4 norm = o*2 -1;
    float color = bl_phong(input.p, norm.xyz, input.light.xyz);
    return color*float4(72,111,56,1)*input.p.y/255*(abs(input.p.z*2)+0.1);
}