#include "memtools/patcher.h"

MemPatcher::MemPatcher()
{

}

MemPatcher::~MemPatcher()
{
    for (auto p : m_mapPatches)
        Restore(p.first);
}

bool MemPatcher::Patch(DWORD dwAddress, uint8_t* pPatch, size_t iLen)
{
    m_mapPatches[dwAddress] = new memorypatch_t(pPatch, iLen);
    memcpy_s(m_mapPatches[dwAddress]->pPatchedBytes, iLen, (void*)dwAddress, iLen);
    DWORD dwOldProt;

    if (VirtualProtect((void*)dwAddress, iLen, PAGE_EXECUTE_READWRITE, &dwOldProt))
    {
        memcpy_s((void*)dwAddress, iLen, pPatch, iLen);
        if (VirtualProtect((void*)dwAddress, iLen, dwOldProt, &dwOldProt))
            return FlushInstructionCache(GetCurrentProcess(), (void*)dwAddress, iLen);
    }
    return false;
}

bool MemPatcher::Restore(DWORD dwAddress)
{
    if (m_mapPatches.find(dwAddress) != m_mapPatches.end())
    {
        DWORD dwOldProt;
        uint8_t* pPatch = m_mapPatches[dwAddress]->pPatchedBytes;
        size_t iLen = m_mapPatches[dwAddress]->iLen;

        if (VirtualProtect((void*)dwAddress, iLen, PAGE_EXECUTE_READWRITE, &dwOldProt))
        {
            memcpy_s((void*)dwAddress, iLen, pPatch, iLen);
            if (VirtualProtect((void*)dwAddress, iLen, dwOldProt, &dwOldProt))
            {
                if (FlushInstructionCache(GetCurrentProcess(), (void*)dwAddress, iLen))
                {
                    m_mapPatches.erase(dwAddress);
                    return true;
                }
            }
        }
    }
    return false;
}
