#include "pch.h"
#include "RTreeRenderer.h"

RTreeRenderer::RTreeRenderer(DxDevice *dxDev, IOutput *output)
    : IRenderer(dxDev, output)
    , initDrawTree(true)
{}

void RTreeRenderer::Render() {
    auto ctxLk = this->dxDev->LockCtxScoped();
    auto d2dCtx = this->dxDev->D2D();

    d2dCtx->BeginDraw();
    d2dCtx->Clear(D2D1::ColorF(D2D1::ColorF::CadetBlue));
    d2dCtx->EndDraw();

    bool initDrawTreeTmp = false;
    std::shared_ptr<RTree> rtreeTmp;

    {
        auto lk = thread::scoped_lock(this->cs);
        rtreeTmp = this->rtree;
        initDrawTreeTmp = this->initDrawTree;
        this->initDrawTree = false;
    }

    if (rtreeTmp) {
        if (initDrawTreeTmp) {
            this->InitDrawTree(*rtreeTmp);
        }

        this->DrawTree(*rtreeTmp);
    }
}

void RTreeRenderer::OutputParametersChanged() {
}

void RTreeRenderer::SetTree(RTree tree) {
    auto copy = std::make_shared<RTree>(tree);

    auto lk = thread::scoped_lock(this->cs);
    this->rtree = std::move(copy);
    this->initDrawTree = true;
}

void RTreeRenderer::InitDrawTree(const RTree &tree) {

}

void RTreeRenderer::DrawTree(const RTree &tree) {
    auto root = tree.GetRoot();


}