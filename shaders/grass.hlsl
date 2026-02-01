cbuffer Transformation : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer Variables : register(b1)
{
    float time;
    float a, b, c;
}

Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float4 light : TEXCOORD3;
    float4 p : POSITION;
    float3x3 world : TEXCOORD4;
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

float2 bezier(float2 p0, float2 p1, float2 p2, float t)
{
    float2 output = (1-t)*(1-t)*p0 + 2*(1-t)*t*p1 + t*t*p2;
    return output;
}

VS_OUTPUT VS_MAIN(float4 pos : POSITION, float2 tex : TEXCOORD0, float3 norm : NORMAL, uint instance_id : SV_InstanceID)
{
    VS_OUTPUT output;
    
    output.light = float4(20,20,20, 1);
    output.uv = tex;
    output.pos = pos;
    output.p = pos;
    output.world = (float3x3)World;

    uint seed = instance_id+1;
    seed *= 20;


    float x1 = randomPCG(seed, -200, 200);
    float y1 = randomPCG(seed, -200, 200);
    float z = sin(x1/2+y1/2+2*time) + 1;
    output.pos.xy += bezier(float2(0,0), float2(0,1), float2(1+z*2,1), pos.y/4.1);

    output.pos.xz += float2(x1, y1);

    output.light = mul(output.light, World);
    
    output.pos = mul(output.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    
    return output;
}

#include <phong.hlsli>

float4 PS_MAIN(VS_OUTPUT input) : SV_TARGET
{
    float4 o = image.Sample(samLinear, input.uv);
    float4 norm = o*2 -1;
    norm.xyz = mul(norm.xyz, input.world);
    float color = bl_phong(input.p, norm.xyz, input.light.xyz);
    return color*float4(72,111,56,255)/255;
}