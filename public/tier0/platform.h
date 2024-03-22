//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
	   
#ifndef PLATFORM_H
#define PLATFORM_H

#include "wchartypes.h"
#include "basetypes.h"
#include "tier0/valve_off.h"

#ifdef _WIN32
#pragma once
#endif

// feature enables
#define NEW_SOFTWARE_LIGHTING

#ifdef POSIX
// need this for _alloca
#include <alloca.h>
#endif

//OSX has this elsewhere. - Solokiller
#ifndef OSX
#include <malloc.h>
#else
#include <stdlib.h>
#include <malloc/malloc.h>
#endif
#include <new>

// need this for memset
#include <string.h>

#ifdef _RETAIL
#define IsRetail() true 
#else
#define IsRetail() false
#endif

#ifdef _DEBUG
#define IsRelease() false
#define IsDebug() true
#else
#define IsRelease() true
#define IsDebug() false
#endif

// Deprecating, infavor of IsX360() which will revert to IsXbox()
// after confidence of xbox 1 code flush
#define IsXbox()	false

// Backported some of these from Source 2013. - Solokiller
// Backported all of them - tmp64
#ifdef _WIN32
	#define IsLinux() false
	#define IsOSX() false
	#define IsPosix() false
	#define PLATFORM_WINDOWS 1 // Windows PC or Xbox 360
	
	#ifndef _X360
		#define IsWindows() true
		#define IsPC() true
		#define IsConsole() false
		#define IsX360() false
		#define IsPS3() false
		#define IS_WINDOWS_PC
		#define PLATFORM_WINDOWS_PC 1 // Windows PC
		#ifdef _WIN64
			#define IsPlatformWindowsPC64() true
			#define IsPlatformWindowsPC32() false
			#define PLATFORM_WINDOWS_PC64 1
		#else
			#define IsPlatformWindowsPC64() false
			#define IsPlatformWindowsPC32() true
			#define PLATFORM_WINDOWS_PC32 1
		#endif
	#else
		#define PLATFORM_X360 1
		#ifndef _CONSOLE
			#define _CONSOLE
		#endif
		#define IsWindows() false
		#define IsPC() false
		#define IsConsole() true
		#define IsX360() true
		#define IsPS3() false
	#endif
	
	// GoldSrc is OpenGL and doesn't use DX->GL layer
	#define IsPlatformOpenGL() true
#elif defined(POSIX)
	#define IsPC() true
	#define IsWindows() false
	#define IsConsole() false
	#define IsX360() false
	#define IsPS3() false
	#if defined( LINUX )
		#define IsLinux() true
	#else
		#define IsLinux() false
	#endif
	
	#if defined( OSX )
		#define IsOSX() true
	#else
		#define IsOSX() false
	#endif
	
	#define IsPosix() true
	#define IsPlatformOpenGL() true
#else
	#error
#endif

typedef unsigned char uint8;
typedef signed char int8;

#ifdef __x86_64__
#define X64BITS
#endif

#if defined( _WIN32 )

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#ifdef X64BITS
typedef __int64 intp;				// intp is an integer that can accomodate a pointer
typedef unsigned __int64 uintp;		// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
#else
typedef __int32 intp;
typedef unsigned __int32 uintp;
#endif

#else // _WIN32

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
#ifdef X64BITS
typedef long long intp;
typedef unsigned long long uintp;
#else
typedef int intp;
typedef unsigned int uintp;
#endif

#endif // else _WIN32

#undef OVERRIDE
#define OVERRIDE override
#if defined(__clang__)
	// warning: 'override' keyword is a C++11 extension [-Wc++11-extensions]
	// Disabling this warning is less intrusive than enabling C++11 extensions
	#pragma GCC diagnostic ignored "-Wc++11-extensions"
#endif


typedef float  float32;
typedef double float64;

// for when we don't care about how many bits we use
typedef unsigned int uint;

#define XBOX_DVD_SECTORSIZE	2048
#define XBOX_HDD_SECTORSIZE	512

// Custom windows messages for Xbox input
#define WM_XREMOTECOMMAND			WM_USER + 100
#define WM_XCONTROLLER_KEY			WM_USER + 101
#define WM_XCONTROLLER_UNPLUGGED	WM_USER + 102

// This can be used to ensure the size of pointers to members when declaring
// a pointer type for a class that has only been forward declared
#ifdef _MSC_VER
#define SINGLE_INHERITANCE __single_inheritance 
#define MULTIPLE_INHERITANCE __multiple_inheritance 
#else
#define SINGLE_INHERITANCE  
#define MULTIPLE_INHERITANCE 
#endif

#ifdef _MSC_VER
#define NO_VTABLE __declspec( novtable )
#else
#define NO_VTABLE
#endif

