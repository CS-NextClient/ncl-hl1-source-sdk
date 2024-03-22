//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef IBORDEREX_H
#define IBORDEREX_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI2.h>
#include <vgui/IBorder.h>

namespace vgui2
{

class IBorderEx : public IBorder
{
public:
	enum backgroundtype_e
	{
		BACKGROUND_FILLED,
		BACKGROUND_TEXTURED,
		BACKGROUND_ROUNDEDCORNERS,
	};

	virtual backgroundtype_e GetBackgroundType() = 0;

    // Specifies the textures that will be used to draw the borders. Half textures are needed for blending borders in corners, if border colors are different. If there are no Half textures, you can pass -1.
    virtual void SetBorderCornerTextures(int cornerBorderTexture1, int cornerBorderTexture2, int cornerBorderTexture3, int cornerBorderTexture4,
                                         int cornerBorderHalfTexture1, int cornerBorderHalfTexture2, int cornerBorderHalfTexture3, int cornerBorderHalfTexture4) = 0;
};

} // namespace vgui2


#endif // IBORDEREX_H
