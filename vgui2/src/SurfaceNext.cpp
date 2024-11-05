#include <Windows.h>
#include <GL/GL.h>

#include <vgui/ISurfaceNext.h>
#include <Color.h>

#include "vgui_internal.h"

using namespace vgui2;

class SurfaceNext : public ISurfaceNext
{
    float alpha_mul_ = 1;

public:
    void Shutdown() override
    {
        g_pSurface->Shutdown();
    }

    void RunFrame() override
    {
        g_pSurface->RunFrame();
    }

    VPANEL GetEmbeddedPanel() override
    {
        return g_pSurface->GetEmbeddedPanel();
    }

    void SetEmbeddedPanel(VPANEL pPanel) override
    {
        g_pSurface->SetEmbeddedPanel(pPanel);
    }

    void PushMakeCurrent(VPANEL panel, bool useInsets) override
    {
        g_pSurface->PushMakeCurrent(panel, useInsets);
    }

    void PopMakeCurrent(VPANEL panel) override
    {
        g_pSurface->PopMakeCurrent(panel);
    }

    void DrawSetColor(int r, int g, int b, int a) override
    {
        ApplyAlphaMultiplier(a);
        g_pSurface->DrawSetColor(r, g, b, a);
    }

    void DrawSetColor(Color col) override
    {
        ApplyAlphaMultiplier(col);
        g_pSurface->DrawSetColor(col);
    }

    void DrawFilledRect(int x0, int y0, int x1, int y1) override
    {
        g_pSurface->DrawFilledRect(x0, y0, x1, y1);
    }

    void DrawOutlinedRect(int x0, int y0, int x1, int y1) override
    {
        g_pSurface->DrawOutlinedRect(x0, y1, x1, y1);
    }

    void DrawLine(int x0, int y0, int x1, int y1) override
    {
        g_pSurface->DrawLine(x0, y0, x1, y1);
    }

    void DrawPolyLine(int* px, int* py, int numPoints) override
    {
        g_pSurface->DrawPolyLine(px, py, numPoints);
    }

    void DrawSetTextFont(HFont font) override
    {
        g_pSurface->DrawSetTextFont(font);
    }

    void DrawSetTextColor(int r, int g, int b, int a) override
    {
        ApplyAlphaMultiplier(a);
        g_pSurface->DrawSetTextColor(r, g, b, a);
    }

    void DrawSetTextColor(Color col) override
    {
        ApplyAlphaMultiplier(col);
        g_pSurface->DrawSetTextColor(col);
    }

    void DrawSetTextPos(int x, int y) override
    {
        g_pSurface->DrawSetTextPos(x, y);
    }

    void DrawGetTextPos(int& x, int& y) override
    {
        g_pSurface->DrawGetTextPos(x, y);
    }

    void DrawPrintText(const wchar_t* text, int textLen) override
    {
        g_pSurface->DrawPrintText(text, textLen);
    }

    void DrawUnicodeChar(wchar_t wch) override
    {
        g_pSurface->DrawUnicodeChar(wch);
    }

    void DrawUnicodeCharAdd(wchar_t wch) override
    {
        g_pSurface->DrawUnicodeCharAdd(wch);
    }

    void DrawFlushText() override
    {
        g_pSurface->DrawFlushText();
    }

    IHTML* CreateHTMLWindow(IHTMLEvents* events, VPANEL context) override
    {
        return g_pSurface->CreateHTMLWindow(events, context);
    }

    void PaintHTMLWindow(IHTML* htmlwin) override
    {
        g_pSurface->PaintHTMLWindow(htmlwin);
    }

    void DeleteHTMLWindow(IHTML* htmlwin) override
    {
        g_pSurface->DeleteHTMLWindow(htmlwin);
    }

    void DrawSetTextureFile(int id, const char* filename, int hardwareFilter, bool forceReload) override
    {
        g_pSurface->DrawSetTextureFile(id, filename, hardwareFilter, forceReload);
    }

    void DrawSetTextureRGBA(int id, const unsigned char* rgba, int wide, int tall, int hardwareFilter,
                            bool forceReload) override
    {
        g_pSurface->DrawSetTextureRGBA(id, rgba, wide, tall, hardwareFilter, forceReload);
    }

    void DrawSetTexture(int id) override
    {
        // In DrawSetTexture there is no reset blending (unlike DrawSetTextureRGBA) override
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        g_pSurface->DrawSetTexture(id);
    }

    void DrawGetTextureSize(int id, int& wide, int& tall) override
    {
        g_pSurface->DrawGetTextureSize(id, wide, tall);
    }

    void DrawTexturedRect(int x0, int y0, int x1, int y1) override
    {
        g_pSurface->DrawTexturedRect(x0, y0, x1, y1);
    }

    bool IsTextureIDValid(int id) override
    {
        return g_pSurface->IsTextureIDValid(id);
    }

