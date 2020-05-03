/***************************************************************************************\
*   File:                                                                               *
*       SelectVaDialog.cpp                                                              *
*                                                                                       *
*   Abstract:                                                                           *
*       Auxiliary dialogue allowing the user to select the required memory              *
*       region based on the system query functions.                                     *
*                                                                                       *
*   Author:                                                                             *
*       G Shabanov () 15-Oct-2019                                                       *
*                                                                                       *
*   Revision History:                                                                   *
\***************************************************************************************/
// $Id: $
#include "stdafx.h"

#include "SelectVaDialog.h"


CSelectVaDialog::CSelectVaDialog(HINSTANCE hInstance, GUI_PARAMETERS *parameters, HWND parentWnd)
    : CDialogBase(hInstance, parentWnd, TRUE, MAKEINTRESOURCE(IDD_MEMREG_SELECT))
{
    m_pParameters = parameters;

    m_SelectedIndex = -1;
}


void
CSelectVaDialog::PopulateMemoryRegions()
/*++

    Function:
        PopulateMemoryRegions

    Pusrpouse:

        The function calls the callback defined by the caller to get a list of regions. 
        It uses this acquired list to populate a text list of regions.

    Arguments:


    Returns:
        void

--*/
{
    
    m_ProgramVaSpace.RemoveAll();

    this->m_pParameters->enumerateMemoryRegions(this->m_pParameters, m_ProgramVaSpace);

    LONG_PTR currentVA = this->m_pParameters->choosenVA;

    ListView_DeleteAllItems(m_hwndMemRegions);

    LVITEM lvI;

    UINT  maxColumn1 = ListView_GetColumnWidth(m_hwndMemRegions, 0);
    UINT  maxColumn2 = ListView_GetColumnWidth(m_hwndMemRegions, 1);
    UINT  maxColumn3 = ListView_GetColumnWidth(m_hwndMemRegions, 2);
    UINT  maxColumn4 = ListView_GetColumnWidth(m_hwndMemRegions, 3);

    INT_PTR iVisible = 0;
    DWORD   dwCxSize = GetDefaultCharWidth(m_hwndMemRegions);

    //
    // In fact, to increase efficiency, this function does not fill the regions, 
    // but only determines their number. Physical filling with data is done in the drawing function.
    //

    for (INT_PTR i = 0, j = m_ProgramVaSpace.count(); i < j; i++)
    {
        const MEMORY_ENUM_ENTRY *entry = &m_ProgramVaSpace[i];


        //
        // setup visible item
        //
        if (currentVA >= entry->regionStart &&
            currentVA < (LONG_PTR)(entry->regionStart + entry->regionSize))
        {
            iVisible = i;
        }

        //
        // memory region start
        //

        TCHAR celText[MAX_TEMP_STRING];
#ifdef __X64__
        _stprintf(celText, _T("0x%016I64X"), entry->regionStart);
#else // __X64__
        _stprintf(celText, _T("0x%08I32X"), entry->regionStart);
#endif // __X64__

        SIZE_T width = (_tcslen(celText)) * dwCxSize;

        if (width > maxColumn1)
            maxColumn1 = (UINT)width;

        //
        // Initialize LVITEM members that are common to all items.
        //

        lvI.pszText   = LPSTR_TEXTCALLBACK;//celText;
        lvI.mask      = LVIF_STATE;
        lvI.stateMask = 0;
        lvI.iSubItem  = 0;
        lvI.state     = 0;
        lvI.iItem  = (int)i;

        ListView_InsertItem(m_hwndMemRegions, &lvI);

        //
        // memory region length
        //
#ifdef __X64__
        _stprintf(celText, _T("0x%016I64X"), entry->regionSize);
#else // __X64__
        _stprintf(celText, _T("0x%08I32X"), entry->regionSize);
#endif // __X64__

        width = (_tcslen(celText)) * dwCxSize;

        if (width > maxColumn2)
            maxColumn2 = (UINT)width;

        lvI.pszText   = LPSTR_TEXTCALLBACK; //celText;
        lvI.mask      = LVIF_STATE;
        lvI.stateMask = 0;
        lvI.iSubItem  = 1;
        lvI.state     = 0;
        lvI.iItem  = (int)i;


        ListView_SetItem(m_hwndMemRegions, &lvI);

        //
        // memory attributes
        //

        celText[0] = entry->attr.read ? _T('R') : _T('.');
        celText[1] = entry->attr.write ? _T('W') : _T('.');
        celText[2] = entry->attr.execute ? _T('X') : _T('.');
        celText[3] = _T(' ');
        celText[4] = 0;

        if (entry->isImage) {
            _tcscat(celText, _T("- I"));
        } else {
            _tcscat(celText, _T("- C"));
        }

        width = (_tcslen(celText) + 1) * dwCxSize;

        if (width > maxColumn3)
            maxColumn3 = (UINT)width;

        lvI.pszText   = LPSTR_TEXTCALLBACK; //celText;
        lvI.mask      = LVIF_STATE;
        lvI.stateMask = 0;
        lvI.iSubItem  = 2;
        lvI.state     = 0;
        lvI.iItem  = (int)i;


        ListView_SetItem(m_hwndMemRegions, &lvI);

        //
        // Dll name column
        //
        width = (_tcslen(entry->moduleName) + 1) * dwCxSize;

        if (width > maxColumn4)
            maxColumn4 = (UINT)width;

        lvI.pszText   = LPSTR_TEXTCALLBACK; //celText;
        lvI.mask      = LVIF_STATE;
        lvI.stateMask = 0;
        lvI.iSubItem  = 3;
        lvI.state     = 0;
        lvI.iItem  = (int)i;


        ListView_SetItem(m_hwndMemRegions, &lvI);
    }

    ListView_SetColumnWidth(m_hwndMemRegions, 0, maxColumn1);
    ListView_SetColumnWidth(m_hwndMemRegions, 1, maxColumn2);
    ListView_SetColumnWidth(m_hwndMemRegions, 2, maxColumn3);
    ListView_SetColumnWidth(m_hwndMemRegions, 3, maxColumn4);

    //
    // scroll list view to region
    //

    ListView_EnsureVisible(
        m_hwndMemRegions, iVisible, FALSE);
        
}

