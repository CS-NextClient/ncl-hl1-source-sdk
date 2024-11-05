//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//


#include <windows.h>
#include <imm.h>
#undef PostMessage

#include <string.h>

#include "vgui_internal.h"
#include "VPanel.h"
#include "UtlVector.h"
#include <KeyValues.h>
#include "tier0/vcrmode.h"

#include <vgui/VGUI2.h>
#include <vgui/IClientPanel.h>
#include <vgui/IInputInternal.h>
#include <vgui/IPanel.h>
#include <vgui/ISurfaceNext.h>
#include <vgui/IVGui.h>
#include <vgui/KeyCode.h>
#include <vgui/MouseCode.h>
#include "vgui/Cursor.h"

#include "UtlLinkedList.h"
#include "tier0/icommandline.h"
#include "vgui_key_translation.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

bool IsDispatchingMessageQueue(void);

using namespace vgui2;

class CInputWin32 : public IInputInternal
{
public:
    CInputWin32();
    ~CInputWin32();

    virtual void RunFrame();

    virtual void PanelDeleted(VPANEL panel);

    virtual void UpdateMouseFocus(int x, int y);
    virtual void SetMouseFocus(VPANEL newMouseFocus);

    virtual void SetCursorPos(int x, int y);
    virtual void GetCursorPos(int& x, int& y);
    virtual void SetCursorOveride(HCursor cursor);
    virtual HCursor GetCursorOveride();


    virtual void SetMouseCapture(VPANEL panel);

    virtual VPANEL GetFocus();
    virtual VPANEL GetMouseOver();

    virtual bool WasMousePressed(MouseCode code);
    virtual bool WasMouseDoublePressed(MouseCode code);
    virtual bool IsMouseDown(MouseCode code);
    virtual bool WasMouseReleased(MouseCode code);
    virtual bool WasKeyPressed(KeyCode code);
    virtual bool IsKeyDown(KeyCode code);
    virtual bool WasKeyTyped(KeyCode code);
    virtual bool WasKeyReleased(KeyCode code);

    virtual void GetKeyCodeText(KeyCode code, char* buf, int buflen);

    virtual void InternalCursorMoved(int x, int y); //expects input in surface space
    virtual void InternalMousePressed(MouseCode code);
    virtual void InternalMouseDoublePressed(MouseCode code);
    virtual void InternalMouseReleased(MouseCode code);
    virtual void InternalMouseWheeled(int delta);
    virtual void InternalKeyCodePressed(KeyCode code);
    virtual void InternalKeyCodeTyped(KeyCode code);
    virtual void InternalKeyTyped(wchar_t unichar);
    virtual void InternalKeyCodeReleased(KeyCode code);

    virtual VPANEL GetAppModalSurface();
    // set the modal dialog panel.
    // all events will go only to this panel and its children.
    virtual void SetAppModalSurface(VPANEL panel);
    // release the modal dialog panel
    // do this when your modal dialog finishes.
    virtual void ReleaseAppModalSurface();

    // returns true if the specified panel is a child of the current modal panel
    // if no modal panel is set, then this always returns TRUE
    virtual bool IsChildOfModalPanel(VPANEL panel);

    // Creates/ destroys "input" contexts, which contains information
    // about which controls have mouse + key focus, for example.
    virtual HInputContext CreateInputContext();
    virtual void DestroyInputContext(HInputContext context);

    // Associates a particular panel with an input context
    // Associating NULL is valid; it disconnects the panel from the context
    virtual void AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot);

    // Activates a particular input context, use DEFAULT_INPUT_CONTEXT
    // to get the one normally used by VGUI
    virtual void ActivateInputContext(HInputContext context);

    VPANEL GetMouseCapture();

    virtual void ResetInputContext(HInputContext context);

    virtual void GetCursorPosition(int& x, int& y);

private:
    VPanel* CalculateNewKeyFocus();

    struct InputContext_t
    {
        VPANEL _rootPanel;

        bool _mousePressed[MOUSE_COUNT];
        bool _mouseDoublePressed[MOUSE_COUNT];
        bool _mouseDown[MOUSE_COUNT];
        bool _mouseReleased[MOUSE_COUNT];
        bool _keyPressed[KEY_CODE_COUNT];
        bool _keyTyped[KEY_CODE_COUNT];
        bool _keyDown[KEY_CODE_COUNT];
        bool _keyReleased[KEY_CODE_COUNT];

        VPanel* _keyFocus;
        VPanel* _oldMouseFocus;
        VPanel* _mouseFocus; // the panel that has the current mouse focus - same as _mouseOver unless _mouseCapture is set
        VPanel* _mouseOver; // the panel that the mouse is currently over, NULL if not over any vgui item

        VPanel* _mouseCapture; // the panel that has currently captured mouse focus
        VPanel* _appModalPanel; // the modal dialog panel.

        int m_nCursorX;
        int m_nCursorY;
    };

    void InitInputContext(InputContext_t* pContext);
    void PanelDeleted(VPanel* pFocus, InputContext_t& context);

    InputContext_t* GetContext(HInputContext context);
    InputContext_t* GetCurrentContext();

    const char* _keyTrans[KEY_LAST];
    HCursor _cursorOverride;

    InputContext_t m_DefaultInputContext;
    HInputContext m_hContext;

    CUtlLinkedList<InputContext_t, HInputContext> m_Contexts;

    bool _updateToggleButtonState = false;
};

