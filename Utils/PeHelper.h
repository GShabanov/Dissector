/***************************************************************************************\
*   File:                                                                              *
*       PeHelper.h                                                                     *
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
#pragma once
#ifndef __PEHELPER_H__
#define __PEHELPER_H__

///
/// copy file by specified handles
///
extern BOOL
CopyFile(HANDLE hInputFile, HANDLE hOutputFile);

///
/// retrive file section header for specified address
///
extern PIMAGE_SECTION_HEADER
GetFileSectionFromAddress(IN PIMAGE_DOS_HEADER DosHeader, PVOID Address);


///
/// callback function for reading data
///
typedef BOOL (* DATA_READ_FUNCTION)(PVOID context, ULONG64 address, PVOID buffer, SIZE_T size);

///
/// get physical offset for relative address
///
extern BOOL
GetPhysicalOffsetFromRelativeAddress(
    const DATA_READ_FUNCTION memoryRead,
    const PVOID context,
    const IN ULONG64 baseAddress,
    const IN ULONG relativeAddress,
    OUT LONG_PTR *offset);


#endif // __PEHELPER_H__