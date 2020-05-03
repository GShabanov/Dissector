/***************************************************************************************\
*   File:                                                                               *
*       SelectVaDialog.h                                                                *
*                                                                                       *
*   Abstract:                                                                           *
*       Virtual address selection dialog                                                *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#pragma once
#ifndef __SELECT_VA_DIALOG_H__
#define __SELECT_VA_DIALOG_H__


class CSelectVaDialog : public CDialogBase
{
public:

private:
    HWND                m_hwndOkExit;
    HWND                m_hwndMemRegions;
    INT                 m_SelectedIndex;

    TArray<MEMORY_ENUM_ENTRY> m_ProgramVaSpace;
    struct _GUI_PARAMETERS   *m_pParameters;
public:

    CSelectVaDialog(HINSTANCE hInstance, struct _GUI_PARAMETERS *parameters, HWND parentWnd = NULL);

    MEMORY_ENUM_ENTRY  GetSelectedRegion()
    {
        if (m_SelectedIndex != -1 && 
            m_SelectedIndex <= m_ProgramVaSpace.count())
        {
            return m_ProgramVaSpace[m_SelectedIndex];
        }
        else
        {
            MEMORY_ENUM_ENTRY faike;
            memset(&faike, 0, sizeof(MEMORY_ENUM_ENTRY));
            return faike;
        }
    }

private:
    void PopulateMemoryRegions();

    virtual INT_PTR DialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

    BOOL  OnInitDialog(
        HWND    hwnd,
        HWND    hWndFocus,
        LPARAM  lParam);

    VOID DrawItemCel(HDC hDC, const MEMORY_ENUM_ENTRY *entry, int index, LPRECT prcClip, BOOL bCenter);

    BOOL  OnCommand(HWND hWnd, int  id, HWND hwndCtl, UINT codeNotify);
    BOOL  OnClose(HWND);
    BOOL  OnDestroy(HWND);
    BOOL  OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
    VOID  OnSize(HWND hwnd, UINT state, int cx, int cy);
};
#endif // __SELECT_VA_DIALOG_H__