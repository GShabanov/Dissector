/***************************************************************************************\
*   File:                                                                               *
*       Configuration.cpp                                                               *
*                                                                                       *
*   Abstract:                                                                           *
*       configuration for Dissector                                                     *
*                                                                                       *
*   Author:                                                                             *
*       Gennady Shabanov () 15-Oct-2007                                                 *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"
#include "configuration.h"

BOOL
CConfiguration::GetConfig()
{

    this->m_hwndRect.left = 0;
    this->m_hwndRect.top  = 0;
    this->m_hwndRect.bottom  = 0;
    this->m_hwndRect.right   = 0;

    HKEY    hkProgramKey;
    if (OpenProgramKey(&hkProgramKey, KEY_QUERY_VALUE) == TRUE)
    {
        DWORD   dwValueType;
        DWORD   dwDataSize;
        DWORD   dwData;

        dwDataSize = sizeof(DWORD);

        if (RegQueryValueEx(
            hkProgramKey, _T("WndLeft"), NULL, &dwValueType, (PBYTE)&dwData, &dwDataSize) == ERROR_SUCCESS && dwValueType == REG_DWORD)
        {
            this->m_hwndRect.left = dwData;
        }

        dwDataSize = sizeof(DWORD);

        if (RegQueryValueEx(
            hkProgramKey, _T("WndTop"), NULL, &dwValueType, (PBYTE)&dwData, &dwDataSize) == ERROR_SUCCESS && dwValueType == REG_DWORD)
        {
            this->m_hwndRect.top = dwData;
        }

        dwDataSize = sizeof(DWORD);

        if (RegQueryValueEx(
            hkProgramKey, _T("WndRight"), NULL, &dwValueType, (PBYTE)&dwData, &dwDataSize) == ERROR_SUCCESS && dwValueType == REG_DWORD)
        {
            this->m_hwndRect.right = dwData;
        }

        dwDataSize = sizeof(DWORD);

        if (RegQueryValueEx(
            hkProgramKey, _T("WndBottom"), NULL, &dwValueType, (PBYTE)&dwData, &dwDataSize) == ERROR_SUCCESS && dwValueType == REG_DWORD)
        {
            this->m_hwndRect.bottom = dwData;
        }

        dwDataSize = sizeof(DWORD);

        if (RegQueryValueEx(
            hkProgramKey, _T("Selected"), NULL, &dwValueType, (PBYTE)&dwData, &dwDataSize) == ERROR_SUCCESS && dwValueType == REG_DWORD)
        {
            this->m_SelectedTab = dwData;
        }

        dwDataSize = sizeof(DWORD);

        if (RegQueryValueEx(
            hkProgramKey, _T("permanent"), NULL, &dwValueType, (PBYTE)&dwData, &dwDataSize) == ERROR_SUCCESS && dwValueType == REG_DWORD)
        {
            this->m_makeChangeOnDisk = dwData;
        }

        

        RegCloseKey(hkProgramKey);
    }

    return TRUE;
}

BOOL
CConfiguration::SaveConfig()
{
    HKEY    hkProgramKey;

    if (OpenProgramKey(&hkProgramKey, KEY_SET_VALUE | KEY_CREATE_SUB_KEY) == TRUE)
    {
        DWORD   dwData = (DWORD)this->m_hwndRect.left;

        RegSetValueEx(
            hkProgramKey, _T("WndLeft"), NULL, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));

        dwData = (DWORD)this->m_hwndRect.top;

        RegSetValueEx(
            hkProgramKey, _T("WndTop"), NULL, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));

        dwData = (DWORD)this->m_hwndRect.right;

        RegSetValueEx(
            hkProgramKey, _T("WndRight"), NULL, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));

        dwData = (DWORD)this->m_hwndRect.bottom;

        RegSetValueEx(
            hkProgramKey, _T("WndBottom"), NULL, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));

        dwData = (DWORD)this->m_SelectedTab;

        RegSetValueEx(
            hkProgramKey, _T("Selected"), NULL, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));

        dwData = (DWORD)this->m_makeChangeOnDisk;

        RegSetValueEx(
            hkProgramKey, _T("permanent"), NULL, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));


        RegCloseKey(hkProgramKey);
    }

    return TRUE;
}

BOOL
CConfiguration::OpenProgramKey(HKEY *hkOutKey, ACCESS_MASK DesiredAccess)
{
    HKEY    hkCurrentUser;
    LSTATUS status = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Hex-Rays\\IDA"), NULL, DesiredAccess, &hkCurrentUser);

    if (status != ERROR_SUCCESS)
        return FALSE;


    HKEY  hkPlugins = NULL;
    status =
        RegCreateKey(
        hkCurrentUser,
        _T("Plugins"),
        &hkPlugins);
        
    RegCloseKey(hkCurrentUser);

    if (status != ERROR_SUCCESS)
    {
        RegCloseKey(hkCurrentUser);
        return FALSE;
    }


    HKEY  hkDumper = NULL;

    status = RegCreateKey(
        hkPlugins,
        _T("MemoryDumper"),
        &hkDumper);

    RegCloseKey(hkPlugins);

    if (status != ERROR_SUCCESS)
    {
        return FALSE;
    }


    *hkOutKey = hkDumper;

    return TRUE;
}


void __fastcall
CConfiguration::tc2mb(TCHAR *szInput, char *szOutput, size_t max_size)
{
    while (*szInput && max_size) {

        *szOutput++ = (char)*szInput++;
        max_size--;
    }

    *szOutput = 0;
}

TCHAR *
CConfiguration::SaveString(char *szString)
{

    SIZE_T paramLength = strlen(szString) + 1;

    TCHAR *tszReturn = reinterpret_cast<TCHAR *>( 
        new UCHAR[paramLength * sizeof(TCHAR)]);

    if (tszReturn == NULL)
        return NULL;

    TCHAR *tcChar = tszReturn;
    char *cConfigChar = szString;

    do 
    {
        *tcChar = (TCHAR)*cConfigChar;

        cConfigChar++;
        tcChar++;
    }
    while ((SIZE_T)(cConfigChar - szString) < paramLength);

    return tszReturn;
}
