#pragma once

// Compiler spellings of the declaration specifiers and byte-swap builtins used by SDK headers.

#ifdef _MSC_VER
#define FORCEINLINE __forceinline
#define NOINLINE __declspec(noinline)
#define FASTCALL __fastcall

#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#define __builtin_bswap64 _byteswap_uint64
#else
#define FORCEINLINE __attribute__((always_inline)) inline
#define NOINLINE __attribute__((noinline))
#define FASTCALL
#endif
