cbuffer ConstantBuffer : register( b0 )
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : COLOR0;
};

//
// Vertex Shader
//
VS_OUTPUT VS( float4 Pos : POSITION)
{
    VS_OUTPUT ou;
    Pos = mul( Pos, World );
    Pos = mul( Pos, View );
    Pos = mul( Pos, Projection );
    ou.Pos = Pos;
    ou.Normal = Pos;
    return ou;
}

float4 PS(VS_OUTPUT input) : SV_Target
{   
    
    return abs(input.Normal);
}
