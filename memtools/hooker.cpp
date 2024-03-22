#include "memtools/hooker.h"

#define NMD_ASSEMBLY_IMPLEMENTATION
#include "nmd_assembly.h"

static uint32_t RelativeToAbsolute(uint32_t dwAddress)
{
	ptrdiff_t dwOffset = *(uint32_t*)dwAddress;
	return (dwAddress + dwOffset + sizeof(uint32_t));
}

static uint32_t AbsoluteToRelative(uint32_t dwAddress, uint32_t dwAbsRef)
{
	return dwAbsRef - dwAddress - sizeof(uint32_t);
}

MemHooker::MemHooker()
{
    m_hExeHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0x1000, 0);
}

MemHooker::~MemHooker()
{
    for (auto p : m_mapHooks)
        UnHook(p.first);
    HeapDestroy(m_hExeHeap);
}

void* MemHooker::Hook(DWORD dwAddress, void* pDest)
{
    auto pHook = new memoryhook_t();
    std::map<size_t, DWORD> mapRelativePatches;

    size_t iLen = 0;
    while (iLen < 4)
    {
        nmd_x86_instruction x86Instruction;
        nmd_x86_decode_buffer((void*)(dwAddress + iLen), 16, &x86Instruction, NMD_X86_MODE_32, NMD_X86_DECODER_FLAGS_GROUP);
        if (x86Instruction.group & NMD_GROUP_RELATIVE_ADDRESSING)
        {
            DWORD dwPos = iLen + (x86Instruction.length - 4);
            DWORD dwRelativeAddr = *(DWORD*)(dwAddress + dwPos);
            DWORD dwCurAddr = dwAddress + iLen;

            // В trampoline могут быть скопированы инструкции с относительными адресами. 
            // Вычисляем и записываем абсолютный адрес для патчинга в trampoline.
            mapRelativePatches[dwPos] = RelativeToAbsolute(dwAddress + dwPos);
        }
        iLen += x86Instruction.length;
        //iLen += nmd_x86_ldisasm((void*)(dwAddress + iLen), 16, NMD_X86_MODE_32);
    }
    void* pTrampoline = HeapAlloc(m_hExeHeap, HEAP_ZERO_MEMORY, iLen + 5);
    pHook->m_dwTrampolineAddr = pTrampoline;

    pHook->pPatchedBytes = new uint8_t[iLen];
    memcpy_s(pHook->pPatchedBytes, iLen, (void*)dwAddress, iLen);
    for (auto prPatch : mapRelativePatches)
    {
        DWORD dwRelativeAddr = (DWORD)((DWORD)pHook->pPatchedBytes + prPatch.first);
        *(DWORD*)dwRelativeAddr = AbsoluteToRelative((DWORD)pHook->m_dwTrampolineAddr + prPatch.first, prPatch.second);
    }

    uint8_t bJmpBuffer[48]; memset(bJmpBuffer, 0x90, iLen);
    bJmpBuffer[0] = 0xE9;
    *(DWORD*)(&bJmpBuffer[0] + 1) = (DWORD)((DWORD)pDest - dwAddress) - 5;

    if (m_Patcher.Patch(dwAddress, &bJmpBuffer[0], iLen))
    {
        memcpy_s(pHook->m_dwTrampolineAddr, iLen, pHook->pPatchedBytes, iLen);
        DWORD pJmpStart = (DWORD)pHook->m_dwTrampolineAddr + iLen;
        if (*(uint8_t*)(dwAddress + iLen) != 0xCC)
        {
            *(uint8_t*)pJmpStart = 0xE9;
            *(DWORD*)(pJmpStart + 1) = (DWORD)(dwAddress - (DWORD)pTrampoline) - 5;
        }
        m_mapHooks[dwAddress] = pHook;

        return pTrampoline;
    }
    return nullptr;
}

bool MemHooker::UnHook(DWORD dwAddress)
{
    if (m_mapHooks.find(dwAddress) != m_mapHooks.end())
    {
        auto pHook = m_mapHooks[dwAddress];

        delete pHook->pPatchedBytes;
        HeapFree(m_hExeHeap, 0, pHook->m_dwTrampolineAddr);
        pHook->m_dwTrampolineAddr = 0;
        m_Patcher.Restore(dwAddress);

        m_mapHooks.erase(dwAddress);
    }
    return false;
}