// This can be used to declare an abstract (interface only) class. 
// Classes marked abstract should not be instantiated.  If they are, and access violation will occur.
//
// Example of use:
// 
// abstract_class CFoo
// {
//      ...
// }
//
// MSDN __declspec(novtable) documentation: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclang/html/_langref_novtable.asp
//
// Note: NJS: This is not enabled for regular PC, due to not knowing the implications of exporting a class with no no vtable.
//       It's probable that this shouldn't be an issue, but an experiment should be done to verify this.
//
#ifndef _XBOX
#define abstract_class class
#else
#define abstract_class class NO_VTABLE
#endif

#define VALVE_RAND_MAX 0x7fff

/*
FIXME: Enable this when we no longer fear change =)

// need these for the limits
#include <limits.h>
#include <float.h>

// Maximum and minimum representable values
#define  INT8_MAX    SCHAR_MAX
#define  INT16_MAX   SHRT_MAX
#define  INT32_MAX   LONG_MAX
#define  INT64_MAX   (((int64)~0) >> 1)

#define  INT8_MIN    SCHAR_MIN
#define  INT16_MIN   SHRT_MIN
#define  INT32_MIN   LONG_MIN
#define  INT64_MIN   (((int64)1) << 63)

#define  UINT8_MAX   ((uint8)~0)
#define  UINT16_MAX  ((uint16)~0)
#define  UINT32_MAX  ((uint32)~0)
#define  UINT64_MAX  ((uint64)~0)

#define  UINT8_MIN   0
#define  UINT16_MIN  0
#define  UINT32_MIN  0
#define  UINT64_MIN  0

#ifndef  UINT_MIN
#define  UINT_MIN    UINT32_MIN
#endif

#define  FLOAT32_MAX FLT_MAX
#define  FLOAT64_MAX DBL_MAX

#define  FLOAT32_MIN FLT_MIN
#define  FLOAT64_MIN DBL_MIN
*/

// portability / compiler settings
#if defined(_WIN32) && !defined(WINDED)

#if defined(_M_IX86)
#define __i386__	1
#endif

// Defines MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH  260
#endif

#elif POSIX
#define _vsnprintf vsnprintf

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef void * HINSTANCE;
#define _MAX_PATH PATH_MAX
#define __cdecl
#define __declspec

// Defines MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH  PATH_MAX
#endif
#endif // defined(_WIN32) && !defined(WINDED)

#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) ) //  need macro for constant expression

// Used to step into the debugger
#ifdef _WIN32
#define DebuggerBreak()  __asm { int 3 }
#else
#define DebuggerBreak()  {}
#endif
#define	DebuggerBreakIfDebugging() if ( !Plat_IsInDebugSession() ) ; else DebuggerBreak()

// C functions for external declarations that call the appropriate C++ methods
#ifndef EXPORT
	#ifdef _WIN32
		#define EXPORT	_declspec( dllexport )
	#else 
		#define EXPORT	__attribute__((visibility("default")))
	#endif
#endif

#if defined __i386__ && !defined __linux__
	#define id386	1
#else
	#define id386	0
#endif  // __i386__

// decls for aligning data
#ifdef _WIN32
        #define DECL_ALIGN(x) __declspec(align(x))

#elif GNUC
	#define DECL_ALIGN(x) __attribute__((aligned(x)))
#else
        #define DECL_ALIGN(x) /* */
#endif

#ifdef _MSC_VER
	// MSVC has the align at the start of the struct
	#define ALIGN4 DECL_ALIGN(4)
	#define ALIGN8 DECL_ALIGN(8)
	#define ALIGN16 DECL_ALIGN(16)
	#define ALIGN32 DECL_ALIGN(32)
	#define ALIGN128 DECL_ALIGN(128)

	#define ALIGN4_POST
	#define ALIGN8_POST
	#define ALIGN16_POST
	#define ALIGN32_POST
	#define ALIGN128_POST
#elif defined( GNUC )
	// gnuc has the align decoration at the end
	#define ALIGN4
	#define ALIGN8 
	#define ALIGN16
	#define ALIGN32
	#define ALIGN128

	#define ALIGN4_POST DECL_ALIGN(4)
	#define ALIGN8_POST DECL_ALIGN(8)
	#define ALIGN16_POST DECL_ALIGN(16)
	#define ALIGN32_POST DECL_ALIGN(32)
	#define ALIGN128_POST DECL_ALIGN(128)
#else
	#error
#endif

// Pull in the /analyze code annotations.
#include "annotations.h"

//-----------------------------------------------------------------------------
// Convert int<-->pointer, avoiding 32/64-bit compiler warnings:
//-----------------------------------------------------------------------------
#define INT_TO_POINTER( i ) (void *)( ( i ) + (char *)NULL )
#define POINTER_TO_INT( p ) ( (int)(uintp)( p ) )


//-----------------------------------------------------------------------------
// Stack-based allocation related helpers
//-----------------------------------------------------------------------------
#if defined( GNUC )
	#define stackalloc( _size )		alloca( ALIGN_VALUE( _size, 16 ) )
