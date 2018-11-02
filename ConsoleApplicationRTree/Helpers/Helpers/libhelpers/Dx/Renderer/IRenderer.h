#pragma once
#include "libhelpers\config.h"
#include "IOutput.h"
#include "../Dx.h"

class IRenderer {
public:
    IRenderer(DxDevice *dxDev, IOutput *output);
    virtual ~IRenderer();

    virtual void Render() = 0;
    virtual void OutputParametersChanged() = 0;


protected:
    DxDevice * dxDev;
    IOutput *output;
};