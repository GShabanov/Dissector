/***************************************************************************************\
*   File:                                                                              *
*       PeHelper.cpp                                                                   *
*                                                                                      *
*   Abstract:                                                                          *
*       PE format helper functions                                                     *
*                                                                                      *
*   Author:                                                                            *
*       G Shabanov          26-Aug-2007                                                *
*                                                                                      *
*   Revision History:                                                                  *
/***************************************************************************************/
// $Id: $
#include "stdafx.h"
#include "PeHelper.h"


PIMAGE_SECTION_HEADER
GetFileSectionFromAddress(IN PIMAGE_DOS_HEADER DosHeader, PVOID Address)
/*++
    Function:
        GetFileSectionFromAddress

    Pusrpouse:

        Check input data block for PE format and
        returns section consists this address

    Arguments:

        DosHeader  - [in] entry to PE file

        Address    - [in] some address

    Returns:
        Image section or null

--*/
{

    __try
    {

        if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return NULL;
        }

        PIMAGE_NT_HEADERS pNTHeader = MAKEPTR(PIMAGE_NT_HEADERS, DosHeader, DosHeader->e_lfanew);

        if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        {
            return NULL;
        }

        register PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION(pNTHeader);

        WORD    NumberOfSections     = pNTHeader->FileHeader.NumberOfSections;
        DWORD   ImageBase            = (DWORD)pNTHeader->OptionalHeader.ImageBase;

        //
        // check for address entry in the region of the section
        //

        for (auto section = 0; section < NumberOfSections; section++)
        {

            LONG_PTR SectionStartAddress = SectionHeader->VirtualAddress + ImageBase;
            LONG_PTR SectionEndAddress   = SectionStartAddress + SectionHeader->Misc.VirtualSize;

            if (SectionStartAddress <= (LONG_PTR)Address && 
                SectionEndAddress > (LONG_PTR)Address)
            {
                return SectionHeader;
            }

            SectionHeader++;

        }

    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return NULL;
}

BOOL
RelativeAddressToSection32(
    const IN DATA_READ_FUNCTION  dataRead,
    const PVOID                  context,
    const IN PIMAGE_NT_HEADERS32 ntHeader,
    const PVOID                  ntHeaderPtr,
    const ULONG_PTR              RelativeAddress,
    IMAGE_SECTION_HEADER        &section
    )
/*++
    Function:
        RelativeAddressToSection32

    Pusrpouse:

        Retrive section header for relative address for 32 bit module

    Arguments:

        dataRead        - [in] data read function

        context         - [in] data read function context

        ntHeader        - [in] nt header for module

        ntHeaderPtr     - [in] pointer for this headers

        RelativeAddress - [in] relative address we search for

        section         - [out] captured section header


    Returns:
        result

--*/
{

    if (!ARGUMENT_PRESENT(ntHeader)) {

        return FALSE;
    }

    ULONG  numberOfSections = (ULONG)ntHeader->FileHeader.NumberOfSections;

    if (numberOfSections == 0)
        return FALSE;

    ULONG currentSectionAddress = (ULONG)((UCHAR *)ntHeaderPtr + 
        FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader) + ntHeader->FileHeader.SizeOfOptionalHeader);

    //
    // check for address entry in the region of the section
    //

    for (ULONG i = 0; i < numberOfSections; i++)
    {
        IMAGE_SECTION_HEADER  Section;

        //
        // read the section header
        //

        if (!dataRead(context, currentSectionAddress, &Section, sizeof(IMAGE_SECTION_HEADER))) {
            return FALSE;
        }

        SIZE_T sectionDataSize = 0;

        //
        // There are cases when the size of the date section is determined only by the raw size.
        // In this case, we determine what size should be.
        //

        if (Section.Misc.VirtualSize > 0 && Section.Misc.VirtualSize > Section.SizeOfRawData)
        {
            sectionDataSize = Section.Misc.VirtualSize;
        }
        else
        {
            sectionDataSize = Section.SizeOfRawData;
        }

        //
        // check in range
        //

        if (RelativeAddress >= Section.VirtualAddress &&
            RelativeAddress < Section.VirtualAddress + sectionDataSize)
        {
            section = Section;
            return TRUE;
        }
        
        currentSectionAddress += sizeof(IMAGE_SECTION_HEADER);
    }

    return NULL;
}


BOOL
RelativeAddressToSection64(
    const IN DATA_READ_FUNCTION     dataRead,
    const PVOID                     context,
    const IN PIMAGE_NT_HEADERS64    ntHeader,
    const IN PVOID                  ntHeaderPtr,
    const IN ULONG_PTR              RelativeAddress,
    OUT IMAGE_SECTION_HEADER        &section
    )
