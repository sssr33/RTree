#include "pch.h"
#include "RTreeRenderer.h"

#include <libhelpers/HSystem.h>
#include <libhelpers/HMath.h>

RTreeRenderer::RTreeRenderer(DxDevice *dxDev, IOutput *output)
    : IRenderer(dxDev, output)
    , initDrawTree(true)
    , boxStride(0)
    , boxVertexCount(0)
{}

void RTreeRenderer::Render() {
    auto ctxLk = this->dxDev->LockCtxScoped();
    auto d2dCtx = this->dxDev->D2D();

    /*d2dCtx->BeginDraw();
    d2dCtx->Clear(D2D1::ColorF(D2D1::ColorF::CadetBlue));
    d2dCtx->EndDraw();*/

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
    this->Init3D(tree);
}

void RTreeRenderer::DrawTree(const RTree &tree) {
    //this->DrawSimple(tree);

    this->Draw3D();
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

void RTreeRenderer::Init3D(const RTree &tree) {
    this->nodeRects.clear();

    this->InitNodeRects(tree.GetRoot());

    const float AddHeightPerLevel = 0.1f;

    for (size_t i = 1; i < this->nodeRects.size(); i++) {
        size_t level = this->nodeRects.size() - 1 - i;
        float heightInc = (float)i * AddHeightPerLevel;
        float posInc = heightInc / 2.f;

        for (auto &tr : this->nodeRects[level]) {
            tr.pos.y += posInc;
            tr.scale.y += heightInc;
        }
    }

    this->InitBoxVertices();
    this->InitPS();
    this->InitRasterizerState();
    this->InitDepthStencilState();
    this->InitBlendState();
}

void RTreeRenderer::InitNodeRects(const std::shared_ptr<const Node> &node, size_t level, float height) {
    if ((level + 1) > this->nodeRects.size()) {
        this->nodeRects.resize(level + 1);
    }

    auto &r = node->rect;
    Transform tr;

    tr.pos.x = (float)(r.left + r.right) / 2.f;
    tr.pos.z = (float)(r.top + r.bottom) / 2.f;
    tr.pos.y = height / 2.f;

    tr.scale.x = (float)(r.right - r.left);
    tr.scale.z = (float)(r.bottom - r.top);
    tr.scale.y = height;

    this->nodeRects[level].push_back(tr);

    for (auto &i : node->child) {
        this->InitNodeRects(i, level + 1, height);
    }
}

void RTreeRenderer::InitBoxVertices() {
    HRESULT hr = S_OK;
    auto d3dDev = this->dxDev->GetD3DDevice();

    this->boxGeom = nullptr;
    this->vs = nullptr;
    this->vsCBuffer = nullptr;
    this->vsInputLayout = nullptr;

    auto vsData = H::System::LoadPackageFile("vs.cso");

    hr = d3dDev->CreateVertexShader(vsData.data(), vsData.size(), nullptr, this->vs.GetAddressOf());
    H::System::ThrowIfFailed(hr);

    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = d3dDev->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsData.data(), vsData.size(), this->vsInputLayout.GetAddressOf());
    H::System::ThrowIfFailed(hr);

    this->vsCBuffer = this->CreateCBuffer(sizeof(DirectX::XMMATRIX));

    DirectX::XMFLOAT3 pos[] = {
        // front
        DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), // left-top
        DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), // right-top
        DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), // left-bottom
        DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), // right-bottom

        // back
        DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), // left-top
        DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), // right-top
        DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), // left-bottom
        DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), // right-bottom
    };

    uint32_t cubeIdx[] = {
        // front
        0, 1, 3,
        3, 2, 0,

        // left
        4, 0, 2,
        2, 6, 4,

        // back
        5, 4, 6,
        6, 7, 5,

        // right
        1, 5, 7,
        7, 3, 1,

        // top
        4, 5, 1,
        1, 0, 4,

        // bottom
        7, 6, 2,
        2, 3, 7,
    };

    std::vector<DirectX::XMFLOAT3> posVec;

    posVec.reserve(ARRAYSIZE(cubeIdx));

    for (auto &i : cubeIdx) {
        posVec.push_back(pos[i]);
    }

    this->boxStride = (uint32_t)sizeof(decltype(posVec)::value_type);
    this->boxVertexCount = (uint32_t)posVec.size();

    D3D11_BUFFER_DESC bufDesc;

    bufDesc.ByteWidth = (uint32_t)(posVec.size() * sizeof(decltype(posVec)::value_type));
    bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    bufDesc.MiscFlags = 0;
    bufDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresData;

    subresData.pSysMem = posVec.data();
    subresData.SysMemPitch = 0;
    subresData.SysMemSlicePitch = 0;

    hr = d3dDev->CreateBuffer(&bufDesc, &subresData, this->boxGeom.GetAddressOf());
    H::System::ThrowIfFailed(hr);
}

