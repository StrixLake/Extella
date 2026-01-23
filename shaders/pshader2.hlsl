Texture2D image : register(t0);
SamplerState samLinear : register( s0 );

cbuffer Inverse : register(b0)
{
    matrix inverseMat;
    matrix persp;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
    float4 p : POSITION;
};

inline bool is_zero(float4 col)
{
    return col.x == 0 || col.y == 0 || col.z == 0 || col.y == 0;
}

float4 main(VS_OUTPUT input) : SV_Target
{
    // 1. Setup grid spacing
    // You had input.p /= 2, so your grid cells are 2.0 units wide
    float2 coord = input.p.xz / 2.0;

    // 2. Calculate derivatives
    // fwidth(coord) tells us how much 'coord' changes from one pixel to the next.
    // This effectively gives us the size of a pixel in grid-space.
    float2 derivative = fwidth(coord);

    // 3. Calculate the distance to the nearest line
    // frac(coord - 0.5) - 0.5 creates a sawtooth wave from -0.5 to 0.5
    // abs(...) folds it to 0.0 to 0.5 (0.0 is the center of the line)
    // dividing by 'derivative' converts that distance into Pixel Units.
    float2 grid = abs(frac(coord - 0.5) - 0.5) / derivative;

    // 4. Determine the intensity of the line
    // We take the minimum distance to either the X or Z axis line.
    float ln = min(grid.x, grid.y);

    // 5. Calculate Color
    // We want the line to be roughly 1.0 pixels wide.
    // If 'line' (distance in pixels) is 0, alpha is 1.
    // If 'line' is 1.0, alpha is 0.
    float minimumThickness = 1.5; // In pixels
    float colorIntensity = 1.5 - min(ln, 1.5);

    // --- Optional: Anti-Moiré Fade ---
    // As the grid gets very far away, the derivatives get huge.
    // This creates "sparkles" (aliasing). We fade the grid out
    // when a pixel covers more than a significant chunk of a grid cell.
    float fade = 1.0 - min(max(derivative.x, derivative.y), 1.0);

    // Combine
    return float4(colorIntensity * fade, colorIntensity * fade, colorIntensity * fade, 1.0);
}