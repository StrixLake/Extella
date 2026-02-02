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

#include <random.hlsli>

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float4 light : TEXCOORD3;
    float4 p : POSITION;
    float4 p1 : POSITION2;
    float3x4 world : TEXCOORD4;
    float3 norm : NORMAL;
};

float2 bezier(float2 p0, float2 p1, float2 p2, float t)
{
    float2 output = p1 + (1-t)*(1-t)*(p0-p1) + t*t*(p2-p1);
    return output;
}

float2 bezierNormal(float2 p0, float2 p1, float2 p2, float t)
{
    float2 normal = 2*(1-t)*(p1-p0) + 2*t*(p2-p1);
    normal = normalize(normal);
    normal = float2(-normal.y, normal.x);
    return normal;
}

VS_OUTPUT VS_MAIN(float4 pos : POSITION, float2 tex : TEXCOORD0, float3 norm : NORMAL, uint instance_id : SV_InstanceID)
{
    VS_OUTPUT output;
    
    output.light = float4(0,20,20, 1);
    output.uv = tex;
    output.pos = pos;
    output.p = pos;
    output.world = (float3x4)World;
    output.norm = norm;

    uint seed = instance_id+1;
    seed *= 20;


    float x1 = randomPCG(seed, -200, 200);
    float y1 = randomPCG(seed, -200, 200);
    float z = sin(pos.x/2+pos.z/2+2*time) + 1;
    output.pos.xy = bezier(float2(0,0), float2(0,3), float2(2,z*2), pos.y/4.1);

    //output.pos.xz += float2(2, 0);

    output.light = mul(output.light, World);
    
    output.pos = mul(output.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    
    output.norm = mul(output.norm, World).xyz;

    return output;
}

#include <phong.hlsli>

float4 PS_MAIN(VS_OUTPUT input) : SV_TARGET
{
    float z = sin(input.p.x/2+input.p.z/2+2*time) + 1;
    float4 norm = 0;
    norm.xy = bezierNormal(float2(0,0), float2(0,3), float2(2,z*2), input.p.y/4.1);
    input.p.xy = bezier(float2(0,0), float2(0,3), float2(2,z*2), input.p.y/4.1);
    input.p = mul(input.p, World);
    norm.xyz = mul(norm.xyz, input.world);
    float color = bl_phong(input.p, norm.xyz, input.light.xyz);
    return color;
}