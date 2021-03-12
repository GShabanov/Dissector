/***************************************************************************************\
*   File:                                                                               *
*       mainDialog.cpp                                                                  *
*                                                                                       *
*   Abstract:                                                                           *
*       The main application dialog box. It contains other dialogues responsible        *
*        for the work.                                                                  *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"

#include "MainDialog.h"
#include "dumpModeDialog.h"
#include "patchModeDialog.h"
#include "noppingModeDialog.h"


CMainDialog::CMainDialog(HINSTANCE hInstance, GUI_PARAMETERS *parameters, HWND parentWnd)
    : CDialogBase(hInstance, parentWnd, FALSE, MAKEINTRESOURCE(IDD_DISSECTOR))
    , m_pTypeofWork(NULL)
    , m_hwndOkExit(NULL)
    , m_hwndFileName(NULL)
    , m_hwndRadioDump(NULL)
    , m_hwndRadioFilling(NULL)
    , m_hwndRadioNopping(NULL)
{
    m_pParameters = parameters;
}

BOOL
CMainDialog::InitTypeOfWork(HWND hwndParent)
/*++

    Function:
        InitTypeOfWork

    Pusrpouse:

        The function that is responsible for choosing the current type of work.

    Arguments:

        hwndParent    - [in] parent window

    Returns:
        BOOL

--*/
{
    if (m_pTypeofWork != NULL)
    {
        delete m_pTypeofWork;
        m_pTypeofWork = NULL;
    }

    //
    // DUMP mode selection
    //
    if (CConfiguration::Get()->m_SelectedTab == Dumping)
    {

        m_pTypeofWork = new CDumpModeDialog(
            this->m_hInstance, hwndParent, 
            m_pParameters);

        if (m_pTypeofWork == NULL)
        {
            return FALSE;
        }

        if (m_pTypeofWork->InitInstance() != TRUE)
        {
            delete m_pTypeofWork;
            m_pTypeofWork = 0;
            return FALSE;
        }

        m_pParameters->workType = Dumping;

        Button_SetCheck(m_hwndRadioDump, 1);
        Button_SetCheck(m_hwndRadioFilling, 0);
        Button_SetCheck(m_hwndRadioNopping, 0);

        return TRUE;
    }

    //
    // PATCH mode selection
    //
    if (CConfiguration::Get()->m_SelectedTab == Patching)
    {

        m_pTypeofWork = new CPatchModeDialog(
            this->m_hInstance, hwndParent, 
            m_pParameters);

        if (m_pTypeofWork == NULL)
        {
            return FALSE;
        }

        if (m_pTypeofWork->InitInstance() != TRUE)
        {
            delete m_pTypeofWork;
            m_pTypeofWork = 0;
            return FALSE;
        }

        m_pParameters->workType = Patching;

        Button_SetCheck(m_hwndRadioDump, 0);
        Button_SetCheck(m_hwndRadioFilling, 1);
        Button_SetCheck(m_hwndRadioNopping, 0);

        return TRUE;
    }

    //
    // NOP mode selection
    //
    if (CConfiguration::Get()->m_SelectedTab == Nopping)
    {

        m_pTypeofWork = new CNoppingModeDialog(
            this->m_hInstance, hwndParent, 
            m_pParameters);

        if (m_pTypeofWork == NULL)
        {
            return FALSE;
        }

        if (m_pTypeofWork->InitInstance() != TRUE)
        {
            delete m_pTypeofWork;
            m_pTypeofWork = 0;
            return FALSE;
        }

        m_pParameters->workType = Nopping;

        Button_SetCheck(m_hwndRadioDump, 0);
        Button_SetCheck(m_hwndRadioFilling, 0);
        Button_SetCheck(m_hwndRadioNopping, 1);

        return TRUE;
    }

    return FALSE;
}

