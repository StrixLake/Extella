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
VS_OUTPUT VS( float4 Pos : POSITION, float4 Normal : NORMAL )
{
    Normal.a = 0;
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Normal = mul( Normal, World );
    output.Normal = mul( output.Normal, View );
    output.Normal = mul( output.Normal, Projection );
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{   
    float4 direction = float4(0.,0.,-1.,0.);
    float3 light_color = 1;
    light_color.r = 1;
    float4 dr = normalize(direction);
    light_color *= dot(dr, normalize(input.Normal));
    float4 rx = 0;
    rx.rbg = light_color;
    return rx;
}
