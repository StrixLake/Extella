Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float4 p : POSITION;
};


float4 main(VS_OUTPUT input) : SV_Target
{
    float3 norm = input.norm;
    float b = -dot(input.norm, normalize(input.p.xyz - float3(0,5,-5)));
    return b;
}