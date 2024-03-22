//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include <cstdio>
#include <cstring>

#include "vgui/IPanel.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"

#include "vgui_border.h"
#include "KeyValues.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui2;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
Border::Border()
{
    _inset[0]=0;
    _inset[1]=0;
    _inset[2]=0;
    _inset[3]=0;
    _name = NULL;
    m_eBackgroundType = Border::BACKGROUND_FILLED;

    memset(_sides, 0, sizeof(_sides));
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
Border::~Border()
{
    delete [] _name;

    for (int i = 0; i < 4; i++)
    {
        delete [] _sides[i].lines;
    }
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Border::SetInset(int left,int top,int right,int bottom)
{
    _inset[SIDE_LEFT] = left;
    _inset[SIDE_TOP] = top;
    _inset[SIDE_RIGHT] = right;
    _inset[SIDE_BOTTOM] = bottom;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Border::GetInset(int& left,int& top,int& right,int& bottom)
{
    left = _inset[SIDE_LEFT];
    top = _inset[SIDE_TOP];
    right = _inset[SIDE_RIGHT];
    bottom = _inset[SIDE_BOTTOM];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Border::Paint(int x, int y, int wide, int tall)
{
    Paint(x, y, wide, tall, -1, 0, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Draws the border with the specified size
//-----------------------------------------------------------------------------
void Border::Paint(int x, int y, int wide, int tall, int breakSide, int breakStart, int breakEnd)
{
    // iterate through and draw all lines
    // draw left
    for (int i = 0; i < _sides[SIDE_LEFT].count; i++)
    {
        line_t *line = &(_sides[SIDE_LEFT].lines[i]);
        surface()->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);

        int start_offset, end_offset;
        GetAdjustedLineOffset(line, SIDE_LEFT, start_offset, end_offset);

        if (breakSide == SIDE_LEFT)
        {
            // split into two section
            if (breakStart > 0)
            {
                // draw before the break Start
                surface()->DrawFilledRect(x + i,                    y + start_offset,
                                          x + i + kBorderThickness, y + breakStart);
            }

            if (breakEnd < (tall - end_offset))
            {
                // draw after break end
                surface()->DrawFilledRect(x + i,                    y + breakEnd + kBorderThickness,
                                          x + i + kBorderThickness, tall - end_offset);
            }
        }
        else
        {
            surface()->DrawFilledRect(x + i,                    y + start_offset,
                                      x + i + kBorderThickness, tall - end_offset);
        }
    }


    // draw top
    for (int i = 0; i < _sides[SIDE_TOP].count; i++)
    {
        line_t *line = &(_sides[SIDE_TOP].lines[i]);
        surface()->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);

        int start_offset, end_offset;
        GetAdjustedLineOffset(line, SIDE_TOP, start_offset, end_offset);

        if (breakSide == SIDE_TOP)
        {
            // split into two section
            if (breakStart > 0)
            {
                // draw before the break Start
                surface()->DrawFilledRect(x + start_offset, y + i,
                                          x + breakStart,   y + i + kBorderThickness);
            }

            if (breakEnd < (wide - end_offset))
            {
                // draw after break end
                surface()->DrawFilledRect(x + breakEnd + kBorderThickness,  y + i,
                                          wide - end_offset,                y + i + kBorderThickness);
            }
        }
        else
        {
            surface()->DrawFilledRect(x + start_offset,  y + i,
                                      wide - end_offset, y + i + kBorderThickness);
        }
    }


    // draw right
    for (int i = 0; i < _sides[SIDE_RIGHT].count; i++)
    {
        line_t *line = &(_sides[SIDE_RIGHT].lines[i]);
        surface()->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);

        int start_offset, end_offset;
        GetAdjustedLineOffset(line, SIDE_RIGHT, start_offset, end_offset);

        surface()->DrawFilledRect(wide - i - kBorderThickness,  y + start_offset,
                                  wide - i,                     tall - end_offset);
    }


    // draw bottom
    for (int i = 0; i < _sides[SIDE_BOTTOM].count; i++)
    {
        line_t *line = &(_sides[SIDE_BOTTOM].lines[i]);
        surface()->DrawSetColor(line->col[0], line->col[1], line->col[2], line->col[3]);

        int start_offset, end_offset;
        GetAdjustedLineOffset(line, SIDE_BOTTOM, start_offset, end_offset);

        surface()->DrawFilledRect(x + start_offset,     tall - i - kBorderThickness,
                                  wide - end_offset,    tall - i);
    }

    PaintCorners(x, y, wide, tall);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Border::Paint(VPANEL panel)
{
    // get panel size
    int wide, tall;

    Panel* p = ipanel()->GetPanel(panel, "GameUI"); //TODO move GameUI in defines ?
    p->GetSize(wide, tall);
    Paint(0, 0, wide, tall, -1, 0, 0);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Border::ApplySchemeSettings(IScheme *pScheme, KeyValues *inResourceData)
{
    // load inset information
    const char *insetString = inResourceData->GetString("inset", "0 0 0 0");

    int left, top, right, bottom;
    GetInset(left, top, right, bottom);
    sscanf(insetString, "%d %d %d %d", &left, &top, &right, &bottom);
    SetInset(left, top, right, bottom);

    // get the border information from the scheme
    ParseSideSettings(SIDE_LEFT, inResourceData->FindKey("Left"),pScheme);
    ParseSideSettings(SIDE_TOP, inResourceData->FindKey("Top"),pScheme);
    ParseSideSettings(SIDE_RIGHT, inResourceData->FindKey("Right"),pScheme);
    ParseSideSettings(SIDE_BOTTOM, inResourceData->FindKey("Bottom"),pScheme);

    m_eBackgroundType = (backgroundtype_e)inResourceData->GetInt("backgroundtype");
}

//-----------------------------------------------------------------------------
// Purpose: parses scheme data
//-----------------------------------------------------------------------------
void Border::ParseSideSettings(int side_index, KeyValues *inResourceData, IScheme *pScheme)
{
    if (!inResourceData)
        return;

    // count the numeber of lines in the side
    int count = 0;
    KeyValues *kv;
    for (kv = inResourceData->GetFirstSubKey(); kv != NULL; kv = kv->GetNextKey())
    {
        count++;
    }

    // allocate memory
    _sides[side_index].count = count;
    _sides[side_index].lines = new line_t[count];

    // iterate through the keys
    //!! this loads in order, ignoring key names
    int index = 0;
    for (kv = inResourceData->GetFirstSubKey(); kv != NULL; kv = kv->GetNextKey())
    {
        line_t *line = &(_sides[side_index].lines[index]);

        // this is the color name, get that from the color table
        const char *col = kv->GetString("color", NULL);
        line->col = pScheme->GetColor(col, Color(0, 0, 0, 0));

        col = kv->GetString("offset", NULL);
        int Start = 0, end = 0;
        if (col)
        {
            sscanf(col, "%d %d", &Start, &end);
        }
        line->startOffset = Start;
        line->endOffset = end;

        index++;
    }
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
const char *Border::GetName()
{
    if (_name)
        return _name;
    return "";
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void Border::SetName(const char *name)
{
    if (_name)
    {
        delete [] _name;
    }

    int len = Q_strlen(name) + 1;
    _name = new char[ len ];
    Q_strncpy( _name, name, len );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
Border::backgroundtype_e Border::GetBackgroundType()
{
    return m_eBackgroundType;
}

void Border::SetBorderCornerTextures(int cornerBorderTexture1, int cornerBorderTexture2, int cornerBorderTexture3, int cornerBorderTexture4,
                                     int cornerBorderHalfTexture1, int cornerBorderHalfTexture2, int cornerBorderHalfTexture3, int cornerBorderHalfTexture4)
{
    _cornerBorderTexture1 = cornerBorderTexture1;
    _cornerBorderTexture2 = cornerBorderTexture2;
    _cornerBorderTexture3 = cornerBorderTexture3;
    _cornerBorderTexture4 = cornerBorderTexture4;
    _cornerBorderTexturesIsValid = _cornerBorderTexture1 != -1 && _cornerBorderTexture2 != -1 && _cornerBorderTexture3 != -1 && _cornerBorderTexture4 != -1;

    _cornerBorderHalfTexture1 = cornerBorderHalfTexture1;
    _cornerBorderHalfTexture2 = cornerBorderHalfTexture2;
    _cornerBorderHalfTexture3 = cornerBorderHalfTexture3;
    _cornerBorderHalfTexture4 = cornerBorderHalfTexture4;
}

void Border::PaintCorners(int x, int y, int wide, int tall)
{
    if (m_eBackgroundType == BACKGROUND_ROUNDEDCORNERS && _cornerBorderTexturesIsValid)
    {
        PaintCorner(Corner::TopLeft, x, y, _cornerBorderTexture1, _cornerBorderHalfTexture1);
        PaintCorner(Corner::TopRight, wide, y, _cornerBorderTexture2, _cornerBorderHalfTexture2);
        PaintCorner(Corner::BottomRight, wide, tall, _cornerBorderTexture3, _cornerBorderHalfTexture3);
        PaintCorner(Corner::BottomLeft, x, tall, _cornerBorderTexture4, _cornerBorderHalfTexture4);
    }
}

void Border::PaintCorner(Corner corner, int x, int y, int texture, int texture_half)
{
    sides_e side1;
    sides_e side2;

    if (corner == Corner::TopLeft)
    {
        side1 = SIDE_LEFT;
        side2 = SIDE_TOP;
    }
    else if (corner == Corner::TopRight)
    {
        side1 = SIDE_TOP;
        side2 = SIDE_RIGHT;
    }
    else if (corner == Corner::BottomRight)
    {
        side1 = SIDE_RIGHT;
        side2 = SIDE_BOTTOM;
    }
    else if (corner == Corner::BottomLeft)
    {
        side1 = SIDE_BOTTOM;
        side2 = SIDE_LEFT;
    }

    if (_sides[side1].count && _sides[side2].count)
    {
        line_t *line1 = &(_sides[side1].lines[0]);
        line_t *line2 = &(_sides[side2].lines[0]);

        surface()->DrawSetColor(line1->col[0], line1->col[1], line1->col[2], line1->col[3]);
        DrawTexture(texture, x, y, corner);

        if (line1->col != line2->col && texture_half != -1)
        {
            surface()->DrawSetColor(line2->col[0], line2->col[1], line2->col[2], line2->col[3]);
            DrawTexture(texture_half, x, y, corner);
        }
    }
}

void Border::GetAdjustedLineOffset(const line_t* line, sides_e side, int& start_offset_out, int& end_offset_out)
{
    start_offset_out = line->startOffset;
    end_offset_out = line->endOffset;

    if (m_eBackgroundType == BACKGROUND_ROUNDEDCORNERS && _cornerBorderTexturesIsValid)
    {
        int w, h;
        if (side == SIDE_TOP || side == SIDE_BOTTOM)
        {
            surface()->DrawGetTextureSize(side == SIDE_TOP ? _cornerBorderTexture1 : _cornerBorderTexture4, w, h);
            start_offset_out += w;

            surface()->DrawGetTextureSize(side == SIDE_TOP ? _cornerBorderTexture2 : _cornerBorderTexture3, w, h);
            end_offset_out += w;
        }
        else
        {
            surface()->DrawGetTextureSize(side == SIDE_LEFT ? _cornerBorderTexture1 : _cornerBorderTexture2, w, h);
            start_offset_out += h;

            surface()->DrawGetTextureSize(side == SIDE_LEFT ? _cornerBorderTexture4 : _cornerBorderTexture3, w, h);
            end_offset_out += h;
        }
    }
}

void Border::DrawTexture(int texture, int x, int y, Corner pivot)
{
    if (texture == -1)
        return;

    int w, h;
    surface()->DrawGetTextureSize(texture, w, h);

    if (pivot == Corner::TopRight || pivot == Corner::BottomRight)
        x -= w;

    if (pivot == Corner::BottomRight || pivot == Corner::BottomLeft)
        y -= h;

    surface()->DrawSetTexture(texture);
    surface()->DrawTexturedRect(x, y, x + w, y + h);
}

