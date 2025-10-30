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
    float2 Normal : COLOR0;
};

//
// Vertex Shader
//
VS_OUTPUT VS( float4 Pos : POSITION, float2 tex : COLOR0)
{
    VS_OUTPUT ou;
    Pos = mul( Pos, World );
    Pos = mul( Pos, View );
    Pos = mul( Pos, Projection );
    ou.Pos = Pos;
    ou.Normal = tex;
    return ou;
}



float4 PS(VS_OUTPUT input) : SV_Target
{   
    //float4 o;
    //o.rg = input.Normal;
    //return o;
    //return float4(abs(input.Pos));
    return image.Sample( samLinear, input.Normal );
}
