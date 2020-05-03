/***************************************************************************************\
*   File:                                                                               *
*       dialogBase.cpp                                                                  *
*                                                                                       *
*   Abstract:                                                                           *
*       base dialog code                                                                *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 1-April-2019                                                      *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"

BOOL
CDialogBase::InitInstance()
/*++

    Function:
        InitInstance

    Pusrpouse:

        Initialize dialog default instance

    Arguments:

    Returns:
        result

--*/
{
    HWND hwnd = CreateDialogParam(
        m_hInstance,
        m_pTemplateName,
        m_hwndParent,
        &CDialogBase::iDialogProc,
        (LPARAM)this);

    if (hwnd == NULL)
    {
        return FALSE;
    }

    //
    // create or use default font for GUI
    //

    HGDIOBJ  hDefaultGuiFont = reinterpret_cast<HFONT>(
        ::GetStockObject(DEFAULT_GUI_FONT));

    if (hDefaultGuiFont != NULL)
    {

        LOGFONT         logfont;
        if (GetObject(hDefaultGuiFont, sizeof(logfont), &logfont) != 0)
        {

            this->m_hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 
                VARIABLE_PITCH | FF_SWISS, logfont.lfFaceName);
        }
    }

    if (this->m_hFont == NULL)
    {
        this->m_hFont = CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 
            VARIABLE_PITCH | FF_SWISS, _T(""));
    }

    this->m_hCollapseFont = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 
        VARIABLE_PITCH | FF_SWISS, _T("Arial"));

    this->m_dwSysWindow = GetSysColor(COLOR_WINDOW);
    this->m_dwSysWindowFrame = GetSysColor(COLOR_BTNFACE);

    //
    // this colors are used for lists
    //

    DWORD dwRed   = (DWORD)GetRValue(this->m_dwSysWindow);
    DWORD dwGreen = (DWORD)GetGValue(this->m_dwSysWindow);
    DWORD dwBlue  = (DWORD)GetBValue(this->m_dwSysWindow);

    if (dwGreen + 0x18 > 0xFF)
    {
        DWORD dwNormal = dwGreen + 0x30 - 0xFF;

        dwRed   = dwRed - dwNormal < 0   ? 0 : dwRed - dwNormal;
        dwGreen = dwGreen - dwNormal < 0 ? 0 : dwGreen - dwNormal;
        dwBlue = dwBlue - dwNormal < 0 ? 0 : dwBlue - dwNormal;
    }

    this->m_dwNormalized = RGB(dwRed, dwGreen, dwBlue);

    return TRUE;
}

BOOL
CDialogBase::Run()
/*++

    Function:
        Run

    Pusrpouse:

        Dialog run loop.
        It is the main message loop used in the program.

    Arguments:
        no

    Returns:
        result

--*/
{
    HWND pwndTop = NULL;

    //
    // In case we want to get a modal dialog, we block the parent. 
    // Thus, our becomes the main one.
    // This code is used to simplify the code model. And also for 
    // refusing to use system modal dialogs.
    //

    if (m_modal)
    {

        pwndTop = GetTopLevelWindow(this->m_hwndParent);

        EnableWindow(pwndTop, FALSE);
    }

    ShowWindow(this->m_hwnd, SHOW_OPENWINDOW);

    UpdateWindow(this->m_hwnd);
    

    MSG msg;
    BOOL bRet = 0;

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (-1 == bRet)
            break;

        if (!IsDialogMessage(this->m_hwnd, &msg))
        {
            //
            // Otherwise, dispatch the message.
            //
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!this->m_Working)
            break;
    }


    //
    // At the end of the cycle, restore the parent window.
    //

    if (m_modal)
    {
        EnableWindow(pwndTop, TRUE);

        SetActiveWindow(pwndTop);
    }

    BOOL  _result = this->m_result;
    
    return _result;
}

DWORD
CDialogBase::GetDefaultCharWidth(IN HWND hWnd)
/*++

    Function:
        GetDefaultCharWidth

    Pusrpouse:

        Get the current font width value. Used to estimate the 
        width of fields in lists.

    Arguments:

        hWnd - [in] handle to window

    Returns:
        size

--*/
{
    DWORD   dwCxSize = 10;
    HDC hcDriveListDC = GetDC(hWnd);

    if (hcDriveListDC != NULL)
    {
        TEXTMETRIC lptm;

        if (GetTextMetrics(hcDriveListDC, &lptm) == TRUE)
        {
            dwCxSize = lptm.tmAveCharWidth;
        }

        ReleaseDC(hWnd, hcDriveListDC);
    }

    return dwCxSize;

}

HWND
CDialogBase::GetTopLevelWindow(HWND hWnd)
/*++

    Function:
        GetTopLevelWindow

    Pusrpouse:
        
        Search for the top-level window of the application.

    Arguments:

        hWnd - [in] handle to window

    Returns:
        Top window

--*/
{
	HWND hWndTop;

	do
	{
		hWndTop = hWnd;
		hWnd = ::GetParent(hWnd);
	}
	while (hWnd != NULL);

	return hWndTop;
}

INT_PTR
CDialogBase::DialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam)
/*++

    Function:
        DialogProc

    Pusrpouse:
        
        The default dialog procedure. It is used if the child class has not provided its own.

    Arguments:

        hwndDlg - [in] dialog HWND 

    Returns:
        Top window

--*/
{
    UNREFERENCED_PARAMETER(hwndDlg);
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    //
    // default handling
    //
    return FALSE;
}

INT_PTR CALLBACK
CDialogBase::iDialogProc(
    _In_  HWND hwndDlg,
    _In_  UINT uMsg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam)
/*++

    Function:
        iDialogProc

    Pusrpouse:

        An internal function designed to receive and set a pointer to an owner class. 
        It is also an intermediate point of message delivery to the handler function itself.
        

    Arguments:

        hwndDlg - [in] dialog HWND 

        uMsg    - [in] message

        wParam  - [in] upper parameter

        lParam  - [in] lower parameter

    Returns:
        message status

--*/
{

    //
    // Trying to get a preset pointer.
    //

    CDialogBase *pWndClass = reinterpret_cast<CDialogBase *>(
        (LONG_PTR)::GetWindowLongPtr(hwndDlg, DWLP_USER));

    //
    // A pointer to the current class is passed at the time the dialog is initialized as a 
    // parameter. In this case, we get it and set it as a window context pointer.
    //
    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtr(hwndDlg, DWLP_USER, (LONG_PTR)lParam);
        pWndClass = reinterpret_cast<CDialogBase *>(lParam);

        pWndClass->m_hwnd = hwndDlg;
    }

    //
    // We skip the events that go before the window is initialized.
    //

    if (pWndClass != NULL)
    {
        return pWndClass->DialogProc(hwndDlg, uMsg, wParam, lParam);
    }
    else
    {
        return TRUE;
    }
}
