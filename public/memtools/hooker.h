#pragma once

#include <map>
#include <windows.h>
#include "patcher.h"

class MemHooker
{
    struct memoryhook_t
    {
        void* m_dwTrampolineAddr = 0;
        uint8_t* pPatchedBytes = nullptr;
        size_t m_iLen = 0;
    };
    std::map<DWORD, memoryhook_t*> m_mapHooks;
    HANDLE m_hExeHeap;
    MemPatcher m_Patcher;

public:
    MemHooker();
    ~MemHooker();

    void* Hook(DWORD pAddress, void* pDest);
    bool UnHook(DWORD pAddress);
};