BOOL 
CMainDialog::OnInitDialog(
    HWND    hwnd,
    HWND    hWndFocus,
    LPARAM  lParam
    )
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

    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);


    m_hwndOkExit = GetDlgItem(hwnd, IDC_OK_EXIT);
    m_hwndFileName = GetDlgItem(hwnd, IDC_DUMP_FNAME);

    m_hwndRadioDump = GetDlgItem(hwnd, IDC_RADIO1);
    m_hwndRadioFilling = GetDlgItem(hwnd, IDC_RADIO2);
    m_hwndRadioNopping = GetDlgItem(hwnd, IDC_RADIO3);

    if (InitTypeOfWork(hwnd) != TRUE)
    {
        return FALSE;
    }

    SetWindowText(m_hwndFileName, this->m_pParameters->szDumpFileName);

    RECT  dialogDefault;
    GetWindowRect(hwnd, &dialogDefault);

    LONG dialogWidth = dialogDefault.right - dialogDefault.left;
    LONG dialogHeight = dialogDefault.bottom - dialogDefault.top;



    RECT  windowRect;
    GetWindowRect(GetDesktopWindow(), &windowRect);

    if (CConfiguration::Get()->m_hwndRect.top < windowRect.top ||
        CConfiguration::Get()->m_hwndRect.bottom > windowRect.bottom ||
        CConfiguration::Get()->m_hwndRect.left < windowRect.left ||
        CConfiguration::Get()->m_hwndRect.right > windowRect.right)
    {
        memset(&CConfiguration::Get()->m_hwndRect, 0, sizeof(RECT));
    }

    if (CConfiguration::Get()->m_hwndRect.right != 0 &&
        CConfiguration::Get()->m_hwndRect.bottom != 0)
    {
        MoveWindow(this->m_hwnd, 
            CConfiguration::Get()->m_hwndRect.left, 
            CConfiguration::Get()->m_hwndRect.top,
            dialogWidth,
            dialogHeight,
            TRUE);
    }


    ShowWindow(this->m_hwnd, SW_SHOWNORMAL);

    UpdateWindow(this->m_hwnd);

    SetCursor(
        LoadCursor(NULL, IDC_ARROW));


    return TRUE;
}

BOOL
CMainDialog::OnClose(HWND)
{
    this->m_result = FALSE;

    DestroyWindow(this->m_hwnd);

    return TRUE;
}

BOOL
CMainDialog::OnDestroy(HWND)
/*++

    Function:
        OnDestroy

    Pusrpouse:

        On Destroy dispatching

    Arguments:

        hWnd        - [in] handle to window

    Returns:
        BOOL

--*/
{
    ShowWindow(this->m_hwnd, SW_HIDE);

    RECT    rect;
    GetWindowRect(this->m_hwnd, &rect);

    CConfiguration::Get()->m_hwndRect = rect;

    SIZE_T length = GetWindowTextLength(this->m_hwndFileName);

    if (length != 0)
    {
        //
        // add null
        //
        length++;

        this->m_pParameters->_freeFunction(
            this->m_pParameters->szDumpFileName);

        this->m_pParameters->szDumpFileName = (TCHAR *)(
            this->m_pParameters->_allocFunction(length * sizeof(TCHAR)));

        if (this->m_pParameters->szDumpFileName != 0)
        {
            GetWindowText(this->m_hwndFileName, this->m_pParameters->szDumpFileName, (INT)length);
        }
    }

    if (this->m_pTypeofWork != NULL)
    {
        delete this->m_pTypeofWork;
    }

    this->m_Working = FALSE;
    this->m_hwnd = NULL;

    return TRUE;
}

BOOL
CMainDialog::OnCommand(
    HWND hWnd,
    int  id,
    HWND hwndCtl,
    UINT codeNotify)
/*++

    Function:
        OnCommand

    Pusrpouse:

        On Command dispatching

    Arguments:

        hWnd        - [in] handle to window

    Returns:
        BOOL

--*/
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(codeNotify);

    if (hwndCtl == m_hwndOkExit)
    {
        switch (id)
        {
        case IDC_OK_EXIT:

            this->m_result = TRUE;
            DestroyWindow(this->m_hwnd);
            break;

        case IDC_SELECT_REGION:

            break;
        }

        return TRUE;
    }
    
    //
    // radio buttons with selection TOW
    //

    if (hwndCtl == m_hwndRadioDump)
    {
        if (CConfiguration::Get()->m_SelectedTab != 0)
        {
            CConfiguration::Get()->m_SelectedTab = 0;

            InitTypeOfWork(this->m_hwnd);
        }
    }

    if (hwndCtl == m_hwndRadioFilling)
    {
        if (CConfiguration::Get()->m_SelectedTab != 1)
        {
            CConfiguration::Get()->m_SelectedTab = 1;

            InitTypeOfWork(this->m_hwnd);
        }
    }

    if (hwndCtl == m_hwndRadioNopping)
    {
        if (CConfiguration::Get()->m_SelectedTab != 2)
        {
            CConfiguration::Get()->m_SelectedTab = 2;

            InitTypeOfWork(this->m_hwnd);
        }
    }

    return FALSE;
}


INT_PTR
CMainDialog::DialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwndDlg, WM_CLOSE, OnClose);
        HANDLE_MSG(hwndDlg, WM_DESTROY, OnDestroy);

        //HANDLE_WM_COMMAND

    default:;

    }

    return FALSE;
}
        