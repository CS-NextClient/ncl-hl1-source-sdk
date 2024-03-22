//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#if defined( WIN32 ) && !defined( _X360 )
#undef ARRAYSIZE
#include <wtypes.h>
#include <winuser.h>
#include "xbox/xboxstubs.h"
#endif
#include "tier0/dbg.h"
#include "vgui_key_translation.h"
#if defined( _X360 )
#include "xbox/xbox_win32stubs.h"
#endif
#ifdef POSIX
#define VK_RETURN -1
#endif

#include "tier2/tier2.h"
#include "vgui/ISystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui2;

KeyCode KeyCode_VirtualKeyToVGUI( int key )
{
	return system()->KeyCode_VirtualKeyToVGUI( key );
}

int KeyCode_VGUIToVirtualKey( KeyCode code )
{
	auto vk = system()->KeyCode_VGUIToVirtualKey(code);

	switch(code) {
		case KEY_LEFT: vk = VK_LEFT; break;
		case KEY_RIGHT: vk = VK_RIGHT; break;
		case KEY_PERIOD: vk = VK_OEM_PERIOD; break;
		case KEY_MINUS: vk = VK_OEM_MINUS; break;
	}
	
	return vk;
}
