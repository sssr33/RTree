#include "IRenderer.h"

IRenderer::IRenderer(DxDevice *dxDev, IOutput *output)
    : dxDev(dxDev), output(output)
{
}

IRenderer::~IRenderer() {
}