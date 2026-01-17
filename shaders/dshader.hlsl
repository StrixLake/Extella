cbuffer ConstantBuffer : register( b0 )
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer tesfactor : register (b1)
{
    int outtesFactor;
    int intesFactor;
    int g;
    float time;
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

    output.pos = float4(vertex, 1.f);
    output.pos.y = sin(output.pos.x + time) + sin(output.pos.z + time);

    float3 norm1 = normalize(float3(1, cos(output.pos.x + time), 0));
    float3 norm2 = normalize(float3(0, cos(output.pos.z + time), 1));
    output.norm = cross(norm2, norm1);

    output.uv = patch[0].uv * bary.x + 
                patch[1].uv * bary.y +
                patch[2].uv * bary.z;


    return output;
}   