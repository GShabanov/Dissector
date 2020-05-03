/***************************************************************************************\
*   File:                                                                               *
*       dumpModeDialog.cpp                                                              *
*                                                                                       *
*   Abstract:                                                                           *
*       Dialog for Dissector in dump mode                                               *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"

#include "dumpModeDialog.h"
#include "SelectVaDialog.h"


BOOL
CDumpModeDialog::OnInitDialog(
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
    m_hwndVaSize = GetDlgItem(hwnd, IDC_TO_SIZE);
    m_hwndCurrentVA = GetDlgItem(hwnd, IDC_CURRENT_VA);

    m_hwndSelectRegion = GetDlgItem(hwnd, IDC_SELECT_REGION);


    BOOL doUpdate = TRUE;
    if (m_parameters->queryDumpParameters != NULL)
    {
        doUpdate = m_parameters->queryDumpParameters(m_parameters);
    }

    if (doUpdate)
    {
        DUMP_PARAMETERS *pDump = m_parameters->pDumpParameters;

        ParamsToDialog(pDump);

        TCHAR    vaString[MAX_TEMP_STRING];

#ifdef __X64__
        _stprintf(vaString, _T("0x%016I64X"), m_parameters->choosenVA);
        SetWindowText(m_hwndCurrentVA, vaString);
#else // __X64__
        _stprintf(vaString, _T("0x%08I32X"), m_parameters->choosenVA);
        SetWindowText(m_hwndCurrentVA, vaString);
#endif // __X64__
    }

    return TRUE;
}

CDumpModeDialog::~CDumpModeDialog()
/*++

    Function:
        ~CDumpModeDialog

    Pusrpouse:

        Class destructor. Additionaly saves users parameters

    Arguments:

    Returns:
        

--*/
{
    DUMP_PARAMETERS  newParameters;

    BOOL _result = FALSE;

    if (DialogToParams(&newParameters) == TRUE)
    {
        *m_parameters->pDumpParameters = newParameters;

        _result = TRUE;
    }

    m_parameters->dialogResult = _result;
}

BOOL
CDumpModeDialog::OnCommand(
    HWND hWnd,
    int  id,
    HWND hwndCtl,
    UINT codeNotify)
/*++

    Function:
        OnCommand

    Pusrpouse:

        Command dispatching for current window

    Arguments:

        hWnd        - [in] handle to window

        id          - [in] Control identifier

        hwndCtl     - [in] handle to control

        codeNotify  - [in] notify code

    Returns:
        BOOL

--*/
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(id);
    UNREFERENCED_PARAMETER(codeNotify);

    //
    // select region dispatch 
    //

    if (hwndCtl == m_hwndSelectRegion)
    {
        BOOL    dialogResult = FALSE;

        //
        // create select virtual address modal dialog
        // In case the user wishes to open the list of current regions. We create a modal 
        // window in relation to this and get the information received.
        //

        CSelectVaDialog  dlg(this->m_hInstance, m_parameters, this->m_hwnd);

        if (dlg.InitInstance() == TRUE)
        {
            dialogResult = dlg.Run();
        }


        if (dialogResult == TRUE)
        {
            MEMORY_ENUM_ENTRY entry = dlg.GetSelectedRegion();

            DUMP_PARAMETERS *pDump = m_parameters->pDumpParameters;

            m_parameters->choosenVA = entry.regionStart;

            pDump->pFromVA = entry.regionStart;
            pDump->pToVA = entry.regionStart + entry.regionSize;

            ParamsToDialog(pDump);

        }

        return TRUE;
    }

    //
    // The main logic of updating the relationships between the input windows.
    // If necessary, you can change the update order and the relationship between the values.
    //

    if ((hwndCtl == m_hwndToVA   ||
         hwndCtl == m_hwndFromVA ||
         hwndCtl == m_hwndVaSize)
         &&
        (codeNotify == EN_CHANGE) &&
         GetFocus() == hwndCtl)
    {

        if (hwndCtl == m_hwndVaSize ||
            hwndCtl == m_hwndFromVA)
        {
            //
            // reupdate To VA
            //
            TCHAR    vaString[MAX_TEMP_STRING];

            LONG_PTR  vaSize = 0;
            LONG_PTR  toFromVa = 0;

            GetWindowText(m_hwndFromVA, vaString, MAX_TEMP_STRING);

            _stscanf(vaString, "0x%I64X", &toFromVa);

            GetWindowText(m_hwndVaSize, vaString, MAX_TEMP_STRING);

            _stscanf(vaString, "0x%X", &vaSize);


            _stprintf(vaString, _T("0x%016I64X"), toFromVa + vaSize);
            SetWindowText(m_hwndToVA, vaString);
        }

        if (hwndCtl == m_hwndToVA)
        {
            //
            // reupdate To VA
            //
            TCHAR    vaString[MAX_TEMP_STRING];
            LONG_PTR  fromVa = 0;
            LONG_PTR  toVa = 0;

            GetWindowText(m_hwndFromVA, vaString, MAX_TEMP_STRING);

            _stscanf(vaString, "0x%016I64X", &fromVa);

            GetWindowText(m_hwndToVA, vaString, MAX_TEMP_STRING);

            _stscanf(vaString, "0x%016I64X", &toVa);

            if (toVa >= fromVa && 
                (toVa - fromVa) < 0xFFFFFFFF) {
                _stprintf(vaString, _T("0x%08I32X"), (ULONG)(toVa - fromVa));
            } else {
                _tcscpy(vaString, _T("err"));
            }

            SetWindowText(m_hwndVaSize, vaString);

        }

    }


    return FALSE;
}