CInputWin32 g_Input;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInputWin32, IInput, VGUI_INPUT_INTERFACE_VERSION_GS, g_Input); // export IInput to everyone else, not IInputInternal!
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInputWin32, IInputInternal, VGUI_INPUTINTERNAL_INTERFACE_VERSION, g_Input); // for use in external surfaces only! (like the engine surface)

namespace vgui2
{
    inline VPANEL VPanelToHandle(VPanel* vguiPanel)
    {
        return (VPANEL)vguiPanel;
    }

    inline VPanel* VHandleToPanel(VPANEL vguiPanel)
    {
        return (VPanel*)vguiPanel;
    }
}


namespace vgui2
{
    IInputInternal* g_pInput = &g_Input;
}


CInputWin32::CInputWin32()
{
    InitInputContext(&m_DefaultInputContext);

    m_hContext = DEFAULT_INPUT_CONTEXT;

    // build key to text translation table
    // first byte unshifted key
    // second byte shifted key
    // the rest is the name of the key
    _keyTrans[KEY_0] = "0)KEY_0";
    _keyTrans[KEY_1] = "1!KEY_1";
    _keyTrans[KEY_2] = "2@KEY_2";
    _keyTrans[KEY_3] = "3#KEY_3";
    _keyTrans[KEY_4] = "4$KEY_4";
    _keyTrans[KEY_5] = "5%KEY_5";
    _keyTrans[KEY_6] = "6^KEY_6";
    _keyTrans[KEY_7] = "7&KEY_7";
    _keyTrans[KEY_8] = "8*KEY_8";
    _keyTrans[KEY_9] = "9(KEY_9";
    _keyTrans[KEY_A] = "aAKEY_A";
    _keyTrans[KEY_B] = "bBKEY_B";
    _keyTrans[KEY_C] = "cCKEY_C";
    _keyTrans[KEY_D] = "dDKEY_D";
    _keyTrans[KEY_E] = "eEKEY_E";
    _keyTrans[KEY_F] = "fFKEY_F";
    _keyTrans[KEY_G] = "gGKEY_G";
    _keyTrans[KEY_H] = "hHKEY_H";
    _keyTrans[KEY_I] = "iIKEY_I";
    _keyTrans[KEY_J] = "jJKEY_J";
    _keyTrans[KEY_K] = "kKKEY_K";
    _keyTrans[KEY_L] = "lLKEY_L"", L";
    _keyTrans[KEY_M] = "mMKEY_M";
    _keyTrans[KEY_N] = "nNKEY_N";
    _keyTrans[KEY_O] = "oOKEY_O";
    _keyTrans[KEY_P] = "pPKEY_P";
    _keyTrans[KEY_Q] = "qQKEY_Q";
    _keyTrans[KEY_R] = "rRKEY_R";
    _keyTrans[KEY_S] = "sSKEY_S";
    _keyTrans[KEY_T] = "tTKEY_T";
    _keyTrans[KEY_U] = "uUKEY_U";
    _keyTrans[KEY_V] = "vVKEY_V";
    _keyTrans[KEY_W] = "wWKEY_W";
    _keyTrans[KEY_X] = "xXKEY_X";
    _keyTrans[KEY_Y] = "yYKEY_Y";
    _keyTrans[KEY_Z] = "zZKEY_Z";
    _keyTrans[KEY_PAD_0] = "0\0KEY_PAD_0";
    _keyTrans[KEY_PAD_1] = "1\0KEY_PAD_1";
    _keyTrans[KEY_PAD_2] = "2\0KEY_PAD_2";
    _keyTrans[KEY_PAD_3] = "3\0KEY_PAD_3";
    _keyTrans[KEY_PAD_4] = "4\0KEY_PAD_4";
    _keyTrans[KEY_PAD_5] = "5\0KEY_PAD_5";
    _keyTrans[KEY_PAD_6] = "6\0KEY_PAD_6";
    _keyTrans[KEY_PAD_7] = "7\0KEY_PAD_7";
    _keyTrans[KEY_PAD_8] = "8\0KEY_PAD_8";
    _keyTrans[KEY_PAD_9] = "9\0KEY_PAD_9";
    _keyTrans[KEY_PAD_DIVIDE] = "//KEY_PAD_DIVIDE";
    _keyTrans[KEY_PAD_MULTIPLY] = "**KEY_PAD_MULTIPLY";
    _keyTrans[KEY_PAD_MINUS] = "--KEY_PAD_MINUS";
    _keyTrans[KEY_PAD_PLUS] = "++KEY_PAD_PLUS";
    _keyTrans[KEY_PAD_ENTER] = "\0\0KEY_PAD_ENTER";
    _keyTrans[KEY_PAD_DECIMAL] = ".\0KEY_PAD_DECIMAL"", L";
    _keyTrans[KEY_LBRACKET] = "[{KEY_LBRACKET";
    _keyTrans[KEY_RBRACKET] = "]}KEY_RBRACKET";
    _keyTrans[KEY_SEMICOLON] = ";:KEY_SEMICOLON";
    _keyTrans[KEY_APOSTROPHE] = "'\"KEY_APOSTROPHE";
    _keyTrans[KEY_BACKQUOTE] = "`~KEY_BACKQUOTE";
    _keyTrans[KEY_COMMA] = ",<KEY_COMMA";
    _keyTrans[KEY_PERIOD] = ".>KEY_PERIOD";
    _keyTrans[KEY_SLASH] = "/?KEY_SLASH";
    _keyTrans[KEY_BACKSLASH] = "\\|KEY_BACKSLASH";
    _keyTrans[KEY_MINUS] = "-_KEY_MINUS";
    _keyTrans[KEY_EQUAL] = "=+KEY_EQUAL"", L";
    _keyTrans[KEY_ENTER] = "\0\0KEY_ENTER";
    _keyTrans[KEY_SPACE] = "  KEY_SPACE";
    _keyTrans[KEY_BACKSPACE] = "\0\0KEY_BACKSPACE";
    _keyTrans[KEY_TAB] = "\0\0KEY_TAB";
    _keyTrans[KEY_CAPSLOCK] = "\0\0KEY_CAPSLOCK";
    _keyTrans[KEY_NUMLOCK] = "\0\0KEY_NUMLOCK";
    _keyTrans[KEY_ESCAPE] = "\0\0KEY_ESCAPE";
    _keyTrans[KEY_SCROLLLOCK] = "\0\0KEY_SCROLLLOCK";
    _keyTrans[KEY_INSERT] = "\0\0KEY_INSERT";
    _keyTrans[KEY_DELETE] = "\0\0KEY_DELETE";
    _keyTrans[KEY_HOME] = "\0\0KEY_HOME";
    _keyTrans[KEY_END] = "\0\0KEY_END";
    _keyTrans[KEY_PAGEUP] = "\0\0KEY_PAGEUP";
    _keyTrans[KEY_PAGEDOWN] = "\0\0KEY_PAGEDOWN";
    _keyTrans[KEY_BREAK] = "\0\0KEY_BREAK";
    _keyTrans[KEY_LSHIFT] = "\0\0KEY_LSHIFT";
    _keyTrans[KEY_RSHIFT] = "\0\0KEY_RSHIFT";
    _keyTrans[KEY_LALT] = "\0\0KEY_LALT";
    _keyTrans[KEY_RALT] = "\0\0KEY_RALT";
    _keyTrans[KEY_LCONTROL] = "\0\0KEY_LCONTROL"", L";
    _keyTrans[KEY_RCONTROL] = "\0\0KEY_RCONTROL"", L";
    _keyTrans[KEY_LWIN] = "\0\0KEY_LWIN";
    _keyTrans[KEY_RWIN] = "\0\0KEY_RWIN";
    _keyTrans[KEY_APP] = "\0\0KEY_APP";
    _keyTrans[KEY_UP] = "\0\0KEY_UP";
    _keyTrans[KEY_LEFT] = "\0\0KEY_LEFT";
    _keyTrans[KEY_DOWN] = "\0\0KEY_DOWN";
    _keyTrans[KEY_RIGHT] = "\0\0KEY_RIGHT";
    _keyTrans[KEY_F1] = "\0\0KEY_F1";
    _keyTrans[KEY_F2] = "\0\0KEY_F2";
    _keyTrans[KEY_F3] = "\0\0KEY_F3";
    _keyTrans[KEY_F4] = "\0\0KEY_F4";
    _keyTrans[KEY_F5] = "\0\0KEY_F5";
    _keyTrans[KEY_F6] = "\0\0KEY_F6";
    _keyTrans[KEY_F7] = "\0\0KEY_F7";
    _keyTrans[KEY_F8] = "\0\0KEY_F8";
    _keyTrans[KEY_F9] = "\0\0KEY_F9";
    _keyTrans[KEY_F10] = "\0\0KEY_F10";
    _keyTrans[KEY_F11] = "\0\0KEY_F11";
    _keyTrans[KEY_F12] = "\0\0KEY_F12";

    // TODO move initialization to a more suitable place, outside of this class
    KeyCode_InitKeyTranslationTable();
}

