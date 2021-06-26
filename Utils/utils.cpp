/***************************************************************************************\
*   File:                                                                               *
*       utils.cpp                                                                       *
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
#include "stdafx.h"
#include "utils.h"

IMPORT_TABLE NtApi = 
{
    "NtQueryInformationThread\0"     \
    "NtQueryInformationProcess\0"    \
    "NtReadVirtualMemory\0"          \
    "\0"
};

BOOL
InitializeNativeApi()
/*++

    Function:
        InitializeNativeApi

    Pusrpouse:

        Initialization function of the list of native functions. The list is filled directly by text lines.

    Arguments:


    Returns:
        BOOL

--*/
{
    HMODULE hModule;
    CHAR   *functionName = NtApi.Strings;
    PVOID  *pfnPtr = NtApi.Pointers;

    hModule = GetModuleHandle(_T("ntdll.dll"));

    if (hModule == NULL) {

        return FALSE;
    }

    for (SIZE_T i = 0; i < sizeof(IMPORT_TABLE) / sizeof(PVOID); i++)
    {
        if (functionName[0] == 0)
            break;

        SIZE_T  nextNameOffset = strlen(functionName) + 1;

        *pfnPtr = GetProcAddress(hModule, functionName);

        if (*pfnPtr == 0)
            return FALSE;

        functionName += nextNameOffset;

        pfnPtr++;
    }

    return TRUE;
}

DWORD
GetThreadProcess(IN DWORD dwThreadId)
/*++

    Function:
        GetThreadProcess

    Pusrpouse:

        Function returns ProcessId by specified ThreadId

    Arguments:

        dwThreadId - [in] thread ID

    Returns:
        ProcessId

--*/
{
    HANDLE hThread = OpenThread(
        THREAD_QUERY_INFORMATION,
        FALSE,
        dwThreadId);

    ULONG dwReturnLength = 0;
    THREAD_BASIC_INFORMATION  BasicInfo;

    if (hThread != NULL)
    {
        memset(&BasicInfo, 0, sizeof(BasicInfo));

        NTSTATUS status = NtApi.NtQueryInformationThread(
            hThread,
            /*ThreadBasicInformation*/ 0,
            &BasicInfo,
            sizeof(BasicInfo),
            &dwReturnLength);
            

        CloseHandle(hThread);

        if (NT_SUCCESS(status))
        {
            return (DWORD)BasicInfo.ClientId.UniqueProcess;
        }
    }

    return 0;
}

LONG_PTR
GetProcessPeb(IN HANDLE hProcess)
/*++

    Function:
        GetProcessPeb

    Pusrpouse:

        Function returns Process Environment block for opened process

    Arguments:

        hProcess - [in] process

    Returns:
        PEB base

--*/
{
    PROCESS_BASIC_INFORMATION ProcessBasicInfo;
    ULONG   returnSize = 0;

    NTSTATUS status = NtApi.NtQueryInformationProcess(
        hProcess,
        ProcessBasicInformation,
        &ProcessBasicInfo,
        sizeof(ProcessBasicInfo),
        &returnSize);

    if (!NT_SUCCESS(status)) {

        return NULL;
    }

    return (LONG_PTR)ProcessBasicInfo.PebBaseAddress;
}


