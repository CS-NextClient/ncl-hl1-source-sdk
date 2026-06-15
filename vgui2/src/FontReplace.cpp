#include "vgui_internal.h"
#include "FontReplace.h"

#include <vgui/ISurface.h>
#include <Color.h>

#include <Windows.h>
#include <GL/GL.h>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <optick.h>

#include "FontManager.h"
#include "Win32Font.h"

using namespace vgui2;

namespace
{
    HFont g_CurrentFont = 0;
    int g_TextX = 0;
    int g_TextY = 0;
    int g_TextColor[4] = { 255, 255, 255, 255 };
    bool g_AlphaTestWasEnabled = false;

    struct GlyphEntry
    {
        int textureID = 0;
        int wide = 0;
        int tall = 0;
        int a = 0;
        int b = 0;
        int c = 0;
        int yOffset = 0;
        bool computed = false; // textureID == 0 is a valid "no glyph" result, so a separate flag is needed
    };

    // the (font, ch) key is dense and bounded, so the hot range is a direct array, the rare tail a map
    struct FontGlyphCache
    {
        static const int kDirect = 0x500;
        GlyphEntry direct[kDirect];
        std::unordered_map<uint16_t, GlyphEntry> extended;
    };

    // indexed by HFont, lazily allocated
    std::vector<std::unique_ptr<FontGlyphCache>> g_GlyphCache;

    void BuildGlyph(GlyphEntry& e, HFont font, wchar_t ch)
    {
        font_t* glyphFont = FontManager().GetFontForChar(font, ch);
        if (glyphFont)
        {
            glyphFont->GetCharABCWidths(ch, e.a, e.b, e.c);
            e.tall = glyphFont->GetHeight();
            e.wide = glyphFont->GetUnderlined() ? (e.a + e.b + e.c) : e.b;

            // align a fallback face's baseline with the primary's
            font_t* primaryFont = FontManager().GetPrimaryFont(font);
            if (primaryFont)
            {
                e.yOffset = primaryFont->GetAscent() - glyphFont->GetAscent();
            }

            if (e.wide > 0 && e.tall > 0)
            {
                std::vector<unsigned char> rgba((size_t)e.wide * e.tall * 4, 0);
                glyphFont->GetCharRGBA(ch, 0, 0, e.wide, e.tall, rgba.data());
                e.textureID = g_pSurface->CreateNewTextureID(false);
                g_pSurface->DrawSetTextureRGBA(e.textureID, rgba.data(), e.wide, e.tall, 0, false);
            }
        }
        e.computed = true;
    }

    GlyphEntry& GetGlyph(HFont font, wchar_t ch)
    {
        if ((size_t)font >= g_GlyphCache.size())
        {
            g_GlyphCache.resize((size_t)font + 1);
        }
        if (!g_GlyphCache[font])
        {
            g_GlyphCache[font] = std::make_unique<FontGlyphCache>();
        }
        FontGlyphCache& fc = *g_GlyphCache[font];

        GlyphEntry& e = (ch < FontGlyphCache::kDirect) ? fc.direct[ch] : fc.extended[ch];
        if (!e.computed)
        {
            BuildGlyph(e, font, ch);
        }
        return e;
    }

    void DrawGlyph(wchar_t ch)
    {
        GlyphEntry& e = GetGlyph(g_CurrentFont, ch);
        if (e.textureID && e.wide > 0 && e.tall > 0)
        {
            int x = g_TextX + e.a;
            int y = g_TextY + e.yOffset;
            g_pSurface->DrawSetTexture(e.textureID);
            g_pSurface->DrawSetColor(g_TextColor[0], g_TextColor[1], g_TextColor[2], g_TextColor[3]);
            g_pSurface->DrawTexturedRect(x, y, x + e.wide, y + e.tall);
        }
        g_TextX += e.a + e.b + e.c;
    }

