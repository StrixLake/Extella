#pragma warning (disable : 3571)

float bl_phong(float4 position, float3 norm, float3 light)
{
    // light
    const float ia = 0.2, is = 1, id = 0.4;
    // material
    const float ks = 1 , kd = 0.4, ka = 0.2, alpha = 80;

    norm = normalize(norm);
    float3 l = -normalize(position.xyz - light.xyz);
    float3 r = 2*dot(l, norm)*(norm-l);
    float3 v = -normalize(position.xyz - float3(0,50,-50));
    float3 h = normalize(l+v);

    float lighting = ka*ia + kd*dot(l, norm)*id + ks*is*pow(dot(norm, h), alpha);

    if(dot(r, v) < 0) lighting = ka*ia + kd*dot(l, norm)*id;

    return lighting;
}