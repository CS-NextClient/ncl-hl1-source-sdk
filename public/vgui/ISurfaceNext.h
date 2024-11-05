#pragma once
#include "ISurface.h"

namespace vgui2
{
    class ISurfaceNext : public ISurface
    {
    public:
        [[nodiscard]] virtual float DrawGetAlphaMultiplier() const = 0;
        virtual void DrawSetAlphaMultiplier(float a) = 0;
    };

    #define VGUI_SURFACE_NEXT_INTERFACE_VERSION "VGUI_SurfaceNext001"
}
