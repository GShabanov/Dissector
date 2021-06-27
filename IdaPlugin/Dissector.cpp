/***************************************************************************************\
*   File:                                                                               *
*       Dissector.cpp                                                                   *
*                                                                                       *
*   Abstract:                                                                           *
*       Entry point to IDA plugin, all nessesary work and initialization placed here    *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 1-April-2019                                                      *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $

#include "stdafx.h"
#include "strings.h"
#include "PeHelper.h"
#include "utils.h"

#include "MainDialog.h"
#include "dumpModeDialog.h"

#include "configuration.h"

VOID DescribeError(DWORD dwError, char *string);
LONG ExceptionFilter(LPEXCEPTION_POINTERS pointers);

BOOL DumpMemoryOperation(const GUI_PARAMETERS *parameters);
BOOL PatchMemoryOperation(const GUI_PARAMETERS *parameters, BOOL inRunProcess = FALSE);

extern plugin_t PLUGIN;
void (idaapi* idaview_marker)(ea_t ea);


//
// mocks
//
int __cdecl
_purecall(void)
{
    return 1;
}

void
Assert(const char *, const char *, const int )
{
    // nothing
}


HWND        g_hwndIdaWnd;
HINSTANCE   g_hInstance;

//static UCHAR           configurationPlace[sizeof(CConfiguration)];
//CConfiguration  *configuration = (CConfiguration *)configurationPlace;

//CConfiguration  *configuration = NULL;

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
/*++

    Function:
        DllMain

    Pusrpouse:

        main DLL entry point. Saving instance PTR

    Arguments:

        hinstDLL    - [in] instance PTR

        fdwReason   - [in] attach reason

    Returns:
        BOOL

--*/
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {

        g_hInstance = hinstDLL;
    }

    return TRUE;
}


VOID
MessageCallback(PVOID, LONG_PTR address, const char *Message)
/*++

    Function:
        MessageCallback

    Pusrpouse:

        messages passed from engine to IDA console

    Arguments:

        address     - [in] address

        Message     - [in] message

    Returns:
        VOID

--*/
{
    CHAR   *pDescribe = static_cast<CHAR *>(_alloca(0x1000));

#undef sprintf

    sprintf(pDescribe, "address 0x%p: %s\r\n", (void *)address, Message);

    msg(pDescribe);

}

int idaapi
PluginInit()
/*++

    Function:
        PluginInit

    Pusrpouse:

        IDA plugin entry

    Arguments:

    Returns:
        int

--*/
{

#ifdef __EA64__
    if (!inf.is_64bit())
#else 
    if (!inf.is_32bit())
#endif // __X64__
    {
        return PLUGIN_SKIP;
    }
    MessageBoxA(NULL, "123", "123", MB_OK);

    msg("[*] PluginInit");

    if (inf.filetype != f_PE)
    {
        return PLUGIN_SKIP;
    }

    if (InitializeNativeApi() != TRUE)
    {
        msg("[-] failed to initialize plugin");
        return PLUGIN_SKIP;
    }

    CConfiguration::Get()->GetConfig();

    return (PLUGIN.flags & PLUGIN_UNL) ? PLUGIN_OK : PLUGIN_KEEP;
}

void idaapi
PluginTerm(void)
/*++

    Function:
        PluginTerm

    Pusrpouse:

        IDA plugin termination callback

    Arguments:

    Returns:
        void

--*/
{
}

BOOL
StoreDataToFile(IN const TCHAR *fileName, IN const PVOID buffer, IN const SIZE_T size)
/*++

    Function:
        StoreDataToFile

    Pusrpouse:
        save data to file helper function

    Arguments:

        fileName - [in] file name

        buffer   - [in] buffer with data

        size     - [in] data size

    Returns:
        void

--*/
{

    HANDLE hFile = CreateFile(
        fileName,
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);


    if (hFile == INVALID_HANDLE_VALUE)
    {
        msg("[-] file is not created %d\n", GetLastError());
        return FALSE;
    }

    SetEndOfFile(hFile);

    DWORD   dwWritten = 0;
    if (!WriteFile(hFile, buffer, (DWORD)size, &dwWritten, NULL))
    {
        msg("[-] failed to write file %d\n", GetLastError());
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);

    return TRUE;
}

