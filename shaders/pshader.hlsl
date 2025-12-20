Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

float4 main(VS_OUTPUT input) : SV_Target
{   
    float3 o = normalize(float3(-1,1,-1));
    float b = dot(o,input.norm);
    o = image.Sample( samLinear, input.uv );
    return float4(o, 1);
}