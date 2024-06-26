//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include <tier0/dbg.h>
#include <FileSystem.h>
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>
#include <clocale>
#include <vgui/SchemeEx.h>
#include <vgui/ImageEx.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern int g_nYou_Must_Add_Public_Vgui_Controls_Vgui_ControlsCpp_To_Your_Project;

namespace vgui2
{

class CSchemeManagerWrapper : public ISchemeManagerEx
{
    std::unordered_map<vgui2::HScheme, SchemeEx*> scheme_ex_;
    std::unordered_map<vgui2::HScheme, std::string> scheme_path_;
    std::unordered_map<std::string, ImageEx*> image_ex_;

public:
	ISchemeManager *m_pEngineIface = nullptr;

	virtual HScheme LoadSchemeFromFile(const char *fileName, const char *tag)
	{
        return LoadSchemeFromFilePath(fileName, nullptr, tag);
	}

	virtual void ReloadSchemes()
	{
		m_pEngineIface->ReloadSchemes();
	}

	virtual HScheme GetDefaultScheme()
	{
		return m_pEngineIface->GetDefaultScheme();
	}

	virtual HScheme GetScheme(const char *tag)
	{
		return m_pEngineIface->GetScheme(tag);
	}

	virtual IImage *GetImage(const char *imageName, bool hardwareFiltered)
	{
        if (image_ex_.count(imageName) == 0)
        {
            auto* image_ex = new ImageEx(g_pVGuiSurface, m_pEngineIface->GetImage(imageName, hardwareFiltered));
            image_ex_.emplace(imageName, image_ex);

            return image_ex;
        }

		return image_ex_[imageName];
	}

	virtual HTexture GetImageID(const char *imageName, bool hardwareFiltered)
	{
		return m_pEngineIface->GetImageID(imageName, hardwareFiltered);
	}

	virtual IScheme *GetIScheme(HScheme scheme)
	{
        if (scheme_ex_.count(scheme) == 0)
        {
            const std::string& path = scheme_path_.count(scheme) ? scheme_path_[scheme] : "";

            auto* scheme_ex = new SchemeEx(m_pEngineIface->GetIScheme(scheme), path);
            scheme_ex_.emplace(scheme, scheme_ex);

            return scheme_ex;
        }

        return scheme_ex_[scheme];
	}

	virtual void Shutdown(bool full = true)
	{
		return m_pEngineIface->Shutdown(full);
	}

	virtual int GetProportionalScaledValue(int normalizedValue)
	{
		return m_pEngineIface->GetProportionalScaledValue(normalizedValue);
	}

	virtual int GetProportionalNormalizedValue(int scaledValue)
	{
		return m_pEngineIface->GetProportionalNormalizedValue(scaledValue);
	}

	// Methods from Source that are wrapped around GoldSrc methods
	virtual HScheme LoadSchemeFromFileEx(VPANEL sizingPanel, const char *fileName, const char *tag)
	{
		return LoadSchemeFromFile(fileName, tag);
	}

	virtual int GetProportionalScaledValueEx(HScheme scheme, int normalizedValue)
	{
		return GetProportionalScaledValue(normalizedValue);
	}

	virtual int GetProportionalNormalizedValueEx(HScheme scheme, int scaledValue)
	{
		return GetProportionalNormalizedValue(scaledValue);
	}

