#ifndef VGUI2_VGUI_SURFACELIB_FONTAMALGAM_H
#define VGUI2_VGUI_SURFACELIB_FONTAMALGAM_H

#include <tier1/utlvector.h>
#include <tier1/utlmap.h>

#include "vguifont.h"

class CFontAmalgam
{
public:
	CFontAmalgam();
	~CFontAmalgam();

	const char* Name();

	void SetName( const char* name );

	void AddFont(font_t* font, int lowRange, int highRange);

	font_t* GetFontForChar(int ch);

	font_t* GetPrimaryFont();

	int GetFontHeight();

	int GetFontMaxWidth();

	const char* GetFontName( int i );

	int GetFlags( int i );

	int GetCount();

	bool GetUnderlined();

	void RemoveAll();

private:
	struct TFontRange
	{
		int lowRange;
		int highRange;
		font_t* font;
	};

	CUtlVector<TFontRange> m_Fonts;

	// resolved face per char, filled lazily; HasChar is a GDI call and GetFontForChar runs every
	// frame for text measurement, so the result is cached. The common range is a direct array, the
	// rare tail (>= kCharCacheSize) a map. Both invalidated whenever the chain changes.
	static const int kCharCacheSize = 0x500;
	font_t* m_CharCache[ kCharCacheSize ];
	CUtlMap<wchar_t, font_t*> m_CharCacheExt;

	char m_szName[ 32 ];

	int m_iMaxWidth = 0;
	int m_iMaxHeight = 0;

private:
	CFontAmalgam( const CFontAmalgam& ) = delete;
	CFontAmalgam& operator=( const CFontAmalgam& ) = delete;
};

#endif //VGUI2_VGUI_SURFACELIB_FONTAMALGAM_H
