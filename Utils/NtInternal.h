/***************************************************************************************\
*   File:                                                                              *
*       NtInternal.h                                                                   *
*                                                                                      *
*   Abstract:                                                                          *
*       Native API poor documented types and defines                                   *
*                                                                                      *
*   Author:                                                                            *
*       G Shabanov          5-Oct-2006                                                 *
*                                                                                      *
*   Revision History:                                                                  *
/***************************************************************************************/
// $Id: $
#pragma once
#ifndef __NTINTERNAL_H__
#define __NTINTERNAL_H__

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
// Determine if an argument is present by testing the value of the pointer
// to the argument value.
//

#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (CHAR *)((ULONG_PTR)(ArgumentPointer)) != (CHAR *)(NULL) )

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//
#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) \
    ((type *)((PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field)))
#endif // CONTAINING_RECORD

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
#endif // FIELD_OFFSET

typedef LONG NTSTATUS;

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L) // ntsubauth
#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023L)
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _STRING {
    USHORT  Length;
    USHORT  MaximumLength;
    PCHAR   Buffer;
} STRING;

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation,
    ProcessQuotaLimits,
    ProcessIoCounters,
    ProcessVmCounters,
    ProcessTimes,
    ProcessBasePriority,
    ProcessRaisePriority,
    ProcessUnused,
    ProcessDebugPort,
    ProcessExceptionPort,
    ProcessAccessToken,
    ProcessLdtInformation,
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    ProcessIoPortHandlers,          // Note: this is kernel mode only
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    ProcessUserModeIOPL,
    ProcessEnableAlignmentFaultFixup,
    ProcessPriorityClass,
    ProcessWx86Information,
    ProcessHandleCount,
    ProcessAffinityMask,
    ProcessPriorityBoost,
    MaxProcessInfoClass
} PROCESSINFOCLASS;

#define DUPLICATE_SAME_ATTRIBUTES   0x00000004
#define OBJ_AUDIT_OBJECT_CLOSE 0x00000004L

