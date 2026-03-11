/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* Graphics Window
* Class LcWindow
*********************************************************************/
#include "GW.h"

//-----------------------------------------------
void LcWindow::DrawCursor (HDC hDC, int x, int y, WPARAM Flags, COLORREF Color)
{
  HPEN    hPen = ::CreatePen( PS_SOLID, 1, Color );  // cursor color
  HGDIOBJ hPrevPen = ::SelectObject( hDC, hPen );

  if (m_ZoomFrame == 2){
    // draw zoom frame
    CGeRect Rect( m_ZoomFramePt0, m_CursorPoint );
    DrawRect( hDC, Rect );
  }else{
    DrawCrosshair( hDC );
    if (m_ZoomFrame != 1){
      if (m_bShowPickbox){
        okDrawSquare( hDC, m_CursorWPt, m_PickBoxSize );
      }
    } 
  }
  ::SelectObject( hDC, hPrevPen );
  ::DeleteObject( hPen );
}

//-----------------------------------------------
void LcWindow::DrawCrosshair (HDC hDC)
{
  POINT wpt[4];
  POINT CurPtWin = m_CursorWPt;
  int   SizeH, SizeV;

  // get cursor size
  if (m_CrossSize <= 0){
    // pixels
    if (m_CrossSize == 0){
      SizeH = 1;
    }else{
      SizeH = abs( m_CrossSize ); 
    }
  }else{
    // percent of window size
    SizeH = max( PSWidth(), PSHeight() );
    if ((1 < m_CrossSize) && (m_CrossSize < 100)){
      SizeH = SizeH * m_CrossSize / 200;
    }
  }
  SizeV = SizeH;
  if (SizeH){
    // draw horizontal line
    wpt[0].x = CurPtWin.x - SizeH;
    wpt[1].x = CurPtWin.x + SizeH;
    wpt[0].y = wpt[1].y = CurPtWin.y;
    ::Polyline( hDC, wpt, 2 );
  }
  if (SizeV){
    // draw vertical line
    wpt[0].y = CurPtWin.y - SizeV;
    wpt[1].y = CurPtWin.y + SizeV;
    wpt[0].x = wpt[1].x = CurPtWin.x;
    ::Polyline( hDC, wpt, 2 );
  }
}

//-----------------------------------------------
void LcWindow::DrawWndCenter (HDC hDC, COLORREF Color)
{
  HPEN hPen = ::CreatePen( PS_DOT, 1, m_ColorCursor );
  HGDIOBJ hPrevPen = ::SelectObject(hDC, hPen);
  UINT    W = PSWidth();
  UINT    H = PSHeight();
  int     x, y;
      
  x = W / 2;
  y = H / 2;
  ::MoveToEx( hDC, 0, y, NULL );
  ::LineTo( hDC, W, y );
  ::MoveToEx( hDC, x, 0, NULL );
  ::LineTo( hDC, x, H );
  ::SelectObject( hDC, hPrevPen );
  ::DeleteObject( hPen );
}

//-----------------------------------------------
void LcWindow::DrawPixel (HDC hDC, double X, double Y, COLORREF Color) const
{
  POINT wpt;
  CoordViewToWnd( X, Y, &wpt );
  ::SetPixel( hDC, wpt.x, wpt.y, Color );
}

//-----------------------------------------------
void LcWindow::DrawBox (HDC hDC, double X, double Y, COLORREF Color) const
{
  POINT wpt;
  CoordViewToWnd( X, Y, &wpt );
  ::SetPixel( hDC, wpt.x-1, wpt.y-1, Color );
  ::SetPixel( hDC, wpt.x-1, wpt.y, Color );
  ::SetPixel( hDC, wpt.x-1, wpt.y+1, Color );
  ::SetPixel( hDC, wpt.x, wpt.y-1, Color );
  ::SetPixel( hDC, wpt.x, wpt.y+1, Color );
  ::SetPixel( hDC, wpt.x+1, wpt.y-1, Color );
  ::SetPixel( hDC, wpt.x+1, wpt.y, Color );
  ::SetPixel( hDC, wpt.x+1, wpt.y+1, Color );
}

