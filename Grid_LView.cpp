/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class COkCtrlGrid implementation
********************************************************************/
#include "framework.h"

//-----------------------------------------------
void LView_SetItemText (HWND hwndLV, WPARAM i, int iSubItem, WCHAR* pszText)
{ 
  LV_ITEM _ms_lvi;
  _ms_lvi.iSubItem = iSubItem;
  _ms_lvi.pszText = pszText;
  ::SendMessage( hwndLV, LVM_SETITEMTEXT, i, (LPARAM)(LV_ITEM*)&_ms_lvi );
}
//-----------------------------------------------
void LView_DeleteAllItems (HWND hwndLV)
{
  ::SendMessage( hwndLV, LVM_DELETEALLITEMS, 0, 0L );
}
//-----------------------------------------------
void LView_DeleteItem (HWND hwndLV, WPARAM i)
{
  ::SendMessage( hwndLV, LVM_DELETEITEM, i, 0L );
}

//-----------------------------------------------
HWND LView_GetHeader (HWND hwndLV)
{
  return (HWND)::SendMessage( hwndLV, LVM_GETHEADER, 0, 0L );
}
//-----------------------------------------------
void LView_GetSubItemRect (HWND hwndLV, WPARAM iItem, int iSubItem, int code, RECT* prc)
{
  ::SendMessage( hwndLV, LVM_GETSUBITEMRECT, iItem,
     ((prc) ? ((((LPRECT)(prc))->top = iSubItem), (((LPRECT)(prc))->left = code), (LPARAM)(prc)) : (LPARAM)(LPRECT)NULL));
}
//-----------------------------------------------
BOOL LView_GetColumn (HWND hwndLV, WPARAM iCol, LV_COLUMN* pcol)
{
  return (BOOL)::SendMessage( hwndLV, LVM_GETCOLUMN, iCol, (LPARAM)pcol );
}
//-----------------------------------------------
LRESULT LView_GetColumnWidth (HWND hwndLV, WPARAM iCol )
{
  return ::SendMessage( hwndLV, LVM_GETCOLUMNWIDTH, iCol, 0 );
}
//-----------------------------------------------
BOOL LView_GetItem (HWND hwndLV, LV_ITEM* pitem)
{
  if (::SendMessage( hwndLV, LVM_GETITEM, 0, (LPARAM)pitem ) == 0){
    return FALSE;
  }
  return TRUE;
}
//-----------------------------------------------
LRESULT LView_GetItemCount (HWND hwndLV)
{
  return ::SendMessage( hwndLV, LVM_GETITEMCOUNT, 0, 0 );
}

//-----------------------------------------------
LRESULT LView_GetItemState (HWND hwndLV, WPARAM i, LPARAM mask)
{
  return ::SendMessage( hwndLV, LVM_GETITEMSTATE, i, mask );
}
//-----------------------------------------------
BOOL LView_SetColumn (HWND hwndLV, WPARAM iCol, const LV_COLUMN* pcol)
{
  if (::SendMessage( hwndLV, LVM_SETCOLUMN, iCol, (LPARAM)(pcol) ) == 0){
    return FALSE;
  }
  return TRUE;
}
//-----------------------------------------------
LRESULT LView_SetExtendedListViewStyle (HWND hwndLV, DWORD Style)
{
  return ::SendMessage( hwndLV, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, Style );
}
//-----------------------------------------------
void LView_SetItemState (HWND hwndLV, WPARAM i, UINT data, UINT mask)
{
  LV_ITEM ms_lvi;
  ms_lvi.stateMask = mask;
  ms_lvi.state = data;
  ::SendMessage( hwndLV, LVM_SETITEMSTATE, i, (LPARAM)(&ms_lvi) );
}
//-----------------------------------------------
void LView_EnsureVisible (HWND hwndLV, WPARAM i)
{
  ::SendMessage( hwndLV, LVM_ENSUREVISIBLE, i, (LPARAM)FALSE );  
}

//-----------------------------------------------
BOOL LView_SortItems (HWND hwndLV, PFNLVCOMPARE pfnCompare, LPARAM lPrm)
{
  if (::SendMessage( hwndLV, LVM_SORTITEMS, (WPARAM)lPrm, (LPARAM)pfnCompare ) == 0){
    return FALSE;
  }
  return TRUE;
}
//-----------------------------------------------
LRESULT LView_SubItemHitTest (HWND hwndLV, LPLVHITTESTINFO plvhti)
{
  return ::SendMessage( hwndLV, LVM_SUBITEMHITTEST, 0, (LPARAM)plvhti );
}
//-----------------------------------------------
LRESULT LView_InsertColumn (HWND hwndLV, WPARAM iCol, const LV_COLUMN* pcol)
{
  return ::SendMessage( hwndLV, LVM_INSERTCOLUMN, iCol, (LPARAM)pcol );
}
//-----------------------------------------------
LRESULT LView_InsertItem (HWND hwndLV, const LV_ITEM* pitem)
{
  return ::SendMessage( hwndLV, LVM_INSERTITEM, 0, (LPARAM)pitem );
}
