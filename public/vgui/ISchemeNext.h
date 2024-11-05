#pragma once
#include "IScheme.h"

namespace vgui2
{
    class ISchemeManagerNext : public ISchemeManager
    {
    public:
        // loads a scheme from a file
        // first scheme loaded becomes the default scheme, and all subsequent loaded scheme are derivitives of that
        [[deprecated]] virtual HScheme LoadSchemeFromFileEx(VPANEL sizingPanel, const char *fileName, const char *tag) = 0;

        // gets the proportional coordinates for doing screen-size independant panel layouts
        // use these for font, image and panel size scaling (they all use the pixel height of the display for scaling)
        [[deprecated]] virtual int GetProportionalScaledValueEx(HScheme scheme, int normalizedValue) = 0;
        [[deprecated]] virtual int GetProportionalNormalizedValueEx(HScheme scheme, int scaledValue) = 0;

        // Path must be writable. See LoadSchemeFromFilePath in vgui/vgui_controls/controls.cpp.
        virtual HScheme LoadSchemeFromFilePath(const char *fileName, const char *pathID, const char *tag) = 0;
    };

    #define VGUI_SCHEME_NEXT_INTERFACE_VERSION "VGUI_SchemeNext001"
}
