cbuffer ConstantBuffer : register( b0 )
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float3 col : COLOR;
};



[maxvertexcount(6)]
void main(triangle VS_OUTPUT vertex[3], inout LineStream<GS_OUTPUT> lineStream)
{
    GS_OUTPUT v1 = {vertex[0].Pos, vertex[0].uv, vertex[0].norm, float3(1,0,0)};
    GS_OUTPUT v2;
    v2.Pos = float4(vertex[0].Pos.xyz + vertex[0].norm, 1);

    v2.Pos = mul(v2.Pos, World);
    v2.Pos = mul(v2.Pos, View);
    v2.Pos = mul(v2.Pos, Projection);

    v1.Pos = mul(v1.Pos, World);
    v1.Pos = mul(v1.Pos, View);
    v1.Pos = mul(v1.Pos, Projection);

    v2.norm = vertex[0].norm;
    v2.uv = vertex[0].uv;

    v2.col = float3(0,1,0);

    lineStream.Append(v1);
    lineStream.Append(v2);
}