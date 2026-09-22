#pragma once

// The MSVC spellings of the declaration specifiers and byte-swap builtins the SDK headers are
// written against.

#define FORCEINLINE __forceinline
#define NOINLINE __declspec(noinline)
#define FASTCALL __fastcall

#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#define __builtin_bswap64 _byteswap_uint64