CInputWin32::~CInputWin32() = default;

void CInputWin32::SetMouseFocus(VPANEL newMouseFocus)
{
    if (!IsChildOfModalPanel(newMouseFocus))
        return;

    auto pCtx = GetCurrentContext();

    auto pNewMouseFocus = VHandleToPanel(newMouseFocus);

    if (newMouseFocus)
    {
        auto pFocus = pNewMouseFocus;

        while (true)
        {
            if (!pFocus->IsMouseInputEnabled())
                return;

            auto isPopup = pFocus->IsPopup();

            pFocus = pFocus->GetParent();

            if (isPopup || !pFocus || !pFocus->GetParent())
                break;
        }
    }

    if (pCtx->_mouseOver == pNewMouseFocus)
    {
        if (pCtx->_mouseCapture || pCtx->_mouseFocus == pNewMouseFocus)
            return;
    }

    pCtx->_oldMouseFocus = pCtx->_mouseOver;
    pCtx->_mouseOver = pNewMouseFocus;

    if (pCtx->_oldMouseFocus)
    {
        if (!pCtx->_mouseCapture ||
            pCtx->_oldMouseFocus->HasParent(pCtx->_mouseCapture))
        {
            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_oldMouseFocus),
                new KeyValues("CursorExited"),
                NULL_HANDLE
            );
        }
    }

    if (pNewMouseFocus)
    {
        if (!pCtx->_mouseCapture ||
            pCtx->_mouseCapture->HasParent(pCtx->_mouseCapture))
        {
            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_mouseOver),
                new KeyValues("CursorEntered"),
                NULL_HANDLE
            );
        }

        auto pFocus = pCtx->_mouseCapture;

        if (!pCtx->_mouseCapture)
            pFocus = pCtx->_mouseOver;

        pCtx->_mouseFocus = pFocus;
    }
}