    int CreateNewTextureID(bool procedural) override
    {
        return g_pSurface->CreateNewTextureID(procedural);
    }

    void GetScreenSize(int& wide, int& tall) override
    {
        g_pSurface->GetScreenSize(wide, tall);
    }

    void SetAsTopMost(VPANEL panel, bool state) override
    {
        g_pSurface->SetAsTopMost(panel, state);
    }

    void BringToFront(VPANEL panel) override
    {
        g_pSurface->BringToFront(panel);
    }

    void SetForegroundWindow(VPANEL panel) override
    {
        g_pSurface->SetForegroundWindow(panel);
    }

    void SetPanelVisible(VPANEL panel, bool state) override
    {
        g_pSurface->SetPanelVisible(panel, state);
    }

    void SetMinimized(VPANEL panel, bool state) override
    {
        g_pSurface->SetMinimized(panel, state);
    }

    bool IsMinimized(VPANEL panel) override
    {
        return g_pSurface->IsMinimized(panel);
    }

    void FlashWindow(VPANEL panel, bool state) override
    {
        if (g_MainWindow != nullptr)
        {
#ifdef WIN32
            ::FlashWindow((HWND)g_MainWindow, state);
#else
            g_pSurface->FlashWindow(panel, state);
#endif
        }
        else
        {
            g_pSurface->FlashWindow(panel, state);
        }
    }

    void SetTitle(VPANEL panel, const wchar_t* title) override
    {
        g_pSurface->SetTitle(panel, title);
    }

    void SetAsToolBar(VPANEL panel, bool state) override
    {
        g_pSurface->SetAsToolBar(panel, state);
    }