BOOL 
CSelectVaDialog::OnInitDialog(
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

    this->m_hwnd = hwnd;

    m_hwndOkExit = GetDlgItem(hwnd, IDC_OK_EXIT);
    m_hwndMemRegions = GetDlgItem(hwnd, IDC_MEMREGIONS);

    ListView_SetExtendedListViewStyle(m_hwndMemRegions,
        LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT/* | LVS_EX_HEADERDRAGDROP*/);

    DWORD   dwCxSize = GetDefaultCharWidth(m_hwndMemRegions);

    LVCOLUMN    lpColumn;

    static TCHAR szColumn1[] = _T("Address");

    lpColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lpColumn.fmt = LVCFMT_LEFT;
    lpColumn.cx  = dwCxSize * sizeof(szColumn1) / sizeof(TCHAR);
    lpColumn.pszText = szColumn1;

    ListView_InsertColumn(m_hwndMemRegions, 0, &lpColumn);

    static TCHAR szColumn2[] = _T("Size");

    lpColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lpColumn.fmt = LVCFMT_LEFT;
    lpColumn.cx  = dwCxSize * sizeof(szColumn2) / sizeof(TCHAR);
    lpColumn.pszText = szColumn2;

    ListView_InsertColumn(m_hwndMemRegions, 1, &lpColumn);

    static TCHAR szColumn3[] = _T("Protection");

    lpColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lpColumn.fmt = LVCFMT_LEFT;
    lpColumn.cx  = dwCxSize * sizeof(szColumn3) / sizeof(TCHAR);
    lpColumn.pszText = szColumn3;

    ListView_InsertColumn(m_hwndMemRegions, 2, &lpColumn);

    static TCHAR szColumn4[] = _T("DllName");

    lpColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lpColumn.fmt = LVCFMT_LEFT;
    lpColumn.cx  = dwCxSize * sizeof(szColumn4) / sizeof(TCHAR);
    lpColumn.pszText = szColumn4;

    ListView_InsertColumn(m_hwndMemRegions, 3, &lpColumn);

    PopulateMemoryRegions();

    return TRUE;
}


BOOL
CSelectVaDialog::OnClose(HWND)
{
    this->m_result = FALSE;

    DestroyWindow(this->m_hwnd);

    return TRUE;
}

