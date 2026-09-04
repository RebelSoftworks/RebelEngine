//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//
#pragma once
#ifndef __EFI__
	#include <malloc.h>
	#include <new>
	#include <string.h>
	#include <math.h>
#endif // !__EFI__
#include "tier0/types.h"
// by default; _WIN32 is defined in both _WIN64 and _WIN32
#if defined(_WIN32)
	#include <crtdbg.h>
#endif // _WIN32
#include "wchartypes.h"
#include "Color.h"
#include "logging.h"
#include "types.h"
#if defined(__EFI__)
// GNU EFI FTW!
	#include <efi.h>
	#include <efilib.h>
#else
	#include <cstdio>
	#include <cstdarg> // this is only needed for GCC/Clang to compile
#endif

// defs
#define OVERRIDE override

#if defined(_WIN32) || defined(_WIN64)
	#define IS_WINDOWS_PC 1
	#define IsLinux() false
	#define IsPosix() false
	#define IsOSX() false
	#ifndef _GAMING_XBOX
		#define IsPC() true
		#define IsX360() false
	#else
		#define IsPC() false
		#define IsX360() true
	#endif // !_GAMING_XBOX
#endif // _WIN64

#ifdef __linux__
	#define POSIX 1
	#define IsLinux() true
	#define IsPosix() true
	#define IsOSX() false
		#ifndef __ANDROID__
			#define IsPC() true
		#else
			#define IsPC() false
		#endif // __ANDROID
	#define IsX360() false
#endif // __linux__

#ifdef __EFI__
	#define POSIX 1
	#define IsLinux() false
	#define IsPosix() true
	#define IsOSX() false
	#define IsPC() true
	#define IsX360() false
#endif // __EFI__


#ifdef __APPLE__
	// mohamed: for some reason this is needed
	#include <TargetConditionals.h>
	#define POSIX 1
	#define IsLinux() false
	#define IsPosix() true
	#define IsOSX() true
		#if (TARGET_OS_IPHONE == 1)
			#define IsPC() false
		#else
			#define IsPC() true
		#endif // __ANDROID
	#define IsX360() false
#endif // __linux__

#if defined(DEBUG) || defined (_DEBUG)
#define IsDebug() true
#define IsRelease() false
#else
#define IsDebug() false
#define IsRelease() true
#endif

#if defined(POSIX) && !defined(__EFI__)
#define FORCEINLINE inline __attribute__((always_inline))
#elif defined(IS_WINDOWS_PC) && !defined(__EFI__)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif // (__linux__)

#ifdef IS_WINDOWS_PC
#define EXPORT _declspec( dllexport )
#define DLL_EXPORT	extern "C" _declspec( dllexport )
#define DLL_IMPORT	extern "C" _declspec( dllimport )
#define DBG_INTERFACE	extern "C" _declspec( dllimport )
#elif !defined(__EFI__) || !defined(__EMSCRIPTEN__)
#define EXPORT	__attribute__ ((visibility("default")))
#define DLL_EXPORT	extern "C" __attribute__ ((visibility("default")))
#define DLL_IMPORT	extern "C" 
#define DBG_INTERFACE	extern "C"
#else
#define EXPORT
#define DLL_EXPORT extern "C"
#define DLL_IMPORT extern "C"
#define DBG_INTERFACE extern "C"
#endif // IS_WINDOWS_PC

// Can't use extern "C" when DLL exporting a global
#ifdef IS_WINDOWS_PC
#define DLL_GLOBAL_EXPORT		extern __declspec( dllexport )
#define DLL_GLOBAL_IMPORT		extern __declspec( dllimport )
#else
#define DLL_GLOBAL_EXPORT		DLL_EXPORT
#define DLL_GLOBAL_IMPORT		extern
#endif // IS_WINDOWS_PC

#define PLATFORM_INTERFACE	DLL_IMPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_IMPORT

// Mohamed: Do we even use this?
#if IS_WINDOWS_PC
#define DLL_EXT_STRING ".dll"
#elif __linux__
#define DLL_EXT_STRING ".so"
#elif __APPLE__
#define DLL_EXT_STRING ".dylib"
#elif __EMSCRIPTEN__
#define DLL_EXT_STRING
#elif __EFI__
#define DLL_EXT_STRING
#else
#error "Unknown platform"
#endif // IS_WINDOWS_PC

#define _getcwd getcwd
#define FMTFUNCTION( a, b )

#define AssertValidReadPtr(...) 
#define AssertValidWritePtr(...) 
#define AssertValidStringPtr(...) 

inline void Msg(const char* pMsgFormat, ...)
{
#ifndef EFI
	std::printf(pMsgFormat);
#else
	SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*)msg);
#endif // !EFI
}

inline void Error(const char* pchText)
{
	Msg("Engine Error: %s\n", pchText);
}

inline float FloatMakePositive(vec_t f)
{
	return fabsf(f);
}

#define _MKSTRING(arg) #arg
#define MKSTRING(arg) _MKSTRING(arg)

#if !defined(IS_WINDOWS_PC)
#define MAX_PATH PATH_MAX
#endif // __linux__