BOOL
ReadDataFromFile(const TCHAR *fileName, OUT PVOID *buffer, OUT SIZE_T *size)
/*++

    Function:
        ReadDataFromFile

    Pusrpouse:
        allocate the buffer and read file data to it

    Arguments:

        fileName - [in] file name

        buffer   - [out] buffer with data

        size     - [out] data size

    Returns:
        BOOL

--*/
{
    if (buffer == NULL || size == NULL)
        return FALSE;

    HANDLE hFile = CreateFile(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);


    if (hFile == INVALID_HANDLE_VALUE)
    {
        msg("[-] file is not created %d\n", GetLastError());
        return FALSE;
    }


    DWORD dwFileSizeHigh = 0;
    DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

    if (dwFileSizeHigh > 0)
    {
        msg("[-] the dump file is to big\n");
        CloseHandle(hFile);
        return FALSE;
    }

    PVOID  data = VirtualAlloc(0, dwFileSize, MEM_COMMIT, PAGE_READWRITE);

    if (data == NULL)
    {
        msg("[-] VirtualAlloc is failed\n");
        CloseHandle(hFile);
        return FALSE;
    }

    DWORD   dwReaded = 0;
    if (!ReadFile(hFile, data, dwFileSize, &dwReaded, NULL))
    {
        msg("[-] failed to read file %d\n", GetLastError());

        VirtualFree(data, 0, MEM_RELEASE);
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);

    *buffer = data;
    *size = dwReaded;

    return TRUE;
}


BOOL
QueryDumpParameters(GUI_PARAMETERS *parameters)
/*++

    Function:
        QueryDumpParameters

    Pusrpouse:
        query DUMP parameters callback called by GUI code

    Arguments:

        parameters - [in] parameters

    Returns:
        BOOL

--*/
{
    LONG_PTR  currentAddress = (LONG_PTR)get_screen_ea();

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ,
        FALSE,
        parameters->dwProcessId);

    if (hProcess == NULL)
    {
        msg("[-] failed to open process ID: %d\n", parameters->dwProcessId);
        return FALSE;
    }

    MEMORY_BASIC_INFORMATION    memBasic;

    memset(&memBasic, 0, sizeof(MEMORY_BASIC_INFORMATION));

    if (VirtualQueryEx(
        hProcess,
        (LPCVOID)(LONG_PTR)currentAddress,
        &memBasic,
        sizeof(memBasic)) != sizeof(memBasic))
    {
        CloseHandle(hProcess);

        msg("[-] failed to query virtual region: %d\n", GetLastError());
        return FALSE;
    }

    parameters->choosenVA = currentAddress;
    parameters->choosenRange.endVA = -1;

    parameters->pDumpParameters->pFromVA = (LONG_PTR)memBasic.BaseAddress;
    parameters->pDumpParameters->pToVA = (LONG_PTR)memBasic.BaseAddress + memBasic.RegionSize;

    CloseHandle(hProcess);
    return TRUE;

}

BOOL
QueryPatchParameters(GUI_PARAMETERS *parameters)
/*++

    Function:
        QueryPatchParameters

    Pusrpouse:
        query PATCH parameters callback called by GUI code

    Arguments:

        parameters - [in] parameters

    Returns:
        BOOL

--*/
{
    LONG_PTR  currentAddress = (LONG_PTR)get_screen_ea();

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ,
        FALSE,
        parameters->dwProcessId);

    if (hProcess == NULL)
    {
        msg("[-] failed to open process ID: %d\n", parameters->dwProcessId);
        return FALSE;
    }

    MEMORY_BASIC_INFORMATION    memBasic;

    memset(&memBasic, 0, sizeof(MEMORY_BASIC_INFORMATION));

    if (VirtualQueryEx(
        hProcess,
        (LPCVOID)(LONG_PTR)currentAddress,
        &memBasic,
        sizeof(memBasic)) != sizeof(memBasic))
    {
        CloseHandle(hProcess);

        msg("[-] failed to query virtual region: %d\n", GetLastError());
        return FALSE;
    }


    DWORD   dwFileDumpSize = GetFileLengthByName(parameters->szDumpFileName);

    if (dwFileDumpSize == 0)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    parameters->pPatchParameters->pFromVA = (LONG_PTR)currentAddress;

    if (dwFileDumpSize != 0)
    {
        parameters->pPatchParameters->pToVA = (LONG_PTR)currentAddress + dwFileDumpSize;
    }
    else
    {
        parameters->pPatchParameters->pToVA = (LONG_PTR)memBasic.BaseAddress + memBasic.RegionSize;
    }

    CloseHandle(hProcess);
    return TRUE;
}