BOOL
CSelectVaDialog::OnDestroy(HWND)
{
    ShowWindow(this->m_hwnd, SW_HIDE);

    RECT    rect;
    GetWindowRect(this->m_hwnd, &rect);

    this->m_Working = FALSE;

    this->m_hwnd = NULL;

    return TRUE;
}

BOOL
CSelectVaDialog::OnCommand(
    HWND hWnd,
    int  id,
    HWND hwndCtl,
    UINT codeNotify)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(codeNotify);

    if (hwndCtl == m_hwndOkExit)
    {
        switch (id)
        {
        case IDC_OK_EXIT:

            this->m_result = TRUE;

            {
                int itemsCount = ListView_GetItemCount(this->m_hwndMemRegions);

                for (register int i = 0; i < itemsCount; i++)
                {
                    LVITEM  lvI;

                    lvI.mask      = LVIF_STATE;
                    lvI.lParam    = 0;
                    lvI.state     = 0;
                    lvI.stateMask = LVIS_SELECTED;
                    lvI.iItem     = i;

                    if (ListView_GetItem(this->m_hwndMemRegions, &lvI))
                    {
                        if (lvI.state & LVIS_SELECTED)
                        {
                            m_SelectedIndex = i;
                            break;
                        }
                    }
                }
            }

            DestroyWindow(this->m_hwnd);
            break;

        case IDC_SELECT_REGION:

            break;
        }

        return TRUE;
    }
    

    return FALSE;
}

VOID
CSelectVaDialog::DrawItemCel(
    HDC   hDC,
    const MEMORY_ENUM_ENTRY *entry,
    int   index,
    LPRECT prcClip,
    BOOL  bCenter)
/*++

    Function:
        DrawItemCel

    Pusrpouse:

        The actual function of drawing data from the current list item. 
        Its gets the data from the list and draws the cell.

    Arguments:

        hDC         - [in] device context

        entry       - [in] memory entry

        index       - [in] index in list

        prcClip     - [in] clip rect

        bCenter     - [in] draw text in center

    Returns:
        void

--*/
{
    TCHAR celText[MAX_TEMP_STRING];

    //
    // text we used to draw the item
    //
    TCHAR *text = celText;

    switch (index)
    {
    case 0:
#ifdef __X64__
        _stprintf(celText, _T("0x%016I64X"), entry->regionStart);
#else // __X64__
        _stprintf(celText, _T("0x%08I32X"), entry->regionStart);
#endif // __X64__
                    
        break;

    case 1:
#ifdef __X64__
        _stprintf(celText, _T("0x%016I64X"), entry->regionSize);
#else // __X64__
        _stprintf(celText, _T("0x%08I32X"), entry->regionSize);
#endif // __X64__
        break;

    case 2:
        celText[0] = entry->attr.read ? _T('R') : _T('.');
        celText[1] = entry->attr.write ? _T('W') : _T('.');
        celText[2] = entry->attr.execute ? _T('X') : _T('.');
        celText[3] = _T(' ');
        celText[4] = 0;

        if (entry->isImage) {
            _tcscat(celText, _T("- I"));
        } else {
            _tcscat(celText, _T("- C"));
        }

        break;

    case 3:
        text = (TCHAR *)entry->moduleName;

        break;

    default:;
    }

    HBRUSH hPaintBrush = CreateSolidBrush(GetBkColor(hDC));

    if (hPaintBrush != NULL)
    {
        ::FillRect(hDC, prcClip, (HBRUSH)hPaintBrush);

        ::DeleteObject((HGDIOBJ)hPaintBrush);
    }

    UINT fmt = DT_CENTER;

    if (bCenter == FALSE)
    {
        prcClip->left = prcClip->left + GetDefaultCharWidth(this->m_hwndMemRegions);

        prcClip->right = prcClip->right - 1;

        fmt = DT_LEFT;
    }

    DrawText(hDC, text, (int)_tcslen(text), prcClip, fmt | DT_WORD_ELLIPSIS | DT_NOPREFIX);
}

