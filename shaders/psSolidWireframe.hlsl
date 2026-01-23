Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 norm : NORMAL;
    float3 dist : POSITION;
    float4 xyz : TEXCOORD1;
    float ldist : TEXCOORD2;
};

float4 main(GS_OUTPUT input) : SV_Target
{
    float4 o = 1;
    float b = abs(dot(normalize(float3(0,0,1)),normalize(input.norm))) + 0.02;
    b = b*b;
    o = image.Sample( samLinear, input.uv ) *b;
    float dist = min(input.dist.x, min(input.dist.y, input.dist.z));
    //if (dist < 0.05) return 1;
    //else return 0;
    return o;
}