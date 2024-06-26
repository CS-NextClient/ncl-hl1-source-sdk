//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Creates a HTML control
//
// $NoKeywords: $
//=============================================================================//

#ifndef HTML_H
#define HTML_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI2.h>
#include <vgui/IImage.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/FileOpenDialog.h>
#include <vgui_controls/TextEntry.h>
#include <html/ihtmlchrome.h>
#include <tier1/UtlMap.h>
#include <tier1/UtlString.h>
#include <string>
#include <vector>

class HTMLComboBoxHost;
class IHTMLSerializer;
class CPopupInputProxy;

namespace vgui2
{

//-----------------------------------------------------------------------------
// Purpose: Control to display HTML content
//			This control utilises a hidden IE window to render a HTML page for you.
//			It can load any valid URL (i.e local files or web pages), you cannot dynamically change the
//			content however (internally to the control that is).
//-----------------------------------------------------------------------------
class HTML: public Panel, public IHTMLResponses_HL1
{
	DECLARE_CLASS_SIMPLE( HTML, Panel );
	// TODO::STYLE
	//DECLARE_STYLE_BASE( "HTML" );
public:

	HTML(Panel *parent, const char *name);
	~HTML();

	void InitializeBrowser(const char* userAgent, bool allowJavaScript = true, bool bPopupWindow = false);

	bool IsAllowJavaScript();
	const char* GetUserAgent();

	struct LangSettings_t {
		std::string prefered = "english";
		std::vector<std::string> available = { "english" };
	};

	void SetLangSettings(std::string prefered, std::vector<std::string> available);
	void SetLangSettings(LangSettings_t* settings);
	LangSettings_t* GetLangSettings();

	void SetSteamId(uint64 steamId3);
	uint64 GetSteamId();

	// IHTML pass through functions
	virtual void OpenURL( const char *URL, const char *pchPostData, bool bForce = false );
	virtual bool StopLoading();
	virtual bool Refresh();
	virtual void OnMove();
	virtual void RunJavascript( const char *pchScript );
	virtual void BrowserErrorStrings( 
		const char *pchTitle, const char *pchHeader, const char *pchDetailCacheMiss, 
		const char *pchDetailBadUURL, const char *pchDetailConnectionProblem, 
		const char *pchDetailProxyProblem, const char *pchDetailUnknown
	);
	virtual void GoBack();
	virtual void GoForward();
	virtual bool BCanGoBack();
	virtual bool BCanGoFoward();

	// event functions you can override and specialize behavior of
	virtual bool OnStartRequest( const char *url, const char *target, const char *pchPostData, bool bIsRedirect );
	virtual void OnFinishRequest(const char *url, const char *pageTitle/*, const CUtlMap < CUtlString, CUtlString > &headers*/ ) {}
	virtual void OnSetHTMLTitle( const char *pchTitle ) {}
	virtual void OnLinkAtPosition( const char *pchURL ) {}
	virtual void OnURLChanged( const char *url, const char *pchPostData, bool bIsRedirect ) {}

	virtual bool OnOpenNewTab( const char *pchURL, bool bForeground ) { return false; }

	// configuration
	virtual void SetScrollbarsEnabled(bool state);
	virtual void SetContextMenuEnabled(bool state);
	virtual void SetViewSourceEnabled( bool state );
	virtual void NewWindowsOnly( bool state );

	bool IsScrolledToBottom();
	bool IsScrollbarVisible();

	// url handlers, lets you have web page links fire vgui events
	// use to have custom web page links, eg. "steam://open/subscriptionpage"
	// message contains "CustomURL", "url"
	virtual void AddCustomURLHandler(const char *customProtocolName, vgui2::Panel *target);

	// overridden to paint our special web browser texture
	virtual void Paint();
	
	// pass messages to the texture component to tell it about resizes
	virtual void OnSizeChanged(int wide,int tall);

	// pass mouse clicks through
	virtual void OnMousePressed(MouseCode code);
	virtual void OnMouseReleased(MouseCode code);
	virtual void OnCursorMoved(int x,int y);
	virtual void OnMouseDoublePressed(MouseCode code);
	virtual void OnKeyTyped(wchar_t unichar);
	virtual void OnKeyCodeTyped(KeyCode code);
	virtual void OnKeyCodeReleased(KeyCode code);
	virtual void PerformLayout();
	virtual void OnMouseWheeled(int delta);
	virtual void PostChildPaint();

	/* message posting:

		"HTMLSliderMoved"	- indicates the scrollbar has moved

		"OnURLChanged"		- indicates a new URL is being loaded
		    "url"
			"postdata"

		"OnFinishRequest"		- indicates all url loaded has completed

		"HTMLBackRequested"		- mouse4 has been pressed on the dialog
		"HTMLForwardRequested"  - mouse5 has been pressed on the dialog

		"SecurityStatus"		- indicates the SSL status of the page (disabled,good,bad)
			"url"
			"secure" - true if an ssl page
			"certerror" - true if there is a cert error loading the page
			"isevcert" - true if this is an EV style cert
			"certname" - name of the entity this cert is issued to
	*/

