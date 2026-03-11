/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class COkCtrlGrid implementation
********************************************************************/
#define _GRID_CPP_
#include "framework.h"
#include <windowsx.h>

void _SetKbHook (BOOL b, COkCtrlGrid* pGrid=NULL);

//*******************************************************************
COkTColumn::COkTColumn ()
{
  m_ID        = 0;
  m_szName    = NULL;
  m_Width     = 1;
  m_VarType   = 0;
  m_bSortable = false;
}

//-----------------------------------------------
COkTColumn::COkTColumn (UINT Id, LPCWSTR szName, UINT Width)
{
  m_ID        = Id;
  m_Width     = Width;
  m_szName    = NULL;
  m_VarType   = OK_GRID_VAR_TEXT;
  m_bSortable = false;
  SetName( szName );
}

//-----------------------------------------------
COkTColumn::~COkTColumn ()
{
  free( m_szName );
}

//-----------------------------------------------
void COkTColumn::SetID (UINT id)
{
  m_ID = id;
}

//-----------------------------------------------
void COkTColumn::SetName (LPCWSTR szName)
{
  size_t Len = wcslen( szName ) + 2;
  if (m_szName){
    free( m_szName );
    m_szName = NULL;
  }
  m_szName = (WCHAR*)(calloc( Len, sizeof(WCHAR) ));
  if ( m_szName){
    wcscpy( m_szName, szName );
  }
}  


//-----------------------------------------------
void COkTColumn::SetWidth (UINT Width)
{
  m_Width = Width;
}

//-----------------------------------------------
void COkTColumn::SetVarType (UINT Type0)
{
  UINT Type = Type0 & 0xFF;
  if (Type == OK_GRID_VAR_TEXT || Type == OK_GRID_VAR_RTEXT ||
      Type == OK_GRID_VAR_LIST ||
      Type == OK_GRID_VAR_OWNERDRAW)
  {
    m_VarType = Type0;
  }
}

//-----------------------------------------------
void COkTColumn::SetSortable (bool bSort)
{
  m_bSortable = bSort;
}


//-----------------------------------------------
void COkTColumn::GetName (WCHAR* szBuf) const
{
  if (m_szName){
    wcscpy( szBuf, m_szName );
  }else{
    szBuf[0] = 0;
  }
}


//*******************************************************************
COkTRow::COkTRow (UINT_PTR id) : COkObject (id,0,0)
{
  m_bSelect = false;
  m_bSelect2 = false;
  m_AltBkColor = 0xFFFFFFFF;
  m_AltFgColor = 0xFFFFFFFF;
  m_bReadOnly = false;
}
//-----------------------------------------------
void COkTRow::SetSelect (bool b)
{
  m_bSelect = b;
  if (b == true){
    m_bSelect2 = b;
  }
}




