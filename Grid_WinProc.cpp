/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class COkCtrlGrid implementation
********************************************************************/
#define _GRID_CPP_
#include "framework.h"


//-----------------------------------------------
static int CALLBACK GridCompareFunc (LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	COkCtrlGrid* pGrid = (COkCtrlGrid*)lParamSort;
  COkTRow* pRow1 = (COkTRow*)lParam1;
  COkTRow* pRow2 = (COkTRow*)lParam2;
  if (pGrid->IsUnsorted()){
    return 0;
  }
  int res = pRow1->CompareWith( pRow2, pGrid->SortColumnID(), pGrid->IsSortUp() );
  return res;
}

//-----------------------------------------------
LRESULT COkCtrlGrid::WinProc (UINT Msg, WPARAM wParam, LPARAM lParam)
{
  NMHDR*             pNmh;
  NMLISTVIEW*        pLV; 
  MEASUREITEMSTRUCT* pMis;
  NMLVKEYDOWN*       pKey;
  NMITEMACTIVATE*    pItem;
  COkTRow*           pRow;
  COkTRow*           pCurRow = NULL;
  HDITEM             hdi;
  LVHITTESTINFO      hti;
  WPARAM             idCtrl;
  LRESULT            state;
  int                i, x, y;
  bool               bCtrl, bShift, bSelect, bEdit;
  RECT               rect;

  int RCLICK = NM_RCLICK;

  memset( &hdi, 0, sizeof(hdi) );
  switch( Msg ){
    case WM_DESTROY:
      break;

    case WM_MEASUREITEM:
      switch( (int)wParam ){
        case ID_LISTVIEW:
          pMis = (MEASUREITEMSTRUCT*) lParam;
          pMis->itemHeight = ITEM_HEIGHT;
          return TRUE;
        case ID_CBOX:
          pMis = (MEASUREITEMSTRUCT*) lParam;
          pMis->itemHeight = ITEM_HEIGHT-3;
          return TRUE;
      }
      break;

    case WM_DRAWITEM:
      switch( (int)wParam ){
        case ID_LISTVIEW:
          OnDrawRaw( (DRAWITEMSTRUCT*)lParam );
          break;
        case ID_CBOX:
          if (m_FireListDraw){
            OnDrawListItem( (DRAWITEMSTRUCT*)lParam );
          }
          break;
      }
      break;

    case WM_KEYDOWN:
      OnKeyDown( wParam, lParam ); 
      break;

    case WM_MOUSEMOVE:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      OnMouseMove( x, y );
      break;

    case WM_LBUTTONUP:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      OnLButtonUp( x, y );
      break;

    case WM_LBUTTONDOWN:
      if (IsCbox()){
        StopEdit( false );
        ::PostMessage( m_hwListView, WM_LBUTTONDOWN, wParam, lParam );
      }
      break;

    case WM_TIMER:
      if (wParam == ID_EDITBOX){
        if (::GetFocus() != m_hwEdit){
          StopEdit( false );
        }else{
          LView_GetSubItemRect( m_hwListView, m_iEditItem, m_iEditColumn, LVIR_BOUNDS, &rect );
          if (memcmp( &m_EditRect, &rect, sizeof(RECT) ) != 0){
            StopEdit( false );
          }
        }
      }
      break;

    case WM_COMMAND:
      idCtrl = LOWORD( wParam );
      switch( idCtrl ) {
        case ID_CBOX:
          switch (HIWORD(wParam)) {
            case CBN_SELENDOK:
              Cbox_Select();
              return TRUE;
          }
          break;
      }
      break;

    case WM_NOTIFY: 
      pNmh = (NMHDR*)lParam; 
      idCtrl = pNmh->idFrom;
      switch( pNmh->code ){
        case NM_KILLFOCUS:
          i = 0;
          break;
        case LVN_COLUMNCLICK:
          pLV = (LPNMLISTVIEW)lParam;
          if (m_Column[pLV->iSubItem].IsSortable()){
            hdi.mask = HDI_BITMAP | HDI_FORMAT;
            hdi.fmt  = HDF_STRING | HDF_LEFT;
            if (m_iSortColumn >= 0){
              Header_SetItem( m_hwHeader, m_iSortColumn, &hdi );
            }
            if (pLV->iSubItem != m_iSortColumn){
              m_bSortUp = true;
              m_iSortColumn = pLV->iSubItem;
            }else{
              m_bSortUp = !m_bSortUp;
            }
            if (m_iSortColumn >= 0){
              hdi.fmt = HDF_STRING | HDF_LEFT | HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
              hdi.hbm = m_bSortUp? m_hBmpSortUp : m_hBmpSortDown;
              Header_SetItem( m_hwHeader, m_iSortColumn, &hdi );
              LView_SortItems( m_hwListView, GridCompareFunc, (LPARAM)this );
            }
          }
          break;

        case NM_CLICK:
        case NM_RCLICK:
          StopEdit( false );
          if (pNmh->idFrom){
            pItem = (LPNMITEMACTIVATE) lParam;
            if (pItem->iItem >= 0){
              m_iCurColumn = pItem->iSubItem;
              if (m_FireClick || m_FireRClick){
                // fire the focused item
                pRow = GetRowByItem( pItem->iItem );
                bEdit = false;
                if (pRow){
                  if (pNmh->code == (UINT)RCLICK){
                    if (m_FireRClick){
                      (*m_FireRClick)( this, pRow, m_Column[m_iCurColumn].ID() );
                    }
                  }else{
                    if (pRow->m_bReadOnly == false){
                      if (m_Column[m_iCurColumn].VarType() == OK_GRID_VAR_LIST){
                        // create combobox control
                        Cbox_Create();
                        return TRUE;
                      }else{
                        if (m_FireClick){
                          (*m_FireClick)( this, pRow, m_Column[m_iCurColumn].ID(), &bEdit );
                          if (bEdit){
                            // edit this cell
                            StartEdit( pItem->iItem, m_iCurColumn );
                          }
                        }
                      }
                    }
                  }
                }
              }
              ::InvalidateRect( m_hwListView, NULL, false );
            }else{
              LView_SetItemState( m_hwListView, m_iCurRow, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
              LView_EnsureVisible( m_hwListView, m_iCurRow );
              if (pNmh->code == (UINT)RCLICK){
                if (m_FireRClick){
                  (*m_FireRClick)( this, NULL, 0 );
                }
              }
            }
          }
          break;

        case NM_DBLCLK:
          pItem = (LPNMITEMACTIVATE) lParam;
          if (pItem->iItem >= 0 && m_FireDblClick){
            pRow = GetRowByItem( pItem->iItem );
            if (pRow->m_bReadOnly == false){
              bEdit = false;
              (*m_FireDblClick)( this, pRow, m_Column[pItem->iSubItem].ID(), &bEdit );
              if (bEdit){
                // edit this cell
                StartEdit( pItem->iItem, m_iCurColumn );
              }
              ::InvalidateRect( m_hwListView, NULL, false );
            }
          }
          break;

        case LVN_ITEMCHANGED:
          pLV = (LPNMLISTVIEW) lParam; 
          state = LView_GetItemState( m_hwListView, pLV->iItem, LVIS_SELECTED | LVIS_FOCUSED );
          pCurRow = m_pCurRow;
          pRow = GetRowByItem( pLV->iItem );
          bCtrl  = (::GetKeyState( VK_CONTROL )<0)? true : false;
          bShift = (::GetKeyState( VK_SHIFT )<0)? true : false;
          if (pLV->uNewState){
            if (state & LVIS_FOCUSED){
              StopEdit( false );
              if (bCtrl || bShift){
                if (bShift){
                  bSelect = false;
                  for (i=0; i<m_nRows; ++i){
                    state = LView_GetItemState( m_hwListView, i, LVIS_SELECTED | LVIS_FOCUSED );
                    if (bSelect){
                      GetRowByItem( i )->SetSelect( true );
                      if ((state & LVIS_FOCUSED) || ((int)i == m_iCurRow)){
                        break;
                      }
                    }else{
                      if ((state & LVIS_FOCUSED) || ((int)i == m_iCurRow)){
                        bSelect = true;
                        GetRowByItem( i )->SetSelect( true );
                      }
                    }
                  }
                }
              }else{
                memset( &hti, 0, sizeof(hti) );
                ::GetCursorPos( &hti.pt );
                ::ScreenToClient( m_hwListView, &hti.pt ); 
                LView_SubItemHitTest( m_hwListView, &hti );
                if (m_Column[hti.iSubItem].IsGroupChange()){
                  if (GetRowByItem( pLV->iItem )->IsSelected()== false){
                    UnselectRows();
                  }
                }else{
                  UnselectRows();
                }
                pRow->SetSelect( true );
              }
              m_iCurRow = pLV->iItem;
              m_pCurRow = GetRowByItem( pLV->iItem );
            }
          }
          if (m_pCurRow != pCurRow){
            if (m_FireSelRow){
              (*m_FireSelRow)( this, m_pCurRow );
            }
          }
          break;

        case LVN_ITEMCHANGING:
          pLV = (LPNMLISTVIEW) lParam; 
          memset( &hti, 0, sizeof(hti) );
          ::GetCursorPos( &hti.pt );
          ::ScreenToClient( m_hwListView, &hti.pt ); 
          LView_SubItemHitTest( m_hwListView, &hti );
          if (m_Column[hti.iSubItem].IsGroupChange()){
            return FALSE;
          }
          bCtrl  = (::GetKeyState( VK_CONTROL )<0)? true : false;
          if (bCtrl){ 
            if (pLV->uNewState & LVIS_FOCUSED){
              pRow = GetRowByItem( pLV->iItem );
              if (pRow->IsSelected()){
                pRow->SetSelect( false );
              }else{
                pRow->SetSelect( true );
              }
              return TRUE;
            }
          }
          return FALSE;

        case LVN_KEYDOWN:
          pKey = (LPNMLVKEYDOWN) lParam; 
          if (pKey->wVKey == 0x41){   // A
            bCtrl  = (::GetKeyState( VK_CONTROL )<0)? true : false;
            if (bCtrl){    // Ctrl + A  - Select all
              SelectRows();
              ::InvalidateRect( m_hwListView, NULL, false );
            }
          }
          switch( pKey->wVKey ){
            case VK_LEFT:
              if (m_iCurColumn > 0){
                --m_iCurColumn;
                ::InvalidateRect( m_hwListView, NULL, false );
              }
              break;
            case VK_RIGHT:
              if (m_iCurColumn < (m_nColumns-1)){
                ++m_iCurColumn;
                ::InvalidateRect( m_hwListView, NULL, false );
              }
              break;
            case VK_INSERT:
              if (m_FireInsert){
                (*m_FireInsert)( this, m_pCurRow );
              }
              break;
            case VK_DELETE:
              if (m_FireDelete){
                pCurRow = m_pCurRow;
                m_pCurRow = NULL;
                (*m_FireDelete)( this, pCurRow );
              }
              break;
            case VK_NEXT:
            case VK_PRIOR:
            case VK_TAB:
            case VK_DOWN:
              if (m_FireKey){
                if ((*m_FireKey)( this, m_pCurRow, m_Column[m_iCurColumn].ID(), pKey->wVKey )){
                  return TRUE;
                }
              }
              break;
            case VK_RETURN:  
              if (m_pCurRow){
                if (m_pCurRow->m_bReadOnly){
                  break;
                }
              }
              if (m_FireEnter){
                if (m_Column[m_iCurColumn].VarType() == OK_GRID_VAR_LIST){
                  // create combobox control
                  Cbox_Create( true );
                  return TRUE;
                }
                bEdit = false;
                (*m_FireEnter)( this, m_pCurRow, m_Column[m_iCurColumn].ID(), &bEdit );
                if (bEdit){
                  // edit this cell
                  StartEdit();
                }else{
                  ::SetFocus( m_hwListView );
                }
              }else{ 
                StartEdit();    // start to edit item m_cItem
              }
              break;
          }
          break;

        case LVN_BEGINDRAG:
          if (m_hDropWnd){
            pLV = (LPNMLISTVIEW) lParam; 
            OnBeginDrag( pLV );
          }
          break;
      }
      break;
  }
  return ::DefWindowProc( m_hwFrame, (UINT)Msg, wParam, lParam );
}


//-----------------------------------------------
void COkCtrlGrid::Update ()
{
  COkTRow*  pRow;
  WCHAR     szBuf[OK_GRID_BUFLEN+2];
  WCHAR     szAst[8]; 
  LVCOLUMNW lvc, lvc2;
  LVITEMW   lvi;
  int       i, j;
  HDITEM    hdi;
  UINT      Style = LVS_EX_FULLROWSELECT;

  memset( &lvc, 0, sizeof(lvc) );
  memset( &lvc2, 0, sizeof(lvc2) );
  wcscpy( szAst, L"!" ); 
  // create columns
  lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
  lvc2.mask = LVCF_WIDTH;
  for (i=0; i<m_nColumns; ++i){
    m_Column[i].GetName( szBuf );
    lvc.pszText = szBuf;
    lvc.cx = (int)(m_Column[i].Width());
    lvc.iSubItem = 0;
    if (LView_GetColumn( m_hwListView, i, &lvc2 )){
      LView_SetColumn( m_hwListView, i, &lvc );
    }else{
      LView_InsertColumn( m_hwListView, i, &lvc );
    }
  }
  LView_SetExtendedListViewStyle( m_hwListView, Style  );

  // fill rows
  i = 0;
  memset( &lvi, 0, sizeof(lvi) );
  LView_DeleteAllItems( m_hwListView );
  pRow = GetFirstRow();
  while( pRow ){
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = i + 1; // to add item at the end of list-view
    lvi.iSubItem = 0;
    lvi.lParam = (LPARAM)pRow;
    lvi.pszText = szAst;
    LView_InsertItem( m_hwListView, &lvi );
    for (j=0; j<m_nColumns; ++j){
      LView_SetItemText( m_hwListView, i, j, szAst ); 
    }
    ++i;
    pRow = GetNextRow( pRow );
  }
  if (m_iSortColumn >= 0){
    LView_SortItems( m_hwListView, GridCompareFunc, (LPARAM)this );
  }

  if (m_hwHeader){
    for (i=0; i<m_nColumns; ++i){
      memset( &hdi, 0, sizeof(hdi) );
      hdi.mask = HDI_BITMAP | HDI_FORMAT;
      hdi.fmt = HDF_STRING | HDF_LEFT;
      if (i == m_iSortColumn){
        // add an image to a header of a sorted column 
        hdi.fmt = HDF_LEFT | HDF_STRING | HDF_BITMAP| HDF_BITMAP_ON_RIGHT;
        hdi.hbm = m_bSortUp? m_hBmpSortUp : m_hBmpSortDown;
      }
      Header_SetItem( m_hwHeader, i, &hdi );
    }
  }
}



static HHOOK        _hHook = NULL;
static COkCtrlGrid* _pGrid = NULL;
static int          _KeyCounter = 0;


//*******************************************************************
//  HOOK PROCEDURE
//*******************************************************************
static LRESULT CALLBACK GridWndHookProc (
  int code,        // hook code
  WPARAM wParam,   // virtual-key code
  LPARAM lParam)   // keystroke-message information
{
  if (code < 0){   
    // do not process message 
    return ::CallNextHookEx( _hHook, code, wParam, lParam ); 
  }
  ++_KeyCounter;
  if (_KeyCounter > 1){
    if (_pGrid->IsEdit()){
      if (wParam==VK_RETURN || 
          wParam==VK_UP || 
          wParam==VK_DOWN ||
          wParam==VK_TAB ||
          wParam==VK_ESCAPE)
      {
        _pGrid->WinProc( WM_KEYDOWN, wParam, lParam );
        return true;
      }
    }else
    if ( _pGrid->IsCbox()){
      if (wParam==VK_RETURN || 
          wParam==VK_TAB ||
          wParam==VK_ESCAPE)
      {
        _pGrid->WinProc( WM_KEYDOWN, wParam, lParam );
        return true;
      }
    }
  }
  return ::CallNextHookEx( _hHook, code, wParam, lParam ); 
}

//-----------------------------------------------
void _SetKbHook (BOOL b, COkCtrlGrid* pGrid)
{
  DWORD idThread=0;
  _pGrid = pGrid;
  _KeyCounter = 0;
  if (b){
    // set message hook
    idThread = ::GetCurrentThreadId();
    if (_hHook){
      ::UnhookWindowsHookEx( _hHook );
    }
    _hHook = ::SetWindowsHookEx( WH_KEYBOARD, (HOOKPROC)GridWndHookProc, pGrid->Inst(), idThread ); 
  }else{
    // disable the hook
    if (_hHook){
      ::UnhookWindowsHookEx( _hHook );
      _hHook = NULL;
      _pGrid = NULL;
    }
  }
}


