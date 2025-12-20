cbuffer ConstantBuffer : register( b0 )
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTesFactor[3] : SV_TessFactor;
    float insideTexFactor : SV_InsideTessFactor;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct DOUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

[domain("tri")]
DOUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float3 bary : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> patch)
{
    DOUTPUT output;

    float3 vertex = patch[0].pos * bary.x +
                    patch[1].pos * bary.y +
                    patch[2].pos * bary.z;

    output.pos = mul(float4(vertex,1.f), World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    output.uv = patch[0].uv * bary.x + 
                patch[1].uv * bary.y +
                patch[2].uv * bary.z;

    output.norm = patch[0].norm * bary.x + 
                patch[1].norm * bary.y +
                patch[2].norm * bary.z;

    return output;
}   