BOOL
ReadMemory(HANDLE hProcess, ULONG64 address, PVOID Buffer, SIZE_T size)
/*++
    This routine is extensible. It is possible that sometime it will be necessary 
    to read the memory outside the boundaries of the WOW64 process
--*/
{
    SIZE_T   readed = 0;

    NTSTATUS status = NtApi.NtReadVirtualMemory(
        hProcess,
        (PVOID)address,
        Buffer,
        size,
        &readed);

    if (!NT_SUCCESS(status) || readed != size)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL
ReadFileFunction(
    HANDLE hFile, ULONG64 address, PVOID buffer, SIZE_T size)
{

    DWORD readed = 0;

    LONG  distanceLow = (LONG)address;
    LONG  distanceHigh = (LONG)((ULONG64)address >> 32);

    SetFilePointer(hFile, distanceLow, &distanceHigh, FILE_BEGIN);

    return ReadFile(hFile, buffer, (DWORD)size, &readed, NULL);
}



typedef struct {
    DWORD               dwProtection;
    PROTECTION_FLAGS    attr;
} PROTECT_TO_FLAGS;

//
// Conversion table of system attributes to internal
//

const PROTECT_TO_FLAGS protect2Flags[] = {
    PAGE_READONLY,          {1, 0, 0},
    PAGE_READWRITE,         {1, 1, 0},
    PAGE_WRITECOPY,         {0, 1, 0},
    PAGE_EXECUTE,           {0, 0, 1},
    PAGE_EXECUTE_READ,      {1, 0, 1},
    PAGE_EXECUTE_READWRITE, {1, 1, 1},
    PAGE_EXECUTE_WRITECOPY, {0, 1, 1}
};


BOOL
EnumerateProcessModules(
    HANDLE hProcess, 
    TArray<MODULE_ENUM_ENTRY> &_array,
    BOOL bCopyPath)
/*++
    Function:
        EnumerateProcessModules

    Pusrpouse:

        Enumerate modules in other process opened by handle.
        This function uses a callback function to read memory. This makes it possible to read 
        some data structures remotely without using a stub in a remote process.

    Arguments:

        hProcess    - [in] remote process

        _array      - [in] enumeration array

        bCopyPath   - [in] sould routine copy full path

    Returns:
        result

--*/
{
    LONG_PTR pebAddress = GetProcessPeb(hProcess);

    if (pebAddress == NULL)
        return FALSE;


    PVOID  tempBuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);

    PEB *remotePeb = (PEB *)tempBuffer;
    PEB_LDR_DATA  pebLdrData;

    //
    // read a PEB data to a temporary structure
    //

    if (ReadMemory(
        hProcess,
        pebAddress,
        remotePeb,
        sizeof(PEB)) != TRUE)
    {

        VirtualFree(tempBuffer, 0, MEM_FREE);
        return FALSE;
    }

    //
    // read a PEB LDR data to a temporary structure
    //

    if (ReadMemory(
        hProcess,
        (LONG_PTR)remotePeb->Ldr,
        &pebLdrData,
        sizeof(PEB_LDR_DATA)) != TRUE)
    {

        VirtualFree(tempBuffer, 0, MEM_FREE);
        return FALSE;
    }

    LONG_PTR   listEntryPtr = (LONG_PTR)pebLdrData.InLoadOrderModuleList.Flink;

    LONG_PTR   headPtr    = (LONG_PTR)remotePeb->Ldr + FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModuleList);

    BOOL    _result = FALSE;

    //
    // We execute a cycle according to the list of PEB, unfortunately, this 
    // operation causes a large number of readings of the remote process.
    //

    while ((LONG_PTR)listEntryPtr != headPtr)
    {

        LONG_PTR dataTableEntryPtr = (LONG_PTR)
            CONTAINING_RECORD(listEntryPtr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        PLDR_DATA_TABLE_ENTRY dataTableEntry = (PLDR_DATA_TABLE_ENTRY)tempBuffer;


        //
        // read LDR data
        //

        if (ReadMemory(
            hProcess,
            dataTableEntryPtr,
            dataTableEntry,
            sizeof(LDR_DATA_TABLE_ENTRY)) != TRUE)
        {
            break;
        }

        //
        // forward to next entry and give an ability to use temorary buffer
        //

        listEntryPtr = (LONG_PTR)dataTableEntry->InLoadOrderLinks.Flink;

        MODULE_ENUM_ENTRY  enumEntry;
        enumEntry.moduleStart =  (LONG_PTR)dataTableEntry->DllBase;
        enumEntry.moduleSize =  (LONG_PTR)dataTableEntry->SizeOfImage;

        //
        // read DLL info
        //

        UNICODE_STRING  baseDllName = dataTableEntry->BaseDllName;

        WCHAR  *dllName = (WCHAR *)tempBuffer;

        dllName[0] = 0;

        if (baseDllName.Length < (0x1000 - sizeof(WCHAR)))
        {
            if (ReadMemory(
                hProcess,
                (LONG_PTR)baseDllName.Buffer,
                dllName,
                baseDllName.Length) != TRUE)
            {
                break;
            }

            dllName[baseDllName.Length / sizeof(WCHAR)] = 0;
        }
        else
        {
            break;
        }

        _wtncpy(enumEntry.moduleName, dllName, 29);

        //
        // copy path if requested
        //

        if (bCopyPath)
        {
            UNICODE_STRING  fullDllName = dataTableEntry->FullDllName;

            WCHAR   *wcLocal = reinterpret_cast<WCHAR *>(
                malloc(fullDllName.Length + sizeof(WCHAR)));

            if (wcLocal != 0)
            {
                if (ReadMemory(
                    hProcess,
                    (LONG_PTR)fullDllName.Buffer,
                    wcLocal,
                    fullDllName.Length) != TRUE)
                {
                    free(enumEntry.modulePath);
                    break;
                }

                wcLocal[fullDllName.Length / sizeof(WCHAR)] = 0;

            }

#ifdef _UNICODE
            enumEntry.modulePath = wcLocal;
#else // _UNICODE

            enumEntry.modulePath = reinterpret_cast<CHAR *>(
                malloc(fullDllName.Length / sizeof(WCHAR) + sizeof(CHAR)));

            if (enumEntry.modulePath != 0)
            {
                _wtncpy(enumEntry.modulePath, wcLocal, fullDllName.Length / sizeof(WCHAR));
            }

            free(wcLocal);

#endif //_UNICODE
        }
        else
        {
            enumEntry.modulePath = 0;
        }

        _array.Add(enumEntry);

        _result = TRUE;


        /*LIST_ENTRY   currentListEntry;
        if (ReadMemory(
            hProcess,
            listEntryPtr,
            &currentListEntry,
            sizeof(LIST_ENTRY)) != TRUE)
        {
            break;
        }

        //
        // forward to next entry and give an ability to use temorary buffer
        // 
        listEntryPtr = (LONG_PTR)currentListEntry.Flink;*/

    }

    

    VirtualFree(tempBuffer, 0, MEM_FREE);
    return _result;
}


