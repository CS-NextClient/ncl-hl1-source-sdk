#include <tier2/SurfaceEx.h>

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>

#include <Color.h>
#include <vgui/IHTML.h>
#include <vgui/ISurface.h>

// Fixing windows defines due to conflicts with class members
#undef CreateFont
#undef PlaySound

SurfaceEx::SurfaceEx(vgui2::ISurface* surface, void* main_window) :
    surf_(surface),
    main_window_(main_window)
{ }

void SurfaceEx::Shutdown()
{
    surf_->Shutdown();
}

void SurfaceEx::RunFrame()
{
    surf_->RunFrame();
}

vgui2::VPANEL SurfaceEx::GetEmbeddedPanel()
{
    return surf_->GetEmbeddedPanel();
}

void SurfaceEx::SetEmbeddedPanel(vgui2::VPANEL pPanel)
{
    surf_->SetEmbeddedPanel(pPanel);
}

void SurfaceEx::PushMakeCurrent(vgui2::VPANEL panel, bool useInsets)
{
    surf_->PushMakeCurrent(panel, useInsets);
}

void SurfaceEx::PopMakeCurrent(vgui2::VPANEL panel)
{
    surf_->PopMakeCurrent(panel);
}

void SurfaceEx::DrawSetColor(int r, int g, int b, int a)
{
    ApplyAlphaMultiplier(a);
    surf_->DrawSetColor(r, g, b, a);
}

void SurfaceEx::DrawSetColor(Color col)
{
    ApplyAlphaMultiplier(col);
    surf_->DrawSetColor(col);
}

void SurfaceEx::DrawFilledRect(int x0, int y0, int x1, int y1)
{
    surf_->DrawFilledRect(x0, y0, x1, y1);
}

void SurfaceEx::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
    surf_->DrawOutlinedRect(x0, y1, x1, y1);
}

void SurfaceEx::DrawLine(int x0, int y0, int x1, int y1)
{
    surf_->DrawLine(x0, y0, x1, y1);
}

void SurfaceEx::DrawPolyLine(int *px, int *py, int numPoints)
{
    surf_->DrawPolyLine(px, py, numPoints);
}

void SurfaceEx::DrawSetTextFont(vgui2::HFont font)
{
    surf_->DrawSetTextFont(font);
}

void SurfaceEx::DrawSetTextColor(int r, int g, int b, int a)
{
    ApplyAlphaMultiplier(a);
    surf_->DrawSetTextColor(r, g, b, a);
}

void SurfaceEx::DrawSetTextColor(Color col)
{
    ApplyAlphaMultiplier(col);
    surf_->DrawSetTextColor(col);
}

void SurfaceEx::DrawSetTextPos(int x, int y)
{
    surf_->DrawSetTextPos(x, y);
}

void SurfaceEx::DrawGetTextPos(int &x, int &y)
{
    surf_->DrawGetTextPos(x, y);
}

void SurfaceEx::DrawPrintText(const wchar_t *text, int textLen)
{
    surf_->DrawPrintText(text, textLen);
}

void SurfaceEx::DrawUnicodeChar(wchar_t wch)
{
    surf_->DrawUnicodeChar(wch);
}

void SurfaceEx::DrawUnicodeCharAdd(wchar_t wch)
{
    surf_->DrawUnicodeCharAdd(wch);
}

void SurfaceEx::DrawFlushText()
{
    surf_->DrawFlushText();
}

vgui2::IHTML *SurfaceEx::CreateHTMLWindow(vgui2::IHTMLEvents *events, vgui2::VPANEL context)
{
    return surf_->CreateHTMLWindow(events, context);
}

void SurfaceEx::PaintHTMLWindow(vgui2::IHTML *htmlwin)
{
    surf_->PaintHTMLWindow(htmlwin);
}

void SurfaceEx::DeleteHTMLWindow(vgui2::IHTML *htmlwin)
{
    surf_->DeleteHTMLWindow(htmlwin);
}

void SurfaceEx::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
    surf_->DrawSetTextureFile(id, filename, hardwareFilter, forceReload);
}

void SurfaceEx::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter,
                                   bool forceReload)
{
    surf_->DrawSetTextureRGBA(id, rgba, wide, tall, hardwareFilter, forceReload);
}

void SurfaceEx::DrawSetTexture(int id)
{
    // In DrawSetTexture there is no reset blending (unlike DrawSetTextureRGBA)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    surf_->DrawSetTexture(id);
}

void SurfaceEx::DrawGetTextureSize(int id, int &wide, int &tall)
{
    surf_->DrawGetTextureSize(id, wide, tall);
}

void SurfaceEx::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
    surf_->DrawTexturedRect(x0, y0, x1, y1);
}

bool SurfaceEx::IsTextureIDValid(int id)
{
    return surf_->IsTextureIDValid(id);
}

int SurfaceEx::CreateNewTextureID(bool procedural)
{
    return surf_->CreateNewTextureID(procedural);
}

