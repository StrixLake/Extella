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
    float4 norm = normalize(image.Sample(samLinear, input.uv));
    float b = dot(normalize(norm), normalize(float3(0,-20,-20)));
    return norm;
}