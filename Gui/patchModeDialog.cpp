/***************************************************************************************\
*   File:                                                                               *
*       patchModeDialog.cpp                                                             *
*                                                                                       *
*   Abstract:                                                                           *
*       This module allows the user to select the necessary parameters for              *
*       the patch file.                                                                 *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"

#include "patchModeDialog.h"

BOOL
CPatchModeDialog::OnInitDialog(
    HWND    hwnd,
    HWND    hWndFocus,
    LPARAM  lParam)
/*++

    Function:
        OnInitDialog

    Pusrpouse:

        The initialization function for a dialog of this type. 
        Called on first initialization.

    Arguments:

        hWnd        - [in] handle to window

        hWndFocus   - [in] the window is currently owning user focus

        lParam      - [in] lParam

    Returns:
        BOOL

--*/
{
    UNREFERENCED_PARAMETER(hWndFocus);
    UNREFERENCED_PARAMETER(lParam);
    m_hwndFromVA = GetDlgItem(hwnd, IDC_FROM_VA);
    m_hwndToVA = GetDlgItem(hwnd, IDC_TO_VA);
    m_hwndSize = GetDlgItem(hwnd, IDC_TO_SIZE);
    m_hwndMakeChangesOnDisk = GetDlgItem(hwnd, IDC_MAKE_PERMANENT);


    BOOL doUpdate = TRUE;
    if (m_parameters->queryPatchParameters != NULL)
    {
        doUpdate = m_parameters->queryPatchParameters(m_parameters);
    }

    TCHAR    vaString[MAX_TEMP_STRING];

    if (doUpdate)
    {

        LONG_PTR deltaSize = 0;
        PATCH_PARAMETERS *pPatching = this->m_parameters->pPatchParameters;

        if (pPatching->pToVA > pPatching->pFromVA)
        {
            deltaSize = pPatching->pToVA - pPatching->pFromVA;
        }
        else
        {
            pPatching->pFromVA = pPatching->pToVA;
        }

        if (pPatching->bMakePermanentChanges == TRUE)
        {
            Button_SetCheck(m_hwndMakeChangesOnDisk, BST_CHECKED);
        }
        else
        {
            Button_SetCheck(m_hwndMakeChangesOnDisk, BST_UNCHECKED);
        }

#ifdef __X64__

        _stprintf(vaString, _T("0x%016I64X"), pPatching->pFromVA);
        SetWindowText(m_hwndFromVA, vaString);

        _stprintf(vaString, _T("0x%016I64X"), pPatching->pToVA);
        SetWindowText(m_hwndToVA, vaString);


        _stprintf(vaString, _T("0x%08I32X"), (ULONG)deltaSize);
        SetWindowText(m_hwndSize, vaString);
#else // __X64__
        _stprintf(vaString, _T("0x%08I32X"), pPatching->pFromVA);
        SetWindowText(m_hwndFromVA, vaString);

        _stprintf(vaString, _T("0x%08I32X"), pPatching->pToVA);
        SetWindowText(m_hwndToVA, vaString);

        _stprintf(vaString, _T("0x%08I32X"), deltaSize);
        SetWindowText(m_hwndSize, vaString);

#endif // __X64__*/
    }
    else
    {
        _tcscpy(vaString, _T("no dump"));
        SetWindowText(m_hwndSize, vaString);
        SetWindowText(m_hwndToVA, vaString);
        SetWindowText(m_hwndFromVA, vaString);
    }

    return TRUE;
}

CPatchModeDialog::~CPatchModeDialog()
/*++

    Function:
        ~CPatchModeDialog

    Pusrpouse:

        Class destructor. Additionaly saves users parameters

    Arguments:

    Returns:
        

--*/
{
    TCHAR    vaString[MAX_TEMP_STRING];
    PATCH_PARAMETERS *pPatching = this->m_parameters->pPatchParameters;

    LONG_PTR  fromVa;
    LONG_PTR  toVa;
#ifdef __X64__

    GetWindowText(m_hwndFromVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%I64X", &fromVa);

    GetWindowText(m_hwndToVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%I64X", &toVa);

#else // __X64__
    GetWindowText(m_hwndFromVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%X", &fromVa);

    GetWindowText(m_hwndToVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%X", &toVa);


#endif // __X64__*/

    pPatching->pFromVA = fromVa;
    pPatching->pToVA = toVa;

    LRESULT result = Button_GetCheck(m_hwndMakeChangesOnDisk);
    
    configuration->m_makeChangeOnDisk = (BST_CHECKED == result) ? TRUE : FALSE;
    pPatching->bMakePermanentChanges = (BST_CHECKED == result) ? TRUE : FALSE;
    
    //m_parameters->dialogResult = FALSE;
}

INT_PTR
CPatchModeDialog::DialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, OnInitDialog);

        default:;
    }

    return FALSE;
}
