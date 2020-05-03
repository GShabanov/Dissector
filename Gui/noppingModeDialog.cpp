/***************************************************************************************\
*   File:                                                                               *
*       noppingModeDialog.cpp                                                           *
*                                                                                       *
*   Abstract:                                                                           *
*       This dialog is used if the user has selected a region and wants to              *
*        fill it with empty operations.                                                 *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"

#include "noppingModeDialog.h"

//==============================================================
// Unfortunately, this code section has not yet been completed.
//==============================================================

BOOL
CNoppingModeDialog::OnInitDialog(
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

    //TCHAR    vaString[MAX_TEMP_STRING];

#if 0
#ifdef __X64__

    _stprintf(vaString, _T("0x%016I64X"), this->m_pParameters->pCurrentVA);
    SetWindowText(m_hwndCurrentVA, vaString);

    _stprintf(vaString, _T("0x%016I64X"), this->m_pParameters->pFromVA);
    SetWindowText(m_hwndFromVA, vaString);

    _stprintf(vaString, _T("0x%016I64X"), this->m_pParameters->pToVA);
    SetWindowText(m_hwndToVA, vaString);
#else // __X64__
    _stprintf(vaString, _T("0x%08I32X"), this->m_pParameters->pCurrentVA);
    SetWindowText(m_hwndCurrentVA, vaString);

    _stprintf(vaString, _T("0x%08I32X"), this->m_pParameters->pFromVA);
    SetWindowText(m_hwndFromVA, vaString);

    _stprintf(vaString, _T("0x%08I32X"), this->m_pParameters->pToVA);
    SetWindowText(m_hwndToVA, vaString);

#endif // __X64__*/
#endif

    return TRUE;
}

BOOL
CNoppingModeDialog::OnDestroy(HWND)
{
    return TRUE;
}

INT_PTR
CNoppingModeDialog::DialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_DESTROY, OnDestroy);

        default:;
    }

    return FALSE;
}