BOOL
EnumerateMemoryRegions(IN const GUI_PARAMETERS *parameters, TArray<MEMORY_ENUM_ENTRY> &_array)
/*++

    Function:
        EnumerateMemoryRegions

    Pusrpouse:

        Enumerate memory regions in the process

    Arguments:

        parameters  - [in] callback parameters

        _array      - [in] enumeration array

    Returns:
        result

--*/
{
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ,
        FALSE,
        parameters->dwProcessId);

    if (hProcess == NULL)
        return FALSE;

    SYSTEM_INFO     sysInfo;
    GetSystemInfo(&sysInfo);

    ULONG_PTR  startAddress = (ULONG_PTR)sysInfo.lpMinimumApplicationAddress;
    ULONG_PTR  currentAddress = startAddress;
    ULONG_PTR  endAddress = (ULONG_PTR)sysInfo.lpMaximumApplicationAddress;


    do
    {
        MEMORY_BASIC_INFORMATION    memInfo;

        if (VirtualQueryEx(
            hProcess,
            (LPCVOID)currentAddress,
            &memInfo,
            sizeof(MEMORY_BASIC_INFORMATION)) >= sizeof(MEMORY_BASIC_INFORMATION))
        {

            MEMORY_ENUM_ENTRY  entry;
            
            entry.regionStart = (LONG_PTR)memInfo.BaseAddress;
            entry.regionSize = (SIZE_T)memInfo.RegionSize;
            entry.attr._align = 0;
            entry.isImage = FALSE;
            entry.moduleName[0] = 0;

            for (int i = 0; i < sizeof(protect2Flags) / sizeof(protect2Flags[0]); i++)
            {
                if (memInfo.Protect & protect2Flags[i].dwProtection)
                {

                    entry.attr._align = protect2Flags[i].attr._align;
                    break;
                }
            }

            if (memInfo.Type & MEM_IMAGE) {
                entry.isImage = TRUE;
            }

            _array.Add(entry);

            currentAddress += memInfo.RegionSize;
        }
        else
        {
            break;
        }

    }
    while (currentAddress < endAddress);


    // 
    // If we have the opportunity, we can fill in additional information about the 
    // modules found. This will make it easier to search nessesary region in memory list.
    //

    TArray<MODULE_ENUM_ENTRY> modules;

    if (EnumerateProcessModules(hProcess, modules) == TRUE)
    {
        //
        // Fill in the information on the found modules
        //
        for (INT_PTR i = 0, z = _array.count(); i < z; i++)
        {
            //
            // Everything is under control, we can have a constant here
            //
            const MEMORY_ENUM_ENTRY  *entry = &_array[i];

            for (INT_PTR j = 0, x = modules.count(); j < x; j++)
            {

                const MODULE_ENUM_ENTRY *moduleEntry = &modules[j];

                if (entry->regionStart >= moduleEntry->moduleStart &&
                    (entry->regionStart + entry->regionSize) <= (moduleEntry->moduleStart + moduleEntry->moduleSize))
                {
                    _tcscpy((TCHAR *)entry->moduleName, moduleEntry->moduleName);
                    break;
                }
            }
        }
    }
    
    return TRUE;
}

