#include "pch.h"
#include "RTreeRenderer.h"

RTreeRenderer::RTreeRenderer(DxDevice *dxDev, IOutput *output)
    : IRenderer(dxDev, output), tree(2, 2)
{}

void RTreeRenderer::Render() {
    auto ctxLk = this->dxDev->LockCtxScoped();
    auto d2dCtx = this->dxDev->D2D();

    d2dCtx->BeginDraw();
    d2dCtx->Clear(D2D1::ColorF(D2D1::ColorF::CadetBlue));
    d2dCtx->EndDraw();
}

void RTreeRenderer::OutputParametersChanged() {
}