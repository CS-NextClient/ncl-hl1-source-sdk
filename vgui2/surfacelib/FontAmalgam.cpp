#include <tier0/platform.h>

#include "FontAmalgam.h"
//#include "Win32Font.h"

// the project builds with NOMINMAX; restore Valve's lowercase min/max for this TU
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CFontAmalgam::CFontAmalgam()
	: m_CharCacheExt( DefLessFunc( wchar_t ) )
{
	m_Fonts.EnsureCapacity(4);
	m_iMaxHeight = 0;
	m_iMaxWidth = 0;
	Q_memset(m_CharCache, 0, sizeof(m_CharCache));
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CFontAmalgam::~CFontAmalgam()
{
}

//-----------------------------------------------------------------------------
// Purpose: Data accessor
//-----------------------------------------------------------------------------
const char *CFontAmalgam::Name()
{
	return m_szName;
}

//-----------------------------------------------------------------------------
// Purpose: Data accessor
//-----------------------------------------------------------------------------
void CFontAmalgam::SetName(const char *name)
{
	Q_strncpy(m_szName, name, sizeof(m_szName));
}

//-----------------------------------------------------------------------------
// Purpose: adds a font to the amalgam
//-----------------------------------------------------------------------------
void CFontAmalgam::AddFont(font_t *font, int lowRange, int highRange)
{
	int i = m_Fonts.AddToTail();

	m_Fonts[i].font = font;
	m_Fonts[i].lowRange = lowRange;
	m_Fonts[i].highRange = highRange;

	m_iMaxHeight = max(font->GetHeight(), m_iMaxHeight);
	m_iMaxWidth = max(font->GetMaxCharWidth(), m_iMaxWidth);

	// the chain changed, so previously resolved per-char faces may be wrong now
	Q_memset(m_CharCache, 0, sizeof(m_CharCache));
	m_CharCacheExt.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: clears the fonts
//-----------------------------------------------------------------------------
void CFontAmalgam::RemoveAll()
{
	// clear out
	m_Fonts.RemoveAll();
	m_iMaxHeight = 0;
	m_iMaxWidth = 0;
	Q_memset(m_CharCache, 0, sizeof(m_CharCache));
	m_CharCacheExt.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: returns the font for the given character
//-----------------------------------------------------------------------------
font_t *CFontAmalgam::GetFontForChar(int ch)
{
	bool direct = (ch >= 0 && ch < kCharCacheSize);
	if (direct)
	{
		if (m_CharCache[ch])
		{
			return m_CharCache[ch];
		}
	}
	else
	{
		auto slot = m_CharCacheExt.Find((wchar_t)ch);
		if (m_CharCacheExt.IsValidIndex(slot))
		{
			return m_CharCacheExt[slot];
		}
	}

	// walk the chain in order and take the first face that covers the range and has the glyph
	font_t *firstInRange = NULL;
	font_t *result = NULL;
	for (int i = 0; i < m_Fonts.Count(); i++)
	{
		if (ch >= m_Fonts[i].lowRange && ch <= m_Fonts[i].highRange)
		{
			if (!firstInRange)
			{
				firstInRange = m_Fonts[i].font;
			}

			if (m_Fonts[i].font->HasChar(ch))
			{
				Assert(m_Fonts[i].font->IsValid());
				result = m_Fonts[i].font;
				break;
			}
		}
	}

	// nothing has the glyph: use the first range-matching face so its missing-glyph box still renders
	if (!result)
	{
		result = firstInRange;
	}

	if (direct)
	{
		m_CharCache[ch] = result;
	}
	else
	{
		m_CharCacheExt.Insert((wchar_t)ch, result);
	}
	return result;
}

//-----------------------------------------------------------------------------
// Purpose: returns the primary (first) face in the chain, which defines the baseline
//-----------------------------------------------------------------------------
font_t *CFontAmalgam::GetPrimaryFont()
{
	if (!m_Fonts.Count())
	{
		return NULL;
	}
	return m_Fonts[0].font;
}

//-----------------------------------------------------------------------------
// Purpose: returns the max height of the font set
//-----------------------------------------------------------------------------
int CFontAmalgam::GetFontHeight()
{
	if (!m_Fonts.Count())
	{
		return m_iMaxHeight;
	}
	return m_Fonts[0].font->GetHeight();
}

//-----------------------------------------------------------------------------
// Purpose: returns the maximum width of a character in a font
//-----------------------------------------------------------------------------
int CFontAmalgam::GetFontMaxWidth()
{
	return m_iMaxWidth;
}

//-----------------------------------------------------------------------------
// Purpose: returns the name of the font that is loaded
//-----------------------------------------------------------------------------
const char *CFontAmalgam::GetFontName(int i)
{
	return m_Fonts[i].font->GetName();
}

//-----------------------------------------------------------------------------
// Purpose: returns the name of the font that is loaded
//-----------------------------------------------------------------------------
int CFontAmalgam::GetFlags(int i)
{
	if (m_Fonts[i].font)
	{
		return m_Fonts[i].font->GetFlags();
	}
	else
	{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: returns the number of fonts this amalgam contains
//-----------------------------------------------------------------------------
int CFontAmalgam::GetCount()
{
	return m_Fonts.Count();
}


//-----------------------------------------------------------------------------
// Purpose: returns the max height of the font set
//-----------------------------------------------------------------------------
bool CFontAmalgam::GetUnderlined()
{
	if (!m_Fonts.Count())
	{
		return false;
	}
	return m_Fonts[0].font->GetUnderlined();
}