//-----------------------------------------------
void LcWindow::DrawPoint (HDC hDC, const CGePoint& Pos, int Mode, double Size, HBRUSH hBrush) const
{
  POINT    WndPos, wpt[4];
  bool     bFilled;
  int      hsz, hsz2, PType;
  double   H;
  HGDIOBJ  hObj;

  PType = Mode & 0x1F;
  bFilled = (Mode&LC_POINT_FILLED)? true : false;
  CoordViewToWnd( Pos, &WndPos );
  if (Size == 0){
    Size = (g_bPntPixSize)? -6.0 : -0.9;
  }
  if (Size < 0.0){
    if (g_bPntPixSize){
      hsz2 = (int)(-Size + 0.5);
    }else{
      // percentage of the design window height
      Size = -Size / 100.0;  // %
      H = (g_PntWndHeight>0)? g_PntWndHeight : PSHeight();
      hsz2 = (int)(H * Size + 0.5);
    }
    hsz = hsz2 / 2;
    if (hsz < 1){
      hsz = 1;
    }
  }else{
    hsz2 = (int)((Size / PixelSize()) + 0.5);
    hsz = hsz2 / 2;
  }
  if (Mode & LC_POINT_SQUARE){
    if (bFilled){
      hObj = ::SelectObject( hDC, hBrush );
      okDrawSquare( hDC, WndPos, hsz, bFilled );
      ::SelectObject( hDC, hObj );
    }else{
      okDrawSquare( hDC, WndPos, hsz, bFilled );
    }
  }
  if (Mode & LC_POINT_CIRCLE){
    if (bFilled){
      hObj = ::SelectObject( hDC, hBrush );
      okDrawCircle( hDC, WndPos, hsz, bFilled );
      ::SelectObject( hDC, hObj );
    }else{
      okDrawCircle( hDC, WndPos, hsz, bFilled );
    }
  }
  if (Mode & LC_POINT_RHOMB){
    if (bFilled){
      hObj = ::SelectObject( hDC, hBrush );
      okDrawRhomb( hDC, WndPos, hsz, bFilled );
      ::SelectObject( hDC, hObj );
    }else{
      okDrawRhomb( hDC, WndPos, hsz, bFilled );
    }
  }
  switch( PType ){
    case LC_POINT_PIXEL:  // pixel
      wpt[0] = WndPos;
      wpt[1].x = wpt[0].x + 1;
      wpt[1].y = wpt[0].y + 1;
      ::Polyline( hDC, wpt, 2 );
      break;
    case LC_POINT_PLUS:   // plus sign through the point
      okDrawCross( hDC, WndPos, hsz );
      break;
    case LC_POINT_X:      // an X through the point
      okDrawX( hDC, WndPos, hsz );
      break;
    case LC_POINT_TICK:   // a vertical tick mark upward from the point
      wpt[0] = wpt[1] = WndPos;
      wpt[1].y = wpt[0].y - hsz;
      ::Polyline( hDC, wpt, 2 );
      break;
  }
}

//-----------------------------------------------
void LcWindow::DrawDot (HDC hDC, const CGePoint& Pos) const
{
  POINT wpt[2];
  CoordViewToWnd( Pos, wpt+0 );
  wpt[1].x = wpt[0].x + 1;
  wpt[1].y = wpt[0].y + 1;
  ::Polyline( hDC, wpt, 2 );
}

//-----------------------------------------------
void LcWindow::DrawDot (HDC hDC, double X, double Y) const
{
  POINT wpt[2];
  CoordViewToWnd( X, Y, wpt+0 );
  wpt[1].x = wpt[0].x + 1;
  wpt[1].y = wpt[0].y + 1;
  ::Polyline( hDC, wpt, 2 );
}

//-----------------------------------------------
void LcWindow::DrawLine (HDC hDC, double x1, double y1, double x2, double y2) const
{
  POINT wpt[2];
  CoordViewToWnd( x1, y1, wpt+0 );
  CoordViewToWnd( x2, y2, wpt+1 );
  ::Polyline( hDC, wpt, 2 );
}

//-----------------------------------------------
void LcWindow::DrawLine (HDC hDC, const CGePoint& Pt1, const CGePoint& Pt2) const
{
  POINT wpt[2];
  CoordViewToWnd( Pt1, wpt+0 );
  CoordViewToWnd( Pt2, wpt+1 );
  ::Polyline( hDC, wpt, 2 );
}

