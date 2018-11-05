#include "pch.h"
#include "RTreeRenderer.h"

#include <libhelpers/HSystem.h>
#include <libhelpers/HMath.h>

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
    auto copy = std::make_shared<RTree>(std::move(tree));

    auto lk = thread::scoped_lock(this->cs);
    this->rtree = std::move(copy);
    this->initDrawTree = true;
}

void RTreeRenderer::InitDrawTree(const RTree &tree) {

}

void RTreeRenderer::DrawTree(const RTree &tree) {
    this->DrawSimple(tree);
}

void RTreeRenderer::DrawSimple(const RTree &tree) {
    auto root = tree.GetRoot();

    int level = 1;

    std::vector<D2D1_RECT_F> rects;

    RTreeRenderer::FillRects(level, root, rects);

    auto d2dCtx = this->dxDev->D2D();

    HRESULT hr = S_OK;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

    hr = d2dCtx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), brush.GetAddressOf());
    H::System::ThrowIfFailed(hr);

    auto &rootRect = root->rect;
    auto rootRectWidth = (float)(rootRect.right - rootRect.left);
    auto rootRectHeight = (float)(rootRect.bottom - rootRect.top);
    auto screenSize = this->output->GetLogicalSize();

    auto rect = H::Math::InscribeRectAR(DirectX::XMFLOAT2(screenSize.x * 0.9f, screenSize.y * 0.9f), DirectX::XMFLOAT2(rootRectWidth, rootRectHeight));

    D2D1::Matrix3x2F transform =
        D2D1::Matrix3x2F::Translation(-rootRectWidth / 2.f, -rootRectHeight / 2.f)
        * D2D1::Matrix3x2F::Scale(rect.x / rootRectWidth, rect.y / rootRectHeight)
        * D2D1::Matrix3x2F::Translation(screenSize.x / 2.f, screenSize.y / 2.f)
        ;

    d2dCtx->BeginDraw();

    for (auto &i : rects) {
        auto leftTop = transform.TransformPoint(D2D1::Point2F(i.left, i.top));
        auto rightBottom = transform.TransformPoint(D2D1::Point2F(i.right, i.bottom));

        d2dCtx->DrawRectangle(D2D1::RectF(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y), brush.Get());
    }

    d2dCtx->EndDraw();
}

void RTreeRenderer::FillRects(int level, const std::shared_ptr<const Node> &node, std::vector<D2D1_RECT_F> &rects) {
    if (level == 0) {
        auto &r = node->rect;
        rects.push_back(D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom));
    }
    else {
        for (auto &i : node->child) {
            RTreeRenderer::FillRects(level - 1, i, rects);
        }
    }
}