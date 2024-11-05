#include <interface.h>
#include <vgui/ILocalize.h>
#include <vstdlib/IKeyValuesSystem.h>

#include "vgui_internal.h"

// VGUI2 version of the IKeyValuesSystem interface
class IKeyValues : public IBaseInterface
{
public:
    // registers the size of the KeyValues in the specified instance
    // so it can build a properly sized memory pool for the KeyValues objects
    // the sizes will usually never differ but this is for versioning safety
    virtual void RegisterSizeofKeyValues(int size) = 0;

    // allocates/frees a KeyValues object from the shared mempool
    virtual void* AllocKeyValuesMemory(int size) = 0;
    virtual void FreeKeyValuesMemory(void* pMem) = 0;

    // symbol table access (used for key names)
    virtual HKeySymbol GetSymbolForString(const char* name) = 0;
    virtual const char* GetStringForSymbol(HKeySymbol symbol) = 0;

    //Used by GoldSource only. - Solokiller
    virtual void GetLocalizedFromANSI(const char* ansi, wchar_t* outBuf, int unicodeBufferSizeInBytes) = 0;
    virtual void GetANSIFromLocalized(const wchar_t* wchar, char* outBuf, int ansiBufferSizeInBytes) = 0;

    // for debugging, adds KeyValues record into global list so we can track memory leaks
    virtual void AddKeyValuesToMemoryLeakList(void* pMem, HKeySymbol name) = 0;
    virtual void RemoveKeyValuesFromMemoryLeakList(void* pMem) = 0;
};

#define IKEYVALUES_INTERFACE_VERSION "KeyValues003"

class KeyValuesWrapper : public IKeyValues, public IKeyValuesSystem
{
public:
    ~KeyValuesWrapper() override = default;

    void RegisterSizeofKeyValues(int size) override
    {
        KeyValuesSystem()->RegisterSizeofKeyValues(size);
    }

    void* AllocKeyValuesMemory(int size) override
    {
        return KeyValuesSystem()->AllocKeyValuesMemory(size);
    }

    void FreeKeyValuesMemory(void* pMem) override
    {
        KeyValuesSystem()->FreeKeyValuesMemory(pMem);
    }

    HKeySymbol GetSymbolForString(const char* name) override
    {
        return KeyValuesSystem()->GetSymbolForString(name);
    }

    const char* GetStringForSymbol(HKeySymbol symbol) override
    {
        return KeyValuesSystem()->GetStringForSymbol(symbol);
    }

    void GetLocalizedFromANSI(const char* ansi, wchar_t* outBuf, int unicodeBufferSizeInBytes) override
    {
        if (*ansi == '#')
        {
            auto pszLocalized = vgui2::g_pVGuiLocalize->Find(ansi);

            if (pszLocalized)
            {
                wchar_t format[] = L"%s1";
                vgui2::g_pVGuiLocalize->ConstructString(outBuf, unicodeBufferSizeInBytes, format, 1, pszLocalized);
                return;
            }
        }

        vgui2::g_pVGuiLocalize->ConvertANSIToUnicode(ansi, outBuf, unicodeBufferSizeInBytes);
    }

    void GetANSIFromLocalized(const wchar_t* wchar, char* outBuf, int ansiBufferSizeInBytes) override
    {
        vgui2::g_pVGuiLocalize->ConvertUnicodeToANSI(wchar, outBuf, ansiBufferSizeInBytes);
    }

    void AddKeyValuesToMemoryLeakList(void* pMem, HKeySymbol name) override
    {
        KeyValuesSystem()->AddKeyValuesToMemoryLeakList(pMem, name);
    }

    void RemoveKeyValuesFromMemoryLeakList(void* pMem) override
    {
        KeyValuesSystem()->RemoveKeyValuesFromMemoryLeakList(pMem);
    }
};

KeyValuesWrapper g_KeyValuesWrapper;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(KeyValuesWrapper, IKeyValues, IKEYVALUES_INTERFACE_VERSION, g_KeyValuesWrapper);

//
// Since tier2 does not link to vgui2 we need to define keyvalues()
//
// Note: KeyValues.cpp from tier1 uses keyvalues() from tier2, which is strange and worth fixing in the future.
//
IKeyValuesSystem* keyvalues()
{
    return &g_KeyValuesWrapper;
}
