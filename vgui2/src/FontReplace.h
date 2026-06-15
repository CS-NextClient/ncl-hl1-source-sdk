#pragma once

#include <vgui/ISurface.h>

namespace vgui2
{
    // Replaces the engine surface's font/text path (creation, metrics, glyph rendering) with
    // vgui2_surfacelib's CFontManager by patching the surface vtable. Routes all VGUI text
    // (engine EngineFont, client HUD, gameui) through CFontManager, enabling per-glyph fallback.
    // Call once after g_pSurface is obtained.
    void FontReplace_Install(ISurface* surface);
}
