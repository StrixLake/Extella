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
    float3 dist : POSITION;
};


float minDistance(float3 p1, float3 p2, float3 p3)
{
    float3 p2p3 = p3 - p2;
    float3 p2p1 = p1 - p2;

    float theta = acos( dot(p2p1, p2p3) / (length(p2p1) * length(p2p3)) );

    return length(p2p1) * sin(theta);
}


[maxvertexcount(3)]
void main(triangle VS_OUTPUT vertex[3], inout TriangleStream<GS_OUTPUT> triStream)
{
    GS_OUTPUT v0;
    GS_OUTPUT v1;
    GS_OUTPUT v2;

    v0.Pos = vertex[0].Pos;
    v0.uv = vertex[0].uv;
    v0.norm = vertex[0].norm;
    v0.dist = 0;

    v1.Pos = vertex[1].Pos;
    v1.uv = vertex[1].uv;
    v1.norm = vertex[1].norm;
    v1.dist = 0;

    v2.Pos = vertex[2].Pos;
    v2.uv = vertex[2].uv;
    v2.norm = vertex[2].norm;
    v2.dist = 0;

    
    v0.Pos = mul(v0.Pos, World);
    v0.Pos = mul(v0.Pos, View);
    v0.Pos = mul(v0.Pos, Projection);

    v1.Pos = mul(v1.Pos, World);
    v1.Pos = mul(v1.Pos, View);
    v1.Pos = mul(v1.Pos, Projection);

    v2.Pos = mul(v2.Pos, World);
    v2.Pos = mul(v2.Pos, View);
    v2.Pos = mul(v2.Pos, Projection);

    v0.dist = float3(minDistance(v0.Pos.xyz, v1.Pos.xyz, v2.Pos.xyz), 0, 0);
    v1.dist = float3(0, minDistance(v1.Pos.xyz, v2.Pos.xyz, v0.Pos.xyz), 0);
    v2.dist = float3(0, 0, minDistance(v2.Pos.xyz, v0.Pos.xyz, v1.Pos.xyz));


    triStream.Append(v0);
    triStream.Append(v1);
    triStream.Append(v2);

}