/*++
    Function:
        RelativeAddressToSection64

    Pusrpouse:

        Retrive section header for relative address for 64 bit module

    Arguments:

        dataRead        - [in] data read function

        context         - [in] data read function context

        ntHeader        - [in] nt header for module

        ntHeaderPtr     - [in] pointer for this headers

        RelativeAddress - [in] relative address we search for

        section         - [out] captured section header


    Returns:
        result

--*/
{

    if (!ARGUMENT_PRESENT(ntHeaderPtr)) {
        return FALSE;
    }


    ULONG  numberOfSections = (ULONG)ntHeader->FileHeader.NumberOfSections;

    if (numberOfSections == 0)
        return FALSE;

    ULONG64 currentSectionAddress = (ULONG64)((UCHAR *)ntHeaderPtr + 
        FIELD_OFFSET(IMAGE_NT_HEADERS64, OptionalHeader) + ntHeader->FileHeader.SizeOfOptionalHeader);

    //
    // check for address entry in the region of the section
    //

    for (ULONG i = 0; i < numberOfSections; i++)
    {
        IMAGE_SECTION_HEADER  Section;

        //
        // read the section header
        //

        if (!dataRead(context, currentSectionAddress, &Section, sizeof(IMAGE_SECTION_HEADER))) {
            return FALSE;
        }

        SIZE_T sectionDataSize = 0;

        //
        // There are cases when the size of the date section is determined only by the raw size.
        // In this case, we determine what size should be.
        //

        if (Section.Misc.VirtualSize > 0 && Section.Misc.VirtualSize >= Section.SizeOfRawData)
        {
            sectionDataSize = Section.Misc.VirtualSize;
        }
        else
        {
            sectionDataSize = Section.SizeOfRawData;
        }

        if (RelativeAddress >= Section.VirtualAddress &&
            RelativeAddress < Section.VirtualAddress + sectionDataSize)
        {
            section = Section;
            return TRUE;
        }
        
        currentSectionAddress += sizeof(IMAGE_SECTION_HEADER);
    }

    return FALSE;
}


BOOL
GetPhysicalOffsetFromRelativeAddress32(
    const IN DATA_READ_FUNCTION dataRead,
    const PVOID                 context,
    const ULONG                 baseAddress,
    IN ULONG relativeAddress,
    OUT LONG_PTR *offset
    )
/*++

    Function:
        GetPhysicalOffsetFromRelativeAddress32

    Pusrpouse:

        Retrive physical offset for relative address in 32 bit module

    Arguments:

        dataRead        - [in] data read function

        context         - [in] data read function context

        baseAddress     - [in] base address to read

        relativeAddress - [in] relative address

        offset          - [out] raw offset


    Returns:
        result

--*/
{
    UCHAR  buffer[sizeof(IMAGE_NT_HEADERS32)];

    if (!dataRead(context, baseAddress, buffer, sizeof(IMAGE_DOS_HEADER))) {
        return FALSE;
    }

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)buffer;


    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return FALSE;
    }

    ULONG imageNtHeadersPtr = (ULONG)baseAddress + dosHeader->e_lfanew;

    if (!dataRead(context, imageNtHeadersPtr, buffer, sizeof(IMAGE_NT_HEADERS32))) {
        return FALSE;
    }

    IMAGE_NT_HEADERS32 *ntHeader = (IMAGE_NT_HEADERS32 *)buffer;

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
        return FALSE;
    }

    IMAGE_SECTION_HEADER Section;

    //
    // try to find suitable section
    //

    if (RelativeAddressToSection32(dataRead, context, ntHeader, (const PVOID)imageNtHeadersPtr, relativeAddress, Section) != TRUE)
    {
        return FALSE;
    }

    //
    // calculate raw offset
    //

    *offset = relativeAddress - Section.VirtualAddress + Section.PointerToRawData;

    return TRUE;
}

BOOL
GetPhysicalOffsetFromRelativeAddress64(
    const IN DATA_READ_FUNCTION dataRead,
    const PVOID context,
    const IN ULONG64 baseAddress,
    const IN ULONG relativeAddress,
    OUT LONG_PTR *offset
    )
