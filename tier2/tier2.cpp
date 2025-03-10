#define TIER2_GAMEUI_INTERNALS

#include <tier2/tier2.h>
#include <FileSystem.h>
#include <IBaseUI.h>
#include <IEngineVGui.h>
#include <IGameUIFuncs.h>
#include <IGameConsole.h>
#include <IGameUI.h>
#include <vgui/IVGui.h>
#include <vgui/IInputInternal.h>
#include <vgui/IPanel.h>
#include <vgui/ILocalize.h>
#include <vgui/ISchemeNext.h>
#include <vgui/ISurfaceNext.h>
#include <vgui/ISystem.h>
#include "KeyValuesCompat.h"

IFileSystem *g_pFullFileSystem;
IBaseUI *g_pBaseUI;
IEngineVGui *g_pEngineVGui;
IGameUIFuncs *g_pGameUIFuncs;
IGameConsole *g_pGameConsole;
IGameUI *g_pGameUI;
vgui2::ISurfaceNext *g_pVGuiSurface;
vgui2::IInputInternal *g_pVGuiInput;
vgui2::IVGui *g_pVGui;
vgui2::IPanel *g_pVGuiPanel;
vgui2::ILocalize *g_pVGuiLocalize;
vgui2::ISchemeManagerNext *g_pVGuiSchemeManager;
vgui2::ISystem *g_pVGuiSystem;

static bool s_bConnected = false;
void SteamAPI_InitForGoldSrc();

void ConnectTier2Libraries(CreateInterfaceFn *pFactoryList, int nFactoryCount)
{
	if (s_bConnected)
		return;

	s_bConnected = true;

	if (!KV_InitKeyValuesSystem(pFactoryList, nFactoryCount))
	{
		Error(_T("tier2: Failed to initialize KeyValues\n"));
		Assert(false);
	}

	for (int i = 0; i < nFactoryCount; ++i)
	{
		if (!g_pFullFileSystem)
		{
			g_pFullFileSystem = (IFileSystem *)pFactoryList[i](FILESYSTEM_INTERFACE_VERSION, NULL);
		}
		if (!g_pBaseUI)
		{
			g_pBaseUI = (IBaseUI *)pFactoryList[i](IBASEUI_NAME, NULL);
		}
		if (!g_pEngineVGui)
		{
			g_pEngineVGui = (IEngineVGui *)pFactoryList[i](VENGINE_VGUI_VERSION, NULL);
		}
		if (!g_pGameUIFuncs)
		{
			g_pGameUIFuncs = (IGameUIFuncs *)pFactoryList[i](IGAMEUIFUNCS_NAME, NULL);
		}
		if (!g_pGameConsole)
		{
			g_pGameConsole = (IGameConsole *)pFactoryList[i](GAMECONSOLE_INTERFACE_VERSION_GS, NULL);
		}
		if (!g_pGameUI)
		{
			g_pGameUI = (IGameUI *)pFactoryList[i](GAMEUI_INTERFACE_VERSION_GS, NULL);
		}
		if (!g_pVGuiSurface)
		{
			g_pVGuiSurface = (vgui2::ISurfaceNext *)pFactoryList[i](VGUI_SURFACE_NEXT_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiInput)
		{
			g_pVGuiInput = (vgui2::IInputInternal *)pFactoryList[i](VGUI_INPUTINTERNAL_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGui)
		{
			g_pVGui = (vgui2::IVGui *)pFactoryList[i](VGUI_IVGUI_INTERFACE_VERSION_GS, NULL);
		}
		if (!g_pVGuiPanel)
		{
			g_pVGuiPanel = (vgui2::IPanel *)pFactoryList[i](VGUI_PANEL_INTERFACE_VERSION_GS, NULL);
		}
		if (!g_pVGuiLocalize)
		{
			g_pVGuiLocalize = (vgui2::ILocalize *)pFactoryList[i](VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiSchemeManager)
		{
			g_pVGuiSchemeManager = (vgui2::ISchemeManagerNext *)pFactoryList[i](VGUI_SCHEME_NEXT_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiSystem)
		{
			g_pVGuiSystem = (vgui2::ISystem *)pFactoryList[i](VGUI_SYSTEM_INTERFACE_VERSION_GS, NULL);
		}
	}
}

void DisconnectTier2Libraries()
{
	if (!s_bConnected)
		return;

    g_pFullFileSystem = nullptr;
    g_pBaseUI = nullptr;
    g_pEngineVGui = nullptr;
    g_pGameUIFuncs = nullptr;
    g_pGameConsole = nullptr;
    g_pGameUI = nullptr;
    g_pVGuiSurface = nullptr;
    g_pVGuiInput = nullptr;
    g_pVGui = nullptr;
    g_pVGuiPanel = nullptr;
    g_pVGuiLocalize = nullptr;
    g_pVGuiSchemeManager = nullptr;
    g_pVGuiSystem = nullptr;

    KV_UninitializeKeyValuesSystem();

	s_bConnected = false;
}
