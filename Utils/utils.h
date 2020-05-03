/***************************************************************************************\
*   File:                                                                               *
*       utils.h                                                                         *
*                                                                                       *
*   Abstract:                                                                           *
*       misc utilites                                                                   *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov  15-Oct-2019                                                        *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#pragma once
#ifndef __UTILS__H__
#define __UTILS__H__

//
// native api import table
//
#pragma warning(push)
#pragma warning(disable : 4200 4201)
union IMPORT_TABLE
{
    CHAR  Strings[];
    PVOID Pointers[];

    struct {
        _NtQueryInformationThread      NtQueryInformationThread;
        _NtQueryInformationProcess     NtQueryInformationProcess;
        _NtReadVirtualMemory           NtReadVirtualMemory;
    };
};
#pragma warning(pop)

extern IMPORT_TABLE NtApi;

extern BOOL InitializeNativeApi();
extern DWORD  GetThreadProcess(DWORD dwThreadId);
extern LONG_PTR GetProcessPeb(HANDLE hProcess);
extern BOOL ReadMemory(HANDLE hProcess, ULONG64 address, PVOID Buffer, SIZE_T size);
extern BOOL ReadFileFunction(HANDLE hFile, ULONG64 address, PVOID buffer, SIZE_T size);

typedef struct _MODULE_ENUM_ENTRY {
    LONG_PTR          moduleStart;
    SIZE_T            moduleSize;
    TCHAR             moduleName[30];
    TCHAR            *modulePath;
} MODULE_ENUM_ENTRY, *PMODULE_ENUM_ENTRY;

extern BOOL EnumerateProcessModules(HANDLE hProcess, TArray<MODULE_ENUM_ENTRY> &_array, BOOL bCopyPath = FALSE);
extern BOOL EnumerateMemoryRegions(const GUI_PARAMETERS *parameters, TArray<MEMORY_ENUM_ENTRY> &_array);
extern BOOL GetModuleForAddress(HANDLE hProcess, LONG_PTR address, MODULE_ENUM_ENTRY *entry);

extern DWORD GetFileLengthByName(TCHAR *fileName);

extern int _wtncpy(TCHAR  *dest, const WCHAR *src, SIZE_T maxCount);

#endif // __UTILS__H__