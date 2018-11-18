#pragma once
#include "Camera.h"
#include "Triangle.h"
#include "IInputController.h"
#include "../RTree.h"

#include <array>
#include <libhelpers/StopWatch.h>
#include <libhelpers/Dx/Renderer/IRenderer.h>

struct Transform {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 scale;
};

struct VSCBuf {
    DirectX::XMMATRIX mvp;
    std::array<float, 4> color;
};

class RTreeRenderer : public IRenderer {
public:
    RTreeRenderer(DxDevice *dxDev, IOutput *output,
        std::unique_ptr<IInputController> inputController);

    void Render() override;
    void OutputParametersChanged() override;

    void SetTree(RTree tree);

private:
    thread::critical_section cs;
    std::shared_ptr<RTree> rtree;
    bool initDrawTree;

    StopWatch watch;
    Camera camera;
    std::unique_ptr<IInputController> inputController;
    std::vector<DirectX::XMFLOAT3> cubeVertices;
    std::vector<uint32_t> cubeIndexes;
    std::vector<Triangle> transformedCubeGeom;
    std::vector<std::vector<Transform>> nodeRects;
    Microsoft::WRL::ComPtr<ID3D11Buffer> boxGeom;
    uint32_t boxStride;
    uint32_t boxVertexCount;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> vsInputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsCBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
    Microsoft::WRL::ComPtr<ID3D11Buffer> psCBuffer;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsStateDrawBack;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rsStateDrawFront;
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
    void SetCubeState();
    void DrawCube(
        const Transform &tr,
        const DirectX::XMMATRIX &view,
        const float (&color)[4]);
    void TransformNodes(const DirectX::XMVECTOR &camPos);
    void TransformBox(const Transform &tr);
    void SortTriangles(const DirectX::XMVECTOR &camPos);

    static void FillRects(int level, const std::shared_ptr<const Node> &node, std::vector<D2D1_RECT_F> &rects);
};