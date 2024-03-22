#pragma once

#include "IImage.h"
#include "Color.h"

class ImageEx : public vgui2::IImage
{
    vgui2::IImage* image_;
    SurfaceEx* surface_ex_;

public:
    explicit ImageEx(SurfaceEx* surface_ex, vgui2::IImage* image);

    void Paint() override;
    void SetPos(int x, int y) override;
    void GetContentSize(int &wide, int &tall) override;
    void GetSize(int &wide, int &tall) override;
    void SetSize(int wide, int tall) override;
    void SetColor(Color col) override;
};