void SurfaceEx::GetScreenSize(int &wide, int &tall)
{
    surf_->GetScreenSize(wide, tall);
}

void SurfaceEx::SetAsTopMost(vgui2::VPANEL panel, bool state)
{
    surf_->SetAsTopMost(panel, state);
}

void SurfaceEx::BringToFront(vgui2::VPANEL panel)
{
    surf_->BringToFront(panel);
}

void SurfaceEx::SetForegroundWindow(vgui2::VPANEL panel)
{
    surf_->SetForegroundWindow(panel);
}

void SurfaceEx::SetPanelVisible(vgui2::VPANEL panel, bool state)
{
    surf_->SetPanelVisible(panel, state);
}

void SurfaceEx::SetMinimized(vgui2::VPANEL panel, bool state)
{
    surf_->SetMinimized(panel, state);
}

bool SurfaceEx::IsMinimized(vgui2::VPANEL panel)
{
    return surf_->IsMinimized(panel);
}

void SurfaceEx::FlashWindow(vgui2::VPANEL panel, bool state)
{
    if (main_window_ != nullptr && *(HWND*)main_window_ != NULL)
    {
#ifdef WIN32
        ::FlashWindow(*(HWND*)main_window_, state);
#else
        surf_->FlashWindow(panel, state);
#endif
    }
    else
    {
        surf_->FlashWindow(panel, state);
    }
}

void SurfaceEx::SetTitle(vgui2::VPANEL panel, const wchar_t *title)
{
    surf_->SetTitle(panel, title);
}

void SurfaceEx::SetAsToolBar(vgui2::VPANEL panel, bool state)
{
    surf_->SetAsToolBar(panel, state);
}

void SurfaceEx::CreatePopup(vgui2::VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput,
                            bool kbInput)
{
    surf_->CreatePopup(panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
}

void SurfaceEx::SwapBuffers(vgui2::VPANEL panel)
{
    surf_->SwapBuffers(panel);
}

void SurfaceEx::Invalidate(vgui2::VPANEL panel)
{
    surf_->Invalidate(panel);
}

void SurfaceEx::SetCursor(vgui2::HCursor cursor)
{
    surf_->SetCursor(cursor);
}

bool SurfaceEx::IsCursorVisible()
{
    return surf_->IsCursorVisible();
}

void SurfaceEx::ApplyChanges()
{
    surf_->ApplyChanges();
}

bool SurfaceEx::IsWithin(int x, int y)
{
    return surf_->IsWithin(x, y);
}

bool SurfaceEx::HasFocus()
{
    return surf_->HasFocus();
}

bool SurfaceEx::SupportsFeature(SurfaceFeature_e feature)
{
    return surf_->SupportsFeature(feature);
}

void SurfaceEx::RestrictPaintToSinglePanel(vgui2::VPANEL panel)
{
    surf_->RestrictPaintToSinglePanel(panel);
}

void SurfaceEx::SetModalPanel(vgui2::VPANEL vpanel)
{
    surf_->SetModalPanel(vpanel);
}

vgui2::VPANEL SurfaceEx::GetModalPanel()
{
    return surf_->GetModalPanel();
}

void SurfaceEx::UnlockCursor()
{
    surf_->UnlockCursor();
}

void SurfaceEx::LockCursor()
{
    surf_->LockCursor();
}

void SurfaceEx::SetTranslateExtendedKeys(bool state)
{
    surf_->SetTranslateExtendedKeys(state);
}

vgui2::VPANEL SurfaceEx::GetTopmostPopup()
{
    return surf_->GetTopmostPopup();
}

void SurfaceEx::SetTopLevelFocus(vgui2::VPANEL panel)
{
    surf_->SetTopLevelFocus(panel);
}

vgui2::HFont SurfaceEx::CreateFont()
{
    return surf_->CreateFont();
}

bool
SurfaceEx::AddGlyphSetToFont(vgui2::HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines,
                             int flags, int lowRange, int highRange)
{
    return surf_->AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
}

bool SurfaceEx::AddCustomFontFile(const char *fontFileName)
{
    return surf_->AddCustomFontFile(fontFileName);
}

int SurfaceEx::GetFontTall(vgui2::HFont font)
{
    return surf_->GetFontTall(font);
}

void SurfaceEx::GetCharABCwide(vgui2::HFont font, int ch, int &a, int &b, int &c)
{
    surf_->GetCharABCwide(font, ch, a, b, c);
}

int SurfaceEx::GetCharacterWidth(vgui2::HFont font, int ch)
{
    return surf_->GetCharacterWidth(font, ch);
}

void SurfaceEx::GetTextSize(vgui2::HFont font, const wchar_t *text, int &wide, int &tall)
{
    surf_->GetTextSize(font, text, wide, tall);
}

vgui2::VPANEL SurfaceEx::GetNotifyPanel()
{
    return surf_->GetNotifyPanel();
}

void SurfaceEx::SetNotifyIcon(vgui2::VPANEL context, vgui2::HTexture icon, vgui2::VPANEL panelToReceiveMessages,
                              const char *text)
{
    surf_->SetNotifyIcon(context, icon, panelToReceiveMessages, text);
}

void SurfaceEx::PlaySound(const char *fileName)
{
    surf_->PlaySound(fileName);
}

int SurfaceEx::GetPopupCount()
{
    return surf_->GetPopupCount();
}

vgui2::VPANEL SurfaceEx::GetPopup(int index)
{
    return surf_->GetPopup(index);
}

bool SurfaceEx::ShouldPaintChildPanel(vgui2::VPANEL childPanel)
{
    return surf_->ShouldPaintChildPanel(childPanel);
}

bool SurfaceEx::RecreateContext(vgui2::VPANEL panel)
{
    return surf_->RecreateContext(panel);
}

void SurfaceEx::AddPanel(vgui2::VPANEL panel)
{
    surf_->AddPanel(panel);
}

void SurfaceEx::ReleasePanel(vgui2::VPANEL panel)
{
    surf_->ReleasePanel(panel);
}

void SurfaceEx::MovePopupToFront(vgui2::VPANEL panel)
{
    surf_->MovePopupToFront(panel);
}

void SurfaceEx::MovePopupToBack(vgui2::VPANEL panel)
{
    surf_->MovePopupToBack(panel);
}

void SurfaceEx::SolveTraverse(vgui2::VPANEL panel, bool forceApplySchemeSettings)
{
    surf_->SolveTraverse(panel, forceApplySchemeSettings);
}

void SurfaceEx::PaintTraverse(vgui2::VPANEL panel)
{
    surf_->PaintTraverse(panel);
}

void SurfaceEx::EnableMouseCapture(vgui2::VPANEL panel, bool state)
{
    surf_->EnableMouseCapture(panel, state);
}

void SurfaceEx::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
    surf_->GetWorkspaceBounds(x, y, wide, tall);
}

