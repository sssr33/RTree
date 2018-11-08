#pragma once
#include "../RTree.h"

#include <libhelpers/Dx/Renderer/IRenderer.h>

struct Transform {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 scale;
};

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

    std::vector<std::vector<Transform>> nodeRects;
    Microsoft::WRL::ComPtr<ID3D11Buffer> boxGeom;
    uint32_t boxStride;
    uint32_t boxVertexCount;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> vsInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
    Microsoft::WRL::ComPtr<ID3D11Buffer> psCBuffer;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dsState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

    void InitDrawTree(const RTree &tree);
    void DrawTree(const RTree &tree);

    void DrawSimple(const RTree &tree);

    void Init3D(const RTree &tree);
    void InitNodeRects(const std::shared_ptr<const Node> &node, size_t level = 0, float height = 1.f);
    void InitBoxVertices();
    void InitPS();
    void InitRasterizerState();
    void InitDepthStencilState();
    void InitBlendState();

    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateCBuffer(size_t size);

    void Draw3D();

    static void FillRects(int level, const std::shared_ptr<const Node> &node, std::vector<D2D1_RECT_F> &rects);
};