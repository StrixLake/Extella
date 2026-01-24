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

VS_OUTPUT main( float4 pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL)
{
    VS_OUTPUT output;

    output.norm = norm;
    output.uv = tex;
    output.pos = pos;
    output.p = pos;

    output.pos = mul(output.pos, World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    return output;
}