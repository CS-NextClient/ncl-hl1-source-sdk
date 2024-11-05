//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: Wraps pointers to basic vgui interfaces
//
// $NoKeywords: $
//===========================================================================//

#pragma once

#include <vgui/IInputInternal.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/ISchemeNext.h>
#include <vgui/ISurfaceNext.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

#include "interface.h"

namespace vgui2
{
    bool VGui_InternalLoadInterfaces(CreateInterfaceFn* factoryList, int numFactories);

    extern IInputInternal* g_pInput;
    extern ISchemeManager* g_pScheme;
    extern ISurface* g_pSurface;
    extern ISurfaceNext* g_pSurfaceNext;
    extern ISystem* g_pSystem;
    extern IVGui* g_pIVgui;
    extern IPanel* g_pIPanel;
    extern IFileSystem* g_pFullFileSystem;
    extern ILocalize* g_pVGuiLocalize;

    extern void* g_MainWindow;

    // methods
    void vgui_strcpy(char* dst, int dstLen, const char* src);
}
