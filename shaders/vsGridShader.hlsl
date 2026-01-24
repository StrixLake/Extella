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
    float4 p : POSITION;
};

//
// Vertex Shader
//
VS_OUTPUT main( float4 Pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL)
{
    VS_OUTPUT ou;

    ou.Pos = Pos;
    ou.uv = tex;
    ou.norm = norm;

    ou.Pos.x *= 5;
    ou.Pos.z *= 5;
    ou.p = ou.Pos;

    ou.Pos = mul(ou.Pos, World);
    ou.Pos = mul(ou.Pos, View);
    ou.Pos = mul(ou.Pos, Projection);

    return ou;
}
