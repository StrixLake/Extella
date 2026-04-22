#include <phong.hlsli>

cbuffer Transformation : register(b0)
{
    matrix Transform;
}

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float3 position : POSITION;
};

VS_OUTPUT VS_MAIN( float4 pos : POSITION, float2 tex : TEXCOORD, float3 norm : NORMAL, uint ins : SV_InstanceID)
{
    VS_OUTPUT output;

    output.norm = norm;
    output.uv = tex;
    output.pos = pos;
    output.position = pos;

    output.pos = mul(output.pos, Transform);

    return output;
}


Texture2D diffuse : register(t0);
SamplerState samLinear : register( s0 );

cbuffer Material : register(b2)
{
    float3 ambient;
    float3 kdiffuse;
    float3 specular;
    float3 transmittance;
    float3 emission;
    float shininess, ior, dissolve;
    float roughness,metallic,sheen;
    float clearcoat_thickness,clearcoat_roughness;
}

cbuffer CameraPosition : register(b3)
{
    float4 CamPosition; 
}
static const float PI = 3.14159265359;

// ===== PBR Helper Functions =====

// GGX/Trowbridge-Reitz Normal Distribution Function
float DistributionGGX(float3 N, float3 H, float r)
{
    float a  = r * r;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom + 0.0001);
}

// Smith's Schlick-GGX Geometry Function
float GeometrySchlickGGX(float NdotV, float r)
{
    float k = ((r + 1.0) * (r + 1.0)) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}
float GeometrySmith(float3 N, float3 V, float3 L, float r)
{
    return GeometrySchlickGGX(max(dot(N, V), 0.0), r)
         * GeometrySchlickGGX(max(dot(N, L), 0.0), r);
}

// Fresnel-Schlick Approximation
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ===== Pixel Shader =====

float4 PS_MAIN(VS_OUTPUT input) : SV_Target
{
    if(dissolve != 1) discard;

    float3 N = normalize(input.norm);
    
    float3 V = normalize(CamPosition.xyz - input.position);

    float3 albedo = kdiffuse;
    float r = max(roughness, 0.04); // Clamp roughness to avoid numerical singularities
    float m = metallic;

    // F0: Dielectrics reflect ~4% at grazing angles; metals tint reflections with albedo
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, m);

    // Hardcoded light setup: 3 lights with physically-based intensities
    static const float3 lightPos[3]   = { float3(60, 60, 60), float3(-60, 60, 80), float3(0, 60, -80) };
    static const float3 lightColor[3] = {
        float3(1.0, 0.95, 0.85) * 5500.0,  // Key light (warm)
        float3(0.85, 0.9, 1.0)  * 5200.0,  // Fill light (cool)
        float3(1.0, 1.0, 1.0)   * 5800.0   // Rim light (neutral)
    };

    float3 Lo = float3(0.0, 0.0, 0.0);

    for (int i = 0; i < 3; i++)
    {
        float3 L = normalize(lightPos[i] - input.position);
        float3 H = normalize(V + L);

        // Inverse-square attenuation
        float distance  = length(lightPos[i] - input.position);
        float3 radiance = lightColor[i] / (distance * distance);

        // --- Cook-Torrance Specular BRDF ---
        float D = DistributionGGX(N, H, r);
        float G = GeometrySmith(N, V, L, r);
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 specBRDF   = (D * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);
        float3 kD = (1.0 - F) * (1.0 - m); // Energy conservation: what isn't reflected is diffused

        float NdotL = max(dot(N, L), 0.0);

        // --- Clearcoat (second specular lobe, e.g. car paint / varnish) ---
        float3 ccContrib = float3(0,0,0);
        if (clearcoat_thickness > 0.0)
        {
            float ccr = max(clearcoat_roughness, 0.04);
            float Dc = DistributionGGX(N, H, ccr);
            float Gc = GeometrySmith(N, V, L, ccr);
            // Clearcoat is dielectric (IOR ~1.5 => F0 ~0.04)
            float Fc = 0.04 + 0.96 * pow(1.0 - max(dot(H, V), 0.0), 5.0);
            float ccSpec = (Dc * Gc * Fc) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);
            ccContrib = clearcoat_thickness * ccSpec * radiance * NdotL;
        }

        // Accumulate: Lambertian diffuse + Cook-Torrance specular + clearcoat
        Lo += (kD * albedo / PI + specBRDF) * radiance * NdotL + ccContrib;
    }

    // Ambient (approximation of environment diffuse)
    float3 ambientLight = ambient * albedo * 0.05;

    // Sheen: fabric-like view-dependent rim glow
    float3 sheenLight = sheen * pow(1.0 - max(dot(N, V), 0.0), 5.0);

    float3 color = ambientLight + Lo + sheenLight + emission;

    float4 texColor = diffuse.Sample(samLinear, input.uv);
    return float4(color * texColor.rgb, 1.0);
}