void SurfaceEx::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
    surf_->GetAbsoluteWindowBounds(x, y, wide, tall);
}

void SurfaceEx::GetProportionalBase(int &width, int &height)
{
    surf_->GetProportionalBase(width, height);
}

void SurfaceEx::CalculateMouseVisible()
{
    surf_->CalculateMouseVisible();
}

bool SurfaceEx::NeedKBInput()
{
    return surf_->NeedKBInput();
}

bool SurfaceEx::HasCursorPosFunctions()
{
    return surf_->HasCursorPosFunctions();
}

void SurfaceEx::SurfaceGetCursorPos(int &x, int &y)
{
    surf_->SurfaceGetCursorPos(x, y);
}

void SurfaceEx::SurfaceSetCursorPos(int x, int y)
{
    surf_->SurfaceSetCursorPos(x, y);
}

void SurfaceEx::DrawTexturedPolygon(vgui2::VGuiVertex *pVertices, int n)
{
    surf_->DrawTexturedPolygon(pVertices, n);
}

int SurfaceEx::GetFontAscent(vgui2::HFont font, wchar_t wch)
{
    return surf_->GetFontAscent(font, wch);
}

void SurfaceEx::SetAllowHTMLJavaScript(bool state)
{
    surf_->SetAllowHTMLJavaScript(state);
}

void SurfaceEx::SetLanguage(const char *pchLang)
{
    surf_->SetLanguage(pchLang);
}

const char *SurfaceEx::GetLanguage()
{
    return surf_->GetLanguage();
}

bool SurfaceEx::DeleteTextureByID(int id)
{
    return surf_->DeleteTextureByID(id);
}

void
SurfaceEx::DrawUpdateRegionTextureBGRA(int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall)
{
    surf_->DrawUpdateRegionTextureBGRA(nTextureID, x, y, pchData, wide, tall);
}

void SurfaceEx::DrawSetTextureBGRA(int id, const unsigned char *pchData, int wide, int tall)
{
    surf_->DrawSetTextureBGRA(id, pchData, wide, tall);
}

void SurfaceEx::CreateBrowser(vgui2::VPANEL panel, IHTMLResponses *pBrowser, bool bPopupWindow,
                              const char *pchUserAgentIdentifier)
{
    surf_->CreateBrowser(panel, pBrowser, bPopupWindow, pchUserAgentIdentifier);
}

void SurfaceEx::RemoveBrowser(vgui2::VPANEL panel, IHTMLResponses *pBrowser)
{
    surf_->RemoveBrowser(panel, pBrowser);
}

IHTMLChromeController *SurfaceEx::AccessChromeHTMLController()
{
    return surf_->AccessChromeHTMLController();
}

float SurfaceEx::DrawGetAlphaMultiplier() const
{
    return alpha_mul_;
}

void SurfaceEx::DrawSetAlphaMultiplier(float a)
{
    alpha_mul_ = a;
}

void SurfaceEx::ApplyAlphaMultiplier(Color& c) const
{
    c[3] = (int)((float)c[3] * alpha_mul_);
}

void SurfaceEx::ApplyAlphaMultiplier(int& a) const
{
    a = (int)((float)a * alpha_mul_);
}