#ifdef _LINUX
	#define mallocsize( _p )	( malloc_usable_size( _p ) )
#elif defined(OSX)
	#define mallocsize( _p )	( malloc_size( _p ) )
#else
#error
#endif
#elif defined ( _WIN32 )
	#define stackalloc( _size )		_alloca( ALIGN_VALUE( _size, 16 ) )
	#define mallocsize( _p )		( _msize( _p ) )
#endif

#define  stackfree( _p )			0

// Linux had a few areas where it didn't construct objects in the same order that Windows does.
// So when CVProfile::CVProfile() would access g_pMemAlloc, it would crash because the allocator wasn't initalized yet.
#ifdef POSIX
	#define CONSTRUCT_EARLY __attribute__((init_priority(101)))
#else
	#define CONSTRUCT_EARLY
	#endif

#if defined(_MSC_VER)
	#define SELECTANY __declspec(selectany)
	#define RESTRICT __restrict
	#define RESTRICT_FUNC __declspec(restrict)
	#define FMTFUNCTION( a, b )
#elif defined(GNUC)
	#define SELECTANY __attribute__((weak))
	#if defined(LINUX) && !defined(DEDICATED)
		#define RESTRICT
	#else
		#define RESTRICT __restrict
	#endif
	#define RESTRICT_FUNC
	// squirrel.h does a #define printf DevMsg which leads to warnings when we try
	// to use printf as the prototype format function. Using __printf__ instead.
	#define FMTFUNCTION( fmtargnumber, firstvarargnumber ) __attribute__ (( format( __printf__, fmtargnumber, firstvarargnumber )))
#else
	#define SELECTANY static
	#define RESTRICT
	#define RESTRICT_FUNC
	#define FMTFUNCTION( a, b )
#endif

//Combined Source 2013 version of this with the Source 2006 version for OSX support. - Solokiller
#if defined( _WIN32 ) && !defined(_XBOX)

// Used for dll exporting and importing
#define DLL_EXPORT				extern "C" __declspec( dllexport )
#define DLL_IMPORT				extern "C" __declspec( dllimport )

// Can't use extern "C" when DLL exporting a class
#define DLL_CLASS_EXPORT		__declspec( dllexport )
#define DLL_CLASS_IMPORT		__declspec( dllimport )

// Can't use extern "C" when DLL exporting a global
#define DLL_GLOBAL_EXPORT		extern __declspec( dllexport )
#define DLL_GLOBAL_IMPORT		extern __declspec( dllimport )

#define DLL_LOCAL

#elif defined GNUC
// Used for dll exporting and importing
#define  DLL_EXPORT   extern "C" __attribute__ ((visibility("default")))
#define  DLL_IMPORT   extern "C"

// Can't use extern "C" when DLL exporting a class
#define  DLL_CLASS_EXPORT __attribute__ ((visibility("default")))
#define  DLL_CLASS_IMPORT

// Can't use extern "C" when DLL exporting a global
#define  DLL_GLOBAL_EXPORT   extern __attribute ((visibility("default")))
#define  DLL_GLOBAL_IMPORT   extern

#define  DLL_LOCAL __attribute__ ((visibility("hidden")))

#elif defined(_XBOX)

#define  DLL_EXPORT				extern   
#define  DLL_IMPORT				extern
#define  DLL_CLASS_EXPORT  
#define  DLL_CLASS_IMPORT   
#define  DLL_GLOBAL_EXPORT   
#define  DLL_GLOBAL_IMPORT   

#else
#error "Unsupported Platform."
#endif

// Used for standard calling conventions
#ifdef _WIN32
	#define  STDCALL				__stdcall
	#define  FASTCALL				__fastcall
	#define  FORCEINLINE			__forceinline	
	// GCC 3.4.1 has a bug in supporting forced inline of templated functions
	// this macro lets us not force inlining in that case
	#define  FORCEINLINE_TEMPLATE		__forceinline	
#else
	#define  STDCALL
	#define  FASTCALL 
	#define  FORCEINLINE			__attribute__ ((always_inline)) inline
	// GCC 3.4.1 has a bug in supporting forced inline of templated functions
	// this macro lets us not force inlining in that case
	#define  FORCEINLINE_TEMPLATE
	#define  __stdcall			__attribute__ ((__stdcall__)) 
#endif

// Force a function call site -not- to inlined. (useful for profiling)
#define DONT_INLINE(a) (((int)(a)+1)?(a):(a))

// Pass hints to the compiler to prevent it from generating unnessecary / stupid code
// in certain situations.  Several compilers other than MSVC also have an equivilent 
// construct.
//
// Essentially the 'Hint' is that the condition specified is assumed to be true at 
// that point in the compilation.  If '0' is passed, then the compiler assumes that
// any subsequent code in the same 'basic block' is unreachable, and thus usually 
// removed.
#ifdef _MSC_VER
	#define HINT(THE_HINT)	__assume((THE_HINT))