void CInputWin32::SetMouseCapture(VPANEL panel)
{
    if (!IsChildOfModalPanel(panel))
        return;

    auto pCtx = GetCurrentContext();

    if (pCtx->_mouseCapture)
    {
        if (pCtx->_mouseCapture == VHandleToPanel(panel))
        {
            g_pSurfaceNext->EnableMouseCapture(panel, true);
            pCtx->_mouseCapture = VHandleToPanel(panel);
            return;
        }

        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseCapture),
            new KeyValues("MouseCaptureLost"),
            NULL_HANDLE
        );
    }

    if (!panel)
    {
        if (pCtx->_mouseCapture)
        {
            g_pSurfaceNext->EnableMouseCapture(
                VPanelToHandle(pCtx->_mouseCapture), false
            );
        }

        pCtx->_mouseCapture = VHandleToPanel(panel);
        return;
    }

    g_pSurfaceNext->EnableMouseCapture(panel, true);
    pCtx->_mouseCapture = VHandleToPanel(panel);
}

void CInputWin32::GetKeyCodeText(KeyCode code, char* buf, int buflen)
{
    if (buf && buflen > 0)
    {
        for (int i = 0; i < buflen; ++i)
        {
            buf[i] = _keyTrans[code][i + 2];
        }
    }
}

VPANEL CInputWin32::GetFocus()
{
    return VPanelToHandle(GetCurrentContext()->_keyFocus);
}

VPANEL CInputWin32::GetMouseOver()
{
    return VPanelToHandle(GetCurrentContext()->_mouseOver);
}

void CInputWin32::SetCursorPos(int x, int y)
{
    if (!g_pSurfaceNext->HasCursorPosFunctions())
    {
        int px, py, pw, pt;
        g_pSurfaceNext->GetAbsoluteWindowBounds(px, py, pw, pt);
    }
    else
    {
        g_pSurfaceNext->SurfaceSetCursorPos(x, y);
    }
}

void CInputWin32::GetCursorPos(int& x, int& y)
{
    //if( g_pSurfaceNext->HasCursorPosFunctions() )
    //	g_pSurfaceNext->SurfaceGetCursorPos( x, y );
    return GetCursorPosition(x, y);
}

bool CInputWin32::WasMousePressed(MouseCode code)
{
    return GetCurrentContext()->_mousePressed[code];
}

bool CInputWin32::WasMouseDoublePressed(MouseCode code)
{
    return GetCurrentContext()->_mouseDoublePressed[code];
}

