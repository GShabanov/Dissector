/***************************************************************************************\
*   File:                                                                               *
*       patchModeDialog.h                                                               *
*                                                                                       *
*   Abstract:                                                                           *
*       patching mode dialog                                                            *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#pragma once
#ifndef __PATCH_MODE_DIALOG_H__
#define __PATCH_MODE_DIALOG_H__


class CPatchModeDialog : public CDialogBase
{
private:
    HWND       m_hwndFromVA;
    HWND       m_hwndToVA;
    HWND       m_hwndSize;
    HWND       m_hwndMakeChangesOnDisk;

    GUI_PARAMETERS *m_parameters;

public:
    CPatchModeDialog(HINSTANCE hInstance, HWND parentWnd, GUI_PARAMETERS *parameters)
        : CDialogBase(hInstance, parentWnd, FALSE, MAKEINTRESOURCE(IDD_MEMORYPATCH))
    {
        m_parameters = parameters;
    }

    virtual ~CPatchModeDialog();

private:
    BOOL OnInitDialog(
        HWND    hwnd,
        HWND    hWndFocus,
        LPARAM  lParam);


    virtual INT_PTR DialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

};

#endif // __PATCH_MODE_DIALOG_H__