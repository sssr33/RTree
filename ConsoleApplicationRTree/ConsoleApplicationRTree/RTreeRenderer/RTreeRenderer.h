#pragma once
#include "../RTree.h"

#include <libhelpers/Dx/Renderer/IRenderer.h>

class RTreeRenderer : public IRenderer {
public:
    RTreeRenderer(DxDevice *dxDev, IOutput *output);

    void Render() override;
    void OutputParametersChanged() override;

private:
    thread::critical_section cs;
    RTree tree;
};