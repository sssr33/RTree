
cbuffer cbuf1 : register(b0) {
    matrix MVP;
}

struct PSInput {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(PSInput input) : SV_Target0 {
    return input.color;
}