#else
	#define HINT(THE_HINT)	0
#endif

// Marks the codepath from here until the next branch entry point as unreachable,
// and asserts if any attempt is made to execute it.
#define UNREACHABLE() { Assert(0); HINT(0); }

// In cases where no default is present or appropriate, this causes MSVC to generate 
// as little code as possible, and throw an assertion in debug.
#define NO_DEFAULT default: UNREACHABLE();

#ifdef _WIN32
// Remove warnings from warning level 4.
#pragma warning(disable : 4514) // warning C4514: 'acosl' : unreferenced inline function has been removed
#pragma warning(disable : 4100) // warning C4100: 'hwnd' : unreferenced formal parameter
#pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#pragma warning(disable : 4512) // warning C4512: 'InFileRIFF' : assignment operator could not be generated
#pragma warning(disable : 4611) // warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable : 4710) // warning C4710: function 'x' not inlined
#pragma warning(disable : 4702) // warning C4702: unreachable code
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 4239) // nonstandard extension used : 'argument' ( conversion from class Vector to class Vector& )
#pragma warning(disable : 4097) // typedef-name 'BaseClass' used as synonym for class-name 'CFlexCycler::CBaseFlex'
#pragma warning(disable : 4324) // Padding was added at the end of a structure
#pragma warning(disable : 4244) // type conversion warning.
#pragma warning(disable : 4305)	// truncation from 'const double ' to 'float '
#pragma warning(disable : 4786)	// Disable warnings about long symbol names
#pragma warning(disable : 4250) // 'X' : inherits 'Y::Z' via dominance

#if _MSC_VER >= 1300
#pragma warning(disable : 4511)	// Disable warnings about private copy constructors
#pragma warning(disable : 4121)	// warning C4121: 'symbol' : alignment of a member was sensitive to packing
#pragma warning(disable : 4530)	// warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc (disabled due to std headers having exception syntax)
#endif

#if _MSC_VER >= 1400
#pragma warning(disable : 4996)	// functions declared deprecated
#endif


#endif

// When we port to 64 bit, we'll have to resolve the int, ptr vs size_t 32/64 bit problems...
#if !defined( _WIN64 ) && defined( WIN32 )
#pragma warning( disable : 4267 )	// conversion from 'size_t' to 'int', possible loss of data
#pragma warning( disable : 4311 )	// pointer truncation from 'char *' to 'int'
#pragma warning( disable : 4312 )	// conversion from 'unsigned int' to 'memhandle_t' of greater size
#endif

#ifdef POSIX
#ifndef _stricmp
#define _stricmp stricmp
#endif
#define strcmpi stricmp
#define stricmp strcasecmp
#define _vsnprintf vsnprintf
#ifndef _alloca
#define _alloca alloca
#endif
#ifdef _snprintf
#undef _snprintf
#endif
#define _snprintf snprintf
#define GetProcAddress dlsym
#define _chdir chdir
#ifndef _strnicmp
#define _strnicmp strnicmp
#endif
#define strnicmp strncasecmp
#define _getcwd getcwd
#define _snwprintf swprintf
#define swprintf_s swprintf
#define wcsicmp _wcsicmp
#define _wcsicmp wcscmp
#define _finite finite
#define _tempnam tempnam
#define _unlink unlink
#define _access access
#define _mkdir(dir) mkdir( dir, S_IRWXU | S_IRWXG | S_IRWXO )
#define _wtoi(arg) wcstol(arg, NULL, 10)
#define _wtoi64(arg) wcstoll(arg, NULL, 10)

typedef uint32 HMODULE;
typedef void *HANDLE;
#endif

#ifndef WIN32
//Ported from Source 2013. - Solokiller
#define _mkdir(dir) mkdir( dir, S_IRWXU | S_IRWXG | S_IRWXO )
#endif

//-----------------------------------------------------------------------------
// FP exception handling
//-----------------------------------------------------------------------------
//#define CHECK_FLOAT_EXCEPTIONS 1

#ifdef _MSC_VER

inline void SetupFPUControlWordForceExceptions()
{
	// use local to get and store control word
	uint16 tmpCtrlW;              
	__asm 
	{
		fnclex                     /* clear all current exceptions */
		fnstcw word ptr [tmpCtrlW] /* get current control word */
		and [tmpCtrlW], 0FCC0h     /* Keep infinity control + rounding control */
		or [tmpCtrlW],   0230h     /* set to 53-bit, mask only inexact, underflow */
		fldcw word ptr [tmpCtrlW]  /* put new control word in FPU */
	}
}

#ifdef CHECK_FLOAT_EXCEPTIONS
	 
inline void SetupFPUControlWord()
{
	SetupFPUControlWordForceExceptions();
}

#else