    // Set the text-draw GL state once per run; DrawSetTexture/DrawTexturedRect don't disturb it
    // between glyphs. The engine's HUD/2D pass leaves GL_ALPHA_TEST on, which would clip the glyph's
    // antialiased coverage, so it is disabled for the run and restored afterwards. glPushAttrib is
    // avoided: it is slow on modern drivers and would run on every text draw.
    void BeginTextRun(bool additive)
    {
        g_AlphaTestWasEnabled = glIsEnabled(GL_ALPHA_TEST) != GL_FALSE;
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, additive ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    void EndTextRun()
    {
        if (g_AlphaTestWasEnabled)
        {
            glEnable(GL_ALPHA_TEST);
        }
    }

    // target slots are __thiscall; __fastcall absorbs ECX=this, EDX=unused
    HFont __fastcall Hook_CreateFont(void*, void*)
    {
        return FontManager().CreateFont();
    }

    bool __fastcall Hook_AddGlyphSetToFont(void*, void*, HFont font, const char* name, int tall, int weight, int blur, int scanlines, int flags, int /*low*/, int /*high*/)
    {
        return FontManager().AddGlyphSetToFont(font, name, tall, weight, blur, scanlines, flags);
    }

    int __fastcall Hook_GetFontTall(void*, void*, HFont font)
    {
        return FontManager().GetFontTall(font);
    }

    void __fastcall Hook_GetCharABCwide(void*, void*, HFont font, int ch, int* a, int* b, int* c)
    {
        FontManager().GetCharABCwide(font, ch, *a, *b, *c);
    }

    int __fastcall Hook_GetCharacterWidth(void*, void*, HFont font, int ch)
    {
        return FontManager().GetCharacterWidth(font, ch);
    }

    void __fastcall Hook_GetTextSize(void*, void*, HFont font, const wchar_t* text, int* wide, int* tall)
    {
        FontManager().GetTextSize(font, text, *wide, *tall);
    }

    int __fastcall Hook_GetFontAscent(void*, void*, HFont font, wchar_t ch)
    {
        return FontManager().GetFontAscent(font, ch);
    }

    void __fastcall Hook_DrawSetTextFont(void*, void*, HFont font)
    {
        g_CurrentFont = font;
    }

    void __fastcall Hook_DrawSetTextColor_i(void*, void*, int r, int g, int b, int a)
    {
        g_TextColor[0] = r;
        g_TextColor[1] = g;
        g_TextColor[2] = b;
        g_TextColor[3] = a;
    }

    void __fastcall Hook_DrawSetTextColor_c(void*, void*, Color col)
    {
        g_TextColor[0] = col[0];
        g_TextColor[1] = col[1];
        g_TextColor[2] = col[2];
        g_TextColor[3] = col[3];
    }

    void __fastcall Hook_DrawSetTextPos(void*, void*, int x, int y)
    {
        g_TextX = x;
        g_TextY = y;
    }

    void __fastcall Hook_DrawGetTextPos(void*, void*, int* x, int* y)
    {
        *x = g_TextX;
        *y = g_TextY;
    }

    void __fastcall Hook_DrawPrintText(void*, void*, const wchar_t* text, int textLen)
    {
        OPTICK_EVENT();
        if (!text)
        {
            return;
        }
        BeginTextRun(false);
        for (int i = 0; i < textLen; i++)
        {
            DrawGlyph(text[i]);
        }
        EndTextRun();
    }

    void __fastcall Hook_DrawUnicodeChar(void*, void*, wchar_t ch)
    {
        BeginTextRun(false);
        DrawGlyph(ch);
        EndTextRun();
    }

    void __fastcall Hook_DrawUnicodeCharAdd(void*, void*, wchar_t ch)
    {
        BeginTextRun(true);
        DrawGlyph(ch);
        EndTextRun();
    }

    void __fastcall Hook_DrawFlushText(void*, void*)
    {
        // immediate-mode per-glyph drawing — nothing batched to flush
    }

    void PatchVtableEntry(void** vtable, int index, void* hook)
    {
        DWORD oldProtect;
        VirtualProtect(&vtable[index], sizeof(void*), PAGE_READWRITE, &oldProtect);
        vtable[index] = hook;
        VirtualProtect(&vtable[index], sizeof(void*), oldProtect, &oldProtect);
    }
}

namespace vgui2
{
    void FontReplace_Install(ISurface* surface)
    {
        if (!surface)
        {
            return;
        }

        void** vt = *reinterpret_cast<void***>(surface);

        // vtable indices from ISurface.h (VGUI_Surface026); slot 0 is the virtual destructor
        PatchVtableEntry(vt, 13, (void*)&Hook_DrawSetTextFont);
        // MSVC lays overloaded virtuals out in reverse order: DrawSetTextColor(Color) at 14, (int...) at 15
        PatchVtableEntry(vt, 14, (void*)&Hook_DrawSetTextColor_c);
        PatchVtableEntry(vt, 15, (void*)&Hook_DrawSetTextColor_i);
        PatchVtableEntry(vt, 16, (void*)&Hook_DrawSetTextPos);
        PatchVtableEntry(vt, 17, (void*)&Hook_DrawGetTextPos);
        PatchVtableEntry(vt, 18, (void*)&Hook_DrawPrintText);
        PatchVtableEntry(vt, 19, (void*)&Hook_DrawUnicodeChar);
        PatchVtableEntry(vt, 20, (void*)&Hook_DrawUnicodeCharAdd);
        PatchVtableEntry(vt, 21, (void*)&Hook_DrawFlushText);
        PatchVtableEntry(vt, 59, (void*)&Hook_CreateFont);
        PatchVtableEntry(vt, 60, (void*)&Hook_AddGlyphSetToFont);
        PatchVtableEntry(vt, 62, (void*)&Hook_GetFontTall);
        PatchVtableEntry(vt, 63, (void*)&Hook_GetCharABCwide);
        PatchVtableEntry(vt, 64, (void*)&Hook_GetCharacterWidth);
        PatchVtableEntry(vt, 65, (void*)&Hook_GetTextSize);
        PatchVtableEntry(vt, 89, (void*)&Hook_GetFontAscent);
    }
}