BOOL
DumpMemoryOperation(const GUI_PARAMETERS *parameters)
/*++

    Function:
        DumpMemoryOperation

    Pusrpouse:
        Dump memory callback called by GUI

    Arguments:

        parameters - [in] parameters

    Returns:
        void

--*/
{
    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ,
        FALSE,
        parameters->dwProcessId);

    if (hProcess == NULL)
    {
        DescribeError(GetLastError(), "[-] failed to open process");
        return FALSE;
    }

    const DUMP_PARAMETERS *pDumpParameters = (const DUMP_PARAMETERS *)parameters->pDumpParameters;

    PVOID  dumpBuffer = NULL;
    SIZE_T  toReadSize = 0;

    toReadSize = pDumpParameters->pToVA - pDumpParameters->pFromVA;

    dumpBuffer = VirtualAlloc(NULL, toReadSize, MEM_COMMIT, PAGE_READWRITE);

    if (dumpBuffer == NULL)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    SIZE_T actualRead = 0;

    BOOL result = ReadProcessMemory(
        hProcess,
        (LPCVOID)pDumpParameters->pFromVA,
        dumpBuffer,
        toReadSize,
        &actualRead);

    CloseHandle(hProcess);

    if (result != TRUE)
    {
        DescribeError(GetLastError(), "[-] failed to read process memory");
        VirtualFree(dumpBuffer, 0, MEM_RELEASE);
        return FALSE;
    }

    result = StoreDataToFile(parameters->szDumpFileName, dumpBuffer, actualRead);

    VirtualFree(dumpBuffer, 0, MEM_RELEASE);

    if (result != TRUE)
    {
        return FALSE;
    }

    return TRUE;

}

BOOL
PatchMemoryOperationInDebug(
    IN const GUI_PARAMETERS *parameters,
    IN const PVOID dumpData,
    IN SIZE_T dumpDataSize)
/*++

    Function:
        PatchMemoryOperationInDebug

    Pusrpouse:
        patch memory GUI callback in debug session

    Arguments:

        parameters   - [in] parameters

        dumpData     - [in] patching data

        dumpDataSize - [in] data size

    Returns:
        void

--*/
{

    
    LONG_PTR  currentAddress = parameters->pPatchParameters->pFromVA;

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE,
        parameters->dwProcessId);

    if (hProcess == nullptr)
    {
        DescribeError(GetLastError(), "[-] failed to open process");
        return FALSE;
    }

    MODULE_ENUM_ENTRY ModuleEntry;

    ModuleEntry.modulePath = NULL;

    if (GetModuleForAddress(hProcess, currentAddress, &ModuleEntry) != TRUE)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    if (ModuleEntry.modulePath == NULL)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    ULONG  rva = (ULONG)(
        (ULONG64)currentAddress - (ULONG64)ModuleEntry.moduleStart);

    BOOL   _return = FALSE;
    //
    // in case of permanent changes
    //
    if (parameters->pPatchParameters->bMakePermanentChanges == TRUE)
    {
        SIZE_T  newSize = (_tcslen(ModuleEntry.modulePath) + 1) * sizeof(TCHAR);
        TCHAR   *newFileName = (TCHAR *)malloc(newSize);

        if (newFileName != nullptr)
        {
            _tcscpy(newFileName, ModuleEntry.modulePath);
            _tcscat(newFileName, _T(".tmp"));

            //
            // if this is first run, cleanup the temp file
            //
            BOOL firstRun = FALSE;

            if (DeleteFile(newFileName) != TRUE)
            {
                if (GetLastError() == ERROR_FILE_NOT_FOUND)
                {
                    firstRun = TRUE;
                }
            }
            else
            {
                firstRun = TRUE;
            }

            if (firstRun)
            {
                if (MoveFile(ModuleEntry.modulePath, newFileName) == TRUE)
                {
                    CopyFile(newFileName, ModuleEntry.modulePath, FALSE);
                }
            }

        }


        LONG_PTR physicalOffsetInFile = 0;


        HANDLE hFile = CreateFile(
            ModuleEntry.modulePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile != INVALID_HANDLE_VALUE)
        {
            if (GetPhysicalOffsetFromRelativeAddress(
                ReadFileFunction, hFile, (ULONG64)0, rva, &physicalOffsetInFile) == TRUE)
            {

                SetFilePointer(hFile, (LONG)physicalOffsetInFile, nullptr, FILE_BEGIN);

                DWORD   dwWritten = 0;

                _return |= WriteFile(hFile, dumpData, (DWORD)dumpDataSize, &dwWritten, NULL);

                if (_return != TRUE)
                {
                    msg("[-] failed to apply patch: %d\n", GetLastError());
                }
            }

            CloseHandle(hFile);
        }
    }


    SIZE_T   dwWritten = 0;
    _return |= WriteProcessMemory(
        hProcess,
        reinterpret_cast<LPVOID>(currentAddress),
        dumpData,
        dumpDataSize,
        &dwWritten);


    //
    // fix code in IDA
    // 
