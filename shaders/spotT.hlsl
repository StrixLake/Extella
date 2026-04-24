cbuffer Transformation : register(b0)
{
    matrix Transform;
}

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

VS_OUTPUT VS_MAIN( float4 pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL, uint ins : SV_InstanceID)
{
    VS_OUTPUT output;

    output.norm = norm;
    output.uv = tex;
    output.pos = pos;

    return output;
}


// hull shader
cbuffer CameraPosition
{
    float4 camposition;
}

struct HS_CONTROL_POINT_OUTPUT
{
    float4 pos : SV_POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTesFactor[3] : SV_TessFactor;
    float insideTexFactor : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT ConstPatch(InputPatch<VS_OUTPUT, 3> ip, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    // calculate the median of the edges
    float4 edge1 = (ip[1].pos + ip[2].pos)/2;
    float4 edge2 = (ip[2].pos + ip[0].pos)/2;
    float4 edge3 = (ip[1].pos + ip[0].pos)/2;

    float4 inner = (ip[1].pos + ip[0].pos + ip[2].pos)/3;

    // calculate the distance from camera
    float a = distance(edge1, camposition);
    float b = distance(edge2, camposition);
    float c = distance(edge3, camposition);
    float d = distance(inner, camposition);

    // invert the distance and make it the tessellation factor
    output.EdgeTesFactor[0] = 1/a * 2.5;
    output.EdgeTesFactor[1] = 1/b * 2.5;
    output.EdgeTesFactor[2] = 1/c * 2.5;

    output.insideTexFactor = 1/d * 3.5;

    return output;
}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstPatch")]
HS_CONTROL_POINT_OUTPUT HS_MAIN(InputPatch<VS_OUTPUT, 3> ip, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT output;
    output.pos = ip[i].pos;

    return output;
}


// domain shader
[domain("tri")]
HS_CONTROL_POINT_OUTPUT DS_MAIN(HS_CONSTANT_DATA_OUTPUT input, float3 bary : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> patch)
{
    HS_CONTROL_POINT_OUTPUT output;

    float4 vertex = patch[0].pos * bary.x +
                    patch[1].pos * bary.y +
                    patch[2].pos * bary.z;

    output.pos = mul(vertex, Transform);

    return output;
}

// pixel shader

float4 PS_MAIN(HS_CONTROL_POINT_OUTPUT input) : SV_Target0
{
    return 1;
}