//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: Core implementation of vgui
//
// $NoKeywords: $
//=============================================================================//

#ifndef VGUI_BORDER_H
#define VGUI_BORDER_H

#ifdef _WIN32
#pragma once
#endif

//#include <vgui/vg.h>
#include <vgui/IBorder.h>
#include <vgui/IBorderEx.h>
#include <vgui/ISchemeNext.h>
#include <Color.h>

class KeyValues;

namespace vgui2
{
    class Border : public IBorderEx
    {
        enum class Corner
        {
            TopLeft,
            TopRight,
            BottomRight,
            BottomLeft
        };

        // border drawing description
        struct line_t
        {
            Color col;
            int startOffset;
            int endOffset;
        };

        struct side_t
        {
            int count;
            line_t* lines;
        };

        const int kBorderThickness = 1;

    public:
        Border();
        ~Border();

        void Paint(VPANEL panel) override;
        void Paint(int x0, int y0, int x1, int y1) override;
        void Paint(int x0, int y0, int x1, int y1, int breakSide, int breakStart, int breakStop) override;
        void SetInset(int left, int top, int right, int bottom) override;
        void GetInset(int& left, int& top, int& right, int& bottom) override;

        void ApplySchemeSettings(IScheme* pScheme, KeyValues* inResourceData) override;

        const char* GetName() override;
        void SetName(const char* name) override;

        backgroundtype_e GetBackgroundType() override;
        void SetBorderCornerTextures(int cornerBorderTexture1, int cornerBorderTexture2, int cornerBorderTexture3, int cornerBorderTexture4,
                                     int cornerBorderHalfTexture1, int cornerBorderHalfTexture2, int cornerBorderHalfTexture3, int cornerBorderHalfTexture4) override;

    protected:
        int _inset[4];

    private:
        // protected copy constructor to prevent use
        Border(Border&) = default;

        void PaintCorners(int x, int y, int wide, int tall);
        void PaintCorner(Corner corner, int x, int y, int texture, int texture_half);
        void GetAdjustedLineOffset(const line_t* line, sides_e side, int& start_offset_out, int& end_offset_out);

        void ParseSideSettings(int side_index, KeyValues* inResourceData, IScheme* pScheme);

        static void DrawTexture(int texture, int x, int y, Corner pivot);

        char* _name;
        side_t _sides[4]; // left, top, right, bottom
        backgroundtype_e m_eBackgroundType;
        int _cornerBorderTexture1 = -1;
        int _cornerBorderTexture2 = -1;
        int _cornerBorderTexture3 = -1;
        int _cornerBorderTexture4 = -1;
        int _cornerBorderHalfTexture1 = -1;
        int _cornerBorderHalfTexture2 = -1;
        int _cornerBorderHalfTexture3 = -1;
        int _cornerBorderHalfTexture4 = -1;
        bool _cornerBorderTexturesIsValid = false;

        friend class VPanel;
    };
} // namespace vgui2

#endif // VGUI_BORDER_H
