#pragma once

#include <windows.h>
#include <map>

class MemPatcher
{
    struct memorypatch_t
    {
        uint8_t* pPatchedBytes;
        size_t iLen;

        memorypatch_t(uint8_t* pPatchBytes, size_t iLen) :
            pPatchedBytes(new uint8_t[iLen]),
            iLen(iLen) 
        {};
        ~memorypatch_t()
        {
            delete pPatchedBytes;
        }
    };
    std::map<DWORD, memorypatch_t*> m_mapPatches;

public:
    MemPatcher();
    ~MemPatcher();

    bool Patch(DWORD dwAddress, uint8_t* pPatch, size_t iLen);
    bool Restore(DWORD dwAddress);
};