inline void SetupFPUControlWord()
{
	// use local to get and store control word
	uint16 tmpCtrlW;
	__asm 
	{
		fnstcw word ptr [tmpCtrlW] /* get current control word */
		and [tmpCtrlW], 0FCC0h     /* Keep infinity control + rounding control */
		or [tmpCtrlW],   023Fh     /* set to 53-bit, mask only inexact, underflow */
		fldcw word ptr [tmpCtrlW]  /* put new control word in FPU */
	}
}

#endif

#else

inline void SetupFPUControlWord()
{
}

#endif // _MSC_VER


//-----------------------------------------------------------------------------
// Purpose: Standard functions for handling endian-ness
//-----------------------------------------------------------------------------

//-------------------------------------
// Basic swaps
//-------------------------------------

template <typename T>
inline T WordSwapC( T w )
{
   uint16 temp;

   temp  = ((*((uint16 *)&w) & 0xff00) >> 8);
   temp |= ((*((uint16 *)&w) & 0x00ff) << 8);

   return *((T*)&temp);
}

template <typename T>
inline T DWordSwapC( T dw )
{
   uint32 temp;

   temp  =   *((uint32 *)&dw) 				>> 24;
   temp |= ((*((uint32 *)&dw) & 0x00FF0000) >> 8);
   temp |= ((*((uint32 *)&dw) & 0x0000FF00) << 8);
   temp |= ((*((uint32 *)&dw) & 0x000000FF) << 24);

   return *((T*)&temp);
}

//-------------------------------------
// Fast swaps
//-------------------------------------

#ifdef _MSC_VER

#define WordSwap  WordSwapAsm
#define DWordSwap DWordSwapAsm

#pragma warning(push)
#pragma warning (disable:4035) // no return value

template <typename T>
inline T WordSwapAsm( T w )
{
   __asm
   {
      mov ax, w
      xchg al, ah
   }
}

template <typename T>
inline T DWordSwapAsm( T dw )
{
   __asm
   {
      mov eax, dw
      bswap eax
   }
}

#pragma warning(pop)

// The assembly implementation is not compatible with floats
template <>
inline float DWordSwapAsm<float>( float f )
{
	return DWordSwapC( f );
}

#else

#define WordSwap  WordSwapC
#define DWordSwap DWordSwapC

#endif

//-------------------------------------
// The typically used methods. 
//-------------------------------------

#if defined(__i386__) && !defined(VALVE_LITTLE_ENDIAN)
#define VALVE_LITTLE_ENDIAN 1
#endif

#if defined( _SGI_SOURCE ) || defined( _X360 )
#define	VALVE_BIG_ENDIAN 1
#endif

// If a swapped float passes through the fpu, the bytes may get changed.
// Prevent this by swapping floats as DWORDs.
#define SafeSwapFloat( pOut, pIn )	(*((uint*)pOut) = DWordSwap( *((uint*)pIn) ))

#if defined(VALVE_LITTLE_ENDIAN)

#define BigShort( val )				WordSwap( val )
#define BigWord( val )				WordSwap( val )
#define BigLong( val )				DWordSwap( val )
#define BigDWord( val )				DWordSwap( val )
#define LittleShort( val )			( val )
#define LittleWord( val )			( val )
#define LittleLong( val )			( val )
#define LittleDWord( val )			( val )
#define LittleQWord( val )			( val )
#define SwapShort( val )			BigShort( val )
#define SwapWord( val )				BigWord( val )
#define SwapLong( val )				BigLong( val )
#define SwapDWord( val )			BigDWord( val )

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		SafeSwapFloat( pOut, pIn )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )
#define SwapFloat( pOut, pIn )		BigFloat( pOut, pIn )

#elif defined(VALVE_BIG_ENDIAN)

#define BigShort( val )				( val )
#define BigWord( val )				( val )
#define BigLong( val )				( val )
#define BigDWord( val )				( val )
#define LittleShort( val )			WordSwap( val )
#define LittleWord( val )			WordSwap( val )
#define LittleLong( val )			DWordSwap( val )
#define LittleDWord( val )			DWordSwap( val )
#define LittleQWord( val )			QWordSwap( val )
#define SwapShort( val )			LittleShort( val )
#define SwapWord( val )				LittleWord( val )
#define SwapLong( val )				LittleLong( val )
#define SwapDWord( val )			LittleDWord( val )

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		( *pOut = *pIn )
#define LittleFloat( pOut, pIn )	SafeSwapFloat( pOut, pIn )
#define SwapFloat( pOut, pIn )		LittleFloat( pOut, pIn )

#else