//-----------------------------------------------
void LcWindow::DrawRect (HDC hDC, double Lef, double Bot, double Rig, double Top) const
{
  POINT wpt[5];
  CoordViewToWnd( Lef, Bot, wpt+0 );
  CoordViewToWnd( Rig, Bot, wpt+1 );
  CoordViewToWnd( Rig, Top, wpt+2 );
  CoordViewToWnd( Lef, Top, wpt+3 );
  wpt[4] = wpt[0];
  if (hDC==0){
    hDC = m_hPaintDC;
  }
  ::Polyline( hDC, wpt, 5 );
}

//-----------------------------------------------
bool LcWindow::DrawXline (HDC hDC, const CGePoint& Pt0, double Angle, bool bRay) const
{
  UINT_PTR   i, j;
  CGeRect    Rect;
  CGeLine    Line, Xline;
  CGePoint   Pnt[5], InterPt, LinPt0, LinPt1;
  CGePoint*  pPnt;
  double     Xmin, Ymin, Xmax, Ymax;
  EnIntersec Res;

  GetViewRect( &Rect );
  Rect.GetPoints( Pnt );
  pPnt = Pnt;
  pPnt->Rotate( Pt0, -Angle );
  Xmin = Xmax = pPnt->x;
  Ymin = Ymax = pPnt->y;
  ++pPnt;
  for (i=1; i<4; ++i){
    if (Angle != 0.0){
      pPnt->Rotate( Pt0, -Angle );
    }
    if (Xmin > pPnt->x){
      Xmin = pPnt->x;
    }
    if (Xmax < pPnt->x){
      Xmax = pPnt->x;
    }
    if (Ymin > pPnt->y){
      Ymin = pPnt->y;
    }
    if (Ymax < pPnt->y){
      Ymax = pPnt->y;
    }
    ++pPnt;
  }
  if ((Ymin < Pt0.y) && (Pt0.y < Ymax)){
    if (bRay){
      Xline.Set( Pt0.x, Pt0.y, Xmax+100.0, Pt0.y );
    }else{
      Xline.Set( Xmin-100.0, Pt0.y, Xmax+100.0, Pt0.y );
    }
    Pnt[4] = Pnt[0];
    j = 0;
    for (i=0; i<4; ++i){
      Line.Set( Pnt[i], Pnt[i+1] );
      Res = Xline.GetIntersection( Line, &InterPt );
      if (Res == EnIntersec::GE_SKEW_CROSS){
        if (j == 0){
          LinPt0 = InterPt;
          ++j;
        }else{
          LinPt1 = InterPt;
          ++j;
          break;
        }
      }
    }
    if (j == 2){
      if (Angle != 0.0){
        LinPt0.Rotate( Pt0, Angle );
        LinPt1.Rotate( Pt0, Angle );
      }
      DrawLine( hDC, LinPt0, LinPt1 );
      return true;
    }
    if (j == 1){
      if (Angle != 0.0){
        LinPt0.Rotate( Pt0, Angle );
      }
      DrawLine( hDC, Pt0, LinPt0 );
      return true;
    }
  }
  return false;
}

//-----------------------------------------------
void LcWindow::DrawPolylines (HDC hDC, const CListPlines& Glyph, bool bFill, bool bContour)
{
  CItemPline* pPline;
  CGeVertex*  pVer;
  POINT*      wpt; 
  UINT_PTR    nVers, i;

  nVers = Glyph.NumVers();
  wpt = (POINT*)calloc( nVers, sizeof(POINT) );
  if (wpt == NULL){
    return;
  }
  if (bFill){
    ::BeginPath( hDC );
  }
  pPline = Glyph.GetFirst();
  while( pPline ){
    nVers = pPline->NumVers();
    i = 0;
    pVer = pPline->GetFirstVer();
    while( pVer ){
      CoordViewToWnd( pVer->m_Pos, wpt+i );
      ++i;
      pVer = pPline->GetNextVer( pVer );
    }
    ::Polyline( hDC, wpt, (int)nVers );
    pPline = Glyph.GetNext( pPline );
  }
  if (bFill){
    ::EndPath( hDC );
    if (bContour){
      ::StrokeAndFillPath( hDC );
    }else{
      ::FillPath( hDC );  
    }
  }
  free( wpt );
}