BOOL
CSelectVaDialog::OnDrawItem(HWND, const DRAWITEMSTRUCT * lpDrawItem)
/*++

    Function:
        OnDrawItem

    Pusrpouse:

        The message function called on the drawing object list.

    Arguments:

        hWnd        - [in] list view window

        lpDrawItem  - [in] draw item structure

    Returns:
        BOOL

--*/
{
    if (lpDrawItem->CtlType != ODT_LISTVIEW ||
        lpDrawItem->hwndItem != this->m_hwndMemRegions)
    {
        return FALSE;
    }

    LV_ITEM     lvi;
    RECT        rcClip;
    UINT        uiFlags = ILD_TRANSPARENT;

    if (lpDrawItem->itemID < 0 ||
        lpDrawItem->itemID > (UINT)m_ProgramVaSpace.count())
    {
        return FALSE;
    }

    const MEMORY_ENUM_ENTRY *entry = &m_ProgramVaSpace[lpDrawItem->itemID];

    //
    // Get the item image to be displayed
    //

    lvi.mask = LVIF_IMAGE | LVIF_STATE;
    lvi.iItem = lpDrawItem->itemID;
    lvi.iSubItem = 0;
    ListView_GetItem(lpDrawItem->hwndItem, &lvi);
    SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));

    //
    // Check to see if this item is selected
    //

    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        //
        // Set the text background and foreground colors
        //

        SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
        SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHT));

        //
        // Also add the ILD_BLEND50 so the images come out selected
        //

        uiFlags |= ILD_BLEND50;
    }
    else
    {
        //
        // Set the text background and foreground colors to the standard window
        // colors
        //
        
        DWORD dwWinColor = this->m_dwNormalized;

        if (entry->isImage)
        {
            dwWinColor += 0x8 << 8;
        }
        else
        {
            dwWinColor += 0x18 << 8;
        }

        SetBkColor(lpDrawItem->hDC, dwWinColor);
    }

    int         iColumn = 0;
    LVCOLUMN    lvColumn;
    DWORD       leftOffset = 0;

    lvColumn.mask = LVCF_WIDTH | LVCF_FMT | LVCF_ORDER;

    //
    // Set up the new clipping rect for the first column text and draw it
    // query columns count
    //

    while (ListView_GetColumn(this->m_hwndMemRegions, iColumn, &lvColumn))
    {
        rcClip.left     = lpDrawItem->rcItem.left + leftOffset;
        rcClip.right    = lpDrawItem->rcItem.left + leftOffset + lvColumn.cx;
        rcClip.top      = lpDrawItem->rcItem.top;
        rcClip.bottom   = lpDrawItem->rcItem.bottom;

        //
        // call actual drawing function for the current cell
        //

        DrawItemCel(lpDrawItem->hDC, entry, iColumn, &rcClip, (lvColumn.fmt & LVCFMT_CENTER) ? TRUE : FALSE);


        leftOffset += lvColumn.cx;

        iColumn++;
    }

    if (GetFocus() == this->m_hwndMemRegions)
    {
        //
        // If we changed the colors for the selected item, undo it
        //

        if (lpDrawItem->itemState & ODS_SELECTED)
        {
            //
            // Set the text background and foreground colors
            //

            SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOW));
        }

        //
        // If the item is focused, now draw a focus rect around the entire row
        //

        if (lpDrawItem->itemState & ODS_FOCUS)
        {
            //
            // Adjust the left edge to exclude the image
            //

            rcClip = lpDrawItem->rcItem;

            rcClip.bottom = rcClip.bottom - 1;

            //
            // Draw the focus rect
            //

            DrawFocusRect(lpDrawItem->hDC, &rcClip);
        }
    }


    return TRUE;
}

VOID
CSelectVaDialog::OnSize(HWND, UINT state, int cx, int cy)
{
    if (state != SIZE_MINIMIZED)
    {
        RECT    rect;

        rect.top = 0;
        rect.bottom = cy - 2;
        rect.left = 0;
        rect.right = cx - 2;

        ::MoveWindow(this->m_hwndMemRegions, 0, 0, cx - 62, cy - 2, TRUE);
        ::MoveWindow(this->m_hwndOkExit, cx - 61, cy - 20, 60, 18, TRUE);

        ::InvalidateRect(this->m_hwnd, &rect, TRUE);
    }
}

INT_PTR
CSelectVaDialog::DialogProc(
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
        HANDLE_MSG(hwndDlg, WM_DRAWITEM, OnDrawItem);
        HANDLE_MSG(hwndDlg, WM_SIZE, OnSize);

        //HANDLE_WM_COMMAND

    default:;

    }

    return FALSE;
}