#if (IDA_SDK_VERSION >= 700)

    show_addr((ea_t)currentAddress);

    del_items((ea_t)currentAddress, DELIT_EXPAND, (asize_t)dumpDataSize, 0);

    // put bytes on analyze screen
    put_bytes((ea_t)currentAddress, dumpData, dumpDataSize);

#else 

    showAddr((ea_t)currentAddress);

    do_unknown_range((ea_t)currentAddress, dumpDataSize, true);

    // put bytes on analyze screen
    put_many_bytes((ea_t)currentAddress, dumpData, dumpDataSize);

#endif


    create_insn((ea_t)currentAddress);

    CloseHandle(hProcess);


    return TRUE;
}

BOOL
PatchMemoryOperationOnDisk(
    IN const GUI_PARAMETERS *parameters,
    IN const PVOID dumpData,
    IN const SIZE_T dumpDataSize
    )
/*++

    Function:
        PatchMemoryOperationOnDisk

    Pusrpouse:
        patch data on disk callback

    Arguments:

        parameters   - [in] parameters

        dumpData     - [in] patching data

        dumpDataSize - [in] data size

    Returns:
        BOOL

--*/
{
    UNREFERENCED_PARAMETER(parameters);
    UNREFERENCED_PARAMETER(dumpData);
    UNREFERENCED_PARAMETER(dumpDataSize);

    /*GetPhysicalOffsetFromRelativeAddress(
        ReadFileFunction, hFile, (ULONG64)0, rva, &physicalOffset1);*/

    return FALSE;
}

BOOL
PatchMemoryOperation(
    IN const GUI_PARAMETERS *parameters,
    IN BOOL inDebug
    )
/*++

    Function:
        PatchMemoryOperation

    Pusrpouse:
        patch callback

    Arguments:

        parameters   - [in] parameters

        inDebug     - [in] patching data

    Returns:
        BOOL

--*/
{
    PVOID  dumpFile = nullptr;
    SIZE_T dumpFileSize = 0;

    if (ReadDataFromFile(parameters->szDumpFileName, &dumpFile, &dumpFileSize) != TRUE)
    {
        return FALSE;
    }

    BOOL _return = FALSE;
    //
    // if the filling is doing in run process
    //
    if (inDebug)
    {
        _return = PatchMemoryOperationInDebug(parameters, dumpFile, dumpFileSize);
    }
    else
    {
        _return = PatchMemoryOperationOnDisk(parameters, dumpFile, dumpFileSize);
    }

    VirtualFree(dumpFile, 0, MEM_RELEASE);

    return _return;
}

BOOL
DissectProcess(DWORD dwProcessId)
/*++

    Function:
        DissectProcess

    Pusrpouse:
        actual start for the plugin dialog

    Arguments:

        dwProcessId   - [in] process ID we use

    Returns:
        BOOL

--*/
{


    BOOL    dialogResult = FALSE;

    DUMP_PARAMETERS     dumpParameters;
    PATCH_PARAMETERS    patchParameters;

    memset(&dumpParameters, 0, sizeof(dumpParameters));
    memset(&patchParameters, 0, sizeof(patchParameters));

    //
    // parameters preparation
    //

    patchParameters.bMakePermanentChanges = CConfiguration::Get()->m_makeChangeOnDisk == TRUE ? TRUE : FALSE;

    GUI_PARAMETERS  parameters;
    //
    // give allocators
    //

    parameters._allocFunction = malloc;
    parameters._freeFunction  = free;

    parameters.szDumpFileName = static_cast<TCHAR *>(
        parameters._allocFunction(MAX_PATH * sizeof(TCHAR)));

    if (parameters.szDumpFileName == nullptr) {

        return FALSE;
    }

    _tcscpy(parameters.szDumpFileName, _T("memory.dmp"));

    parameters.context = nullptr;
    parameters.dwProcessId = dwProcessId;
    parameters.pDumpParameters = &dumpParameters;
    parameters.pPatchParameters = &patchParameters;

    parameters.enumerateMemoryRegions = &EnumerateMemoryRegions;
    parameters.queryDumpParameters = &QueryDumpParameters;
    parameters.queryPatchParameters = &QueryPatchParameters;
    parameters.queryNopingParameters = nullptr;


    //
    // create dialog 
    //
    CMainDialog  dlg(g_hInstance, &parameters, GetActiveWindow());

    //
    // run modeless
    //
    if (dlg.InitInstance() == TRUE)
    {
        dialogResult = dlg.Run();
    }


    if (dialogResult != TRUE)
    {
        parameters._freeFunction(parameters.szDumpFileName);
        return FALSE;
    }
    else
    {

        BOOL result = FALSE;

        //
        // perform the work according to work type selected
        //

        switch (parameters.workType)
        {
        case Dumping:
            result = DumpMemoryOperation(&parameters);
            break;

        case Patching:
            result = PatchMemoryOperation(&parameters, TRUE);
        case Nopping:
            break;

        default:;
        }

        parameters._freeFunction(parameters.szDumpFileName);
        return result;
    }

}


