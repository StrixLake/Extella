#include <phong.hlsli>

cbuffer Transformation : register(b0)
{
    matrix Transform;
}

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float3 position : POSITION;
};

VS_OUTPUT VS_MAIN( float4 pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL, uint ins : SV_InstanceID)
{
    VS_OUTPUT output;

    output.norm = norm;
    output.uv = tex;
    output.pos = pos;
    output.position = pos;

    output.pos = mul(output.pos, Transform);

    return output;
}


Texture2D diffuse : register(t0);
SamplerState samLinear : register( s0 );

cbuffer Material : register(b2)
{
    float3 ambient;
    float3 kdiffuse;
    float3 specular;
    float3 transmittance;
    float3 emission;
    float shininess, ior, dissolve;
    float roughness,metallic,sheen;
    float clearcoat_thickness,clearcoat_roughness;
}

cbuffer CameraPosition : register(b3)
{
    float4 CamPosition; 
}

float4 PS_MAIN(VS_OUTPUT input) : SV_Target
{
    float4 o = float4(diffuse.Sample(samLinear, input.uv).xyz*kdiffuse, 1);
    return float4(input.norm, 1);
}