	virtual HScheme LoadSchemeFromFilePath(const char *fileName, const char *pathID, const char *tag)
	{
		/**
		 * Since Source 2007 KeyValues support platform conditionals:
		 *     "name" "Tahoma"  [!$OSX]
		 *     "name" "Verdana" [$OSX]
		 * GoldSource, however, does not, so schemes containing them will be parsed incorrectly.
		 *
		 * This LoadSchemeFromFile will load fileName, parse conditionals and save the file
		 * as filename.res.i and load it with m_pEngineIface->LoadSchemeFromFile.
		 *
		 * It will only do that if file was changed since last time it was compiled.
		 */

		constexpr int SCHEME_VERSION = 2;

		char fileNameComp[256];
		snprintf(fileNameComp, sizeof(fileNameComp), "%s.i", fileName);

		// Get orig file modification date
		int origModDate = g_pFullFileSystem->GetFileTime(fileName);

		// Try to open compiled file
		bool bNeedRecompile = false;
		KeyValues *compiled = new KeyValues("Scheme");

		if (compiled->LoadFromFile(g_pFullFileSystem, fileNameComp, pathID))
		{
			// Check modification date and version
			int modDate = compiled->GetInt("OrigModDate");
			int version = compiled->GetInt("PreprocessorVersion", 0);
			if (origModDate == 0 || modDate == 0 || origModDate != modDate || version != SCHEME_VERSION)
			{
				bNeedRecompile = true;
			}
			else
			{
				// Also check includes
				KeyValues *includes = compiled->FindKey("Includes");

				if (includes)
				{
					for (KeyValues *sub = includes->GetFirstValue(); sub; sub = sub->GetNextValue())
					{
						const char *includeName= sub->GetName();
						int date = sub->GetInt();

						if (date != g_pFullFileSystem->GetFileTime(includeName))
						{
							bNeedRecompile = true;
							break;
						}
					}
				}
			}
		}
		else
		{
			// Failed to open
			bNeedRecompile = true;
		}
		compiled->deleteThis();

		if (bNeedRecompile)
		{
			Msg(_T("LoadSchemeFromFile: '%s' has changed, will be recompiled.\n"), fileName);

			// Load original file
			KeyValuesAD orig(new KeyValues("Scheme"));
			std::set<std::string> includeList;

			if (orig->LoadFromFile(g_pFullFileSystem, fileName, pathID, &includeList))
			{
				// Set modification date and version
				orig->SetInt("OrigModDate", origModDate);
				orig->SetInt("PreprocessorVersion", SCHEME_VERSION);

				// Save all include dates as well
				KeyValues *includes = new KeyValues("Includes");

				for (const std::string &i : includeList)
				{
					int date = g_pFullFileSystem->GetFileTime(i.c_str());

					// A hack so we can have slashes in the key name
					// Otherwise they would've been parsed as a path
					KeyValues *kv = new KeyValues(i.c_str());
					kv->SetInt(nullptr, date);
					includes->AddSubKey(kv);
				}

				orig->AddSubKey(includes);

				// Save new file
				char dir[256];
				V_ExtractFilePath(fileName, dir, sizeof(dir));
				g_pFullFileSystem->CreateDirHierarchy(dir, pathID);
				orig->SaveToFile(g_pFullFileSystem, fileNameComp, pathID);
			}
			else
			{
				Error(_T("LoadSchemeFromFile: Failed to open '%s'\n"), fileName);
				return NULL_HANDLE;
			}
		}

		// Load compiled file
        HScheme scheme =  m_pEngineIface->LoadSchemeFromFile(fileNameComp, tag);
        scheme_path_[scheme] = fileName;

        return scheme;
	}
};

static CSchemeManagerWrapper s_SchemeManagerWrapper;

static char g_szControlsModuleName[256];

ISchemeManagerEx *scheme()
{
    return &s_SchemeManagerWrapper;
}

//-----------------------------------------------------------------------------
// Purpose: Initializes the controls
//-----------------------------------------------------------------------------
extern "C" { extern int _heapmin(); }

bool VGui_InitInterfacesList( const char *moduleName, CreateInterfaceFn *factoryList, int numFactories )
{
	g_nYou_Must_Add_Public_Vgui_Controls_Vgui_ControlsCpp_To_Your_Project = 1;

	// If you hit this error, then you need to include memoverride.cpp in the project somewhere or else
	// you'll get crashes later when vgui_controls allocates KeyValues and vgui tries to delete them.
#if !defined(NO_MALLOC_OVERRIDE) && defined( WIN32 )
	if ( _heapmin() != 1 )
	{
		Assert( false );
		Error( "Must include memoverride.cpp in your project." );
	}
#endif	
	// keep a record of this module name
	strncpy(g_szControlsModuleName, moduleName, sizeof(g_szControlsModuleName));
	g_szControlsModuleName[sizeof(g_szControlsModuleName) - 1] = 0;

	// initialize our locale (must be done for every vgui dll/exe)
	setlocale(LC_CTYPE, "");
	setlocale(LC_TIME, "");
	setlocale(LC_COLLATE, "");
	setlocale(LC_MONETARY, "");

	// NOTE: Vgui expects to use these interfaces which are defined in tier3.lib
	if ( !g_pVGui || !g_pVGuiInput || !g_pVGuiPanel || 
		 !g_pVGuiSurface || !g_pVGuiSchemeManager || !g_pVGuiSystem )
	{
		Warning(_T("vgui_controls is missing a required interface!\n"));
		return false;
	}

	s_SchemeManagerWrapper.m_pEngineIface = g_pVGuiSchemeManager;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: returns the name of the module this has been compiled into
//-----------------------------------------------------------------------------
const char *GetControlsModuleName()
{
	return g_szControlsModuleName;
}

} // namespace vgui2



