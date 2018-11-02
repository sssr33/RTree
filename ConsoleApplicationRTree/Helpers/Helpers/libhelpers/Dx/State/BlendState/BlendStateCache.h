#pragma once
#include "..\OMBlendState.h"

struct BlendState {
    Microsoft::WRL::ComPtr<ID3D11BlendState> state;
    float blendFactor[4];
    uint32_t sampleMask;

    BlendState();

    BlendState(
        const Microsoft::WRL::ComPtr<ID3D11BlendState> &state,
        float blendFactor[4],
        uint32_t sampleMask);

    BlendState(
        const Microsoft::WRL::ComPtr<ID3D11BlendState> &state,
        const DirectX::XMVECTORF32 &blendFactor,
        uint32_t sampleMask);
};


class BlendStateCache {
public:
    BlendStateCache(ID3D11Device *dev);
    ~BlendStateCache();

    const BlendState *GetDefaultState();
    const BlendState *GetAlphaBlendState();
    const BlendState *GetMultiplyBlendState();
    const BlendState *GetHighlightBlendState();
    const BlendState *GetAlphaBlendingBlendState();

private:
    ID3D11Device *dxDev;

    BlendState defaultState;
    BlendState alphaBlendState;
    BlendState multiplyBlendState;
    BlendState highlightingBlendState;
    BlendState alphaBlendingBlendState;
};
