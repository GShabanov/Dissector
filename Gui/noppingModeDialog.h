/***************************************************************************************\
*   File:                                                                               *
*       noppingModeDialog.h                                                             *
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
#pragma once
#ifndef __NOPPING_MODE_DIALOG_H__
#define __NOPPING_MODE_DIALOG_H__


class CNoppingModeDialog : public CDialogBase
{
private:
    HWND       m_hwndFromVA;
    HWND       m_hwndToVA;
    HWND       m_hwndSize;
    HWND       m_hwndMakeChangesOnDisk;

    GUI_PARAMETERS *m_parameters;

public:
    CNoppingModeDialog(HINSTANCE hInstance, HWND parentWnd, GUI_PARAMETERS *parameters)
        : CDialogBase(hInstance, parentWnd, FALSE, MAKEINTRESOURCE(IDD_MEMORYNOPPING))
    {
        m_parameters = parameters;
    }


private:
    BOOL OnInitDialog(
        HWND    hwnd,
        HWND    hWndFocus,
        LPARAM  lParam);

    BOOL OnDestroy(HWND);

    virtual INT_PTR DialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

};

#endif // __NOPPING_MODE_DIALOG_H__