/*++

    Function:
        GetPhysicalOffsetFromRelativeAddress64

    Pusrpouse:

        Retrive physical offset for relative address in 64 bit module

    Arguments:

        dataRead        - [in] data read function

        context         - [in] data read function context

        baseAddress     - [in] base address to read

        relativeAddress - [in] relative address

        offset          - [out] raw offset


    Returns:
        result

--*/
{
    UCHAR  buffer[sizeof(IMAGE_NT_HEADERS64)];

    if (!dataRead(context, baseAddress, buffer, sizeof(IMAGE_DOS_HEADER))) {
        return FALSE;
    }

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)buffer;

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return FALSE;
    }

    ULONG64 imageNtHeadersPtr = (ULONG64)baseAddress + dosHeader->e_lfanew;

    if (!dataRead(context, imageNtHeadersPtr, buffer, sizeof(IMAGE_NT_HEADERS64))) {
        return FALSE;
    }

    IMAGE_NT_HEADERS64 *ntHeader = (IMAGE_NT_HEADERS64 *)buffer;

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
        return FALSE;
    }

    if (ntHeader->FileHeader.NumberOfSections == 0)
        return FALSE;


    IMAGE_SECTION_HEADER Section;

    //
    // try to find suitable section
    //

    if (RelativeAddressToSection64(
        dataRead, context,
        ntHeader, (const PVOID)imageNtHeadersPtr, relativeAddress, Section) != TRUE)
    {
        return FALSE;
    }

    //
    // calculate raw offset
    //

    *offset = relativeAddress - Section.VirtualAddress + Section.PointerToRawData;

    return TRUE;
}

BOOL
GetPhysicalOffsetFromRelativeAddress(
    const IN DATA_READ_FUNCTION dataRead,
    const PVOID                 context,
    const IN ULONG64            baseAddress,
    const IN ULONG              relativeAddress,
    OUT LONG_PTR               *offset
    )
/*++
    Function:
        GetPhysicalOffsetFromVirtualAddress

    Pusrpouse:

        The function gets the physical address-offset inside the file at the 
        specified virtual address.

    Arguments:

        dataRead          - [in] data read function

        context           - [in] data read function context

        baseAddress       - [in] base address of image

        relativeAddress   - [in] relative address 

        offset            - [out] raw offset

    Returns:
        BOOL

--*/
{
    UCHAR  buffer[sizeof(IMAGE_NT_HEADERS64)];

    if (!dataRead(context, baseAddress, buffer, sizeof(IMAGE_DOS_HEADER))) {
        return FALSE;
    }

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)buffer;

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return FALSE;
    }

    if (!dataRead(
        context,
        baseAddress + dosHeader->e_lfanew,
        buffer,
        sizeof(IMAGE_NT_HEADERS))
        )
    {
        return FALSE;
    }

    IMAGE_NT_HEADERS *ntHeader = (IMAGE_NT_HEADERS *)buffer;

    if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
        return FALSE;
    }

    //
    // depending on the section header call appropriate routine
    //

    if (ntHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
    {
        return GetPhysicalOffsetFromRelativeAddress32(dataRead, context, (const ULONG)baseAddress, relativeAddress, offset);
    }
    else if (ntHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
    {
        return GetPhysicalOffsetFromRelativeAddress64(dataRead, context, (const ULONG64)baseAddress, relativeAddress, offset);
    }
    else
    {
        return FALSE;
    }


}

BOOL
CopyFile(HANDLE hInputFile, HANDLE hOutputFile)
/*++
    Function:
        CopyFile

    Pusrpouse:

        Copy binary file from source to destination

    Arguments:

        hInputFile  - [in] handle to opened input file

        hOutputFile - [in] handle to opened output file

    Returns:
        result

--*/
{

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);

    SIZE_T allocSize;
    DWORD dwSizeLow;
    DWORD dwSizeHigh;

    dwSizeLow = GetFileSize(hInputFile, &dwSizeHigh);

    //
    // allocate the data buffer
    //

    if (dwSizeHigh > 0) {

        allocSize = SystemInfo.dwAllocationGranularity * 4;
    }
    else
    {
        if (dwSizeLow & SystemInfo.dwAllocationGranularity)
        {
            allocSize = SystemInfo.dwAllocationGranularity * 4;
        }
        else
        {
            allocSize = SystemInfo.dwAllocationGranularity;
        }
    }

    LPVOID FileData = 
        VirtualAlloc(NULL, allocSize, MEM_COMMIT, PAGE_READWRITE);

    if (FileData != NULL)
    {
        return FALSE;
    }

    DWORD dwReaded = NULL;

    //
    // file copy loop
    //

    while (ReadFile(
        hInputFile,
        FileData,
        (DWORD)allocSize,
        &dwReaded, NULL) == TRUE && dwReaded > NULL)
    {
        WriteFile(hOutputFile, FileData, dwReaded, &dwReaded, NULL);
    }

    FlushFileBuffers(hOutputFile);

    VirtualFree(FileData, 0, MEM_RELEASE);

    return TRUE;

}

