/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* Graphics Window
* Class LcWindow
*********************************************************************/
#include <memory.h>
#include "GW.h"

//-----------------------------------------------
bool LcWindow::WndProc (HWND hWnd, UINT idMsg, WPARAM wParam, LPARAM lParam)
{
  bool bRet = true;

  if (hWnd){
    if (hWnd != m_hWnd){
      return false;
    }
  }
  switch( idMsg ){
    case WM_PAINT:
      OnPaint();  
      break;

    case WM_MOUSEMOVE:     OnMouseMove( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_MOUSELEAVE:    OnMouseLeave();  break;
    case WM_MOUSEWHEEL:    OnMouseWheel( LOWORD(wParam), HIWORD(wParam), LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_LBUTTONDOWN:   OnLButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_LBUTTONUP:     OnLButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_LBUTTONDBLCLK: OnLButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_RBUTTONDOWN:   OnRButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_RBUTTONUP:     OnRButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_MBUTTONDOWN:   OnMButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_MBUTTONUP:     OnMButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
    case WM_MBUTTONDBLCLK: OnMButtonDblClk( wParam, LOWORD(lParam), HIWORD(lParam) );  break;
//    case WM_KEYDOWN:       OnKeyDown( wParam, lParam );  break;
    case WM_HSCROLL:       OnHScroll( LOWORD(wParam), HIWORD(wParam) );  break;
    case WM_VSCROLL:       OnVScroll( LOWORD(wParam), HIWORD(wParam) );  break;
    case WM_SETFOCUS:      OnSetFocus();  break;
    case WM_KILLFOCUS:     OnKillFocus();  break;
    default:
      bRet = false;
  }
  return bRet;
}

//-----------------------------------------------
void LcWindow::OnPaint ()
{
  HDC     hOutDC, hMemDC, hDC;
  UINT    nLines;
  UINT    W = PSWidth();
  UINT    H = PSHeight();
  HBRUSH  hBrush;
  HGDIOBJ hPrevBrush;
  PAINTSTRUCT ps;
  CGeRect ViewRect;

  ::BeginPaint( m_hWnd, &ps );
  if (H>0 && W>0){
    hOutDC = ps.hdc;
    // create memory DC
    hMemDC = ::CreateCompatibleDC( hOutDC );
    ::SelectObject( hMemDC, m_hWndBitmap );
    hDC = hMemDC;

    // clear background
    hBrush = ::CreateSolidBrush( m_ColorBack );
    hPrevBrush = ::SelectObject( hDC, hBrush );
    ::PatBlt( hDC, 0, 0, W, H, PATCOPY );
    ::SelectObject( hDC, hPrevBrush );
    ::DeleteObject( hBrush );

    if (m_pEventProc){
      // allow the parent application to draw something in the window
      m_hPaintDC = hDC;
      (m_pEventProc)( GW_EVENT_PAINT, m_EventPrm1 );
      m_hPaintDC = NULL;
    }

    if (m_bDrawCenter){
      DrawWndCenter( hDC, m_ColorCursor );
    }
    nLines = m_Bmi.bmiHeader.biHeight;
    ::GetDIBits( hMemDC, m_hWndBitmap, 0, nLines, m_Bits, &m_Bmi, DIB_RGB_COLORS );

    // copy drawing's view from a memory to a window
    ::BitBlt( hOutDC, 0, 0, W, H, hMemDC, 0,0, SRCCOPY );

    // delete memory DC
    ::DeleteDC( hMemDC );
    hDC = hMemDC = NULL;
  }
  ::EndPaint( m_hWnd, &ps );

  // display cursor and dragged objects
  if (m_bMouseLeave == false){
    OnMouseMove( 0, (WORD)m_CursorWPt.x, (WORD)m_CursorWPt.y, true );
  }
}


//-----------------------------------------------
void LcWindow::OnMouseMove (WPARAM Flags, short x, short y, bool bOnPaint)
{
  DWORD    Err;
  HDC      hDrawDC, hMemDC;
  int      dx, dy, nLines, RetVal;
  bool     bShift = (Flags & MK_SHIFT)? true : false;
  bool     bCtrl  = (Flags & MK_CONTROL)? true : false;

  m_CursorWPt.x = x;
  m_CursorWPt.y = y;

  if (!m_bHasFocus){
    // no focus - no MouseMove processing
    if (!bOnPaint){
      ::SetCursor( m_hCurMain );
    }
    return;
  }

  if (m_bMouseLeave){
    m_bMouseLeave = false;
    UpdateCursor();
  }

  if (m_bPanMode){
    // real-time pan
    if (1){
//      ::SetCursor( g_hCurPan2 );
      dx = m_MovePrevWPt.x - m_CursorWPt.x;
      dy = m_CursorWPt.y - m_MovePrevWPt.y;
      if ((abs(dx) > g_PanStep) || (abs(dy) > g_PanStep)){
        SaveForZoomPrev( 5 );  // Pan realtime
        Pan( dx, dy );
        UpdateScrollers();
        Invalidate();
        m_MovePrevWPt = m_CursorWPt;
      }
    }
    return;
  }

  hDrawDC = ::GetDC( m_hWnd );
  if (hDrawDC == NULL){
    Err = ::GetLastError();
    return;
  }

  // Correct input coordinates using object snap
  CoordWndToView( m_CursorWPt, &m_CursorPoint );
  if (m_pEventProc){
    RetVal = (m_pEventProc)( GW_EVENT_SNAP, m_EventPrm1 );
    if (RetVal == 1){
      // m_CursorPoint was changed in the event procedure
    }
  }

  hMemDC = ::CreateCompatibleDC( hDrawDC );
  nLines = m_Bmi.bmiHeader.biHeight;
  nLines = ::SetDIBits( hMemDC, m_hWndBitmap2, 0, nLines, m_Bits, &m_Bmi, DIB_RGB_COLORS );
  ::SelectObject( hMemDC, m_hWndBitmap2 );
    DrawCursor( hMemDC, m_CursorWPt.x, m_CursorWPt.y, Flags, m_ColorCursor );
  ::BitBlt( hDrawDC, 0, 0, PSWidth(), PSHeight(), hMemDC, 0,0, SRCCOPY );
  ::DeleteDC( hMemDC );
  ::ReleaseDC( m_hWnd, hDrawDC );
  hDrawDC = NULL;
  m_MovePrevWPt = m_CursorWPt;

  if (m_pEventProc){
    (m_pEventProc)( GW_EVENT_MOUSEMOVE, m_EventPrm1 );
  }
}

//-----------------------------------------------
void LcWindow::OnMouseLeave ()
{
  m_bMouseLeave = true;
  RePaint();
}


//-----------------------------------------------
void LcWindow::OnMouseWheel (WORD Flags, short Delta, short x, short y)
{
  POINT   Pt;
  CGeRect ViewRect;
  double  Xdrw, Ydrw;

  SaveForZoomPrev( 1 );  // Wheel
  Pt.x = x;
  Pt.y = y;

  // the WM_MOUSEWHEEL message passes x,y as screen coordinates
  // therefore it must be converted to window coordinates
  ::ScreenToClient( m_hWnd, &Pt );
  if (Contains( Pt )){
    CoordWndToView( Pt, &Xdrw, &Ydrw );
    if (Delta > 0){
      ZoomIn( true, Xdrw, Ydrw );
    }else{
      ZoomOut( true, Xdrw, Ydrw );
    }
    UpdateScrollers();
    Invalidate();
  }
}

//-----------------------------------------------
void LcWindow::OnLButtonDown (WPARAM Flags, short x, short y)
{
  CGeRect Rect;
  if (::GetFocus() != m_hWnd){
    SetFocus();
  }
  UpdateCursor();
  m_CursorWPt.x = x;
  m_CursorWPt.y = y;
  CoordWndToView( m_CursorWPt, &m_CursorPoint );
  m_ClickPoint = m_CursorPoint;
  m_ClickWPt.x = x;
  m_ClickWPt.y = y;
  if (m_ZoomFrame == 0){
    // start panning of view  
    PanRT_Start( x, y );
  }else{
    switch( m_ZoomFrame ){
      case 1:
        m_ZoomFramePt0 = m_CursorPoint;
        m_ZoomFrame = 2;
        break;
      case 2:
        m_ZoomFrame = 0;
        Rect.Set( m_ZoomFramePt0, m_CursorPoint );
        ZoomRect( Rect, true );
        break;
    }
  }
}

//-----------------------------------------------
void LcWindow::OnLButtonUp (WPARAM Flags, short x, short y)
{
  // end panning of view
  PanRT_End();
}


//-----------------------------------------------
void LcWindow::OnLButtonDblClk (WPARAM Flags, short x, short y)
{
  POINT    wpt;
  CGePoint CenPt;

  if (LevelZoomIn()) {
    LevelZoomOut();
  }
  else {
    wpt.x = x;
    wpt.y = y;
    CoordWndToView(wpt, &CenPt);
    LevelZoomIn(10.0, &CenPt);
  }
  UpdateScrollers();
  Invalidate();
}

//-----------------------------------------------
void LcWindow::OnRButtonDown (WPARAM Flags, short x, short y)
{
  CGeRect Rect, Rect2;
  bool  bCtrl  = (::GetKeyState( VK_CONTROL )<0)? true : false;
  bool  bShift = (::GetKeyState( VK_SHIFT )<0)? true : false;

  if (m_pEventProc){
    (m_pEventProc)( GW_EVENT_RBDOWN, m_EventPrm1 );
  }
}

//-----------------------------------------------
void LcWindow::OnRButtonUp (WPARAM Flags, short x, short y)
{
}

//-----------------------------------------------
void LcWindow::OnMButtonDown (WPARAM Flags, short x, short y)
{
  PanRT_Start( x, y );
}

//-----------------------------------------------
void LcWindow::OnMButtonUp (WPARAM Flags, short x, short y)
{
  PanRT_End();
}

//-----------------------------------------------
void LcWindow::OnMButtonDblClk (WPARAM Flags, short x, short y)
{
  ZoomExtents();
}

//-----------------------------------------------
void LcWindow::OnHScroll (WORD SBCode, WORD Pos)
{
  int PosDelta=0;
  if (SBCode == SB_THUMBPOSITION || SBCode == SB_THUMBTRACK){
    PosDelta = Pos - m_ScrollPosX;
  }
  SaveForZoomPrev( 2 );  // HScroll
  Scroll( false, SBCode, PosDelta, true );
  UpdateScrollers();
  Invalidate();
}

//-----------------------------------------------
void LcWindow::OnVScroll (WORD SBCode, WORD Pos)
{
  int PosDelta=0;
  if (SBCode == SB_THUMBPOSITION || SBCode == SB_THUMBTRACK){
    PosDelta = Pos - m_ScrollPosY;
  }
  SaveForZoomPrev( 3 );  // VScroll
  Scroll( true, SBCode, PosDelta, true );
  UpdateScrollers();
  Invalidate();
}

//-----------------------------------------------
void LcWindow::OnSetFocus ()
{
  m_bHasFocus = true;
}

//-----------------------------------------------
void LcWindow::OnKillFocus ()
{
  m_bHasFocus = false;
  RePaint();
}

//-----------------------------------------------
void LcWindow::OnUpdateCS ()
{
  Invalidate();
}


//-----------------------------------------------
void LcWindow::PanRT_Start (short x, short y)
{
  if (HasFocus() == false){
    SetFocus();
  }
  m_MovePrevWPt.x = x;
  m_MovePrevWPt.y = y;
  m_bPanMode = true;
  ::SetCapture( m_hWnd );
  RePaint();
  ::SetCursor( g_hCurPan1 );
}

//-----------------------------------------------
void LcWindow::PanRT_End ()
{
  if (m_bPanMode){
    m_bPanMode = false;
    UpdateScrollers();
    Invalidate();
    ::ReleaseCapture();
    UpdateCursor();
  }
}