INT_PTR
CDumpModeDialog::DialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam)
/*++

    Function:
        DialogProc

    Pusrpouse:

        dispatching dialog procedure

    Arguments:

    Returns:
        void

--*/
{
    switch (uMsg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_COMMAND, OnCommand);

        default:;
    }

    return FALSE;
}

VOID
CDumpModeDialog::ParamsToDialog(const DUMP_PARAMETERS *pDump)
/*++

    Function:
        ParamsToDialog

    Pusrpouse:

        Parameters to dialog data convertation

    Arguments:

        pDump       - [in] Dump parameters

    Returns:
        void

--*/
{
    TCHAR    vaString[MAX_TEMP_STRING];


#ifdef __X64__

    _stprintf(vaString, _T("0x%016I64X"), pDump->pFromVA);
    SetWindowText(m_hwndFromVA, vaString);

    _stprintf(vaString, _T("0x%016I64X"), pDump->pToVA);
    SetWindowText(m_hwndToVA, vaString);

    if (pDump->pToVA >= pDump->pFromVA && 
        (pDump->pToVA - pDump->pFromVA) < 0xFFFFFFFF) {

        _stprintf(vaString, _T("0x%08I32X"), (ULONG)(pDump->pToVA - pDump->pFromVA));

    } else {

        _tcscpy(vaString, _T("err"));
    }

    SetWindowText(m_hwndVaSize, vaString);
#else // __X64__

    _stprintf(vaString, _T("0x%08I32X"), pDump->pFromVA);
    SetWindowText(m_hwndFromVA, vaString);

    _stprintf(vaString, _T("0x%08I32X"), pDump->pToVA);
    SetWindowText(m_hwndToVA, vaString);

    if (pDump->pToVA >= pDump->pFromVA) {

        _stprintf(vaString, _T("0x%08I32X"), pDump->pToVA - pDump->pFromVA);
    } else {

        _tcscpy(vaString, _T("err"));
    }

    SetWindowText(m_hwndVaSize, vaString);
#endif // __X64__

}

BOOL
CDumpModeDialog::DialogToParams(DUMP_PARAMETERS *pDump)
/*++

    Function:
        DialogToParams

    Pusrpouse:

        dialog data to parameters convertation

    Arguments:

        pDump       - [in] Dump parameters

    Returns:
        BOOL

--*/
{
    TCHAR    vaString[MAX_TEMP_STRING];

    LONG_PTR  fromVa = 0;
    LONG_PTR  toVa = 0;

#ifdef __X64__

    //currentVa = _ttoi64(vaString);

    GetWindowText(m_hwndFromVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%I64X", &fromVa);
    //fromVa = _ttoi64(vaString);

    GetWindowText(m_hwndToVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%I64X", &toVa);
    //toVa = _ttoi64(vaString);
#else // __X64__

    GetWindowText(m_hwndFromVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%X", &fromVa);

    GetWindowText(m_hwndToVA, vaString, MAX_TEMP_STRING);

    _stscanf(vaString, "0x%X", &toVa);
#endif // __X64__

    if (fromVa >= toVa)
    {
        return FALSE;
    }


    pDump->pFromVA = fromVa;
    pDump->pToVA = toVa;

    return TRUE;
}
