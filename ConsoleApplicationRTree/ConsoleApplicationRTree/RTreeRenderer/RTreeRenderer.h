#pragma once
#include "../RTree.h"

#include <libhelpers/Dx/Renderer/IRenderer.h>

class RTreeRenderer : public IRenderer {
public:
    RTreeRenderer(DxDevice *dxDev, IOutput *output);

    void Render() override;
    void OutputParametersChanged() override;

    void SetTree(RTree tree);

private:
    thread::critical_section cs;
    std::shared_ptr<RTree> rtree;
    bool initDrawTree;

    Microsoft::WRL::ComPtr<ID3D11Buffer> boxGeom;

    void InitDrawTree(const RTree &tree);
    void DrawTree(const RTree &tree);
};