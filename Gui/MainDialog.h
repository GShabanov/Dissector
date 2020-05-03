/***************************************************************************************\
*   File:                                                                               *
*       mainDialog.h                                                                    *
*                                                                                       *
*   Abstract:                                                                           *
*       Main dialog for all modes                                                       *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#pragma once
#ifndef __MAIN_DIALOG_H__
#define __MAIN_DIALOG_H__


class CMainDialog : public CDialogBase
{
public:

private:
    CDialogBase       *m_pTypeofWork;
    HWND                m_hwndOkExit;
    HWND                m_hwndFileName;

    //----------------------
    // change type of work
    HWND   m_hwndRadioDump;
    HWND   m_hwndRadioFilling;
    HWND   m_hwndRadioNopping;


    struct _GUI_PARAMETERS   *m_pParameters;
public:

    CMainDialog(HINSTANCE hInstance, struct _GUI_PARAMETERS *parameters, HWND parentWnd = NULL);

private:
    virtual INT_PTR DialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

    BOOL  InitTypeOfWork(HWND hwndParent);

    BOOL  OnInitDialog(
        HWND    hwnd,
        HWND    hWndFocus,
        LPARAM  lParam);

    BOOL  OnCommand(HWND hWnd, int  id, HWND hwndCtl, UINT codeNotify);
    BOOL  OnClose(HWND);
    BOOL  OnDestroy(HWND);

        
};
#endif // __MAIN_DIALOG_H__