/***************************************************************************************\
*   File:                                                                               *
*       dialogBase.h                                                                    *
*                                                                                       *
*   Abstract:                                                                           *
*       Base difinition for dialogs                                                     *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#pragma once
#ifndef __DIALOG_BASE_H__
#define __DIALOG_BASE_H__

class CDialogBase
{
public:
    explicit CDialogBase(HINSTANCE hInstance, HWND parentWnd, BOOL modal, LPCSTR pTemplateName)
        : m_hInstance(hInstance)
        , m_hwndParent(parentWnd)
        , m_hwnd(NULL)
        , m_modal(modal)
        , m_pTemplateName(pTemplateName)
    {
        m_Working = TRUE;
        m_result = FALSE;
    }

    virtual ~CDialogBase()
    {
        DeleteObject(this->m_hFont);
        DeleteObject(this->m_hCollapseFont);

        if (m_hwnd != NULL)
        {
            DestroyWindow(m_hwnd);

            m_hwnd = NULL;
        }
    }

protected:
    LPCSTR     m_pTemplateName;
    HINSTANCE  m_hInstance;
    HWND       m_hwndParent;
    HWND       m_hwnd;

    BOOL       m_modal;
    BOOL       m_Working;
    BOOL       m_result;

    //
    // colors and fonts
    //
    HFONT                   m_hFont;
    HFONT                   m_hCollapseFont;
    DWORD                   m_dwSysWindow;
    DWORD                   m_dwSysWindowFrame;
    DWORD                   m_dwNormalized;

    static HWND  GetTopLevelWindow(HWND hWnd);
    static DWORD GetDefaultCharWidth(HWND hWnd);

public:
    BOOL  InitInstance();

    BOOL  Run();


protected:
    virtual INT_PTR DialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

private:
    static INT_PTR CALLBACK iDialogProc(
      _In_  HWND hwndDlg,
      _In_  UINT uMsg,
      _In_  WPARAM wParam,
      _In_  LPARAM lParam);

};

#endif // __DIALOG_BASE_H__