    void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput) override
    {
        g_pSurface->CreatePopup(panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
    }

    void SwapBuffers(VPANEL panel) override
    {
        g_pSurface->SwapBuffers(panel);
    }

    void Invalidate(VPANEL panel) override
    {
        g_pSurface->Invalidate(panel);
    }

    void SetCursor(HCursor cursor) override
    {
        g_pSurface->SetCursor(cursor);
    }

    bool IsCursorVisible() override
    {
        return g_pSurface->IsCursorVisible();
    }

    void ApplyChanges() override
    {
        g_pSurface->ApplyChanges();
    }

    bool IsWithin(int x, int y) override
    {
        return g_pSurface->IsWithin(x, y);
    }

    bool HasFocus() override
    {
        return g_pSurface->HasFocus();
    }

    bool SupportsFeature(SurfaceFeature_e feature) override
    {
        return g_pSurface->SupportsFeature(feature);
    }

    void RestrictPaintToSinglePanel(VPANEL panel) override
    {
        g_pSurface->RestrictPaintToSinglePanel(panel);
    }

    void SetModalPanel(VPANEL vpanel) override
    {
        g_pSurface->SetModalPanel(vpanel);
    }

    VPANEL GetModalPanel() override
    {
        return g_pSurface->GetModalPanel();
    }

    void UnlockCursor() override
    {
        g_pSurface->UnlockCursor();
    }

    void LockCursor() override
    {
        g_pSurface->LockCursor();
    }

    void SetTranslateExtendedKeys(bool state) override
    {
        g_pSurface->SetTranslateExtendedKeys(state);
    }

    VPANEL GetTopmostPopup() override
    {
        return g_pSurface->GetTopmostPopup();
    }

    void SetTopLevelFocus(VPANEL panel) override
    {
        g_pSurface->SetTopLevelFocus(panel);
    }

    HFont CreateFont() override
    {
        return g_pSurface->CreateFont();
    }

    bool AddGlyphSetToFont(HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) override
    {
        return g_pSurface->AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
    }

    bool AddCustomFontFile(const char* fontFileName) override
    {
        return g_pSurface->AddCustomFontFile(fontFileName);
    }

    int GetFontTall(HFont font) override
    {
        return g_pSurface->GetFontTall(font);
    }

    void GetCharABCwide(HFont font, int ch, int& a, int& b, int& c) override
    {
        g_pSurface->GetCharABCwide(font, ch, a, b, c);
    }

    int GetCharacterWidth(HFont font, int ch) override
    {
        return g_pSurface->GetCharacterWidth(font, ch);
    }

    void GetTextSize(HFont font, const wchar_t* text, int& wide, int& tall) override
    {
        g_pSurface->GetTextSize(font, text, wide, tall);
    }

    VPANEL GetNotifyPanel() override
    {
        return g_pSurface->GetNotifyPanel();
    }

    void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char* text) override
    {
        g_pSurface->SetNotifyIcon(context, icon, panelToReceiveMessages, text);
    }

    void PlaySound(const char* fileName) override
    {
        g_pSurface->PlaySound(fileName);
    }

    int GetPopupCount() override
    {
        return g_pSurface->GetPopupCount();
    }

    VPANEL GetPopup(int index) override
    {
        return g_pSurface->GetPopup(index);
    }

    bool ShouldPaintChildPanel(VPANEL childPanel) override
    {
        return g_pSurface->ShouldPaintChildPanel(childPanel);
    }

    bool RecreateContext(VPANEL panel) override
    {
        return g_pSurface->RecreateContext(panel);
    }

    void AddPanel(VPANEL panel) override
    {
        g_pSurface->AddPanel(panel);
    }

    void ReleasePanel(VPANEL panel) override
    {
        g_pSurface->ReleasePanel(panel);
    }

    void MovePopupToFront(VPANEL panel) override
    {
        g_pSurface->MovePopupToFront(panel);
    }

    void MovePopupToBack(VPANEL panel) override
    {
        g_pSurface->MovePopupToBack(panel);
    }

    void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings) override
    {
        g_pSurface->SolveTraverse(panel, forceApplySchemeSettings);
    }

    void PaintTraverse(VPANEL panel) override
    {
        g_pSurface->PaintTraverse(panel);
    }

    void EnableMouseCapture(VPANEL panel, bool state) override
    {
        g_pSurface->EnableMouseCapture(panel, state);
    }

    void GetWorkspaceBounds(int& x, int& y, int& wide, int& tall) override
    {
        g_pSurface->GetWorkspaceBounds(x, y, wide, tall);
    }

    void GetAbsoluteWindowBounds(int& x, int& y, int& wide, int& tall) override
    {
        g_pSurface->GetAbsoluteWindowBounds(x, y, wide, tall);
    }

    void GetProportionalBase(int& width, int& height) override
    {
        g_pSurface->GetProportionalBase(width, height);
    }

    void CalculateMouseVisible() override
    {
        g_pSurface->CalculateMouseVisible();
    }

    bool NeedKBInput() override
    {
        return g_pSurface->NeedKBInput();
    }

    bool HasCursorPosFunctions() override
    {
        return g_pSurface->HasCursorPosFunctions();
    }

    void SurfaceGetCursorPos(int& x, int& y) override
    {
        g_pSurface->SurfaceGetCursorPos(x, y);
    }

    void SurfaceSetCursorPos(int x, int y) override
    {
        g_pSurface->SurfaceSetCursorPos(x, y);
    }

    void DrawTexturedPolygon(VGuiVertex* pVertices, int n) override
    {
        g_pSurface->DrawTexturedPolygon(pVertices, n);
    }

    int GetFontAscent(HFont font, wchar_t wch) override
    {
        return g_pSurface->GetFontAscent(font, wch);
    }

    void SetAllowHTMLJavaScript(bool state) override
    {
        g_pSurface->SetAllowHTMLJavaScript(state);
    }

    void SetLanguage(const char* pchLang) override
    {
        g_pSurface->SetLanguage(pchLang);
    }

    const char* GetLanguage() override
    {
        return g_pSurface->GetLanguage();
    }

    bool DeleteTextureByID(int id) override
    {
        return g_pSurface->DeleteTextureByID(id);
    }

    void DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char* pchData, int wide, int tall) override
    {
        g_pSurface->DrawUpdateRegionTextureBGRA(nTextureID, x, y, pchData, wide, tall);
    }

    void DrawSetTextureBGRA(int id, const unsigned char* pchData, int wide, int tall) override
    {
        g_pSurface->DrawSetTextureBGRA(id, pchData, wide, tall);
    }

    void CreateBrowser(VPANEL panel, IHTMLResponses* pBrowser, bool bPopupWindow, const char* pchUserAgentIdentifier) override
    {
        g_pSurface->CreateBrowser(panel, pBrowser, bPopupWindow, pchUserAgentIdentifier);
    }

    void RemoveBrowser(VPANEL panel, IHTMLResponses* pBrowser) override
    {
        g_pSurface->RemoveBrowser(panel, pBrowser);
    }

    IHTMLChromeController* AccessChromeHTMLController() override
    {
        return g_pSurface->AccessChromeHTMLController();
    }

    [[nodiscard]] float DrawGetAlphaMultiplier() const override
    {
        return alpha_mul_;
    }

    void DrawSetAlphaMultiplier(float a) override 
    {
        alpha_mul_ = a;
    }

private:
    void ApplyAlphaMultiplier(Color& c) const
    {
        c[3] = (int)((float)c[3] * alpha_mul_);
    }

    void ApplyAlphaMultiplier(int& a) const
    {
        a = (int)((float)a * alpha_mul_);
    }
};

SurfaceNext g_SurfaceNext;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(SurfaceNext, ISurfaceNext, VGUI_SURFACE_NEXT_INTERFACE_VERSION, g_SurfaceNext);

namespace vgui2
{
    ISurfaceNext* g_pSurfaceNext = &g_SurfaceNext;
}