void RTreeRenderer::InitPS() {
    HRESULT hr = S_OK;
    auto d3dDev = this->dxDev->GetD3DDevice();

    this->ps = nullptr;
    this->psCBuffer = nullptr;

    auto psData = H::System::LoadPackageFile("ps.cso");

    hr = d3dDev->CreatePixelShader(psData.data(), psData.size(), nullptr, this->ps.GetAddressOf());
    H::System::ThrowIfFailed(hr);

    this->psCBuffer = this->CreateCBuffer(sizeof(DirectX::XMMATRIX));
}

void RTreeRenderer::InitRasterizerState() {
    HRESULT hr = S_OK;
    auto d3dDev = this->dxDev->GetD3DDevice();

    this->rsState = nullptr;

    D3D11_RASTERIZER_DESC desc;

    desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = FALSE;
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0.f;
    desc.DepthBiasClamp = 0.f;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = FALSE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;

    hr = d3dDev->CreateRasterizerState(&desc, this->rsState.GetAddressOf());
    H::System::ThrowIfFailed(hr);
}

void RTreeRenderer::InitDepthStencilState() {
    HRESULT hr = S_OK;
    auto d3dDev = this->dxDev->GetD3DDevice();

    this->dsState = nullptr;

    D3D11_DEPTH_STENCIL_DESC desc;

    desc.DepthEnable = FALSE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS;
    desc.StencilEnable = FALSE;
    desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace = desc.FrontFace;

    hr = d3dDev->CreateDepthStencilState(&desc, this->dsState.GetAddressOf());
    H::System::ThrowIfFailed(hr);
}

void RTreeRenderer::InitBlendState() {
    HRESULT hr = S_OK;
    auto d3dDev = this->dxDev->GetD3DDevice();

    this->blendState = nullptr;

    D3D11_BLEND_DESC desc;

    desc.AlphaToCoverageEnable = FALSE;
    desc.IndependentBlendEnable = FALSE;

    for (auto &i : desc.RenderTarget) {
        i.BlendEnable = FALSE;
    }

    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = d3dDev->CreateBlendState(&desc, this->blendState.GetAddressOf());
    H::System::ThrowIfFailed(hr);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> RTreeRenderer::CreateCBuffer(size_t size) {
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC desc;
    Microsoft::WRL::ComPtr<ID3D11Buffer> buf;
    auto d3dDev = this->dxDev->GetD3DDevice();

    desc.ByteWidth = (uint32_t)size;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    hr = d3dDev->CreateBuffer(&desc, nullptr, buf.GetAddressOf());
    H::System::ThrowIfFailed(hr);

    return buf;
}

void RTreeRenderer::Draw3D() {
    auto d3dCtx = this->dxDev->D3D();

    {
        DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();

        transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixTranslation(-1.f, 1.f, 2.f));

        auto outputSize = this->output->GetLogicalSize();

        auto proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.f), outputSize.x / outputSize.y, 0.1f, 10.f);

        auto mvp = DirectX::XMMatrixMultiplyTranspose(transform, proj);

        d3dCtx->UpdateSubresource(this->vsCBuffer.Get(), 0, nullptr, &mvp, 0, 0);
    }

    d3dCtx->ClearRenderTargetView(this->output->GetD3DRtView(), DirectX::Colors::Goldenrod);

    d3dCtx->IASetInputLayout(this->vsInputLayout.Get());

    {
        uint32_t offset = 0;

        d3dCtx->IASetVertexBuffers(0, 1, this->boxGeom.GetAddressOf(), &this->boxStride, &offset);
    }

    d3dCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dCtx->VSSetConstantBuffers(0, 1, this->vsCBuffer.GetAddressOf());
    d3dCtx->VSSetShader(this->vs.Get(), nullptr, 0);

    d3dCtx->PSSetConstantBuffers(0, 1, this->psCBuffer.GetAddressOf());
    d3dCtx->PSSetShader(this->ps.Get(), nullptr, 0);

    d3dCtx->RSSetState(this->rsState.Get());
    d3dCtx->OMSetDepthStencilState(this->dsState.Get(), 0);
    d3dCtx->OMSetBlendState(this->blendState.Get(), DirectX::Colors::White.f, 0xFF);

    d3dCtx->Draw(this->boxVertexCount, 0);
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