	MESSAGE_FUNC_INT( OnSetCursorVGUI, "SetCursor", cursor );

	virtual void OnCommand( const char *pchCommand );

	void AddHeader( const char *pchHeader, const char *pchValue );
	void OnKillFocus();
	void OnSetFocus();
	
	void Find( const char *pchSubStr );
	void StopFind();
	void FindNext();
	void FindPrevious();
	void ShowFindDialog();
	void HideFindDialog();
	bool FindDialogVisible();
	int HorizontalScrollMax() { return m_scrollHorizontal.m_nMax; }
	int VerticalScrollMax() { return m_scrollVertical.m_nMax; }

	void GetLinkAtPosition( int x, int y );

	void HidePopup();

#ifdef DBGFLAG_VALIDATE
	virtual void Validate( CValidator &validator, const char *pchName )
	{
		ValidateObj( m_CustomURLHandlers );
		BaseClass::Validate( validator, pchName );
	}
#endif // DBGFLAG_VALIDATE

	void PaintComboBox();
protected:
	virtual void ApplySchemeSettings( IScheme *pScheme );

	IHTMLSerializer *m_Serializer = nullptr;

	friend class ::HTMLComboBoxHost;
	vgui2::Menu *m_pContextMenu;

	CPopupInputProxy* m_pInputProxy = NULL;

private:
	// IHTMLResponses_HL1 callbacks from the browser engine
	virtual void BrowserReady() override;
	virtual void BrowserNeedsPaint( int textureid, 
									int wide, int tall, const unsigned char* rgba, 
									int updatex, int updatey, int updatewide, int updatetall, 
									int combobox_wide, int combobox_tall, const unsigned char* combobox_rgba ) override;
	virtual void BrowserStartRequest( const char *url, const char *target, const char *postdata, bool isredirect ) override;
	virtual void BrowserURLChanged( const char *url, const char *postdata, bool isredirect ) override;
	virtual void BrowserFinishedRequest( const char *url, const char *pagetitle ) override;
	virtual void BrowserShowPopup() override;
	virtual void BrowserHidePopup() override;
	virtual void BrowserSizePopup( int x, int y, int wide, int tall ) override;
	virtual void BrowserHorizontalScrollBarSizeResponse( int x, int y, int wide, int tall, int scroll, int scroll_max, float zoom ) override;
	virtual void BrowserVerticalScrollBarSizeResponse( int x, int y, int wide, int tall, int scroll, int scroll_max, float zoom ) override;
	virtual void BrowserGetZoomResponse( float flZoom ) override;
	virtual void BrowserCanGoBackandForward( bool bgoback, bool bgoforward ) override;
	virtual void BrowserJSAlert( const char *message ) override;
	virtual void BrowserJSConfirm( const char *message ) override;
	virtual void BrowserPopupHTMLWindow( const char *url, int wide, int tall, int x, int y ) override;
	virtual void BrowserSetHTMLTitle( const char *title ) override;
	virtual void BrowserLoadingResource() override;
	virtual void BrowserStatusText( const char *text ) override;
	virtual void BrowserSetCursor( int in_cursor ) override;
	virtual void BrowserFileLoadDialog() override;
	virtual void BrowserShowToolTip( const char *text ) override;
	virtual void BrowserUpdateToolTip( const char *text ) override;
	virtual void BrowserHideToolTip() override;
	virtual void BrowserClose() override;
	virtual void BrowserLinkAtPositionResponse( const char *url, int x, int y ) override;

	void PostURL(const char *URL, const char *pchPostData/*, bool force*/);
	virtual void BrowserResize();
	virtual void CalcScrollBars();
	void UpdateSizeAndScrollBars();
	
	MESSAGE_FUNC( OnSliderMoved, "ScrollBarSliderMoved" );
	MESSAGE_FUNC_CHARPTR( OnFileSelected, "FileSelected", fullpath );
	MESSAGE_FUNC( OnFileSelectionCancelled, "FileSelectionCancelled" );
	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );
	MESSAGE_FUNC_PTR( OnEditNewLine, "TextNewLine", panel );
	MESSAGE_FUNC_INT( DismissJSDialog, "DismissJSDialog", result );

	void UpdateCachedHTMLValues();

	LangSettings_t m_LangSettings;
	uint64 m_uSteamId = 0;

	void TrySetHeadersFromLangSettings();
	void TrySetHeadersFromSteamId();