bool CInputWin32::IsMouseDown(MouseCode code)
{
    return GetCurrentContext()->_mouseDown[code];
}

void CInputWin32::SetCursorOveride(HCursor cursor)
{
    //Nothing
}

HCursor CInputWin32::GetCursorOveride()
{
    //Nothing
    return NULL_HANDLE;
}

bool CInputWin32::WasMouseReleased(MouseCode code)
{
    return GetCurrentContext()->_mousePressed[code];
}

bool CInputWin32::WasKeyPressed(KeyCode code)
{
    return GetCurrentContext()->_keyPressed[code];
}

bool CInputWin32::IsKeyDown(KeyCode code)
{
    return GetCurrentContext()->_keyDown[code];
}

bool CInputWin32::WasKeyTyped(KeyCode code)
{
    return GetCurrentContext()->_keyTyped[code];
}

bool CInputWin32::WasKeyReleased(KeyCode code)
{
    return GetCurrentContext()->_keyReleased[code];
}

VPANEL CInputWin32::GetAppModalSurface()
{
    return VPanelToHandle(m_DefaultInputContext._appModalPanel);
}

void CInputWin32::SetAppModalSurface(VPANEL panel)
{
    m_DefaultInputContext._appModalPanel = VHandleToPanel(panel);
}

void CInputWin32::ReleaseAppModalSurface()
{
    m_DefaultInputContext._appModalPanel = nullptr;
}

void CInputWin32::GetCursorPosition(int& x, int& y)
{
    auto pCtx = GetCurrentContext();

    x = pCtx->m_nCursorX;
    y = pCtx->m_nCursorY;
}

void CInputWin32::RunFrame()
{
    if (m_hContext == DEFAULT_INPUT_CONTEXT)
        _updateToggleButtonState = true;

    auto pCtx = GetCurrentContext();

    if (pCtx->_keyFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_keyFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_keyFocus),
            new KeyValues("KeyFocusTicked"),
            NULL_HANDLE
        );
    }

    if (pCtx->_mouseFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseFocus),
            new KeyValues("MouseFocusTicked"),
            NULL_HANDLE
        );
    }

    memset(pCtx->_mousePressed, 0, sizeof(pCtx->_mousePressed));
    memset(pCtx->_mouseDoublePressed, 0, sizeof(pCtx->_mouseDoublePressed));
    memset(pCtx->_mouseReleased, 0, sizeof(pCtx->_mouseReleased));
    memset(pCtx->_keyPressed, 0, sizeof(pCtx->_keyPressed));
    memset(pCtx->_keyTyped, 0, sizeof(pCtx->_keyTyped));
    memset(pCtx->_keyReleased, 0, sizeof(pCtx->_keyReleased));

    auto pNewFocus = CalculateNewKeyFocus();

    if (pCtx->_keyFocus != pNewFocus)
    {
        if (pCtx->_keyFocus)
        {
            pCtx->_keyFocus->Client()->InternalFocusChanged(true);

            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_keyFocus),
                new KeyValues("KillFocus"),
                NULL_HANDLE
            );

            pCtx->_keyFocus->Client()->Repaint();
        }

        if (pCtx->_keyFocus)
        {
            auto pFocus = pCtx->_keyFocus;

            while (pFocus && !pFocus->IsPopup())
            {
                pFocus = pFocus->GetParent();
            }

            if (pFocus)
            {
                pFocus->Client()->Repaint();
            }
        }

        if (!pNewFocus)
        {
            pCtx->_keyFocus = nullptr;
            return;
        }

        pNewFocus->Client()->InternalFocusChanged(false);

        g_pIVgui->PostMessage(
            VPanelToHandle(pNewFocus),
            new KeyValues("SetFocus"),
            NULL_HANDLE
        );

        pNewFocus->Client()->Repaint();

        auto pFocus = pNewFocus;

        while (pFocus && !pFocus->IsPopup())
        {
            pFocus = pFocus->GetParent();
        }

        if (pFocus)
        {
            pFocus->Client()->Repaint();
        }

        pCtx->_keyFocus = pNewFocus;

        pNewFocus->MoveToFront();
    }
}

