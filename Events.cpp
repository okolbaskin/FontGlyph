/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* class CFontGlyph
********************************************************************/
#include <stdio.h>
#include "framework.h"
#include "FontGlyph.h"

//-----------------------------------------------
int CFontGlyph::OnMouseMove ()
{
  CGePoint Pnt;
  double x, y;
  m_pGWnd->GetCursorCoords( NULL, &Pnt );
  x = Pnt.x / m_FontHeight;
  y = Pnt.y / m_FontHeight;
  m_Info.SetCoords( x, y, m_szSnapInfo );
  return 0;
}

//-----------------------------------------------
int CFontGlyph::OnRButtonDown ()
{
  POINT   wpt;
  HWND    hWnd;
  WCHAR   szBuf[64];
  CGeRect Rect;
  UINT    Flags, Cmd;
  HMENU   hMenu = ::CreatePopupMenu();
  if (hMenu){
    wcscpy( szBuf, L"Zoom Cell" );
    ::AppendMenu( (HMENU)hMenu, MF_STRING, ID_ZOOMFONT,  szBuf );
    wcscpy( szBuf, L"Zoom Extents" );
    ::AppendMenu( (HMENU)hMenu, MF_STRING, ID_ZOOMEXT,  szBuf );
    wcscpy( szBuf, L"Zoom Frame" );
    ::AppendMenu( (HMENU)hMenu, MF_STRING, ID_ZOOMRECT,  szBuf );
    wcscpy( szBuf, L"Zoom Previous" );
    if (m_pGWnd->HasZoomPrev()){
      Flags = MF_STRING | MF_ENABLED;
    }else{
      Flags = MF_STRING | MF_DISABLED;
    }
    ::AppendMenu( (HMENU)hMenu, Flags, ID_ZOOMPREV,  szBuf );
    m_pGWnd->GetCursorCoords( &wpt, NULL );
    hWnd = m_pGWnd->GetHWND();

    // display menu
    ::ClientToScreen( hWnd, &wpt );
    Flags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY;
    Cmd = ::TrackPopupMenu( hMenu, Flags, wpt.x, wpt.y, 0, hWnd, NULL );
    ::DestroyMenu( hMenu );
    switch( Cmd ){
      case ID_ZOOMFONT: ZoomCharCell(true); break;
      case ID_ZOOMEXT:  m_pGWnd->ZoomExtents(); break;
      case ID_ZOOMRECT: m_pGWnd->ZoomFrame(); break;
      case ID_ZOOMPREV: m_pGWnd->ZoomPrevious(); break;
      default: 
        m_pGWnd->Invalidate();
        break;
    }
  }
  return 0;
}

//-----------------------------------------------
int CFontGlyph::OnMouseSnap ()
{
  int    i, iPline, iPnt, nPnts;
  double Xmin, Xmax, Ymin, Ymax, x, y;
  CGePoint CurPnt;
  GLYPHPOINT* pVer;
  double Delta;

  m_szSnapInfo[0] = 0;
  if (m_nVers > 0 && m_bShowCtrlPoints){
    Delta = m_pGWnd->PixelSize() * 4.0;
    m_pGWnd->GetCursorCoords( NULL, &CurPnt );
    Xmin = CurPnt.x - Delta;
    Xmax = CurPnt.x + Delta;
    Ymin = CurPnt.y - Delta;
    Ymax = CurPnt.y + Delta;
    iPline = 0;
    iPnt = 0;
    nPnts = m_PLSize[iPline];
    pVer = m_Ver;
    for (i=0; i<m_nVers; ++i){
      x = pVer->x;
      y = pVer->y;
      if ((Xmin < x) && (x < Xmax) && (Ymin < y) && (y < Ymax)){
        swprintf( m_szSnapInfo, L"(Point %d of polyline %d)", iPnt, iPline );
        // set new cursor coordinates
        CurPnt.Set( x, y );
        m_pGWnd->SetCursorCoords( CurPnt );
        // exit the cycle
        return 1;
      }
      ++iPnt;
      if (iPnt >= nPnts){
        ++iPline;
        nPnts = m_PLSize[iPline];
        iPnt = 0;
      }
      ++pVer;
    }
  }
  return 0;
}