// @Note (toml 05-02-02): this technique expects the compiler to
// optimize the expression and eliminate the other path. On any new
// platform/compiler this should be tested.
inline short BigShort( short val )		{ int test = 1; return ( *(char *)&test == 1 ) ? WordSwap( val )  : val; }
inline uint16 BigWord( uint16 val )		{ int test = 1; return ( *(char *)&test == 1 ) ? WordSwap( val )  : val; }
inline long BigLong( long val )			{ int test = 1; return ( *(char *)&test == 1 ) ? DWordSwap( val ) : val; }
inline uint32 BigDWord( uint32 val )	{ int test = 1; return ( *(char *)&test == 1 ) ? DWordSwap( val ) : val; }
inline short LittleShort( short val )	{ int test = 1; return ( *(char *)&test == 1 ) ? val : WordSwap( val ); }
inline uint16 LittleWord( uint16 val )	{ int test = 1; return ( *(char *)&test == 1 ) ? val : WordSwap( val ); }
inline long LittleLong( long val )		{ int test = 1; return ( *(char *)&test == 1 ) ? val : DWordSwap( val ); }
inline uint32 LittleDWord( uint32 val )	{ int test = 1; return ( *(char *)&test == 1 ) ? val : DWordSwap( val ); }
inline uint64 LittleQWord( uint64 val )	{ int test = 1; return ( *(char *)&test == 1 ) ? val : QWordSwap( val ); }
inline short SwapShort( short val )					{ return WordSwap( val ); }
inline uint16 SwapWord( uint16 val )				{ return WordSwap( val ); }
inline long SwapLong( long val )					{ return DWordSwap( val ); }
inline uint32 SwapDWord( uint32 val )				{ return DWordSwap( val ); }

// Pass floats by pointer for swapping to avoid truncation in the fpu
inline void BigFloat( float *pOut, const float *pIn )		{ int test = 1; ( *(char *)&test == 1 ) ? SafeSwapFloat( pOut, pIn ) : ( *pOut = *pIn ); }
inline void LittleFloat( float *pOut, const float *pIn )	{ int test = 1; ( *(char *)&test == 1 ) ? ( *pOut = *pIn ) : SafeSwapFloat( pOut, pIn ); }
inline void SwapFloat( float *pOut, const float *pIn )		{ SafeSwapFloat( pOut, pIn ); }

#endif

#if _X360
FORCEINLINE unsigned long LoadLittleDWord( const unsigned long *base, unsigned int dwordIndex )
		{
			return __loadwordbytereverse( dwordIndex<<2, base );
		}

FORCEINLINE void StoreLittleDWord( unsigned long *base, unsigned int dwordIndex, unsigned long dword )
		{
			__storewordbytereverse( dword, dwordIndex<<2, base );
		}
#else
FORCEINLINE unsigned long LoadLittleDWord( const unsigned long *base, unsigned int dwordIndex )
	{
		return LittleDWord( base[dwordIndex] );
	}

FORCEINLINE void StoreLittleDWord( unsigned long *base, unsigned int dwordIndex, unsigned long dword )
	{
		base[dwordIndex] = LittleDWord(dword);
	}
#endif

//-----------------------------------------------------------------------------
// DLL export for platform utilities
//-----------------------------------------------------------------------------
#ifndef STATIC_TIER0

#ifdef TIER0_DLL_EXPORT
	#define PLATFORM_INTERFACE	DLL_EXPORT
	#define PLATFORM_OVERLOAD	DLL_GLOBAL_EXPORT
#else
	#define PLATFORM_INTERFACE	DLL_IMPORT
	#define PLATFORM_OVERLOAD	DLL_GLOBAL_IMPORT
#endif

#else	// BUILD_AS_DLL

#define PLATFORM_INTERFACE	extern
#define PLATFORM_OVERLOAD	

#endif	// BUILD_AS_DLL


PLATFORM_INTERFACE double			Plat_FloatTime();		// Returns time in seconds since the module was loaded.
PLATFORM_INTERFACE unsigned long	Plat_MSTime();			// Time in milliseconds.

// b/w compatibility
//#define Sys_FloatTime Plat_FloatTime


// Processor Information:
struct CPUInformation
{
	int	 m_Size;		// Size of this structure, for forward compatability.

	bool m_bRDTSC : 1,	// Is RDTSC supported?
		 m_bCMOV  : 1,  // Is CMOV supported?
		 m_bFCMOV : 1,  // Is FCMOV supported?
		 m_bSSE	  : 1,	// Is SSE supported?
		 m_bSSE2  : 1,	// Is SSE2 Supported?
		 m_b3DNow : 1,	// Is 3DNow! Supported?
		 m_bMMX   : 1,	// Is MMX supported?
		 m_bHT	  : 1;	// Is HyperThreading supported?

	uint8 m_nLogicalProcessors,		// Number op logical processors.
		    m_nPhysicalProcessors;	// Number of physical processors

	int64 m_Speed;						// In cycles per second.

	tchar* m_szProcessorID;				// Processor vendor Identification.
};

//Disable this warning because we can't fix it anyway. - Solokiller
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif

PLATFORM_INTERFACE const CPUInformation& GetCPUInformation();

#ifdef __clang__
#pragma clang diagnostic pop
#endif

