#pragma once
#include "RTreeRenderer.h"

#include <memory>
#include <libhelpers/Window/Window.h>
#include <libhelpers/Dx/Renderer/HwndRenderer.h>

class RTreeWindow : public Window {
public:
    RTreeWindow(WindowBaseData &baseData);

    RTreeRenderer *GetRenderer();

    ProcessMsgResult ProcessMsg(uint32_t msg, WPARAM wparam, LPARAM lparam) override;

private:
    std::unique_ptr<HwndRenderer<RTreeRenderer>> renderer;
};