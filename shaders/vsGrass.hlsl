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
};

VS_OUTPUT main(float4 pos : POSITION, float2 tex : TEXCOORD0, float3 norm : NORMAL, float2 position : TEXCOORD1, uint instance_id : SV_InstanceID)
{
    VS_OUTPUT output;

    output.norm = norm;
    output.uv = tex;
    output.pos = pos;
    output.p = pos;

    output.pos.xz += position;

    output.pos = mul(output.pos, World);
    output.norm = mul(output.norm, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    return output;
}