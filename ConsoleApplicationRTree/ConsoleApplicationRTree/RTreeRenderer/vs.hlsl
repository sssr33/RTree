
cbuffer cbuf1 : register(b0) {
    matrix MVP;
    float4 color;
}

struct VSInput {
    float3 pos : POSITION0;
    float4 color : COLOR0;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

PSInput main(VSInput input) {
    PSInput res = (PSInput)0;

    res.pos = mul(float4(input.pos, 1.f), MVP);
    res.color = color * input.color;

    return res;
}