BOOL
GetModuleForAddress(HANDLE hProcess, LONG_PTR address, MODULE_ENUM_ENTRY *entry)
/*++

    Function:
        GetModuleForAddress

    Pusrpouse:

        Retrive information for address in remote process

    Arguments:

        hProcess    - [in] remote process

        address     - [in] address

        entry       - [out] entry

    Returns:
        result

--*/
{
    TArray<MODULE_ENUM_ENTRY> _array;

    if (EnumerateProcessModules(hProcess, _array, TRUE) != TRUE) {
        return FALSE;
    }


    BOOL  _return = FALSE;
    for (INT_PTR j = 0, x = _array.count(); j < x; j++)
    {

        MODULE_ENUM_ENTRY *moduleEntry = &_array[j];

        //
        // capture module coresponding the address specified
        //
        if (address >= moduleEntry->moduleStart &&
            address < (LONG_PTR)(moduleEntry->moduleStart + moduleEntry->moduleSize))
        {
            *entry = *moduleEntry;

            moduleEntry->modulePath = 0;

            _return = TRUE;
        }

        if (moduleEntry->modulePath != 0)
        {
            free(moduleEntry->modulePath);
            moduleEntry->modulePath = 0;
        }

    }

    return _return;
}


DWORD
GetFileLengthByName(TCHAR *fileName)
{
    DWORD   dwFileSize = 0;

    if (GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES)
    {

        HANDLE hFile = CreateFile(
            fileName,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD   dwFileSizeHigh = 0;
            dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

            //
            // dumps more than 4gb is unsupported
            //
            if (dwFileSizeHigh != 0)
                dwFileSize = 0;

            CloseHandle(hFile);
        }
    }

    return dwFileSize;
}

int
_wtncpy(TCHAR  *dest, const WCHAR *src, SIZE_T maxCount)
{
    SIZE_T k = 0;

    while (*src)
    {
        *dest = (TCHAR)*src;
        dest++;
        src++;

        k++;

        if (k >= maxCount)
            break;
    }

    *dest = 0;

    return (int)k;
}