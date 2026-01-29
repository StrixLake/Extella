Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float4 p : POSITION;
    float4 light : POSITION1;
};


float4 main(VS_OUTPUT input) : SV_Target
{

    // light
    const float ia = 0.2, is = 0.5, id = 0.3;
    // material
    const float ks = 0.3, kd = 0.4, ka = 0.3, alpha = 80;

    float3 norm = normalize(input.norm);
    float3 l = -normalize(input.p.xyz - input.light.xyz);
    float3 r = 2*dot(l, norm)*(norm-l);
    float3 v = -normalize(input.p.xyz - float3(0,5,-5));
    float3 h = normalize(l+v);

    float light = ka*ia + kd*dot(l, norm)*id + ks*is*pow((dot(norm, h)), alpha);

    float lambert = dot(norm, l);
    if(dot(r, v) < 0) light = ka*ia + kd*dot(l, norm)*id;
    return light;
}