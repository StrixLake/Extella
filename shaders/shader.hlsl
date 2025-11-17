Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

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

//
// Vertex Shader
//
VS_OUTPUT VS( float4 Pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL)
{
    VS_OUTPUT ou;
    Pos = mul( Pos, World );
    Pos = mul( Pos, View );
    Pos = mul( Pos, Projection );
    ou.Pos = Pos;
    ou.uv = tex;
    ou.norm = mul( float4(norm, 1), World );
    ou.norm = mul( ou.norm, View );
    ou.norm = mul( ou.norm, Projection );
    
    return ou;
}



float4 PS(VS_OUTPUT input) : SV_Target
{   
    float3 o = normalize(float3(-1,1,-1));
    float b = dot(o,input.norm);
    return image.Sample( samLinear, input.uv );
}
