/***************************************************************************************\
*   File:                                                                               *
*       configuration.h                                                                 *
*                                                                                       *
*   Abstract:                                                                           *
*       configuration points for Dissector                                              *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#pragma once
#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

typedef struct _DUMP_PARAMETERS {
    LONG_PTR   pFromVA;
    LONG_PTR   pToVA;
} DUMP_PARAMETERS, *PDUMP_PARAMETERS;

typedef struct _PATCH_PARAMETERS {
    LONG_PTR   pFromVA;
    LONG_PTR   pToVA;
    BOOL       bMakePermanentChanges;
} PATCH_PARAMETERS, *PPATCH_PARAMETERS;

#pragma warning(push)
#pragma warning(disable : 4201)
typedef union _PROTECTION_FLAGS {

    struct {
        ULONG   read    : 1;
        ULONG   write   : 1;
        ULONG   execute : 1;
    };

    ULONG    _align;
} PROTECTION_FLAGS;
#pragma warning(pop)

//
// A locally defined structure for defining information about a region.
//

typedef struct _MEMORY_ENUM_ENTRY {
    LONG_PTR          regionStart;
    SIZE_T            regionSize;
    PROTECTION_FLAGS  attr;
    BOOL              isImage;
    TCHAR             moduleName[30];
} MEMORY_ENUM_ENTRY, *PMEMORY_ENUM_ENTRY;

//
// Various callbacks used by the GUI to retrieve information from the engine.
//

typedef BOOL (*_QueryDumpParameters)(struct _GUI_PARAMETERS *parameters);
typedef BOOL (*_QueryPatchParameters)(struct _GUI_PARAMETERS *parameters);
typedef BOOL (*_QueryNopingParameters)(struct _GUI_PARAMETERS *parameters);
typedef BOOL (*_EnumerateMemoryRegions)(const struct _GUI_PARAMETERS *parameters, TArray<MEMORY_ENUM_ENTRY> &_array);

//
// the choosen work in GUI
//

typedef enum _CHOOSEN_WORK {
    Dumping  = 0,
    Patching = 1,
    Nopping  = 2,
} CHOOSEN_WORK;

//
// The structure connecting the user GUI and the engine. 
// Used to save parameters and pass necessary arguments.
//

typedef struct _GUI_PARAMETERS {

    //
    // executable module
    //
    TCHAR              *szExecutableFileName;

    //
    // dump name
    //

    TCHAR              *szDumpFileName;

    //
    // custom context (not used)
    //

    PVOID               context;

    //
    // process ID in debug sessions
    //

    DWORD               dwProcessId;

    union {
        LONG_PTR            choosenVA;
        struct {
            LONG_PTR        startVA;
            LONG_PTR        endVA;
        } choosenRange;
    };

    //
    // external allocators 
    //
    void*                 (* _allocFunction)(size_t size);
    void                  (* _freeFunction)(void *data);

    CHOOSEN_WORK            workType;
    BOOL                    dialogResult;
    DUMP_PARAMETERS        *pDumpParameters;
    PATCH_PARAMETERS       *pPatchParameters;

    //
    // callbacks
    //
    _QueryDumpParameters    queryDumpParameters;
    _QueryPatchParameters queryPatchParameters;
    _QueryNopingParameters  queryNopingParameters;
    _EnumerateMemoryRegions enumerateMemoryRegions;

} GUI_PARAMETERS, *PGUI_PARAMETERS;


//
// Class configuration
//

class CConfiguration : public Singleton<CConfiguration>
{
public:
    RECT    m_hwndRect;
    ULONG   m_SelectedTab;
    BOOL    m_makeChangeOnDisk;

    inline CConfiguration()
    {
        m_SelectedTab = 0;
        m_makeChangeOnDisk = 0;
        memset(&m_hwndRect, 0, sizeof(m_hwndRect));
    }

    BOOL GetConfig();
    BOOL SaveConfig();

private:
    static void __fastcall tc2mb(TCHAR *szInput, char *szOutput, size_t max_size);
    static TCHAR  *SaveString(char *szString);


    BOOL OpenCurrentUserKey(HKEY *hkOutKey, ACCESS_MASK DesiredAccess);
    BOOL OpenProgramKey(HKEY *hkOutKey, ACCESS_MASK DesiredAccess);

};

#endif // __DTECONFIGURATION_H__