	vgui2::Panel *m_pInteriorPanel;
	vgui2::ScrollBar *_hbar,*_vbar;
	vgui2::DHANDLE<vgui2::FileOpenDialog> m_hFileOpenDialog;
	class CHTMLFindBar : public vgui2::EditablePanel
	{
		DECLARE_CLASS_SIMPLE( CHTMLFindBar, EditablePanel );
	public:
		CHTMLFindBar( HTML *parent );
		void SetText( const char *pchText ) { m_pFindBar->SetText( pchText ); }
		void GetText( char *pText, int ccText ) { m_pFindBar->GetText( pText, ccText ); }
		void OnCommand( const char *pchCmd );
		void ShowCountLabel() { m_pFindCountLabel->SetVisible( true ); }
		void HideCountLabel() { m_pFindCountLabel->SetVisible( false ); }
		void SetHidden( bool bState  ) { m_bHidden = bState; }
		bool BIsHidden() { return m_bHidden; }

	private:
		vgui2::TextEntry *m_pFindBar;
		vgui2::HTML *m_pParent;
		vgui2::Label *m_pFindCountLabel;
		bool m_bHidden;
	};

	CHTMLFindBar *m_pFindBar;
	HTMLComboBoxHost *m_pComboBoxHost;

	int m_iMouseX,m_iMouseY; // where the mouse is on the control

	int m_iScrollBorderX,m_iScrollBorderY;
	int m_iWideLastHTMLSize, m_iTalLastHTMLSize;
	int m_iCopyLinkMenuItemID;

	bool m_bScrollBarEnabled;
	bool m_bContextMenuEnabled;
	int	m_iScrollbarSize;
	bool m_bNewWindowsOnly;
	int m_nViewSourceAllowedIndex;
	CUtlString m_sDragURL;
	int m_iDragStartX, m_iDragStartY;

	struct CustomURLHandler_t
	{
		PHandle hPanel;
		char url[32];
	};
	CUtlVector<CustomURLHandler_t> m_CustomURLHandlers;

	int m_iHTMLTextureID; // vgui texture id
	// Track the texture width and height requested so we can tell
	// when the size has changed and reallocate the texture.
	int m_allocedTextureWidth;
	int m_allocedTextureHeight;
	int m_iComboBoxTextureID; // vgui texture id of the combo box
	bool m_bNeedsFullTextureUpload; 
	int m_allocedComboBoxWidth;
	int m_allocedComboBoxHeight;
	CUtlString m_sCurrentURL; // the url of our current page
	// find in page state
	bool m_bInFind;
	CUtlString m_sLastSearchString;

	bool m_bCanGoBack; // cache of forward and back state
	bool m_bCanGoForward;

	bool m_bAllowJavaScript = true;
	CUtlString m_sUserAgent;

	struct LinkAtPos_t
	{
		LinkAtPos_t() { m_nX = m_nY = 0; }
		uint32 m_nX;
		uint32 m_nY;
		CUtlString m_sURL;
	};
	LinkAtPos_t m_LinkAtPos; // cache for link at pos requests, because the request is async
	bool m_bRequestingDragURL; // true if we need a response for a drag url loc
	bool m_bRequestingCopyLink; // true if we wanted to copy the link under the cursor

	struct ScrollData_t
	{
		ScrollData_t() 
		{
			m_bVisible = false;
			m_nX = m_nY = m_nWide = m_nTall = m_nMax = m_nScroll = 0;
		}

		bool operator==( ScrollData_t const &src ) const
		{
			return m_bVisible == src.m_bVisible && 
				m_nX == src.m_nX &&
				m_nY == src.m_nY &&
				m_nWide == src.m_nWide &&
				m_nTall == src.m_nTall &&
				m_nMax == src.m_nMax &&
				m_nScroll == src.m_nScroll;
		}

		bool operator!=( ScrollData_t const &src ) const
		{	
			return !operator==(src);
		}


		bool m_bVisible; // is the scroll bar visible
		int m_nX; /// where cef put the scroll bar
		int m_nY;
		int m_nWide;
		int m_nTall;  // how many pixels of scroll in the current scroll knob
		int m_nMax; // most amount of pixels we can scroll
		int m_nScroll; // currently scrolled amount of pixels
		float m_flZoom; // zoom level this scroll bar is for
	};

	ScrollData_t m_scrollHorizontal; // details of horizontal scroll bar
	ScrollData_t m_scrollVertical; // details of vertical scroll bar
	float m_flZoom; // current page zoom level

	CUtlString m_sPendingURLLoad; // cache of url to load if we get a PostURL before the cef object is mage
	CUtlString m_sPendingPostData; // cache of the post data for above

	struct CustomCursorCache_t
	{
		CustomCursorCache_t() {}
		CustomCursorCache_t( const void *pchData ) { m_pchData = pchData; }
		float m_CacheTime; // the time we cached the cursor
		CursorCode m_Cursor; // the vgui handle to it
		const void *m_pchData; // the pointer to the cursor char data so we can detect the same cursor being used
		bool operator==(const CustomCursorCache_t& rhs) const
		{
			return m_pchData == rhs.m_pchData ; 
		}
	};
	//CUtlVector<CustomCursorCache_t> m_vecHCursor;
	//CUtlVector<HTMLCommandBuffer_t *> m_vecPendingMessages;
};

} // namespace vgui

#endif // HTML_H