void CInputWin32::UpdateMouseFocus(int x, int y)
{
    auto pCtx = GetCurrentContext();

    VPANEL topMost = NULL_HANDLE;

    if (pCtx->_rootPanel)
    {
        IClientPanel* pClient = VHandleToPanel(pCtx->_rootPanel)->Client();

        if (!pClient)
        {
            SetMouseFocus(NULL_HANDLE);
            return;
        }

        topMost = pClient->IsWithinTraverse(x, y, false);
    }
    else
    {
        if (!g_pSurfaceNext->IsCursorVisible() ||
            !g_pSurfaceNext->IsWithin(x, y))
        {
            SetMouseFocus(NULL_HANDLE);
            return;
        }

        for (auto i = g_pSurfaceNext->GetPopupCount() - 1; i >= 0; --i)
        {
            auto pPopup = VHandleToPanel(g_pSurfaceNext->GetPopup(i));

            auto wantsMouse = pPopup->IsMouseInputEnabled();

            auto isVisible = !g_pSurfaceNext->IsMinimized(VPanelToHandle(pPopup));

            auto pParent = pPopup->GetParent();

            while (isVisible && pParent)
            {
                isVisible = pParent->IsVisible();
                pParent = pParent->GetParent();
            }

            if (wantsMouse && isVisible)
            {
                topMost = pPopup->Client()->IsWithinTraverse(x, y, false);

                if (topMost)
                    break;
            }
        }

        if (!topMost)
        {
            topMost = VHandleToPanel(
                g_pSurfaceNext->GetEmbeddedPanel()
            )->Client()->IsWithinTraverse(x, y, false);
        }
    }

    if (!topMost)
    {
        SetMouseFocus(NULL_HANDLE);
        return;
    }

    VPANEL focus = NULL_HANDLE;

    if (IsChildOfModalPanel(topMost))
        focus = topMost;

    SetMouseFocus(focus);
}

void CInputWin32::PanelDeleted(VPANEL panel)
{
    auto pPanel = VHandleToPanel(panel);

    for (int i = m_Contexts.Head(); i != m_Contexts.InvalidIndex(); i = m_Contexts.Next(i))
    {
        PanelDeleted(pPanel, m_Contexts[i]);
    }

    PanelDeleted(pPanel, m_DefaultInputContext);
}

void CInputWin32::InternalCursorMoved(int x, int y)
{
    auto pCtx = GetCurrentContext();

    if (pCtx->m_nCursorX != x || pCtx->m_nCursorY != y)
    {
        pCtx->m_nCursorX = x;
        pCtx->m_nCursorY = y;

        UpdateMouseFocus(x, y);

        if (pCtx->_mouseCapture)
        {
            if (IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseCapture)))
            {
                if (pCtx->_mouseOver &&
                    pCtx->_mouseOver != pCtx->_mouseCapture &&
                    pCtx->_mouseOver->HasParent(pCtx->_mouseCapture))
                {
                    g_pIVgui->PostMessage(
                        VPanelToHandle(pCtx->_mouseOver),
                        new KeyValues("CursorMoved", "xpos", x, "ypos", y),
                        NULL_HANDLE
                    );
                }

                g_pIVgui->PostMessage(
                    VPanelToHandle(pCtx->_mouseCapture),
                    new KeyValues("CursorMoved", "xpos", x, "ypos", y),
                    NULL_HANDLE
                );
            }
        }
        else if (pCtx->_mouseFocus)
        {
            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_mouseFocus),
                new KeyValues("CursorMoved", "xpos", x, "ypos", y),
                NULL_HANDLE
            );
        }
    }
}

void CInputWin32::InternalMousePressed(MouseCode code)
{
    auto pCtx = GetCurrentContext();

    pCtx->_mousePressed[code] = true;
    pCtx->_mouseDown[code] = true;

    if (pCtx->_mouseCapture &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseCapture)))
    {
        bool bShouldSetCapture = false;

        if (pCtx->_mouseOver &&
            pCtx->_mouseOver != pCtx->_mouseCapture &&
            pCtx->_mouseOver->HasParent(pCtx->_mouseCapture))
        {
            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_mouseOver),
                new KeyValues("MousePressed", "code", code),
                NULL_HANDLE
            );
        }
        else
            bShouldSetCapture = true;

        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseCapture),
            new KeyValues("MousePressed", "code", code),
            NULL_HANDLE
        );

        if (bShouldSetCapture)
            SetMouseCapture(NULL_HANDLE);
    }
    else if (pCtx->_mouseFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseFocus),
            new KeyValues("MousePressed", "code", code),
            NULL_HANDLE
        );
    }

    if (IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseOver)))
    {
        g_pSurfaceNext->SetTopLevelFocus(VPanelToHandle(pCtx->_mouseOver));
    }

    if (m_hContext == DEFAULT_INPUT_CONTEXT)
    {
        if (_updateToggleButtonState)
            _updateToggleButtonState = false;
    }
}

