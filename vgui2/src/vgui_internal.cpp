//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: Core implementation of vgui
//
// $NoKeywords: $
//=============================================================================//

#include "vgui_internal.h"

#include <vgui/ISurfaceNext.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <FileSystem.h>

#include <Windows.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace vgui2
{
    IPanel* g_pIPanel = nullptr;

    ISurface* g_pSurface = nullptr;
    IFileSystem* g_pFullFileSystem = nullptr;
    ILocalize* g_pVGuiLocalize = nullptr;

    extern void* g_MainWindow = nullptr;

    //-----------------------------------------------------------------------------
    // Purpose:
    // Output : Returns true on success, false on failure.
    //-----------------------------------------------------------------------------
    bool VGui_InternalLoadInterfaces(CreateInterfaceFn* factoryList, int numFactories)
    {
        g_pIPanel = (IPanel*)Sys_GetFactoryThis()(VGUI_PANEL_INTERFACE_VERSION_GS, nullptr);

        // loads all the interfaces
        g_pSurface = (ISurface*)InitializeInterface(VGUI_SURFACE_INTERFACE_VERSION_GS, factoryList, numFactories);
        g_pFullFileSystem = (IFileSystem*)InitializeInterface(FILESYSTEM_INTERFACE_VERSION, factoryList, numFactories);
        g_pVGuiLocalize = (ILocalize*)InitializeInterface(VGUI_LOCALIZE_INTERFACE_VERSION, factoryList, numFactories);

        if (!g_pSurface || !g_pFullFileSystem || !g_pVGuiLocalize)
        {
            Error("IVGui unable to connect to required interfaces!\n");
            return false;
        }

        g_MainWindow = GetActiveWindow();

        return true;
    }
}