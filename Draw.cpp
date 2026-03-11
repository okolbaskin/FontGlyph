/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* class CFontGlyph
********************************************************************/
#include <stdio.h>
#include "framework.h"
#include "FontGlyph.h"


//-----------------------------------------------
void CFontGlyph::DrawItem (int id, DRAWITEMSTRUCT& dis)
{
  switch( id ){
    case ID_LIST_FONTS:
      DrawFontsLine( dis );
      break;
    case ID_LIST_CHARS:
      DrawCharsLine( dis );
      break;
  }
}

//-----------------------------------------------
void CFontGlyph::DrawFontsLine (DRAWITEMSTRUCT& dis) const
{
  HBRUSH     bg;
  COLORREF   PenColor;
  RECT       rect = dis.rcItem;
  int        idBmp;
  const CFontName* pFont = m_FontName + dis.itemID;

  ::SelectObject( dis.hDC, ::GetStockObject(DEFAULT_GUI_FONT) );
  if (dis.itemAction & (ODA_DRAWENTIRE | ODA_SELECT)){
    if (dis.itemState & ODS_SELECTED){
      bg = ::CreateSolidBrush( ::GetSysColor(COLOR_HIGHLIGHT) );
      ::SetTextColor( dis.hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
      PenColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    }else{
      bg = ::CreateSolidBrush( ::GetSysColor(COLOR_WINDOW) );
      ::SetTextColor( dis.hDC, ::GetSysColor(COLOR_WINDOWTEXT) );
      PenColor = ::GetSysColor(COLOR_WINDOWTEXT);
    }
    ::FillRect( dis.hDC, &rect, bg );
    ::SetBkMode( dis.hDC, TRANSPARENT );
    ::SetTextAlign( dis.hDC, TA_LEFT | TA_TOP );
    if (pFont->Type() == FONT_TYPE_TTF){
      idBmp = IDB_FONTWIN;
    }else{
      idBmp = 0;
    }
    okDrawBitmap( m_hInst, dis.hDC, rect.left+2, rect.top+1, idBmp, 15, 15 );
    ::TextOut( dis.hDC, rect.left+20, rect.top+1, pFont->Name(), pFont->NumChars() );

    ::DeleteObject( bg );
  }
}


//-----------------------------------------------
void CFontGlyph::DrawCharsLine (DRAWITEMSTRUCT& dis)
{
  UINT  i, j;
  int   x[NUM_CHARSROW+2], Ymax, Code;
  int   Xmax = dis.rcItem.right;
  int   y = dis.rcItem.top;
  int   h = dis.rcItem.bottom - dis.rcItem.top + 1; 
  RECT  rect, clipRect;
  HRGN  hRgn;
  CSubsetRow* pSubsetRow = (CSubsetRow*)m_Subsets.GetRow();  // current subset row

  if (pSubsetRow == NULL){
    return;
  }
  if (dis.itemAction & (ODA_DRAWENTIRE | ODA_SELECT)){
    ::GetClientRect( m_hListChars, &rect );
    Ymax = rect.bottom - rect.top;
    x[0] = dis.rcItem.left;
    for (i=0; i<NUM_CHARSROW; ++i){
      x[i+1] = x[i] + m_CellSize;
    }
    rect = clipRect = dis.rcItem;
    if (clipRect.bottom > Ymax){
      clipRect.bottom = Ymax;
    }

    for (i=0; i<NUM_CHARSROW; ++i){
      rect.left = clipRect.left = x[i];
      rect.right = clipRect.right = x[i+1];
      hRgn = ::CreateRectRgnIndirect( &clipRect );
      ::SelectClipRgn( dis.hDC, hRgn );
      j = (dis.itemID * NUM_CHARSROW) + i;
      Code = GetCharCode( pSubsetRow, j );
      if (Code){
        if (j == m_iCurSubChar){
          DrawCharCell( dis.hDC, rect, Code, true );
        }else{
          DrawCharCell( dis.hDC, rect, Code, false );
        }
      }
      ::SelectClipRgn( dis.hDC, NULL );
      ::DeleteObject( hRgn );
    }
  }
}


//-----------------------------------------------
void CFontGlyph::DrawCharCell (HDC hDC, RECT Rect, int CharCode, bool bCurrent)
{
  POINT   Pt[5];
  HPEN    hPen;
  HBRUSH  hBrush;
  HGDIOBJ hPrevPen, hPrevBrush, hPrevFont;
  HFONT   hFont;
  WCHAR   szChar[2];
  int     x, y;

  --(Rect.right);
  --(Rect.bottom);

  // select brush
  if (CharCode >= 0){
    hBrush = ::CreateSolidBrush( RGB(255,255,255) );
  }else{
    hBrush = ::CreateSolidBrush( RGB(255,255,255) );
  }
  hPrevBrush = ::SelectObject( hDC, hBrush );

  // select pen
  if (CharCode >= 0){
    if (bCurrent){
      hPen = ::CreatePen( PS_SOLID, 5, RGB(255,0,0) );
    }else{
      hPen = ::CreatePen( PS_SOLID, 1, RGB(128,128,128) );
    }
  }else{
    hPen = ::CreatePen( PS_SOLID, 1, RGB(255,255,255) );
  }
  hPrevPen = ::SelectObject( hDC, hPen );

  // fill background
  Pt[0].x = Pt[1].x = Rect.left;
  Pt[2].x = Pt[3].x = Rect.right;
  Pt[0].y = Pt[3].y = Rect.top;
  Pt[1].y = Pt[2].y = Rect.bottom;
  Pt[4] = Pt[0];  
  ::Polygon( hDC, Pt, 5 );
  ::SelectObject( hDC, hPrevPen );
  ::DeleteObject( hPen );
  ::SelectObject( hDC, hPrevBrush );
  ::DeleteObject( hBrush );

  // draw character
  if (CharCode >= 0){
    //--- TTF ---
    m_LF.lfHeight = (int)((Rect.bottom - Rect.top)*0.78 + 0.5);
    hFont = ::CreateFontIndirect( &m_LF );
    if (hFont){
      ::SetBkMode( hDC, TRANSPARENT	);
      ::SetTextColor( hDC, RGB(0,0,0) );
      ::SetTextAlign( hDC, TA_CENTER | TA_BASELINE );
      hPrevFont = ::SelectObject( hDC, hFont );
      x = (Rect.left + Rect.right) / 2;
      y = Rect.bottom - (Rect.bottom - Rect.top) / 5;
      szChar[0] = CharCode;
      szChar[1] = 0;
      ::TextOut( hDC, x, y, szChar, 1 );
      ::SelectObject( hDC, hPrevFont );
      ::DeleteObject( hFont );
    }
  }
}


//-----------------------------------------------
int CFontGlyph::DrawGlyph ()
{
  double ht, X, Y, X0, Y0;
  int    wx, wy;
  UINT   i, j, n, k;
  GLYPHPOINT* pVer;
  bool        bSpline = false;
  bool        bDup;
  CGePoint    Pnt;
  CGePolyline Pline, Pline2;
  CListPlines Glyph, Glyph2;
  WCHAR       szNum[8];

  HDC hDC = m_pGWnd->GetDC();

  COLORREF   ColorOrig = RGB(185,0,100);
  COLORREF   ColorAxes = RGB(255,0,0);
  COLORREF   ColorRect = RGB(0,0,255);
  COLORREF   ColorLine = RGB(0,0,0);
  COLORREF   ColorFill = RGB(192,192,192);
  COLORREF   ColorPntF = RGB(0,255,255);
  COLORREF   ColorPntB = RGB(0,0,255);
  COLORREF   ColorNums = RGB(0,160,0);
  HPEN       hPen[6];
  HBRUSH     hBrush[3];
  HGDIOBJ    hPrevPen, hPrevBrush;

  // create pens and brushes  
  hPen[0]   = ::CreatePen( PS_DOT  , 1, ColorAxes );
  hPen[1]   = ::CreatePen( PS_SOLID, 1, ColorOrig );
  hPen[2]   = ::CreatePen( PS_SOLID, 1, ColorRect );
  hPen[3]   = ::CreatePen( PS_SOLID, 2, ColorLine );
  hPen[4]   = ::CreatePen( PS_SOLID, 1, ColorPntB );
  hPen[5]   = ::CreatePen( PS_DOT  , 1, ColorPntB );
  hBrush[0] = ::CreateSolidBrush( ColorOrig );
  hBrush[1] = ::CreateSolidBrush( ColorFill );
  hBrush[2] = ::CreateSolidBrush( ColorPntF );

  hPrevPen = ::SelectObject( hDC, hPen[0] );
  hPrevBrush = ::SelectObject( hDC, hBrush[0] );
  // base line (horizontal)
  m_pGWnd->DrawXline( hDC, CGePoint(0,0), 0, false );
  // base line (vertical)
  m_pGWnd->DrawXline( hDC, CGePoint(0,0), GE_DEG90, false );
  // top line
  m_pGWnd->DrawXline( hDC, CGePoint(0,m_FontHeight), 0, false );
  // right vertical line (next char position)
  m_pGWnd->DrawXline( hDC, CGePoint(m_W2,0), GE_DEG90, false );

  // Origin point
  ::SelectObject( hDC, hPen[1] );
  m_pGWnd->DrawPoint( hDC, CGePoint(0,0), LC_POINT_PLUS | LC_POINT_RHOMB | LC_POINT_FILLED, -8 );

  // font's top and bottom lines
  ::SetTextColor( hDC, ColorAxes );
  ::SetTextAlign( hDC, TA_TOP | TA_RIGHT);
  ::SetBkMode( hDC, TRANSPARENT );
  // text "0.0"
  ht = m_FontHeight / 25.0;
  m_pGWnd->CoordViewToWnd( 0.0, -ht/5.0, &wx, &wy );
  ::TextOut( hDC, wx, wy, L"0.0 ", 4 );
  // text "1.0"
  ::SetTextAlign( hDC, TA_BASELINE | TA_RIGHT );
  m_pGWnd->CoordViewToWnd( 0.0, m_FontHeight+ht/5.0, &wx, &wy );
  ::TextOut( hDC, wx, wy, L"1.0 ", 4 );

  // Symbol polygon
  pVer = m_Ver;
  for (i=0; i<m_nPL; ++i){
    Pline.Clear();
    Pline2.Clear();
    n = m_PLSize[i];
    for (j=0; j<n; ++j){
      Pnt.Set( pVer->x, pVer->y );
      Pline.AddVer( Pnt, pVer->bFix?1:0 );
      if (pVer->bFix == 0){
        bSpline = true;
      }
      Pline2.AddVer( Pnt );
      ++pVer;
    }
    if (bSpline){
      Pline.Interpolate( GE_PLFIT_LINQUAD, 15 ); 
    }
    Glyph.Add( &Pline );
    Glyph2.Add( &Pline2 );  // without spline segments
  }
  ::SelectObject( hDC, hPen[3] );
  ::SelectObject( hDC, hBrush[1] );
  m_pGWnd->DrawPolylines( hDC, Glyph, true, true );

  // Control points
  if (m_bShowCtrlPoints){
    if (bSpline && m_bShowCtrlLines){
      ::SelectObject( hDC, hPen[5] );
      m_pGWnd->DrawPolylines( hDC, Glyph2, false, true );
    }
    if (m_bShowCtrlNums){
      ::SelectObject( hDC, ::GetStockObject(DEFAULT_GUI_FONT) );
      ::SetTextAlign( hDC, TA_BASELINE | TA_LEFT );
      ::SetTextColor( hDC, ColorNums );
    }
    ::SelectObject( hDC, hPen[4] );
    ::SelectObject( hDC, hBrush[2] );
    pVer = m_Ver;
    k = 0;
    for (i=0; i<m_nPL; ++i){
      n = m_PLSize[i];
      X0 = pVer->x;
      Y0 = pVer->y;
      for (j=0; j<n; ++j){
        X = pVer->x;
        Y = pVer->y;
        if (j>0 && X==X0 && Y==Y0){
          bDup = true;
        }else{
          bDup = false;
          // LC_PROP_G_PNTPIXSIZE is true, therefore point size is pixels
          if (pVer->bFix){
            m_pGWnd->DrawPoint( hDC, CGePoint(X,Y), LC_POINT_PIXEL | LC_POINT_SQUARE| LC_POINT_FILLED, -6 );
          }else{
            m_pGWnd->DrawPoint( hDC, CGePoint(X,Y), LC_POINT_PIXEL | LC_POINT_CIRCLE | LC_POINT_FILLED, -4 );
          }
        }
        if (m_bShowCtrlNums){
          m_pGWnd->CoordViewToWnd( X, Y, &wx, &wy );
          _itow( k, szNum, 10 );
          if (bDup){
            ++wx;
            wy+=10;
          }
          okDrawText( hDC, wx+4, wy-1, szNum );
        }
        ++pVer;
        ++k;
      }
    }
  }

  // max char box
  if (m_bShowCharBox){
    ::SelectObject( hDC, hPen[2] );
    ht = m_CharDescent + m_CharAscent;
    m_pGWnd->DrawRect( hDC, 0, -m_CharDescent, m_W2, ht );
  }

  // restore DC
  ::SelectObject( hDC, hPrevPen );
  ::SelectObject( hDC, hPrevBrush );
  ::DeleteObject( hPen[0] );
  ::DeleteObject( hPen[1] );
  ::DeleteObject( hPen[2] );
  ::DeleteObject( hPen[3] );
  ::DeleteObject( hPen[4] );
  ::DeleteObject( hPen[5] );
  ::DeleteObject( hBrush[0] );
  ::DeleteObject( hBrush[1] );
  ::DeleteObject( hBrush[2] );
  return 0;
}