void CInputWin32::InternalMouseDoublePressed(MouseCode code)
{
    auto pCtx = GetCurrentContext();

    pCtx->_mouseDoublePressed[code] = true;

    if (pCtx->_mouseCapture &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseCapture)))
    {
        if (pCtx->_mouseOver &&
            pCtx->_mouseOver != pCtx->_mouseCapture &&
            pCtx->_mouseOver->HasParent(pCtx->_mouseCapture))
        {
            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_mouseOver),
                new KeyValues("MouseDoublePressed", "code", code),
                NULL_HANDLE
            );
        }

        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseCapture),
            new KeyValues("MouseDoublePressed", "code", code),
            NULL_HANDLE
        );
    }
    else if (pCtx->_mouseFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseFocus),
            new KeyValues("MouseDoublePressed", "code", code),
            NULL_HANDLE
        );
    }

    if (IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseOver)))
    {
        g_pSurfaceNext->SetTopLevelFocus(VPanelToHandle(pCtx->_mouseOver));
    }
}

void CInputWin32::InternalMouseReleased(MouseCode code)
{
    auto pCtx = GetCurrentContext();

    pCtx->_mouseReleased[code] = true;
    pCtx->_mouseDown[code] = false;

    if (pCtx->_mouseCapture &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseCapture)))
    {
        if (pCtx->_mouseOver &&
            pCtx->_mouseOver != pCtx->_mouseCapture &&
            pCtx->_mouseOver->HasParent(pCtx->_mouseCapture))
        {
            g_pIVgui->PostMessage(
                VPanelToHandle(pCtx->_mouseOver),
                new KeyValues("MouseReleased", "code", code),
                NULL_HANDLE
            );
        }

        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseCapture),
            new KeyValues("MouseReleased", "code", code),
            NULL_HANDLE
        );
    }
    else if (pCtx->_mouseFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseFocus),
            new KeyValues("MouseReleased", "code", code),
            NULL_HANDLE
        );
    }
}

void CInputWin32::InternalMouseWheeled(int delta)
{
    auto pCtx = GetCurrentContext();

    if (pCtx->_mouseFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_mouseFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_mouseFocus),
            new KeyValues("MouseWheeled", "delta", delta),
            NULL_HANDLE
        );
    }
}

void CInputWin32::InternalKeyCodePressed(KeyCode code)
{
    if (!IsKeyCode(code))
        return;

    auto pCtx = GetCurrentContext();

    pCtx->_keyDown[code] = true;
    pCtx->_keyPressed[code] = true;

    auto pMessage = new KeyValues("KeyCodePressed", "code", code);

    if (pCtx->_keyFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_keyFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_keyFocus),
            pMessage,
            NULL_HANDLE
        );
    }
    else
    {
        pMessage->deleteThis();
    }

    if (m_hContext == DEFAULT_INPUT_CONTEXT)
    {
        if (_updateToggleButtonState)
            _updateToggleButtonState = false;
    }
}

void CInputWin32::InternalKeyCodeTyped(KeyCode code)
{
    if (!IsKeyCode(code))
        return;

    auto pCtx = GetCurrentContext();

    pCtx->_keyTyped[code] = true;

    auto pMessage = new KeyValues("KeyCodeTyped", "code", code);

    if (pCtx->_keyFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_keyFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_keyFocus),
            pMessage,
            NULL_HANDLE
        );
    }
    else
    {
        pMessage->deleteThis();
    }
}

void CInputWin32::InternalKeyTyped(wchar_t unichar)
{
    auto pMessage = new KeyValues("KeyTyped", "unichar", unichar);

    auto pCtx = GetCurrentContext();

    if (pCtx->_keyFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_keyFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_keyFocus),
            pMessage,
            NULL_HANDLE
        );
    }
    else
    {
        pMessage->deleteThis();
    }
}

void CInputWin32::InternalKeyCodeReleased(KeyCode code)
{
    if (!IsKeyCode(code))
        return;

    auto pCtx = GetCurrentContext();

    pCtx->_keyReleased[code] = true;
    pCtx->_keyDown[code] = false;

    //Could avoid allocating this by doing this inside the if body
    auto pMessage = new KeyValues("KeyCodeReleased", "code", code);

    if (pCtx->_keyFocus &&
        IsChildOfModalPanel(VPanelToHandle(pCtx->_keyFocus)))
    {
        g_pIVgui->PostMessage(
            VPanelToHandle(pCtx->_keyFocus),
            pMessage,
            NULL_HANDLE
        );
    }
    else
    {
        pMessage->deleteThis();
    }
}

HInputContext CInputWin32::CreateInputContext()
{
    auto index = m_Contexts.AddToTail();

    InitInputContext(&m_Contexts[index]);

    return index;
}

void CInputWin32::DestroyInputContext(HInputContext context)
{
    if (m_hContext == context)
        ActivateInputContext(DEFAULT_INPUT_CONTEXT);

    m_Contexts.Remove(context);
}

void CInputWin32::AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot)
{
    auto pCtx = GetContext(context);

    if (pCtx->_rootPanel != pRoot)
    {
        ResetInputContext(context);
        pCtx->_rootPanel = pRoot;
    }
}