//-----------------------------------------------
void CFontGlyph::OnSelectFont (int Cmd)
{
  int   i;
  POINT Pt;
  HMENU hPopupMenu;

  m_bNoZoom = true;
  if (Cmd){
    FileOpenTTF( m_FontName[m_iFont].Name(), m_FontName[m_iFont].FileName(), Cmd );
  }else{
    i = (int)::SendMessage( m_hListFonts, LB_GETCURSEL, 0, 0 );
    if (i != LB_ERR){
      m_iFont = i;
      if (m_FontName[i].Type() == FONT_TYPE_TTF){
        // call popup menu
        hPopupMenu = ::CreatePopupMenu();
        if (hPopupMenu){
          ::AppendMenu( hPopupMenu, MF_STRING, TTF_REGULAR   , L"Regular" );
          ::AppendMenu( hPopupMenu, MF_STRING, TTF_BOLD      , L"Bold" );
          ::AppendMenu( hPopupMenu, MF_STRING, TTF_ITALIC    , L"Italic" );
          ::AppendMenu( hPopupMenu, MF_STRING, TTF_BOLDITALIC, L"Bold Italic" );
          // display menu
          ::GetCursorPos( &Pt );
          ::TrackPopupMenu( hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, Pt.x, Pt.y, 0, m_hwMain, NULL );
          ::DestroyMenu( hPopupMenu );
        }
      }
    }
  }
}

//-----------------------------------------------
void CFontGlyph::OnSelectSubset (CSubsetRow* pRow)
{
  int nc, i;

  nc = pRow->UnicodeSubset()->nUsedCodes;
  m_nSbarLines = (nc / NUM_CHARSROW) + 1;   // number of rows in the "Chars" listbox
  m_iCurSubChar = 0;
  ::SendMessage( m_hListChars, LB_RESETCONTENT, 0, 0 );
  for (i=0; i<m_nSbarLines; ++i){
    ::SendMessage( m_hListChars, LB_ADDSTRING, 0, (LPARAM)L"!" );
  }
  m_CharCode = GetCharCode( pRow, m_iCurSubChar );
  GetGlyph_TTF( m_CharCode );
  m_Info.SetSubset( pRow->UnicodeSubset() );
  UpdateInfoChar();
  ::SendMessage( m_hListChars, LB_SETCURSEL, 0, 0 );
  ::InvalidateRect( m_hListChars, NULL, false );
}


//-----------------------------------------------
void CFontGlyph::OnSelectChar ()
{
  CSubsetRow* pRow = (CSubsetRow*)m_Subsets.GetRow();  // current Unicode subset
  POINT    Pt;
  RECT     Rect;
  UINT     j, k;
  int      x1, x2, iCode;
  int      i = (int)::SendMessage( m_hListChars, LB_GETCURSEL, 0, 0 );

  if (i != LB_ERR){
    if (pRow == NULL){
      return;
    }
    // find char index in the Unicode subset by mouse click position 
    iCode = -1;
    if (::GetCursorPos( &Pt )){
      if (::ScreenToClient( m_hListChars, &Pt )){
        ::GetClientRect( m_hListChars, &Rect );
        x1 = Rect.left;
        for (j=0; j<NUM_CHARSROW; ++j){
          x2 = x1 + m_CellSize;
          if ((x1 < Pt.x) && (Pt.x < x2)){
            k = (i * NUM_CHARSROW) + j;
            if (k < pRow->UnicodeSubset()->nUsedCodes){
              iCode = k;
              break;
            }
          }
          x1 = x2;
        }
      }
    }
    if (iCode < 0){
      return;
    }
    if (m_iCurSubChar == iCode){
      // click on the same char - do nothing
      return;
    }
    m_iCurSubChar = iCode;
    m_CharCode = -1;
    iCode = GetCharCode( NULL, m_iCurSubChar );
    if (iCode >= 0){
      m_CharCode = iCode;
    }
    if (m_CharCode >= 0){
      GetGlyph_TTF( m_CharCode );
    }
  }
  ::InvalidateRect( m_hListChars, NULL, false );
  UpdateInfoChar();
}





//-----------------------------------------------
int CFontGlyph::OnGWndEvent (int EventType)
{
  int RetVal = 0;
  switch( EventType ){
    case GW_EVENT_PAINT:      RetVal = DrawGlyph();  break;
    case GW_EVENT_MOUSEMOVE:  RetVal = OnMouseMove();  break;
    case GW_EVENT_RBDOWN:     RetVal = OnRButtonDown();  break;
    case GW_EVENT_SNAP:       RetVal = OnMouseSnap();  break;
  }
  return RetVal;
}
