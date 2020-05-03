/***************************************************************************************\
*   File:                                                                               *
*       dumpModeDialog.h                                                                *
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
#pragma once
#ifndef __DUMP_MODE_DIALOG_H__
#define __DUMP_MODE_DIALOG_H__


class CDumpModeDialog : public CDialogBase
{
private:
    HWND       m_hwndFromVA;
    HWND       m_hwndToVA;
    HWND       m_hwndVaSize;
    HWND       m_hwndCurrentVA;
    HWND       m_hwndSelectRegion;

    GUI_PARAMETERS *m_parameters;

public:
    CDumpModeDialog(HINSTANCE hInstance, HWND parentWnd, GUI_PARAMETERS *parameters)
        : CDialogBase(hInstance, parentWnd, FALSE, MAKEINTRESOURCE(IDD_MEMORYDUMP))
    {
        m_parameters = parameters;
    }

    virtual ~CDumpModeDialog();

private:
    VOID  ParamsToDialog(const struct _DUMP_PARAMETERS *pDump);
    BOOL  DialogToParams(struct _DUMP_PARAMETERS *pDump);

    BOOL  OnInitDialog(
        HWND    hwnd,
        HWND    hWndFocus,
        LPARAM  lParam);

    BOOL  OnCommand(HWND hWnd, int  id, HWND hwndCtl, UINT codeNotify);

    virtual INT_PTR DialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

};

#endif // __DUMP_MODE_DIALOG_H__