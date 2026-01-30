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

// vertex shader
VS_OUTPUT VS_MAIN( float4 Pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL)
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


// pixel shader
float4 PS_MAIN(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.p.xz;
    float2 duv = fwidth(uv);
    uv = abs(frac(uv / 2) - 0.5) / duv;
    uv = smoothstep(0, 1 , 1 - uv);

    return max(uv.x,uv.y)/ sqrt(length(input.p.xz));
}