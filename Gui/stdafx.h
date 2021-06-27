#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#define USE_DANGEROUS_FUNCTIONS

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_IE 0x0500

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500       // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINDOWS      // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0510 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE           // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500    // Change this to the appropriate value to target IE 5.0 or later.
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501    // Change this to the appropriate value to target IE 5.0 or later.
#endif

#pragma warning(disable : 26432)
#pragma warning(disable : 26433)
#pragma warning(disable : 26440)
#pragma warning(disable : 26443)
#pragma warning(disable : 26455)
#pragma warning(disable : 26461)
#pragma warning(disable : 26477)
#pragma warning(disable : 26493)
#pragma warning(disable : 26495)
#pragma warning(disable : 26496)
#pragma warning(disable : 26408)


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <commdlg.h>
#include <commctrl.h>


#ifdef _DEBUG
#undef ASSERT
extern void Assert(const char *expression, const char *file, const int line);

#define ASSERT(_exp_) \
    if (!(_exp_)) { \
    Assert(#_exp_, __FILE__, __LINE__); \
    __debugbreak(); };
#else // _DEBUG
#undef ASSERT

#define ASSERT(_exp_)
#endif // _DEBUG

#include "singleton.h"
#include "tarray.h"

#include "resource.h"

#include "..\Utils\NtInternal.h"

//
//
//

__pragma(warning(suppress: 6255))
extern "C" void * __cdecl _alloca(size_t);
#pragma intrinsic(_alloca)


//
// The best place to spot our placement new ;)
//

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE

__pragma(warning(suppress: 26440))
inline void *__cdecl operator new(size_t, void *_Where) throw()
{	// construct array with placement at _Where
	return (_Where);
}

__pragma(warning(suppress: 26440))
inline void __cdecl operator delete(void *, void *) throw()
{	// delete if placement new fails
}
#endif // __PLACEMENT_NEW_INLINE

//
// For simple string operations, there is no need to allocate memory; you can use the stack 
// memory. In this case, this buffer is more than enough to generate text messages.
//
#define MAX_TEMP_STRING  100

#include "configuration.h"
#include "dialogBase.h"


#ifdef __IDP__
#include "..\IdaPlugin\idainc.h"
#endif // __IDP__