// ---------------------------------------------------------------------------------- //
// Performance Monitoring Events - L2 stats etc...
// ---------------------------------------------------------------------------------- //
PLATFORM_INTERFACE void InitPME();
PLATFORM_INTERFACE void ShutdownPME();
//PLATFORM_INTERFACE bool g_bPME;



//-----------------------------------------------------------------------------
// Thread related functions
//-----------------------------------------------------------------------------
// Registers the current thread with Tier0's thread management system. 
// This should be called on every thread created in the game.
PLATFORM_INTERFACE unsigned long Plat_RegisterThread( const tchar *pName = _T("Source Thread"));

// Registers the current thread as the primary thread.
PLATFORM_INTERFACE unsigned long Plat_RegisterPrimaryThread();

// VC-specific. Sets the thread's name so it has a friendly name in the debugger.
// This should generally only be handled by Plat_RegisterThread and Plat_RegisterPrimaryThread
PLATFORM_INTERFACE void	Plat_SetThreadName( unsigned long dwThreadID, const tchar *pName );

// These would be private if it were possible to export private variables from a .DLL.
// They need to be variables because they are checked by inline functions at performance
// critical places.
PLATFORM_INTERFACE unsigned long Plat_PrimaryThreadID;

// Returns the ID of the currently executing thread.
PLATFORM_INTERFACE unsigned long Plat_GetCurrentThreadID();

// Returns the ID of the primary thread.
inline unsigned long Plat_GetPrimaryThreadID()
{
	return Plat_PrimaryThreadID;
}

// Returns true if the current thread is the primary thread.
inline bool Plat_IsPrimaryThread()
{
	//return true;
	return (Plat_GetPrimaryThreadID() == Plat_GetCurrentThreadID() );
}

//-----------------------------------------------------------------------------
// Process related functions
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE const tchar *Plat_GetCommandLine();
#ifndef _WIN32
// helper function for OS's that don't have a ::GetCommandLine() call
PLATFORM_INTERFACE void Plat_SetCommandLine( const char *cmdLine );
#endif

//-----------------------------------------------------------------------------
// Security related functions
//-----------------------------------------------------------------------------
// Ensure that the hardware key's drivers have been installed.
PLATFORM_INTERFACE bool Plat_VerifyHardwareKeyDriver();

// Ok, so this isn't a very secure way to verify the hardware key for now.  It 
// is primarially depending on the fact that all the binaries have been wrapped
// with the secure wrapper provided by the hardware keys vendor.
PLATFORM_INTERFACE bool Plat_VerifyHardwareKey();	

// The same as above, but notifies user with a message box when the key isn't in 
// and gives him an opportunity to correct the situation.
PLATFORM_INTERFACE bool Plat_VerifyHardwareKeyPrompt();

// Can be called in real time, doesn't perform the verify every frame.  Mainly just
// here to allow the game to drop out quickly when the key is removed, rather than
// allowing the wrapper to pop up it's own blocking dialog, which the engine doesn't 
// like much.
PLATFORM_INTERFACE bool Plat_FastVerifyHardwareKey();



//-----------------------------------------------------------------------------
// Include additional dependant header components.
//-----------------------------------------------------------------------------
#include "tier0/fasttimer.h"


//-----------------------------------------------------------------------------
// Just logs file and line to simple.log
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void* Plat_SimpleLog( const tchar* file, int line );

//#define Plat_dynamic_cast Plat_SimpleLog(__FILE__,__LINE__),dynamic_cast

#if _X360
#define Plat_FastMemset XMemSet
#define Plat_FastMemcpy XMemCpy
#else
#define Plat_FastMemset memset
#define Plat_FastMemcpy memcpy
#endif

//-----------------------------------------------------------------------------
// Returns true if debugger attached, false otherwise
//-----------------------------------------------------------------------------
#if defined(_WIN32)
PLATFORM_INTERFACE bool Plat_IsInDebugSession();
#else
#define Plat_IsInDebugSession() (false)
#endif

//-----------------------------------------------------------------------------
// Methods to invoke the constructor, copy constructor, and destructor
//-----------------------------------------------------------------------------

template <class T>
inline T* Construct( T* pMemory )
{
	return ::new( pMemory ) T;
}

template <class T, typename ARG1>
inline T* Construct( T* pMemory, ARG1 a1 )
{
	return ::new( pMemory ) T( a1 );
}

template <class T, typename ARG1, typename ARG2>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2 )
{
	return ::new( pMemory ) T( a1, a2 );
}

template <class T, typename ARG1, typename ARG2, typename ARG3>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3 )
{
	return ::new( pMemory ) T( a1, a2, a3 );
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4 )
{
	return ::new( pMemory ) T( a1, a2, a3, a4 );
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4, ARG5 a5 )
{
	return ::new( pMemory ) T( a1, a2, a3, a4, a5 );
}