#if (IDA_SDK_VERSION >= 700)
bool idaapi
PluginRun(size_t /*arg */)
#else
void idaapi
PluginRun(int /*arg */)
#endif // (IDA_SDK_VERSION > 660)

/*++

    Function:
        PluginRun

    Pusrpouse:
        plugin run IDA callback

    Arguments:

    Returns:
        BOOL

--*/
{
    CConfiguration::Get()->GetConfig();

#if (IDA_SDK_VERSION >= 700)
    set_auto_state(st_Think);
#else
    setStat(st_Think);
#endif //

    
    if (get_process_state() == DSTATE_NOTASK)
    {
#if (IDA_SDK_VERSION >= 700)
        set_auto_state(st_Ready);
        return true;
#else 
        setStat(st_Ready);
        return;
#endif //
    }


    thid_t currentDebugThread = get_current_thread();

    DWORD dwProcessId = GetThreadProcess((DWORD)currentDebugThread);

    if (dwProcessId == 0)
    {
        msg("[-] failed to get process ID\n");
#if (IDA_SDK_VERSION >= 700)
        set_auto_state(st_Ready);
        return false;
#else 
        setStat(st_Ready);
        return;
#endif //
    }

    BOOL result = DissectProcess(dwProcessId);

#if (IDA_SDK_VERSION >= 700)
    set_auto_state(st_Ready);
#else 
    setStat(st_Ready);
#endif //

    msg("[*] finished\n");

    CConfiguration::Get()->SaveConfig();

#if (IDA_SDK_VERSION >= 700)
    return result == TRUE ? true : false;
#else
    UNREFERENCED_PARAMETER(result);
#endif // (IDA_SDK_VERSION >= 700)
}


LONG
ExceptionFilter(LPEXCEPTION_POINTERS pointers)
/*++

    Function:
        ExceptionFilter

    Pusrpouse:
        exception filter to protect main IDA module

    Arguments:

    Returns:
        LONG

--*/
{    

    TCHAR   *pExceptionDescribe = (TCHAR *)_alloca(0x1000 * sizeof(TCHAR));

#undef sprintf

    _stprintf(pExceptionDescribe, _T("Main module exception: address 0x%p, code 0x%04X"),
        pointers->ExceptionRecord->ExceptionAddress,
        pointers->ExceptionRecord->ExceptionCode);

    ::MessageBox(g_hwndIdaWnd, pExceptionDescribe, _T("exception"), MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}


//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------
plugin_t PLUGIN =
{
    IDP_INTERFACE_VERSION,
        PLUGIN_UNL,            // plugin flags
        PluginInit,                        // initialize
        PluginTerm,                        // terminate. this pointer may be NULL.
        PluginRun,                         // invoke plugin
        comment,
        help,                 // multiline help about the plugin
        wanted_name,          // the preferred short name of the plugin
        wanted_hotkey         // the preferred hotkey to run the plugin
};


VOID
DescribeError(DWORD dwError, char *string)
{
    TCHAR   *lpMsgBuf = nullptr;

    if (!::FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL ))
    {
        // Handle the error.
        return;
    }

    CHAR   *szErrorDescribe = (CHAR *)_alloca(0x1000);

    sprintf(szErrorDescribe, "Error: %s\r\n error: %s [%u]\r\n",
        string,
        lpMsgBuf, dwError);

    msg(szErrorDescribe);

    ::LocalFree(lpMsgBuf);

}
