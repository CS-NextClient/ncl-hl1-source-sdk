#pragma once

#include <vgui/ISurface.h>
#include <Color.h>

class SurfaceEx : public vgui2::ISurface
{
    vgui2::ISurface* surf_;
    void* main_window_;

    float alpha_mul_ = 1;

public:
    explicit SurfaceEx(vgui2::ISurface* surface, void* main_window);

    void Shutdown() override;
    void RunFrame() override;
    vgui2::VPANEL GetEmbeddedPanel() override;
    void SetEmbeddedPanel(vgui2::VPANEL pPanel) override;
    void PushMakeCurrent(vgui2::VPANEL panel, bool useInsets) override;
    void PopMakeCurrent(vgui2::VPANEL panel) override;
    void DrawSetColor(int r, int g, int b, int a) override;
    void DrawSetColor(Color col) override;
    void DrawFilledRect(int x0, int y0, int x1, int y1) override;
    void DrawOutlinedRect(int x0, int y0, int x1, int y1) override;
    void DrawLine(int x0, int y0, int x1, int y1) override;
    void DrawPolyLine(int *px, int *py, int numPoints) override;
    void DrawSetTextFont(vgui2::HFont font) override;
    void DrawSetTextColor(int r, int g, int b, int a) override;
    void DrawSetTextColor(Color col) override;
    void DrawSetTextPos(int x, int y) override;
    void DrawGetTextPos(int &x, int &y) override;
    void DrawPrintText(const wchar_t *text, int textLen) override;
    void DrawUnicodeChar(wchar_t wch) override;
    void DrawUnicodeCharAdd(wchar_t wch) override;
    void DrawFlushText() override;
    vgui2::IHTML *CreateHTMLWindow(vgui2::IHTMLEvents *events, vgui2::VPANEL context) override;
    void PaintHTMLWindow(vgui2::IHTML *htmlwin) override;
    void DeleteHTMLWindow(vgui2::IHTML *htmlwin) override;
    void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) override;
    void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter,
                            bool forceReload) override;
    void DrawSetTexture(int id) override;
    void DrawGetTextureSize(int id, int &wide, int &tall) override;
    void DrawTexturedRect(int x0, int y0, int x1, int y1) override;
    bool IsTextureIDValid(int id) override;
    int CreateNewTextureID(bool procedural) override;
    void GetScreenSize(int &wide, int &tall) override;
    void SetAsTopMost(vgui2::VPANEL panel, bool state) override;
    void BringToFront(vgui2::VPANEL panel) override;
    void SetForegroundWindow(vgui2::VPANEL panel) override;
    void SetPanelVisible(vgui2::VPANEL panel, bool state) override;
    void SetMinimized(vgui2::VPANEL panel, bool state) override;
    bool IsMinimized(vgui2::VPANEL panel) override;
    void FlashWindow(vgui2::VPANEL panel, bool state) override;
    void SetTitle(vgui2::VPANEL panel, const wchar_t *title) override;
    void SetAsToolBar(vgui2::VPANEL panel, bool state) override;
    void CreatePopup(vgui2::VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput,
                     bool kbInput) override;
    void SwapBuffers(vgui2::VPANEL panel) override;
    void Invalidate(vgui2::VPANEL panel) override;
    void SetCursor(vgui2::HCursor cursor) override;
    bool IsCursorVisible() override;
    void ApplyChanges() override;
    bool IsWithin(int x, int y) override;
    bool HasFocus() override;
    bool SupportsFeature(SurfaceFeature_e feature) override;
    void RestrictPaintToSinglePanel(vgui2::VPANEL panel) override;
    void SetModalPanel(vgui2::VPANEL vpanel) override;
    vgui2::VPANEL GetModalPanel() override;
    void UnlockCursor() override;
    void LockCursor() override;
    void SetTranslateExtendedKeys(bool state) override;
    vgui2::VPANEL GetTopmostPopup() override;
    void SetTopLevelFocus(vgui2::VPANEL panel) override;
    vgui2::HFont CreateFont() override;
    bool
    AddGlyphSetToFont(vgui2::HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines,
                      int flags, int lowRange, int highRange) override;
    bool AddCustomFontFile(const char *fontFileName) override;
    int GetFontTall(vgui2::HFont font) override;
    void GetCharABCwide(vgui2::HFont font, int ch, int &a, int &b, int &c) override;
    int GetCharacterWidth(vgui2::HFont font, int ch) override;
    void GetTextSize(vgui2::HFont font, const wchar_t *text, int &wide, int &tall) override;
    vgui2::VPANEL GetNotifyPanel() override;
    void SetNotifyIcon(vgui2::VPANEL context, vgui2::HTexture icon, vgui2::VPANEL panelToReceiveMessages,
                       const char *text) override;
    void PlaySound(const char *fileName) override;
    int GetPopupCount() override;
    vgui2::VPANEL GetPopup(int index) override;
    bool ShouldPaintChildPanel(vgui2::VPANEL childPanel) override;
    bool RecreateContext(vgui2::VPANEL panel) override;
    void AddPanel(vgui2::VPANEL panel) override;
    void ReleasePanel(vgui2::VPANEL panel) override;
    void MovePopupToFront(vgui2::VPANEL panel) override;
    void MovePopupToBack(vgui2::VPANEL panel) override;
    void SolveTraverse(vgui2::VPANEL panel, bool forceApplySchemeSettings) override;
    void PaintTraverse(vgui2::VPANEL panel) override;
    void EnableMouseCapture(vgui2::VPANEL panel, bool state) override;
    void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) override;
    void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) override;
    void GetProportionalBase(int &width, int &height) override;
    void CalculateMouseVisible() override;
    bool NeedKBInput() override;
    bool HasCursorPosFunctions() override;
    void SurfaceGetCursorPos(int &x, int &y) override;
    void SurfaceSetCursorPos(int x, int y) override;
    void DrawTexturedPolygon(vgui2::VGuiVertex *pVertices, int n) override;
    int GetFontAscent(vgui2::HFont font, wchar_t wch) override;
    void SetAllowHTMLJavaScript(bool state) override;
    void SetLanguage(const char *pchLang) override;
    const char *GetLanguage() override;
    bool DeleteTextureByID(int id) override;
    void DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide,
                                     int tall) override;
    void DrawSetTextureBGRA(int id, const unsigned char *pchData, int wide, int tall) override;
    void CreateBrowser(vgui2::VPANEL panel, IHTMLResponses *pBrowser, bool bPopupWindow,
                       const char *pchUserAgentIdentifier) override;
    void RemoveBrowser(vgui2::VPANEL panel, IHTMLResponses *pBrowser) override;
    IHTMLChromeController *AccessChromeHTMLController() override;


    [[nodiscard]] float DrawGetAlphaMultiplier() const;
    void DrawSetAlphaMultiplier(float a);

private:
    void ApplyAlphaMultiplier(Color& c) const;
    void ApplyAlphaMultiplier(int& a) const;

};