template <class T, class P>
inline void ConstructOneArg( T* pMemory, P const& arg)
{
	::new( pMemory ) T(arg);
}

template <class T, class P1, class P2 >
inline void ConstructTwoArg( T* pMemory, P1 const& arg1, P2 const& arg2)
{
	::new( pMemory ) T(arg1, arg2);
}

template <class T, class P1, class P2, class P3 >
inline void ConstructThreeArg( T* pMemory, P1 const& arg1, P2 const& arg2, P3 const& arg3)
{
	::new( pMemory ) T(arg1, arg2, arg3);
}

template <class T>
inline T* CopyConstruct( T* pMemory, T const& src )
{
	return ::new( pMemory ) T(src);
}

template <class T>
inline void Destruct( T* pMemory )
{
	pMemory->~T();

#ifdef _DEBUG
	memset( reinterpret_cast<void*>( pMemory ), 0xDD, sizeof(T) );
#endif
}


//
// GET_OUTER()
//
// A platform-independent way for a contained class to get a pointer to its
// owner. If you know a class is exclusively used in the context of some
// "outer" class, this is a much more space efficient way to get at the outer
// class than having the inner class store a pointer to it.
//
//	class COuter
//	{
//		class CInner // Note: this does not need to be a nested class to work
//		{
//			void PrintAddressOfOuter()
//			{
//				printf( "Outer is at 0x%x\n", GET_OUTER( COuter, m_Inner ) );
//			}
//		};
//		
//		CInner m_Inner;
//		friend class CInner;
//	};

#define GET_OUTER( OuterType, OuterMember ) \
   ( ( OuterType * ) ( (uint8 *)this - offsetof( OuterType, OuterMember ) ) )


/*	TEMPLATE_FUNCTION_TABLE() 

    (Note added to platform.h so platforms that correctly support templated 
	 functions can handle portions as templated functions rather than wrapped
	 functions)

	Helps automate the process of creating an array of function 
	templates that are all specialized by a single integer.  
	This sort of thing is often useful in optimization work.  

	For example, using TEMPLATE_FUNCTION_TABLE, this:

	TEMPLATE_FUNCTION_TABLE(int, Function, ( int blah, int blah ), 10)
	{
		return argument * argument;
	}

	is equivilent to the following:

	(NOTE: the function has to be wrapped in a class due to code 
	generation bugs involved with directly specializing a function 
	based on a constant.)
	
	template<int argument>
	class FunctionWrapper
	{
	public:
		int Function( int blah, int blah )
		{
			return argument*argument;
		}
	}

	typedef int (*FunctionType)( int blah, int blah );

	class FunctionName
	{
	public:
		enum { count = 10 };
		FunctionType functions[10];
	};

	FunctionType FunctionName::functions[] =
	{
		FunctionWrapper<0>::Function,
		FunctionWrapper<1>::Function,
		FunctionWrapper<2>::Function,
		FunctionWrapper<3>::Function,
		FunctionWrapper<4>::Function,
		FunctionWrapper<5>::Function,
		FunctionWrapper<6>::Function,
		FunctionWrapper<7>::Function,
		FunctionWrapper<8>::Function,
		FunctionWrapper<9>::Function
	};
*/

PLATFORM_INTERFACE bool vtune( bool resume );


#define TEMPLATE_FUNCTION_TABLE(RETURN_TYPE, NAME, ARGS, COUNT)			\
																		\
typedef RETURN_TYPE (FASTCALL *__Type_##NAME) ARGS;						\
																		\
template<const int nArgument>											\
struct __Function_##NAME												\
{																		\
	static RETURN_TYPE FASTCALL Run ARGS;								\
};																		\
																		\
template <const int i>														\
struct __MetaLooper_##NAME : __MetaLooper_##NAME<i-1>					\
{																		\
	__Type_##NAME func;													\
	inline __MetaLooper_##NAME() { func = __Function_##NAME<i>::Run; }	\
};																		\
																		\
template<>																\
struct __MetaLooper_##NAME<0>											\
{																		\
	__Type_##NAME func;													\
	inline __MetaLooper_##NAME() { func = __Function_##NAME<0>::Run; }	\
};																		\
																		\
class NAME																\
{																		\
private:																\
    static const __MetaLooper_##NAME<COUNT> m;							\
public:																	\
	enum { count = COUNT };												\
	static const __Type_##NAME* functions;								\
};																		\
const __MetaLooper_##NAME<COUNT> NAME::m;								\
const __Type_##NAME* NAME::functions = (__Type_##NAME*)&m;				\
template<const int nArgument>													\
RETURN_TYPE FASTCALL __Function_##NAME<nArgument>::Run ARGS						


#define LOOP_INTERCHANGE(BOOLEAN, CODE)\
	if( (BOOLEAN) )\
	{\
		CODE;\
	} else\
	{\
		CODE;\
	}

#include "tier0/valve_on.h"

#endif /* PLATFORM_H */
