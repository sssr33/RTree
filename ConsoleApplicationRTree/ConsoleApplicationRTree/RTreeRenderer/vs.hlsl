
cbuffer cbuf1 : register(b0) {
    matrix MVP;
}

struct VSInput {
    float3 pos : POSITION0;
};

struct PSInput {
    float4 pos : SV_POSITION;
};

PSInput main(VSInput input) {
    PSInput res = (PSInput)0;

    res.pos = mul(float4(input.pos, 1.f), MVP);

    return res;
}