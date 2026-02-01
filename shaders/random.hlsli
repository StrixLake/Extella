
uint PCGHash(inout uint state) {
    const uint x = state;

    state = x * 747796405u + 2891336453u;

    const uint word = ((x >> ((x >> 28u) + 4u)) ^ x) * 277803737u;

    return (word >> 22u) ^ word;
}

 float randomPCG(inout uint state, float mi, float ma) {
    const uint n = (PCGHash(state) >> 9u) | 0x3F800000u;

    return (asfloat(n) - 1.0f) * (ma - mi) + mi;
}