void CInputWin32::ActivateInputContext(HInputContext context)
{
    m_hContext = context;
}

VPANEL CInputWin32::GetMouseCapture()
{
    return VPanelToHandle(m_DefaultInputContext._mouseCapture);
}

bool CInputWin32::IsChildOfModalPanel(VPANEL panel)
{
    if (panel)
    {
        auto pModal = GetCurrentContext()->_appModalPanel;

        if (pModal)
        {
            return VHandleToPanel(panel)->HasParent(pModal);
        }
    }

    return true;
}

void CInputWin32::ResetInputContext(HInputContext context)
{
    InitInputContext(GetCurrentContext());
}

void CInputWin32::InitInputContext(InputContext_t* pContext)
{
    pContext->_rootPanel = NULL_HANDLE;

    pContext->_keyFocus = nullptr;
    pContext->_oldMouseFocus = nullptr;
    pContext->_mouseFocus = nullptr;
    pContext->_mouseOver = nullptr;
    pContext->_mouseCapture = nullptr;
    pContext->_appModalPanel = nullptr;

    pContext->m_nCursorX = 0;
    pContext->m_nCursorY = 0;

    memset(pContext->_mousePressed, 0, sizeof(pContext->_mousePressed));
    memset(pContext->_mouseDoublePressed, 0, sizeof(pContext->_mouseDoublePressed));
    memset(pContext->_mouseDown, 0, sizeof(pContext->_mouseDown));
    memset(pContext->_mouseReleased, 0, sizeof(pContext->_mouseReleased));
    memset(pContext->_keyPressed, 0, sizeof(pContext->_keyPressed));
    memset(pContext->_keyTyped, 0, sizeof(pContext->_keyTyped));
    memset(pContext->_keyDown, 0, sizeof(pContext->_keyDown));
    memset(pContext->_keyReleased, 0, sizeof(pContext->_keyReleased));
}

CInputWin32::InputContext_t* CInputWin32::GetContext(HInputContext context)
{
    if (context == DEFAULT_INPUT_CONTEXT)
        return &m_DefaultInputContext;

    return &m_Contexts[m_hContext];
}

CInputWin32::InputContext_t* CInputWin32::GetCurrentContext()
{
    return GetContext(m_hContext);
}

void CInputWin32::PanelDeleted(VPanel* pFocus, InputContext_t& context)
{
    if (context._keyFocus == pFocus)
    {
        context._keyFocus = nullptr;
    }

    if (context._oldMouseFocus == pFocus)
    {
        context._oldMouseFocus = nullptr;
    }

    if (context._mouseFocus == pFocus)
    {
        context._mouseFocus = nullptr;
    }

    if (context._mouseOver == pFocus)
    {
        context._mouseOver = nullptr;
    }

    if (context._mouseCapture == pFocus)
    {
        SetMouseCapture(NULL_HANDLE);
        context._mouseCapture = nullptr;
    }

    if (context._appModalPanel == pFocus)
    {
        ReleaseAppModalSurface();
    }
}

VPanel* CInputWin32::CalculateNewKeyFocus()
{
    auto pCtx = GetCurrentContext();

    VPanel* pFocus;

    if (pCtx->_rootPanel)
    {
        pFocus = VHandleToPanel(
            VHandleToPanel(
                pCtx->_rootPanel
            )->Client()->GetCurrentKeyFocus()
        );

        if (!pFocus)
            pFocus = VHandleToPanel(pCtx->_rootPanel);
    }
    else
    {
        pFocus = nullptr;

        for (auto i = g_pSurfaceNext->GetPopupCount() - 1; i >= 0; --i)
        {
            auto pPopup = VHandleToPanel(g_pSurfaceNext->GetPopup(i));

            if (!pPopup->IsPopup() ||
                !pPopup->IsVisible() ||
                !pPopup->IsKeyBoardInputEnabled() ||
                g_pSurfaceNext->IsMinimized(VPanelToHandle(pPopup)))
                continue;

            auto isVisible = pPopup->IsVisible();

            auto pParent = pPopup->GetParent();

            while (pParent && (isVisible = pParent->IsVisible()))
            {
                pParent = pParent->GetParent();
            }

            if (!isVisible)
                continue;

            if (!g_pSurfaceNext->IsMinimized(VPanelToHandle(pPopup)))
            {
                pFocus = VHandleToPanel(
                    pPopup->Client()->GetCurrentKeyFocus()
                );

                if (!pFocus)
                    pFocus = pPopup;
                break;
            }
        }
    }

    if (!g_pSurfaceNext->HasFocus())
        pFocus = nullptr;

    if (!IsChildOfModalPanel(VPanelToHandle(pFocus)))
        pFocus = nullptr;

    return pFocus;
}