typedef enum _OBJECT_INFORMATION_CLASS {
    ObjectBasicInformation,         // OBJECT_BASIC_INFORMATION
    ObjectNameInformation,          // OBJECT_NAME_INFORMATION
    ObjectTypeInformation,          // OBJECT_TYPE_INFORMATION
    ObjectAllInformation,           // OBJECT_ALL_INFORMATION
    ObjectHandleFlagInformation,    // OBJECT_HANDLE_FLAG_INFORMATION
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

typedef struct _OBJECT_HANDLE_FLAG_INFORMATION {
    BOOLEAN Inherit;
    BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_FLAG_INFORMATION, *POBJECT_HANDLE_FLAG_INFORMATION;

typedef struct _OBJECT_BASIC_INFORMATION {
    ULONG Attributes; 
    ACCESS_MASK DesiredAccess; 
    ULONG HandleCount; 
    ULONG ReferenceCount; 
    ULONG PagedPoolQuota; 
    ULONG NonPagedPoolQuota; 
    ULONG HandleTableAddress;
    ULONG ObjectAddress;
    ULONG ObjectHeaderAddress;
    ULONG NameInformationLength;
    ULONG TypeInformationLength;
    ULONG SecurityDescriptorLength;
    LARGE_INTEGER CreateTime;
} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;


typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _OBJECT_TYPE_INFORMATION {
    UNICODE_STRING TypeName; 
    ULONG TotalNumberOfHandles; 
    ULONG TotalNumberOfObjects; 
    WCHAR Unused1[8]; 
    ULONG HighWaterNumberOfHandles; 
    ULONG HighWaterNumberOfObjects; 
    WCHAR Unused2[8]; 
    ACCESS_MASK InvalidAttributes; 
    GENERIC_MAPPING GenericMapping; 
    ACCESS_MASK ValidAttributes; 
    BOOLEAN SecurityRequired; 
    BOOLEAN MaintainHandleCount; 
    USHORT  MaintainTypeList; 
    int     PoolType; 
    ULONG   DefaultPagedPoolCharge; 
    ULONG DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {    /* Class */ /*Query*/ /*Set*/
    SystemBasicInformation,                     // 0        Y        N
    SystemProcessorInformation,                 // 1        Y        N
    SystemPerformanceInformation,               // 2        Y        N
    SystemTimeOfDayInformation,                 // 3        Y        N
    SystemNotImplemented1,                      // 4        Y        N
    SystemProcessesAndThreadsInformation,       // 5        Y        N
    SystemCallCounts,                           // 6        Y        N
    SystemConfigurationInformation,             // 7        Y        N
    SystemProcessorTimes,                       // 8        Y        N
    SystemGlobalFlag,                           // 9        Y        Y
    SystemNotImplemented2,                      // 10       Y        N
    SystemModuleInformation,                    // 11       Y        N
    SystemLockInformation,                      // 12       Y        N
    SystemNotImplemented3,                      // 13       Y        N
    SystemNotImplemented4,                      // 14       Y        N
    SystemNotImplemented5,                      // 15       Y        N
    SystemHandleInformation,                    // 16       Y        N
    SystemObjectInformation,                    // 17       Y        N
    SystemPagefileInformation,                  // 18       Y        N
    SystemInstructionEmulationCounts,           // 19       Y        N
    SystemInvalidInfoClass1,                    // 20
    SystemCacheInformation,                     // 21       Y        Y
    SystemPoolTagInformation,                   // 22       Y        N
    SystemProcessorStatistics,                  // 23       Y        N
    SystemDpcInformation,                       // 24       Y        Y
    SystemNotImplemented6,                      // 25       Y        N
    SystemLoadImage,                            // 26       N        Y
    SystemUnloadImage,                          // 27       N        Y
    SystemTimeAdjustment,                       // 28       Y        Y
    SystemNotImplemented7,                      // 29       Y        N
    SystemNotImplemented8,                      // 30       Y        N
    SystemNotImplemented9,                      // 31       Y        N
    SystemCrashDumpInformation,                 // 32       Y        N
    SystemExceptionInformation,                 // 33       Y        N
    SystemCrashDumpStateInformation,            // 34       Y       Y/N
    SystemKernelDebuggerInformation,            // 35       Y        N
    SystemContextSwitchInformation,             // 36       Y        N
    SystemRegistryQuotaInformation,             // 37       Y        Y
    SystemLoadAndCallImage,                     // 38       N        Y
    SystemPrioritySeparation,                   // 39       N        Y
    SystemNotImplemented10,                     // 40       Y        N
    SystemNotImplemented11,                     // 41       Y        N
    SystemInvalidInfoClass2,                    // 42
    SystemInvalidInfoClass3,                    // 43
    SystemTimeZoneInformation,                  // 44       Y        N
    SystemLookasideInformation,                 // 45       Y        N
    SystemSetTimeSlipEvent,                     // 46       N        Y
    SystemCreateSession,                        // 47       N        Y
    SystemDeleteSession,                        // 48       N        Y
    SystemInvalidInfoClass4,                    // 49
    SystemRangeStartInformation,                // 50       Y        N
    SystemVerifierInformation,                  // 51       Y        Y
    SystemAddVerifier,                          // 52       N        Y
    SystemSessionProcessesInformation           // 53       Y        N
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER SpareLi1;
    LARGE_INTEGER SpareLi2;
    LARGE_INTEGER SpareLi3;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SpareUl2;
    ULONG SpareUl3;
    ULONG PeakVirtualSize;
    ULONG VirtualSize;
    ULONG PageFaultCount;
    ULONG PeakWorkingSetSize;
    ULONG WorkingSetSize;
    ULONG QuotaPeakPagedPoolUsage;
    ULONG QuotaPagedPoolUsage;
    ULONG QuotaPeakNonPagedPoolUsage;
    ULONG QuotaNonPagedPoolUsage;
    ULONG PagefileUsage;
    ULONG PeakPagefileUsage;
    ULONG PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


typedef enum _FILE_INFORMATION_CLASS_FULL {
// end_wdm
    FileDirectoryInformation         = 1,
    FileFullDirectoryInformation,   // 2
    FileBothDirectoryInformation,   // 3
    FileBasicInformation,           // 4  wdm
    FileStandardInformation,        // 5  wdm
    FileInternalInformation,        // 6
    FileEaInformation,              // 7
    FileAccessInformation,          // 8
    FileNameInformation,            // 9
    FileRenameInformation,          // 10
    FileLinkInformation,            // 11
    FileNamesInformation,           // 12
    FileDispositionInformation,     // 13
    FilePositionInformation,        // 14 wdm
    FileFullEaInformation,          // 15
    FileModeInformation,            // 16
    FileAlignmentInformation,       // 17
    FileAllInformation,             // 18
    FileAllocationInformation,      // 19
    FileEndOfFileInformation,       // 20 wdm
    FileAlternateNameInformation,   // 21
    FileStreamInformation,          // 22
    FilePipeInformation,            // 23
    FilePipeLocalInformation,       // 24
    FilePipeRemoteInformation,      // 25
    FileMailslotQueryInformation,   // 26
    FileMailslotSetInformation,     // 27
    FileCompressionInformation,     // 28
    FileObjectIdInformation,        // 29
    FileCompletionInformation,      // 30
    FileMoveClusterInformation,     // 31
    FileQuotaInformation,           // 32
    FileReparsePointInformation,    // 33
    FileNetworkOpenInformation,     // 34
    FileAttributeTagInformation,    // 35
    FileTrackingInformation,        // 36
    FileIdBothDirectoryInformation, // 37
    FileIdFullDirectoryInformation, // 38
    FileValidDataLengthInformation, // 39
    FileShortNameInformation,       // 40
    FileMaximumInformation
// begin_wdm
} FILE_INFORMATION_CLASS_FULL, *PFILE_INFORMATION_CLASS_FULL;

typedef struct _FILE_BASIC_INFORMATION {                    // ntddk wdm nthal
    LARGE_INTEGER CreationTime;                             // ntddk wdm nthal
    LARGE_INTEGER LastAccessTime;                           // ntddk wdm nthal
    LARGE_INTEGER LastWriteTime;                            // ntddk wdm nthal
    LARGE_INTEGER ChangeTime;                               // ntddk wdm nthal
    ULONG FileAttributes;                                   // ntddk wdm nthal
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;         // ntddk wdm nthal

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct _FILE_NAME_INFORMATION {                     // ntddk
    ULONG FileNameLength;                                   // ntddk
    WCHAR FileName[1];                                      // ntddk
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;           // ntddk

typedef struct _FILE_DISPOSITION_INFORMATION {                  // ntddk nthal
    BOOLEAN DeleteFile;                                         // ntddk nthal
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION; // ntddk nthal

typedef struct _FILE_STANDARD_INFORMATION {                 // ntddk wdm nthal
    LARGE_INTEGER AllocationSize;                           // ntddk wdm nthal
    LARGE_INTEGER EndOfFile;                                // ntddk wdm nthal
    ULONG NumberOfLinks;                                    // ntddk wdm nthal
    BOOLEAN DeletePending;                                  // ntddk wdm nthal
    BOOLEAN Directory;                                      // ntddk wdm nthal
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;   // ntddk wdm nthal

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

typedef struct _FILE_PIPE_INFORMATION {
     ULONG ReadMode;
     ULONG CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

typedef struct _FILE_PIPE_LOCAL_INFORMATION {
     ULONG NamedPipeType;
     ULONG NamedPipeConfiguration;
     ULONG MaximumInstances;
     ULONG CurrentInstances;
     ULONG InboundQuota;
     ULONG ReadDataAvailable;
     ULONG OutboundQuota;
     ULONG WriteQuotaAvailable;
     ULONG NamedPipeState;
     ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef struct _FILE_PIPE_REMOTE_INFORMATION {
     LARGE_INTEGER CollectDataTime;
     ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;


typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_VALID_ATTRIBUTES    0x000003F2L

#ifndef InitializeObjectAttributes
#define InitializeObjectAttributes(p, n, a, r, s)   { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES);          \
    (p)->RootDirectory = r;                           \
    (p)->Attributes = a;                              \
    (p)->ObjectName = n;                              \
    (p)->SecurityDescriptor = s;                      \
    (p)->SecurityQualityOfService = NULL; }
#endif // InitializeObjectAttributes


typedef struct _LDR_DATA_TABLE_ENTRY {

  LIST_ENTRY        InLoadOrderLinks;
  LIST_ENTRY        InMemoryOrderLinks;
  LIST_ENTRY        InInitializationOrderLinks;
  PVOID             DllBase;
  PVOID             EntryPoint;
  ULONG             SizeOfImage;
  UNICODE_STRING    FullDllName;
  UNICODE_STRING    BaseDllName;
  ULONG             Flags;
  USHORT            LoadCount;
  USHORT            TlsIndex;
  LIST_ENTRY        HashLinks;
  PVOID             SectionPointer;
  ULONG             CheckSum;
  ULONG             TimeDateStamp;
  PVOID             LoadedImports;
  PVOID             EntryPointActivationContext;
  PVOID             PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA {

  ULONG Length;
  UCHAR Initialized;
  PVOID SsHandle;
  LIST_ENTRY InLoadOrderModuleList;
  LIST_ENTRY InMemoryOrderModuleList;
  LIST_ENTRY InInitializationOrderModuleList;
  PVOID EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _CLIENT_ID {
  HANDLE UniqueProcess;
  HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _PEB {

  UCHAR         InheritedAddressSpace;
  UCHAR         ReadImageFileExecOptions;
  UCHAR         BeingDebugged;
  UCHAR         SpareBool;
  PVOID         Mutant;
  PVOID         ImageBaseAddress;
  PPEB_LDR_DATA  Ldr;
  struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
  /* ***** other fields are not nessesary for me **** */
} PEB, *PPEB;

typedef struct _TEB {

  NT_TIB    NtTib;
  PVOID     EnvironmentPointer;
  CLIENT_ID ClientId;
  PVOID     ActiveRpcHandle;
  PVOID     ThreadLocalStoragePointer;
  PPEB      ProcessEnvironmentBlock;
} TEB, *PTEB;

typedef struct _EXHANDLE 
{
    union 
    {
        struct 
        {
            //  Application available tag bits
            ULONG TagBits : 2;

            //  The handle table entry index
            ULONG Index : 30;
        } a;

        HANDLE GenericHandleOverlay;
    };
} EXHANDLE, *PEXHANDLE;

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS  ExitStatus;
    PVOID     TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG     AffinityMask;
    ULONG     Priority;
    ULONG     BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID     ExitStatus;
    PPEB      PebBaseAddress;
    PVOID     AffinityMask;
    PVOID     BasePriority;
    ULONG_PTR UniqueProcessId;
    PVOID     InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

#ifndef IMAGE_NT_HEADER
#define IMAGE_NT_HEADER(_base) \
    reinterpret_cast<PIMAGE_NT_HEADERS32>( \
    reinterpret_cast<PCHAR>(_base) + \
    reinterpret_cast<PIMAGE_DOS_HEADER>(_base)->e_lfanew)
#endif


#ifndef THREAD_ALERT
#define THREAD_ALERT          (0x0004)
#endif // THREAD_ALERT

typedef enum _DEBUG_CONTROL_CODE {
    DebugGetTraceInformation = 1,
    DebugSetInternalBreakpoint,
    DebugSetSpecialCall,
    DebugClearSpecialCalls,
    DebugQuerySpecialCalls,
    DebugDbgBreakPoint
} DEBUG_CONTROL_CODE;

typedef struct _DBGKD_QUERY_SPECIAL_CALLS { // DebugQuerySpecialCalls
    DWORD NumberOfSpecialCalls;
    DWORD SpecialCalls[ANYSIZE_ARRAY];
} DBGKD_QUERY_SPECIAL_CALLS, *PDBGKD_QUERY_SPECIAL_CALLS;


#define PAGE_SIZE 0x1000

#define SECTION_ALIGN(Va, Align) ((ULONG)((ULONG_PTR)(Va) + Align - 1) & ~(Align - 1))
#define MAKEPTR(cast, ptr, addValue) (cast)(ULONG_PTR)((ULONG)(ULONG_PTR)(ptr) + (addValue))
#define ROUND_TO_PAGES(Size)  ((ULONG)((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ROUND_UP(Address, To)  \
    (((ULONG_PTR)(Address) + (ULONG_PTR)(To) - 1) & ~((ULONG_PTR)(To) - 1))
#define ROUND_DOWN(Address, To) ((ULONG_PTR)(Address) & ~((ULONG_PTR)(To)-1))


/*typedef enum _THREAD_INFORMATION_CLASS 
{
    ThreadBasicInformation, 
    ThreadTimes, 
    ThreadPriority, 
    ThreadBasePriority, 
    ThreadAffinityMask, 
    ThreadImpersonationToken, 
    ThreadDescriptorTableEntry, 
    ThreadEnableAlignmentFaultFixup, 
    ThreadEventPair, 
    ThreadQuerySetWin32StartAddress, 
    ThreadZeroTlsCell, 
    ThreadPerformanceCount, 
    ThreadAmILastThread, 
    ThreadIdealProcessor, 
    ThreadPriorityBoost, 
    ThreadSetTlsArrayAddress, 
    ThreadIsIoPending, 
    ThreadHideFromDebugger,
} THREAD_INFORMATION_CLASS;*/


typedef struct _RTL_RELATIVE_NAME {
  STRING RelativeName;
  PVOID ContainingDirectory;
} RTL_RELATIVE_NAME, *PRTL_RELATIVE_NAME;

typedef BOOL
(NTAPI *_RtlDosPathNameToNtPathName_U)(
    IN PCWSTR DosPathName,
    OUT PUNICODE_STRING NtPathName,
    OUT PWSTR* FilePathInNtPathName OPTIONAL,
    OUT RTL_RELATIVE_NAME* RelativeName OPTIONAL);

typedef NTSTATUS
(NTAPI *_NtDeleteFile)(
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

typedef NTSTATUS
(NTAPI *_NtSetInformationFile) (
    __in HANDLE FileHandle,
    __out PIO_STATUS_BLOCK IoStatusBlock,
    __in_bcount(Length) PVOID FileInformation,
    __in ULONG Length,
    __in FILE_INFORMATION_CLASS_FULL FileInformationClass
    );


typedef NTSTATUS
(NTAPI *_NtQueryInformationThread)(
  _In_       HANDLE ThreadHandle,
  /*_In_       THREAD_INFORMATION_CLASS ThreadInformationClass,*/
  _In_       int ThreadInformationClass,
  _Inout_    PVOID ThreadInformation,
  _In_       ULONG ThreadInformationLength,
  _Out_opt_  PULONG ReturnLength
);

typedef NTSTATUS
(NTAPI *_NtQueryInformationProcess)(
    IN HANDLE ProcessHandle,
    IN int ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL);
        
typedef NTSTATUS
(NTAPI *_NtReadVirtualMemory)(
    __in HANDLE ProcessHandle,
    __in_opt PVOID BaseAddress,
    __out_bcount(BufferSize) PVOID Buffer,
    __in SIZE_T BufferSize,
    __out_opt PSIZE_T NumberOfBytesRead);

#endif // __NTINTERNAL_H__