const wchar_t* szClassCtrlGrid = L"OkCtrlGrid";
//-----------------------------------------------
LRESULT CALLBACK ProcCtrlGrid (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  COkCtrlGrid* pGrid = (COkCtrlGrid*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
  if (pGrid){
    return pGrid->WinProc( Msg, wParam, lParam );
  }
  return ::DefWindowProc( hWnd, (UINT)Msg, wParam, lParam);
}


//*******************************************************************
COkCtrlGrid::COkCtrlGrid (UINT MaxCols)
{
  m_hInst        = NULL;
  m_hwFrame      = NULL;
  m_hwListView   = NULL;
  m_hwHeader     = NULL;
  m_hwEdit       = NULL;
  m_hwCbox       = NULL;
  m_WndExStyle   = WS_EX_CLIENTEDGE;
  m_WndStyle     = 0;
  m_iEditItem    = -1;
  m_iEditColumn  = -1;
  memset( &m_EditRect, 0, sizeof(m_EditRect) );
  m_MaxColumns   = MaxCols;
  m_Column       = new COkTColumn[m_MaxColumns];
  m_nColumns     = 0;
  m_nRows        = 0;
  m_iCurRow      = m_iCurColumn = 0;
  m_pCurRow      = NULL;
  m_iSortColumn  = -1;
  m_bSortUp      = true;
  m_hBmpSortDown = NULL;
  m_hBmpSortUp   = NULL;
  m_pDragRow     = NULL;
  m_bDragging    = false;
  m_hDropWnd     = NULL;
  m_hDropWndMsg  = NULL;
  m_DropWParam   = 0;
  m_DropLParam   = 0;
  m_hCursorMain  = NULL;
  m_hCursorNoDrop= NULL;
  m_hCursorDrop  = NULL; 
  m_FireDrawCell = NULL;
  m_FireClick    = NULL;
  m_FireRClick   = NULL;
  m_FireDblClick = NULL;
  m_FireAppend   = NULL;
  m_FireInsert   = NULL;
  m_FireDelete   = NULL;
  m_FireEnter    = NULL;
  m_FireKey      = NULL;
  m_FireSelRow   = NULL;
  m_FireListNum  = NULL;
  m_FireListGet  = NULL;
  m_FireListSet  = NULL;
  m_FireListDraw = NULL;
  m_Param = 0;
}

//-----------------------------------------------
COkCtrlGrid::~COkCtrlGrid ()
{
  delete[] m_Column;
  ::DeleteObject( m_hBmpSortDown );
  ::DeleteObject( m_hBmpSortUp );
}

//-----------------------------------------------
void COkCtrlGrid::SetWndStyle (INT_PTR Style, INT_PTR ExStyle)
{
  m_WndStyle   = (DWORD)Style;
  m_WndExStyle = (DWORD)ExStyle;
}

//-----------------------------------------------
bool COkCtrlGrid::Create (INT_PTR X, INT_PTR Y, INT_PTR W, INT_PTR H, HWND hwParent, INT_PTR ID, HINSTANCE hInst, bool bSortable)
{
  bool  bOK = false;
  DWORD ExStyle = WS_EX_CLIENTEDGE;
  DWORD Style = WS_CHILD | WS_VISIBLE;
  WNDCLASSEXW wc;
  COLORREF BmpUp[64], BmpDown[64], Clr;
  INT_PTR  i;
  INT_PTR  up1[7]={3, 18,20, 33,37, 48,54};
  INT_PTR  up2[25]={11, 19, 26,27,28, 34,35,36, 41,42,43,44,45, 49,50,51,52,53, 56,57,58,59,60,61,62 };
  INT_PTR  down1[7]={8,14, 25,29, 42,44, 59};
  INT_PTR  down2[25]={0,1,2,3,4,5,6, 9,10,11,12,13, 17,18,19,20,21, 26,27,28, 34,35,36, 43, 51};

  m_hInst = hInst;
  memset( &wc, 0, sizeof(WNDCLASSEX) );
  memset( BmpUp, 0, sizeof(BmpUp) );
  memset( BmpDown, 0, sizeof(BmpDown) );
  // Check if the class is already registered
  BOOL gc = ::GetClassInfoEx( hInst, szClassCtrlGrid, &wc );
  if (gc){
    bOK = true;
  }else{
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpszClassName = szClassCtrlGrid;
//    wc.style         = CS_OWNDC | CS_BYTEALIGNCLIENT;
    wc.hInstance     = hInst;
    wc.lpfnWndProc   = (WNDPROC)ProcCtrlGrid;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = ::LoadCursor( NULL, IDC_ARROW );
    wc.cbClsExtra    = NULL;
    wc.cbWndExtra    = NULL;
    wc.hIconSm       = NULL;
    if (::RegisterClassEx( &wc )){
      bOK = true;
    }
  }
  if (bOK){
    // create a frame window
    ExStyle = 0; //m_WndExStyle;
    Style = WS_CHILD | WS_VISIBLE;
    m_hwFrame = ::CreateWindowEx( ExStyle, szClassCtrlGrid, L"", Style, (int)X, (int)Y, (int)W, (int)H, 
                                   hwParent, (HMENU)ID, hInst, NULL );
    if (m_hwFrame){
      ::SetWindowLongPtr( m_hwFrame, GWLP_USERDATA, (LONG_PTR)this );
      // ListView control
      ExStyle = m_WndExStyle;
      Style = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;
      if (m_WndStyle & WS_BORDER){
        Style |= WS_BORDER;
      }
      if (bSortable == false){ //idBmpUp==0 && idBmpDown==0){
        Style |= LVS_NOSORTHEADER;
      }
      m_hwListView = ::CreateWindowEx( ExStyle, WC_LISTVIEWW, L"", Style, 0, 0, (int)W, (int)H, 
                                        m_hwFrame, (HMENU)ID_LISTVIEW, hInst, NULL );
      if (m_hwListView){
        m_hwHeader = LView_GetHeader( m_hwListView );
        Clr = ::GetSysColor( COLOR_3DFACE );
        for (i=0; i<64; ++i){
          BmpUp[i] = Clr;
          BmpDown[i] = Clr;
        }
        Clr = RGB(128,128,128);
        for (i=0; i<7; ++i){
          BmpUp[up1[i]] = Clr;
          BmpDown[down1[i]] = Clr;
        }
        Clr = RGB(0,0,0);
        for (i=0; i<25; ++i){
          BmpUp[up2[i]] = Clr;
          BmpDown[down2[i]] = Clr;
        }
        m_hBmpSortUp    = ::CreateBitmap( 8, 8, 1, 32, BmpUp );
        m_hBmpSortDown  = ::CreateBitmap( 8, 8, 1, 32, BmpDown );
        m_hCursorNoDrop = ::LoadCursor( NULL, IDC_NO );
        m_hCursorDrop   = ::LoadCursor( NULL, IDC_HAND );
        return true;
      }
    }
  }
  return false;
}


//-----------------------------------------------
void COkCtrlGrid::MoveWindow (INT_PTR X, INT_PTR Y, INT_PTR W, INT_PTR H) const
{
  if (m_hwFrame){
    ::MoveWindow( m_hwFrame, (int)X, (int)Y, (int)W, (int)H, false );
    ::MoveWindow( m_hwListView, 0, 0, (int)W, (int)H, true );
  }
}


//-----------------------------------------------
void COkCtrlGrid::Redraw () const
{
  ::InvalidateRect( m_hwListView, NULL, true );
}


//-----------------------------------------------
void COkCtrlGrid::SetFocus () const
{
  ::SetFocus( m_hwListView );
}

//-----------------------------------------------
void COkCtrlGrid::SetDropWindow (HWND hWnd, HWND hWndMsg, WPARAM wParam, LPARAM lParam)
{
  m_hDropWnd    = hWnd;
  m_hDropWndMsg = hWndMsg;
  m_DropWParam  = wParam;
  m_DropLParam  = lParam;
}

//-----------------------------------------------
void COkCtrlGrid::SetDropCursor (HCURSOR hcurDrop)
{
  m_hCursorDrop = hcurDrop;
}

//-----------------------------------------------
void COkCtrlGrid::SetParam (LPARAM Param)
{
  m_Param = Param;
}

//-----------------------------------------------
bool COkCtrlGrid::SetVisible (bool bVisible)
{
  LONG_PTR Style = ::GetWindowLongPtr( m_hwFrame, GWL_STYLE );
  if (bVisible){
    if (Style & WS_VISIBLE){
      return false;
    }
    Style |= WS_VISIBLE;
  }else{
    if (Style & WS_VISIBLE){
      Style &= ~WS_VISIBLE;
      SelectRows( false );
    }else{
      return false;
    }
  }
  ::SetWindowLongPtr( m_hwFrame, GWL_STYLE, Style );
  return true;
}

//-----------------------------------------------
bool COkCtrlGrid::IsVisible () const
{
  LONG_PTR Style = ::GetWindowLongPtr( m_hwFrame, GWL_STYLE );
  if (Style & WS_VISIBLE){
    return true;
  }
  return false;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_DrawCell (OK_FUN_GRIDDRAWCELL pFunc)
{
  m_FireDrawCell = pFunc;
}


//-----------------------------------------------
void COkCtrlGrid::OnEvent_Click (OK_FUN_GRIDCLICK pFunc)
{
  m_FireClick = pFunc;
}


//-----------------------------------------------
void COkCtrlGrid::OnEvent_RClick (OK_FUN_GRIDRCLICK pFunc)
{
  m_FireRClick = pFunc;
}


//-----------------------------------------------
void COkCtrlGrid::OnEvent_DblClk (OK_FUN_GRIDDBLCLK pFunc)
{
  m_FireDblClick = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_Append (OK_FUN_GRIDAPPEND pFunc)
{
  m_FireAppend = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_Insert (OK_FUN_GRIDINSERT pFunc)
{
  m_FireInsert = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_Delete (OK_FUN_GRIDDELETE pFunc)
{
  m_FireDelete = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_Enter (OK_FUN_GRIDENTER pFunc)
{
  m_FireEnter = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_Key (OK_FUN_GRIDKEY pFunc)
{
  m_FireKey = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_SelRow (OK_FUN_GRIDSELROW pFunc)
{
  m_FireSelRow = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_ListNum (OK_FUN_GRIDLINUM pFunc)
{
  m_FireListNum = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_ListGet (OK_FUN_GRIDLIGET pFunc)
{
  m_FireListGet = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_ListSet (OK_FUN_GRIDLISET pFunc)
{
  m_FireListSet = pFunc;
}

//-----------------------------------------------
void COkCtrlGrid::OnEvent_ListDraw (OK_FUN_GRIDLIDRAW pFunc)
{
  m_FireListDraw = pFunc;
}



//-----------------------------------------------
void COkCtrlGrid::OnDrawRaw (DRAWITEMSTRUCT* pDis)
{
  static bool bEnabled = true;
  WCHAR    szBuf[OK_GRID_BUFLEN+2];
  int      i, w, x0, y0, rw, rh;
  RECT     rect, rect2, rcItem;
  POINT    Pt[5];
  HRGN     hRgn = 0;
  HPEN     hPen = 0;
  HPEN     hPenLine = 0;
  HBRUSH   hBrush = 0;
  COLORREF BkColor, FgColor, PrevTextColor;
  int      PrevBkMode, PrevAlign;
  HGDIOBJ  hPrevFont, hPrevPen;
  HDC      hDC = pDis->hDC;
  bool     bSelected = (pDis->itemState & ODS_SELECTED)? true : false;
  COkTRow* pRow;
  HBITMAP  hBmp;
  HGDIOBJ  hPrevBmp;

  if (::IsWindowEnabled( m_hwListView )){
    bEnabled = true;
  }else{
    if (bEnabled){
      // paint background
      ::GetClientRect( m_hwListView, &rect );
      hBrush = ::CreateSolidBrush( RGB(255,255,255 ) );
      ::FillRect( hDC, &rect, hBrush );
      ::DeleteObject( hBrush );
      hBrush = 0;
    }
    bEnabled = false;
  }

  x0 = pDis->rcItem.left;
  y0 = pDis->rcItem.top;
  rw = pDis->rcItem.right - x0 + 1;
  rh = pDis->rcItem.bottom - y0 + 1;
  rcItem.left = rcItem.top = 0;
  rcItem.right = rw - 1;
  rcItem.bottom = rh - 1;
  hBmp = ::CreateCompatibleBitmap( pDis->hDC, rw+2, rh+2 );
  hDC = ::CreateCompatibleDC( pDis->hDC );
  hPrevBmp = ::SelectObject( hDC, hBmp );

  if (pDis->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)){
    if (IsEdit() == false){
      m_iEditItem = -1;
      m_iEditColumn = -1;
    }
    pRow = GetRowByItem( pDis->itemID );
    pRow->UpdateSelect();
    if (bSelected){
      BkColor = RGB( 10, 36, 106 );
      FgColor = RGB( 255, 255, 255 );
    }else{
      if (pRow->IsSelected()){
        BkColor = RGB( 20, 46, 116 );
        FgColor = RGB( 255, 255, 255 );
      }else{
        if (pRow->m_AltBkColor == 0xFFFFFFFF){
          BkColor = RGB( 255, 255, 255 );
        }else{
          BkColor = pRow->m_AltBkColor;
        }
        if (pRow->m_AltFgColor == 0xFFFFFFFF){
          FgColor = RGB( 0, 0, 0 );
        }else{
          FgColor = pRow->m_AltFgColor;
        }
      }
    }
    hPen = ::CreatePen( PS_SOLID, 1, FgColor ); //RGB(255,0,0) );
    hPenLine = ::CreatePen( PS_SOLID, 1, RGB(191,192,193) ); 
    hPrevPen = ::SelectObject( hDC, hPen );
    // clear background
    hBrush = ::CreateSolidBrush( BkColor );
    rect = rcItem;
    ++(rect.right);
    ++(rect.bottom);
    ::FillRect( hDC, &rect, hBrush );
    ::DeleteObject( hBrush );
    // set text parameters
    PrevBkMode    = ::SetBkMode( hDC, TRANSPARENT );
    hPrevFont     = ::SelectObject( hDC, ::GetStockObject(DEFAULT_GUI_FONT) );
    PrevTextColor = ::SetTextColor( hDC, FgColor );
    PrevAlign     = ::SetTextAlign( hDC, TA_LEFT | TA_TOP );

    memset( szBuf, 0, sizeof(szBuf) );
    rect = rcItem;
    for (i=0; i<m_nColumns; ++i){
      w = (int)LView_GetColumnWidth( m_hwListView, i );
      m_Column[i].SetWidth( w );
      rect.right = rect.left + w;
      rect2 = rect;
      ++(rect2.left);
      --(rect2.right);
      ++(rect2.top);
      --(rect2.bottom);
      // draw a rect around current item in a row
      if (i == m_iCurColumn && pRow == m_pCurRow){
        Pt[0].x = Pt[3].x = rect2.left;
        Pt[0].y = Pt[1].y = rect2.top;
        Pt[1].x = Pt[2].x = rect2.right-1;
        Pt[2].y = Pt[3].y = rect2.bottom-1;
        Pt[4] = Pt[0];
        ::Polyline( hDC, Pt, 5 );
      }else{
        ::SelectObject( hDC, hPenLine );
        Pt[0].x = Pt[1].x = rect.right-1;
        Pt[0].y = rect.top-1;
        Pt[1].y = rect.bottom+1;
        ::Polyline( hDC, Pt, 2 );
        ::SelectObject( hDC, hPen );
      }
      if (((int)(pDis->itemID) != m_iEditItem) || (i != m_iEditColumn)){
        // set clipping
        hRgn = ::CreateRectRgnIndirect( &rect2 );
        ::SelectClipRgn( hDC, hRgn );
        // draw item
        switch( m_Column[i].VarType() ){
          case OK_GRID_VAR_TEXT:
            ::SetTextAlign( hDC, TA_LEFT | TA_TOP );
            pRow->GetValue( this, m_Column[i].ID(), szBuf, OK_GRID_BUFLEN-1 );
            szBuf[OK_GRID_BUFLEN-1] = 0;
            ::TextOut( hDC, rect2.left+3, rect2.top+1, szBuf, (int)(wcslen(szBuf)) );
            break;
          case OK_GRID_VAR_RTEXT:
            ::SetTextAlign( hDC, TA_RIGHT | TA_TOP );
            pRow->GetValue( this, m_Column[i].ID(), szBuf, OK_GRID_BUFLEN-1 );
            szBuf[OK_GRID_BUFLEN-1] = 0;
            ::TextOut( hDC, rect2.right-3, rect2.top+1, szBuf, (int)(wcslen(szBuf)) );
            break;
          case OK_GRID_VAR_LIST:
            ::SetTextAlign( hDC, TA_LEFT | TA_TOP );
            pRow->GetValue( this, m_Column[i].ID(), szBuf, OK_GRID_BUFLEN-1 );
            szBuf[OK_GRID_BUFLEN-1] = 0;
            ::TextOut( hDC, rect2.left+3, rect2.top+1, szBuf, (int)(wcslen(szBuf)) );
            break;
          case OK_GRID_VAR_OWNERDRAW:
            if (m_FireDrawCell){
              (*m_FireDrawCell)( hDC, rect2, this, pRow, m_Column[i].ID() );
            }
            break;
        }
        // delete clipping
        ::SelectClipRgn( hDC, NULL );
        ::DeleteObject( hRgn );
      }
      rect.left = rect.right;
    }

    // copy drawing's view from a memory to a window
    ::BitBlt( pDis->hDC, (int)x0, (int)y0, (int)rw, (int)rh, hDC, 0,0, SRCCOPY );

    // restore values
    ::SetBkMode( hDC, (int)PrevBkMode );
    ::SelectObject( hDC, hPrevFont );
    ::SetTextColor( hDC, PrevTextColor );
    ::SetTextAlign( hDC, (int)PrevAlign );
    ::SelectObject( hDC, hPrevPen );
    ::DeleteObject( hPen );
    ::DeleteObject( hPenLine );
    ::SelectObject( hDC, hPrevFont );

    ::SelectObject( hDC, hPrevBmp );
    ::DeleteObject( hBmp );
    // delete memory DC
    ::DeleteDC( hDC );

    if (IsEdit()){
      ::InvalidateRect( m_hwEdit, NULL, false );
    }
  }
}

//-----------------------------------------------
void COkCtrlGrid::OnDrawListItem (DRAWITEMSTRUCT* pDis)
{
  UINT     idColumn = m_Column[m_iCurColumn].ID();
  bool     bSelected = (pDis->itemState & ODS_SELECTED)? true : false;
  int      PrevBkMode, PrevAlign, Value;
  COLORREF BkColor, FgColor, PrevTextColor;
  HBRUSH   hBrush;
  HRGN     hRgn;
  HDC      hDC = pDis->hDC;
  RECT     Rect = pDis->rcItem;

  if (pDis->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)){
  if (bSelected){
    BkColor = RGB( 10, 36, 106 );
    FgColor = RGB( 255, 255, 255 );
  }else{
    BkColor = RGB( 255, 255, 255 );
    FgColor = RGB( 0, 0, 0 );
  }
  PrevBkMode    = ::SetBkMode( hDC, TRANSPARENT );
  PrevTextColor = ::SetTextColor( hDC, FgColor );
  PrevAlign     = ::SetTextAlign( hDC, TA_LEFT | TA_TOP );
  // set clipping
  hRgn = ::CreateRectRgnIndirect( &Rect );
  ::SelectClipRgn( hDC, hRgn );
  // fill background
  hBrush = ::CreateSolidBrush( BkColor );
  ::FillRect( hDC, &Rect, hBrush ); 
  ::DeleteObject( hBrush );

  Value = (int)::SendMessage( m_hwCbox, CB_GETITEMDATA, pDis->itemID, 0 );
  (*m_FireListDraw)( this, m_pCurRow, idColumn, Value, hDC, Rect, bSelected );

  // delete clipping
  ::SelectClipRgn( hDC, NULL );
  ::DeleteObject( hRgn );
  // restore values
  ::SetBkMode( hDC, PrevBkMode );
  ::SetTextColor( hDC, PrevTextColor );
  ::SetTextAlign( hDC, PrevAlign );
  }
}


// WM_KEYDOWN
//-----------------------------------------------
void COkCtrlGrid::OnKeyDown (WPARAM KeyCode, LPARAM /*KeyData*/)
{
  COkTRow* pRow;
  if (::IsWindow(m_hwEdit)){
    switch( KeyCode ){
      case VK_UP:      
        StopEdit( true );
        pRow = GetPrevRow( m_pCurRow );
        if (pRow){
          SelectRow( pRow );
        }
        break;
      case VK_DOWN:
        StopEdit( true );
        pRow = GetNextRow( m_pCurRow );
        if (pRow){
          SelectRow( pRow );
        }
        break;
      case VK_RETURN:  
        StopEdit( true );
        return;
      case VK_ESCAPE:  
        StopEdit( false );   // don't save changes
        return;
      default:
        return;
    }
  }else{
    if (::IsWindow(m_hwCbox)){
      switch( KeyCode ){
        case VK_TAB:
        case VK_ESCAPE:  
          StopEdit( false );   // don't save changes
          break;
        case VK_RETURN:
          Cbox_Select();
          break;
      }
    }
  }
}

// LVN_BEGINDRAG
//-----------------------------------------------
void COkCtrlGrid::OnBeginDrag (LPNMLISTVIEW pLV) 
{ 
  // Hide the mouse pointer, and direct mouse input to the 
  // parent window. 
  m_hCursorMain  = ::GetCursor();
  ::SetCursor( m_hCursorNoDrop );
  ::SetCapture( m_hwFrame );

  m_pDragRow  = GetRowByItem( pLV->iItem );
  m_bDragging = true;
}


//-----------------------------------------------
void COkCtrlGrid::OnMouseMove (int Xcur, int Ycur) const
{ 
  HWND   hWnd;
  POINT  Pt;
  LPARAM ivar;

  if (m_bDragging){ 
    Pt.x = Xcur;
    Pt.y = Ycur;
    ::ClientToScreen( m_hwFrame, &Pt );
    hWnd = ::WindowFromPoint( Pt );
    if (hWnd == m_hDropWnd){
      ivar = m_DropLParam;
      ::SendMessage( m_hDropWndMsg, WM_USER, m_DropWParam, (LPARAM)&ivar );
      ::SetCursor( (ivar==0)? m_hCursorNoDrop : m_hCursorDrop );
    }
  } 
} 


//-----------------------------------------------
void COkCtrlGrid::OnLButtonUp (int Xcur, int Ycur) 
{ 
  HWND   hWnd;
  POINT  Pt;
  LPARAM ivar;

  if (m_bDragging){
    Pt.x = Xcur;
    Pt.y = Ycur;
    ::ClientToScreen( m_hwFrame, &Pt );
    ::ReleaseCapture(); 
    ::SetCursor( m_hCursorMain );
    hWnd = ::WindowFromPoint( Pt );
    if (hWnd == m_hDropWnd){
      ivar = m_DropLParam + 1;
      ::SendMessage( m_hDropWndMsg, WM_USER, m_DropWParam, (LPARAM)&ivar );
    }
    m_pDragRow = NULL;
    m_bDragging = false;
  } 
}


//-----------------------------------------------
void COkCtrlGrid::ClearColumns ()
{
  m_nColumns = 0;
}

//-----------------------------------------------
bool COkCtrlGrid::AddColumn (UINT Id, LPCWSTR szName, UINT Width, UINT VarType, bool bSortable)
{
  INT_PTR i = m_nColumns;
  if (m_nColumns < m_MaxColumns){
    ++m_nColumns;
    m_Column[i].SetID( Id );
    m_Column[i].SetName( szName );
    m_Column[i].SetWidth( Width );
    m_Column[i].SetVarType( VarType );
    m_Column[i].SetSortable( bSortable );
    return true;
  }
  return false;
}


//-----------------------------------------------
COkTRow* COkCtrlGrid::AddRow (COkTRow* pRowObject) 
{
  COkTRow* pRow = (COkTRow*)m_Rows.Add(pRowObject);
  if (pRow){ 
    ++m_nRows;
  }
  return pRow;
}


//-----------------------------------------------
bool COkCtrlGrid::DeleteRow (COkTRow* pRowObject) 
{
  INT_PTR iItem = GetRowIndex( pRowObject );
  if (iItem >= 0){
    if (pRowObject == m_pCurRow){
      m_pCurRow = NULL;
    }
    LView_DeleteItem( m_hwListView, iItem );
    if (m_Rows.Remove( pRowObject )){
      m_nRows--;
      return true;
    }
  }
  return false;
}


//-----------------------------------------------
void COkCtrlGrid::ClearRows ()
{
  COkTRow* pCurRow = m_pCurRow;
  LView_DeleteAllItems( m_hwListView );
  m_Rows.Clear();
  m_nRows = 0;
  m_pCurRow = NULL;
  m_iCurRow = 0;
  if (m_pCurRow != pCurRow){
    if (m_FireSelRow){
      (*m_FireSelRow)( this, m_pCurRow );
    }
  }
}


//-----------------------------------------------
bool COkCtrlGrid::SelectRow (COkTRow* pRow0)
{
  int i = 0;
  COkTRow* pRow;
  COkTRow* pCurRow = m_pCurRow;
  for (; i<m_nRows; ++i){
    pRow = GetRowByItem( i );
    if (pRow == pRow0){
      m_pCurRow = pRow;
      m_iCurRow = i;
      LView_SetItemState( m_hwListView, i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
      LView_EnsureVisible( m_hwListView, i );
      if (m_pCurRow != pCurRow){
        if (m_FireSelRow){
          (*m_FireSelRow)( this, m_pCurRow );
        }
      }
      return true;
    }
  }
  return false;
}

//-----------------------------------------------
bool COkCtrlGrid::SelectRow0 ()
{
  COkTRow* pRow = GetFirstRow();
  if (pRow){
   return SelectRow( pRow );
  }
  return NULL;
}


//-----------------------------------------------
void COkCtrlGrid::SelectRows (bool b)
{
  COkTRow* pRow = GetFirstRow();
  while( pRow ){
    pRow->SetSelect( b );
    pRow = GetNextRow( pRow );
  }
}


//-----------------------------------------------
void COkCtrlGrid::UnselectRows ()
{
  SelectRows( false );
}


//-----------------------------------------------
bool COkCtrlGrid::SelectColumn (int index)
{
  if (index < m_nColumns){
    m_iCurColumn = index;
    return true;
  }
  return false;
}

//-----------------------------------------------
UINT COkCtrlGrid::SortColumnID () const 
{
  if (m_iSortColumn >= 0){
    return m_Column[m_iSortColumn].ID();
  }
  return 0;
}

//-----------------------------------------------
void COkCtrlGrid::Unsort ()
{
  m_iSortColumn = -1;
}

//-----------------------------------------------
void COkCtrlGrid::SortColumn (int index, bool bSortUp)
{
  if ((0 <= index) && (index < m_nColumns)){
    m_Column[index].SetSortable();
    m_iSortColumn = index;
    m_bSortUp = bSortUp;
  }
}

//-----------------------------------------------
COkTRow* COkCtrlGrid::GetRow (INT_PTR index) const
{
  INT_PTR i = 0;
  COkTRow* pRow = GetFirstRow();
  while( pRow ){
    if (i == index){
      return pRow;
    }
    ++i;
    pRow = GetNextRow( pRow );
  }
  return NULL;
}

//-----------------------------------------------
COkTRow* COkCtrlGrid::GetRowWithID (COkID Id) const
{
  COkTRow* pRow = GetFirstRow();
  while( pRow ){
    if (pRow->ID() == Id){
      return pRow;
    }
    pRow = GetNextRow( pRow );
  }
  return NULL;
}

//-----------------------------------------------
COkTRow* COkCtrlGrid::GetRowByItem (INT_PTR iItem) const
{
  COkTRow* pRow;
  LVITEM  lvi;
  memset( &lvi, 0, sizeof(lvi) );
  lvi.iItem = (int)iItem;
  lvi.mask = (UINT)LVIF_PARAM;
  LView_GetItem( m_hwListView, &lvi );
  pRow = (COkTRow*)(lvi.lParam);
  if (pRow){
    if (pRow->ID() > 1023){
      iItem = 0;
    }
  }
  return pRow;
}

//-----------------------------------------------
INT_PTR COkCtrlGrid::GetRowIndex (const COkTRow* p) const
{
  INT_PTR  i;
  LRESULT  nItems = LView_GetItemCount( m_hwListView );
  LVITEM   lvi;
  COkTRow* pRow;

  memset( &lvi, 0, sizeof(lvi) );
  for (i=0; i<nItems; ++i){
    lvi.iItem = (int)i;
    lvi.mask = (UINT)LVIF_PARAM;
    if (LView_GetItem( m_hwListView, &lvi)){
      pRow = (COkTRow*)(lvi.lParam);
      if (pRow == p){
        return i;
      }
    }
  }
  return -1;  // not found
}



//-----------------------------------------------
COkTColumn* COkCtrlGrid::GetColumn (INT_PTR index) const
{
  if (index < m_nColumns){
    return m_Column + index;
  }
  return NULL;
}



//-----------------------------------------------
void COkCtrlGrid::StartEdit (UINT iItem, UINT iColumn)
{
  COkTRow* pRow;
  DWORD    ExStyle, Style;
  WCHAR    szBuf[OK_GRID_BUFLEN+2];
  int      x, y, w, h;

  pRow = GetRowByItem( iItem );
  memset( szBuf, 0, sizeof(szBuf) );
  pRow->GetValue( this, m_Column[iColumn].ID(), szBuf, OK_GRID_BUFLEN-1 );
  szBuf[OK_GRID_BUFLEN-1] = 0;
  LView_GetSubItemRect( m_hwListView, iItem, iColumn, LVIR_BOUNDS, &m_EditRect );
  x = m_EditRect.left+2;
  y = m_EditRect.top+2;
  w = (int)(m_Column[iColumn].Width()) - 4;
  h = ITEM_HEIGHT-3;
  ExStyle = 0;
  Style = ES_AUTOHSCROLL | ES_LEFT | WS_CHILD | WS_VISIBLE;
  _SetKbHook( true, this );   // in order to catch Enter, Tab, Up, Down, Esc keys in the EDIT control
  m_hwEdit = ::CreateWindowEx( ExStyle, L"EDIT", szBuf, Style, x,y,w,h, m_hwListView, (HMENU)ID_EDITBOX, m_hInst, 0 );
  if (m_hwEdit){
    ::SendMessage( m_hwEdit, EM_SETSEL, 0, -1 );
    ::SendMessage( m_hwEdit, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), true );
    ::ShowWindow( m_hwEdit, SW_SHOW );
    ::UpdateWindow( m_hwEdit );
    ::SetFocus( m_hwEdit );
    m_iEditItem = iItem;
    m_iEditColumn = iColumn;
    ::SetTimer( m_hwFrame, ID_EDITBOX, 100, NULL );
  }
}

// stop to edit item m_cItem
//-----------------------------------------------
void COkCtrlGrid::StopEdit (bool bSave)
{
  COkTRow* pRow;
  WCHAR szBuf[OK_GRID_BUFLEN+2];
  if (IsEdit()){
      ::KillTimer( m_hwFrame, ID_EDITBOX );
      _SetKbHook( false );   // disable keyboard hook
      if (bSave){
        memset( szBuf, 0, sizeof(szBuf) );
        ::SendMessage( m_hwEdit, WM_GETTEXT, OK_GRID_BUFLEN-1, (LPARAM)szBuf );
        szBuf[OK_GRID_BUFLEN-1] = 0;
        pRow = GetRowByItem( m_iEditItem );
        pRow->SetValue( this, m_Column[m_iEditColumn].ID(), szBuf );
      }
      // close edit control
      ::DestroyWindow( m_hwEdit );
      m_hwEdit = NULL;
      m_iEditItem = -1;
      m_iEditColumn = -1;
      Redraw();
  }else{
    if (IsCbox()){
      _SetKbHook( false );   // disable keyboard hook
      ::EnableWindow( m_hwListView, true );
      // close combobox control
      ::DestroyWindow( m_hwCbox );
      m_hwCbox = NULL;
      m_iEditItem = -1;
      m_iEditColumn = -1;
      Redraw();
    }
  }
}

// start to edit a current field
//-----------------------------------------------
void COkCtrlGrid::StartEdit ()
{
  if (m_iCurRow >= 0 && m_iCurColumn >= 0){
    StartEdit( m_iCurRow, m_iCurColumn );
  }
}


//-----------------------------------------------
bool COkCtrlGrid::IsEdit () const
{
  if (::IsWindow( m_hwEdit )){
    return true;
  }
  return false;
}

//-----------------------------------------------
bool COkCtrlGrid::IsCbox () const
{
  if (::IsWindow( m_hwCbox )){
    return true;
  }
  return false;
}

//-----------------------------------------------
void COkCtrlGrid::Cbox_Create (bool bOpen)
{
  COkTRow* pRow;
  WCHAR    szBuf[OK_GRID_BUFLEN+2];
  int      x, y, w;
  int      ExStyle, Style;
  LRESULT  val;
  UINT     i, j, n, idColumn;
  int      iItem = m_iCurRow;
  int      iColumn = m_iCurColumn;
  bool     bCurrent = false;

  if (::IsWindow(m_hwCbox)){
    return;
  }
  m_hwCbox = NULL;
  if ((m_iCurRow < 0) || (m_iCurColumn < 0)){
    return;
  }
  pRow = GetRowByItem( iItem );
  memset( szBuf, 0, sizeof(szBuf) );
  idColumn = m_Column[iColumn].ID();
  pRow->GetValue( this, idColumn, szBuf, OK_GRID_BUFLEN-1 );
  szBuf[OK_GRID_BUFLEN-1] = 0;
  LView_GetSubItemRect( m_hwListView, iItem, iColumn, LVIR_BOUNDS, &m_EditRect );
  x = m_EditRect.left;
  y = m_EditRect.top+1;
  w = (int)(m_Column[iColumn].Width());
  ExStyle = 0;
  Style = CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_CHILD | WS_VSCROLL | WS_VISIBLE; // | CBS_SORT;
  _SetKbHook( true, this );   // in order to catch Esc key in the COMBOBOX control
  m_hwCbox = ::CreateWindowEx( ExStyle, L"COMBOBOX",  L"", Style, x,y-1,w,300, m_hwFrame, (HMENU)ID_CBOX, m_hInst, 0 );
  if (m_hwCbox){
    n = 0;
    if (m_FireListNum){
      (*m_FireListNum)( this, idColumn, &n );
    }
    for (i=j=0; i<n; ++i){
      if (m_FireListGet){
        (*m_FireListGet)( this, pRow, idColumn, i, &val, &bCurrent );
        if (bCurrent){
          j = i;
        }
        ::SendMessage( m_hwCbox, CB_ADDSTRING, 0, (LPARAM)L"!" );  //abcdefgh" );
        ::SendMessage( m_hwCbox, CB_SETITEMDATA, i, val );
      }
    }
    // set current item
    ::SendMessage( m_hwCbox, CB_SETCURSEL, j, 0 );

    ::SendMessage( m_hwCbox, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), true );
    ::EnableWindow( m_hwListView, false );
    ::ShowWindow( m_hwCbox, SW_SHOW );
    ::SetFocus( m_hwCbox );
    if (bOpen){
      // open listbox
      ::SendMessage( m_hwCbox, CB_SHOWDROPDOWN, true, 0 );
    }
    ::InvalidateRect( m_hwCbox, NULL, false );
  }
}

// user clicked item inside of a value combobox
//-----------------------------------------------
void COkCtrlGrid::Cbox_Select ()
{
  COkTRow* pRow = GetRowByItem( m_iCurRow );
  UINT     idColumn;
  LRESULT  Val;
  LRESULT  i = ::SendMessage( m_hwCbox, CB_GETCURSEL, 0, 0 );

  Val = ::SendMessage( m_hwCbox, CB_GETITEMDATA, i, 0 );
  if (m_FireListSet && pRow && m_iCurColumn){
    idColumn = m_Column[m_iCurColumn].ID();
    (*m_FireListSet)( this, pRow, idColumn, Val );
  }
  // close the combobox in order to stop edit
  _SetKbHook( false );   // disable keyboard hook
  ::EnableWindow( m_hwListView, true );
  ::PostMessage( m_hwCbox, WM_CLOSE, 0, 0 );
  SetFocus();
}




/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* class CSubsetRow
********************************************************************/
#include <stdio.h>
#include "FontGlyph.h"


extern USUBSET  g_USubSet[];

//-----------------------------------------------
bool CSubsetRow::GetValue (const COkCtrlGrid* pGrid, UINT idColumn, WCHAR* szValue, UINT MaxChars)
{
  int n;
  switch( idColumn ){
    case 0:  wcscpy( szValue, g_USubSet[m_iRec].szDesc );   break;
    case 1:  swprintf( szValue, L"%d", g_USubSet[m_iRec].nUsedCodes );   break;
    case 2:  swprintf( szValue, L"%04X", g_USubSet[m_iRec].Start );   break;
    case 3:  swprintf( szValue, L"%04X", g_USubSet[m_iRec].End );   break;
    case 4:  
      n = g_USubSet[m_iRec].End - g_USubSet[m_iRec].Start + 1;
      swprintf( szValue, L"%d", n );   
      break;
    default:
      return false;
  }
  return true;
}


//-----------------------------------------------
USUBSET* CSubsetRow::UnicodeSubset() const 
{
  return g_USubSet + m_iRec;
}


//-----------------------------------------------
int CSubsetRow::CompareWith (COkTRow* pRow2, UINT idColumn, bool bSortUp)
{
  int Ret = 0;
  int   n1, n2;
  USUBSET* pSet1 = UnicodeSubset();
  USUBSET* pSet2 = ((CSubsetRow*)pRow2)->UnicodeSubset();

  switch( idColumn ){
    case 0:  // subset name
      Ret = wcscmp( pSet1->szDesc, pSet2->szDesc );
      break;

    case 1:  // number of used codes
      if (pSet1->nUsedCodes > pSet2->nUsedCodes){
        Ret = 1;
      }else{
        if (pSet1->nUsedCodes < pSet2->nUsedCodes){
          Ret = -1;
        }
      }
      break;

    case 2:  // Start
      if (pSet1->Start > pSet2->Start){
        Ret = 1;
      }else{
        if (pSet1->Start < pSet2->Start){
          Ret = -1;
        }
      }
      break;

    case 3:  // End
      if (pSet1->End > pSet2->End){
        Ret = 1;
      }else{
        if (pSet1->End < pSet2->End){
          Ret = -1;
        }
      }
      break;

    case 4:  // Range's size
      n1 = pSet1->End - pSet1->Start + 1;
      n2 = pSet2->End - pSet2->Start + 1;
      if (n1 > n2){
        Ret = 1;
      }else{
        if (n1 < n2){
          Ret = -1;
        }
      }
      break;
    default:
      return 0;
  }
  if (bSortUp == false){
    Ret *= -1;
  }
  return Ret;
}



