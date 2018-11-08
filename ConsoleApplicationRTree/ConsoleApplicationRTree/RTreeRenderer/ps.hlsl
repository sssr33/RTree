
cbuffer cbuf1 : register(b0) {
    matrix MVP;
}

struct PSInput {
    float4 pos : SV_POSITION;
};

float4 main(PSInput input) : SV_Target0 {
    return float4(0.f, 1.f, 0.f, 1.f);
}