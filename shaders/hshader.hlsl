
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

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

HS_CONSTANT_DATA_OUTPUT ConstantPatchFunction(InputPatch<VS_OUTPUT, 3> ip, uint PatchID : SV_PrimitiveID)
{

    HS_CONSTANT_DATA_OUTPUT output;

    output.EdgeTesFactor[0] = 3;
    output.EdgeTesFactor[1] = 3;
    output.EdgeTesFactor[2] = 3;

    output.insideTexFactor = 3;
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantPatchFunction")]
HS_CONTROL_POINT_OUTPUT main(InputPatch<VS_OUTPUT, 3> ip, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT output;
    output.pos = ip[i].Pos;
    output.uv = ip[i].uv;
    output.norm = ip[i].norm;

    return output;
}
