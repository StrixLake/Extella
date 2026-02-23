cbuffer Transformation : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}


Texture2D image : register(t0);
SamplerState samLinear : register( s0 );


struct VS_OUTPUT
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

VS_OUTPUT VS_MAIN(float4 pos : POSITION, float2 tex : TEXCOORD0, float3 norm : NORMAL)
{
    VS_OUTPUT output;
    output.position = pos;
    output.color = pos;

    if(output.position.z != 0)
    {
        output.position.xy *= 10;
    }


    output.position = mul(output.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    return output;
}


float4 PS_MAIN(VS_OUTPUT input) : SV_TARGET
{
    float4 color = 0.5;
    color.w = 0.1;
    return color;
}