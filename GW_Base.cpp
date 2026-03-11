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
#include "resource.h"

HCURSOR  LcWindow::g_hCurPan1   = 0;
HCURSOR  LcWindow::g_hCurPan2   = 0;
HCURSOR  LcWindow::g_hCurZoom   = 0;
HCURSOR  LcWindow::g_hCurArrow  = 0;
HCURSOR  LcWindow::g_hCurCross  = 0;
HCURSOR  LcWindow::g_hCurFinger = 0;
HCURSOR  LcWindow::g_hCurHelp   = 0;
HCURSOR  LcWindow::g_hCurNO     = 0;
HCURSOR  LcWindow::g_hCurWait   = 0;

int  LcWindow::g_PanStep = 3;
int  LcWindow::g_PickBoxSize = 4;
bool LcWindow::g_bPntPixSize = true;
UINT LcWindow::g_PntWndHeight = 0;

const WCHAR* szGWndClass = L"GWndClass";

//-----------------------------------------------
LRESULT CALLBACK GW_WndProc (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  LcWindow* pw = (LcWindow*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
  if (pw){
    if (pw->WndProc( hWnd, Msg, wParam, lParam)){
      return 0;
    }
  }
  return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}


//-----------------------------------------------
void RegisterWndClass (HINSTANCE hInst)
{
  WNDCLASSEX wc;
  BOOL  gc;

  memset( &wc, 0, sizeof(WNDCLASSEX) );
  // Check if the class already registered
  gc = ::GetClassInfoEx( hInst, szGWndClass, &wc );
  if (!gc){
    // register the window class
    memset( &wc, 0, sizeof(WNDCLASSEX) );
    wc.cbSize         = sizeof(WNDCLASSEX); 
    wc.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc    = (WNDPROC)GW_WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInst;
    wc.hIcon          = NULL;
    wc.hCursor        = NULL;
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = szGWndClass;
    wc.hIconSm        = NULL;
    ::RegisterClassEx( &wc );
  }
}


//=============================================================================
LcWindow::LcWindow (HINSTANCE hInst, HWND hWndParent, int Style)
{
  int X, Y, W, H;
  DWORD ExStyle = 0;
  DWORD WStyle = WS_CHILD | WS_VISIBLE;

  m_hInst = (hInst)? hInst : ::GetModuleHandle(NULL);
  m_FrameOrig.x = X = 0;
  m_FrameOrig.y = Y = 0;
  m_FrameSize.cx = W = 500;
  m_FrameSize.cy = H = 400;

  // Init cursors 
  if (g_hCurArrow == 0){
    g_hCurArrow  = (HCURSOR)::LoadCursor( NULL, IDC_ARROW );  // Standard arrow
    g_hCurCross  = (HCURSOR)::LoadCursor( NULL, IDC_CROSS );  // Crosshair
    g_hCurFinger = (HCURSOR)::LoadCursor( NULL, IDC_HAND );   // Hand
    g_hCurHelp   = (HCURSOR)::LoadCursor( NULL, IDC_HELP );   // Arrow and question mark
    g_hCurNO     = (HCURSOR)::LoadCursor( NULL, IDC_NO );     // Slashed circle
    g_hCurWait   = (HCURSOR)::LoadCursor( NULL, IDC_WAIT );   // Hourglass
    g_hCurPan1   = (HCURSOR)::LoadImage( hInst, MAKEINTRESOURCE(CUR_ZOOMPAN1), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR );
    g_hCurPan2   = (HCURSOR)::LoadImage( hInst, MAKEINTRESOURCE(CUR_ZOOMPAN2), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR );
    g_hCurZoom   = (HCURSOR)::LoadImage( hInst, MAKEINTRESOURCE(CUR_ZOOMRT)  , IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR );
  }

  m_ColorBack = RGB(0,0,0);
  m_ColorFore = RGB(255,255,255);
  m_ColorCursor = RGB(205,225,155);
  m_CursorWPt.x = m_CursorWPt.y = 0;
  m_MovePrevWPt.x = m_MovePrevWPt.y = 0;
  
  memset( &m_Bmi, 0, sizeof(m_Bmi) );
  memset( m_Colors, 0, sizeof(m_Colors) );
  m_Bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  m_hWndBitmap = NULL;
  m_hWndBitmap2 = NULL;
  m_Bits = NULL;
  m_pEventProc = NULL; 
  m_EventPrm1 = NULL;
  m_hPaintDC = NULL;
    
  m_hCurMain = g_hCurArrow;
  m_bMouseLeave = true;
  m_bHasFocus = false;
  m_bPanMode = false;
  m_ZoomFrame = 0;
  InvalidateClickPoint();
  m_bShowSysCursor = true;
  m_bShowCrosshair = true;
  m_CrossSize = 100;
  m_bShowPickbox = true;
  m_PickBoxSize = 4;    
  m_bDrawCenter = false;
  m_IntVal = 0;

  if (Style & WS_VSCROLL){
    WStyle |= WS_VSCROLL;
  }
  if (Style & WS_HSCROLL){
    WStyle |= WS_HSCROLL;
  }
  if (Style & WS_BORDER){
    WStyle |= WS_BORDER;
  }
  if (Style & WS_EX_CLIENTEDGE){
    ExStyle |= WS_EX_CLIENTEDGE;
  }
  if (Style & WS_EX_STATICEDGE){
    ExStyle |= WS_EX_STATICEDGE;
  }
  RegisterWndClass( hInst );
  m_hWnd = ::CreateWindowEx( ExStyle, szGWndClass, L"", WStyle, X, Y, W, H, hWndParent, 0, hInst, 0 );
  if (m_hWnd){
    SetPSRect( 0, 0, W, H, false );
    m_bSteadyScrollBars = true;
    m_WndStyle  = (UINT)::GetWindowLongPtr( m_hWnd, GWL_STYLE );
    ::SetWindowLongPtr( m_hWnd, GWLP_USERDATA, (LONG_PTR)this );
  }
}

//-----------------------------------------------
LcWindow::~LcWindow ()
{
  if (m_hWndBitmap){
    ::DeleteObject( m_hWndBitmap );
    m_hWndBitmap = NULL;
  }
  if (m_hWndBitmap2){
    ::DeleteObject( m_hWndBitmap2 );
    m_hWndBitmap2 = NULL;
  }
  if (m_Bits){
    free( m_Bits );
    m_Bits = NULL;
  }
  if (::IsWindow( m_hWnd )){
    ::DestroyWindow( m_hWnd );
    m_hWnd = NULL;
  }
}



//-----------------------------------------------
void LcWindow::CreateWndBitmap ()
{
  int n;
  int W = PSWidth();
  int H = PSHeight();
  HDC hDC = ::GetDC(0);
  if (W == 0){
    W = 320;
  }
  if (H == 0){
    H = 240;
  }
  if (m_hWndBitmap){
    ::DeleteObject( m_hWndBitmap );
  }
  if (m_hWndBitmap2){
    ::DeleteObject( m_hWndBitmap2 );
  }
  if (m_Bits){
    free( m_Bits );
  }
  memset( &m_Bmi, 0, sizeof(m_Bmi) );
  memset( m_Colors, 0, sizeof(m_Colors) );
  m_Bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  m_hWndBitmap = ::CreateCompatibleBitmap( hDC, W, H );
  m_hWndBitmap2 = ::CreateCompatibleBitmap( hDC, W, H );
  n = ::GetDIBits( hDC, m_hWndBitmap, 0, H, NULL, &m_Bmi, DIB_RGB_COLORS );
  if (n > 0){
    n = m_Bmi.bmiHeader.biSizeImage;
    m_Bits = (BYTE*)malloc( n );   // used to clone m_hWndBitmap into m_hWndBitmap2
  }
}


//-----------------------------------------------
void LcWindow::Resize (int Lef, int Top, int W, int H)
{
  RECT Rect;

  if (::IsWindow( m_hWnd )){
    ::MoveWindow( m_hWnd, Lef, Top, W, H, false );
    ::GetClientRect( m_hWnd, &Rect );
    SetPSRect( 0, 0, Rect.right, Rect.bottom, true );
    CreateWndBitmap();
    m_FrameOrig.x  = Lef;
    m_FrameOrig.y  = Top;
    m_FrameSize.cx = W;
    m_FrameSize.cy = H;

    UpdateScrollers();
    Invalidate();
  }
}

//-----------------------------------------------
void LcWindow::InvalidateClickPoint ()
{
  m_ClickPoint.Set( -12345.678, -12345.678 );
  m_ClickWPt.x = m_ClickWPt.y = -12345678;
}

//-----------------------------------------------
void LcWindow::SetFocus () const
{
  if (m_hWnd){
    if (::GetFocus() != m_hWnd){
      ::SetFocus( m_hWnd );
      UpdateCursor();
      RePaint();
    }
  }
}


//-----------------------------------------------
void LcWindow::UpdateWindow () const
{
  if (m_hWnd){
    ::UpdateWindow( m_hWnd ); 
  }
}

// set system cursor (to display arrow or not)
//-----------------------------------------------
void LcWindow::UpdateCursor () const
{
  if (m_bPanMode){
    ::SetCursor( g_hCurPan1 );
  }else{
    if (m_bShowSysCursor){
      ::SetCursor( m_hCurMain );
    }else{
      ::SetCursor( 0 );
    }
  }
}

//-----------------------------------------------
bool LcWindow::SetMainCursor (HCURSOR hCur)
{
  m_hCurMain = hCur;
  UpdateCursor();
  return true;
}

//-----------------------------------------------
void LcWindow::Invalidate () const
{
  if (m_hWnd){
    ::InvalidateRect( m_hWnd, NULL, true );
  }
}

//-----------------------------------------------
void LcWindow::RePaint (HDC hDC) const
{
  DWORD Err;
  HDC  hOutDC, hMemDC;

  if (m_hWndBitmap){
    if (hDC){
      hOutDC = hDC;
    }else{
      hOutDC = ::GetDC( m_hWnd );
      if (hOutDC == NULL){
        Err = ::GetLastError();
        return;
      }
    }
    hMemDC = ::CreateCompatibleDC( hOutDC );
    ::SelectObject( hMemDC, m_hWndBitmap );
    ::BitBlt( hOutDC, 0, 0, Width(), Height(), hMemDC, 0,0, SRCCOPY );
    ::DeleteDC( hMemDC );
    if (hDC == NULL){
      ::ReleaseDC( m_hWnd, hOutDC );
    }
  }
}

//-----------------------------------------------
void LcWindow::ZoomExtents (bool bWithBasePt)
{
  SaveForZoomPrev();
  ZoomExtentsCS( bWithBasePt );
  Invalidate();
}

//-----------------------------------------------
void LcWindow::ZoomFrame ()
{
  if (m_ZoomFrame == 0){
    m_ZoomFrame = 1;
  }
}

//-----------------------------------------------
void LcWindow::UpdateScrollers ()
{
  SCROLLINFO si;
  CGeRect    DwgExt;     // drawing extents at 2D display projection
  CGeRect    ViewRect;   // drawing rectangle in a window
  CGeRect    Ext;        // extents rect included View rect and drawing extents DwgExt
  double     Scale=0.;   // <model units>/<pixel>
  int        RangeX, RangeY, UnitX, UnitY, PosX, PosY;

  Scale = PixelSize();   
  GetViewRect( &ViewRect );
  GetExtRect( &DwgExt );

  if ((m_WndStyle & WS_HSCROLL) || (m_WndStyle & WS_VSCROLL)){

    // limits rect
    Ext = ViewRect;
    if (DwgExt.IsNotNull()){
      Ext.UpdateExtentsBy( DwgExt );
    }

    RangeX = (int)(Ext.Width() / Scale + 0.5);
    RangeY = (int)(Ext.Height() / Scale + 0.5);
    UnitX  = (int)(ViewRect.Width() / Scale + 0.5);
    UnitY  = (int)(ViewRect.Height() / Scale + 0.5);
    // horizontal scroller position 
    PosX = RangeX / 2;
    PosY = RangeY / 2;
    if (RangeX > UnitX){
      PosX = (int)((ViewRect.Left() - Ext.Left()) / Scale + 0.5);
      if (PosX < 0){
        PosX = 0;
      }else{
        if (PosX > RangeX - UnitX){
          PosX = RangeX;
        }
      }
    }
    // vertical scroller's position
    if (RangeY > UnitY){
      PosY = (int)((Ext.Top() - ViewRect.Top()) / Scale + 0.5);
      if (PosY < 0){
        PosY = 0;
      }else{
        if (PosY > RangeY - UnitY){
          PosY = RangeY;
        }
      }
    }
    while( RangeX > 65535 ){
      RangeX /= 2;
      UnitX  /= 2;
      PosX   /= 2;
    }
    while( RangeY > 65535 ){
      RangeY /= 2;
      UnitY  /= 2;
      PosY   /= 2;
    }
    // Ajust scrollbars in a window
    memset( &si, 0, sizeof(si) );
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    if (m_bSteadyScrollBars){
      si.fMask |= SIF_DISABLENOSCROLL;
    }
    if (m_WndStyle & WS_HSCROLL){
      si.nMin = 1;
      si.nMax = RangeX;
      si.nPage = UnitX;
      si.nPos = PosX;
      ::SetScrollInfo( m_hWnd, SB_HORZ, &si, TRUE );
    }
    if (m_WndStyle & WS_VSCROLL){
      si.nMin = 1;
      si.nMax = RangeY;
      si.nPage = UnitY;
      si.nPos = PosY;
      ::SetScrollInfo( m_hWnd, SB_VERT, &si, TRUE );
    }
    m_ScrollRangeX = RangeX;
    m_ScrollRangeY = RangeY;
    m_ScrollUnitX  = UnitX ;
    m_ScrollUnitY  = UnitY ;
    m_ScrollPosX   = PosX  ;
    m_ScrollPosY   = PosY  ;
  }
}


//-----------------------------------------------
void LcWindow::SetEventProc (F_GWEVENT pProc, HANDLE Prm1)
{
  m_pEventProc = pProc;
  m_EventPrm1 = Prm1;
}

//-----------------------------------------------
void LcWindow::GetCursorCoords (POINT* pWinPt, CGePoint* pDrwPt) const
{
  if (pWinPt){
    *pWinPt = m_CursorWPt;
  }
  if (pDrwPt){
    *pDrwPt = m_CursorPoint;
  }
}

//-----------------------------------------------
void LcWindow::GetClickCoords (POINT* pWinPt, CGePoint* pDrwPt) const
{
  if (pWinPt){
    *pWinPt = m_ClickWPt;
  }
  if (pDrwPt){
    *pDrwPt = m_ClickPoint;
  }
}

//-----------------------------------------------
void LcWindow::GetCursorRect (CGeRect* pRect) const
{
  double L, B, R, T;
  double Size = PixelSize() * g_PickBoxSize;
  L = m_CursorPoint.x - Size;
  B = m_CursorPoint.y - Size;
  R = m_CursorPoint.x + Size;
  T = m_CursorPoint.y + Size;
  pRect->Set( L, B, R, T );
}

//-----------------------------------------------
void LcWindow::SetCursorCoords (const CGePoint& Pnt)
{
  m_CursorPoint = Pnt;
  CoordViewToWnd( m_CursorPoint, &m_CursorWPt );
}



// Draw a dot
//-----------------------------------------------
void okDrawDot (HDC hDC, POINT Pt0)
{
  ::MoveToEx( hDC, Pt0.x, Pt0.y, NULL );
  ::LineTo( hDC, Pt0.x+1, Pt0.y );
}

// Draw a cross
//-----------------------------------------------
void okDrawCross (HDC hDC, POINT Pt0, UINT HalfSize)
{
  POINT wpt[2];
  wpt[0].x = Pt0.x - HalfSize;
  wpt[1].x = Pt0.x + HalfSize + 1;
  wpt[0].y = wpt[1].y = Pt0.y;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].y = Pt0.y - HalfSize;
  wpt[1].y = Pt0.y + HalfSize + 1;
  wpt[0].x = wpt[1].x = Pt0.x;
  ::Polyline( hDC, wpt, 2 );
}


// Draw an X cross
//-----------------------------------------------
void okDrawX (HDC hDC, POINT Pt0, UINT HalfSize)
{
  POINT wpt[2];
  wpt[0].x = Pt0.x - HalfSize;
  wpt[0].y = Pt0.y - HalfSize;
  wpt[1].x = Pt0.x + HalfSize + 1;
  wpt[1].y = Pt0.y + HalfSize + 1;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = Pt0.x - HalfSize;
  wpt[0].y = Pt0.y + HalfSize;
  wpt[1].x = Pt0.x + HalfSize + 1;
  wpt[1].y = Pt0.y - HalfSize - 1;
  ::Polyline( hDC, wpt, 2 );
}


// Draw a square
//-----------------------------------------------
void okDrawSquare (HDC hDC, POINT Pt0, UINT HalfSize, bool bFill)
{
  POINT wpt[5];
  if (HalfSize > 0){
    wpt[0].x = wpt[1].x = Pt0.x - HalfSize;
    wpt[2].x = wpt[3].x = Pt0.x + HalfSize;
    wpt[0].y = wpt[3].y = Pt0.y - HalfSize;
    wpt[1].y = wpt[2].y = Pt0.y + HalfSize;
    wpt[4] = wpt[0];
    if (bFill){
      ::Polygon( hDC, wpt, 4 );
    }else{
      ::Polyline( hDC, wpt, 5 );
    }
  }else{
    wpt[0] = wpt[1] = Pt0;
    ++(wpt[1].x);
    ++(wpt[1].y);
    ::Polyline( hDC, wpt, 2 );
  }
}


// Draw a rhomb
//-----------------------------------------------
void okDrawRhomb (HDC hDC, POINT Pt0, UINT HalfSize, bool bFill)
{
  POINT wpt[5];
  wpt[0].x = Pt0.x - HalfSize;
  wpt[0].y = Pt0.y;
  wpt[1].x = Pt0.x;
  wpt[1].y = Pt0.y - HalfSize;
  wpt[2].x = Pt0.x + HalfSize;
  wpt[2].y = Pt0.y;
  wpt[3].x = Pt0.x;
  wpt[3].y = Pt0.y + HalfSize;
  wpt[4] = wpt[0];
  if (bFill){
    ::Polygon( hDC, wpt, 4 );
  }else{
    ::Polyline( hDC, wpt, 5 );
  }
}


// Draw a circle
//-----------------------------------------------
void okDrawCircle (HDC hDC, POINT Pt0, UINT R, bool bFill)
{
  if (bFill){
    if (R == 3){
      ::BeginPath( hDC );
      ::MoveToEx( hDC, Pt0.x-1, Pt0.y-3, NULL );
      ::LineTo( hDC, Pt0.x+1, Pt0.y-3 );
      ::LineTo( hDC, Pt0.x+3, Pt0.y-1 );
      ::LineTo( hDC, Pt0.x+3, Pt0.y+1 );
      ::LineTo( hDC, Pt0.x+1, Pt0.y+3 );
      ::LineTo( hDC, Pt0.x-1, Pt0.y+3 );
      ::LineTo( hDC, Pt0.x-3, Pt0.y+1 );
      ::LineTo( hDC, Pt0.x-3, Pt0.y-1 );
      ::LineTo( hDC, Pt0.x-1, Pt0.y-3 );
      ::EndPath( hDC );
      ::StrokeAndFillPath( hDC );
    }else{
      ::Ellipse( hDC, Pt0.x-R, Pt0.y-R, Pt0.x+R+1, Pt0.y+R+1 );
    }
  }else{
    if (R == 3){
      ::MoveToEx( hDC, Pt0.x-1, Pt0.y-3, NULL );
      ::LineTo( hDC, Pt0.x+1, Pt0.y-3 );
      ::LineTo( hDC, Pt0.x+3, Pt0.y-1 );
      ::LineTo( hDC, Pt0.x+3, Pt0.y+1 );
      ::LineTo( hDC, Pt0.x+1, Pt0.y+3 );
      ::LineTo( hDC, Pt0.x-1, Pt0.y+3 );
      ::LineTo( hDC, Pt0.x-3, Pt0.y+1 );
      ::LineTo( hDC, Pt0.x-3, Pt0.y-1 );
      ::LineTo( hDC, Pt0.x-1, Pt0.y-3 );
    }else{
      ::Arc( hDC, Pt0.x-R, Pt0.y-R, Pt0.x+R+1, Pt0.y+R+1, Pt0.x-R, Pt0.y-R, Pt0.x-R, Pt0.y-R  );
    }
  }
}

// Draw a rectangle
//-----------------------------------------------
void okDrawRect (HDC hDC, const RECT& rect, bool bFill)
{
  POINT wpt[5];
  wpt[0].x = wpt[1].x = rect.left;
  wpt[2].x = wpt[3].x = rect.right;
  wpt[0].y = wpt[3].y = rect.top;
  wpt[1].y = wpt[2].y = rect.bottom;
  wpt[4] = wpt[0];
  if (bFill){
    ::Polygon( hDC, wpt, 4 );
  }else{
    ::Polyline( hDC, wpt, 5 );
  }
}

// Draw a rectangle by width line
//-----------------------------------------------
void okDrawWidRect (HDC hDC, const RECT& rect)
{
  POINT wpt[2];

  // left
  wpt[0].x = rect.left - 1;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.left - 1;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.left;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.left;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.left + 1;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.left + 1;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  // top
  wpt[0].x = rect.left;
  wpt[0].y = rect.top - 1;
  wpt[1].x = rect.right;
  wpt[1].y = rect.top - 1;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.left;
  wpt[0].y = rect.top;
  wpt[1].x = rect.right;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.left;
  wpt[0].y = rect.top + 1;
  wpt[1].x = rect.right;
  wpt[1].y = rect.top + 1;
  ::Polyline( hDC, wpt, 2 );
  // right
  wpt[0].x = rect.right + 1;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.right + 1;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.right;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.right;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.right - 1;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.right - 1;
  wpt[1].y = rect.top;
  ::Polyline( hDC, wpt, 2 );
  // bottom
  wpt[0].x = rect.left;
  wpt[0].y = rect.bottom + 1;
  wpt[1].x = rect.right;
  wpt[1].y = rect.bottom + 1;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.left;
  wpt[0].y = rect.bottom;
  wpt[1].x = rect.right;
  wpt[1].y = rect.bottom;
  ::Polyline( hDC, wpt, 2 );
  wpt[0].x = rect.left;
  wpt[0].y = rect.bottom - 1;
  wpt[1].x = rect.right;
  wpt[1].y = rect.bottom - 1;
  ::Polyline( hDC, wpt, 2 );
}


// Draw a Text (instead of ::TextOut)
//-----------------------------------------------
void okDrawText (HDC hDC, int x, int y, LPCWSTR szText)
{
  ::TextOut( hDC, x, y, szText, (int)wcslen(szText) );
}

//-----------------------------------------------
void okDrawArrow (HDC hDC, POINT Ptw, double Ang, int Len, int HalfWidth)
{
  POINT    wpt[3];
  CGePoint Pt(Ptw.x,Ptw.y);
  CGePoint Pt1;
  CGePoint Pt0 = Pt.PolarBy( Ang, -Len );
  double   W = HalfWidth;

  Ang += GE_DEG90;
  Pt1 = Pt0.PolarBy( Ang, W );
  Pt0.Polar( Ang, -W );
  wpt[0].x = (int)(Pt0.x + 0.5);
  wpt[0].y = (int)(Pt0.y + 0.5);
  wpt[1].x = (int)(Pt1.x + 0.5);
  wpt[1].y = (int)(Pt1.y + 0.5);
  wpt[2].x = (int)(Pt.x + 0.5);
  wpt[2].y = (int)(Pt.y + 0.5);
  ::Polygon( hDC, wpt, 3 );
}

//-----------------------------------------------
void okDrawArrow (HDC hDC, POINT Ptw0, POINT Ptw1, int Len, int HalfWidth)
{
  POINT    wpt[3];
  CGePoint Pt0(Ptw0.x,Ptw0.y);
  CGePoint Pt(Ptw1.x,Ptw1.y);
  CGePoint Pt1;
  double   Ang = Pt0.Angle(Pt);
  double   W = HalfWidth;

  Pt0 = Pt.PolarBy( Ang, -Len );
  Ang += GE_DEG90;
  Pt1 = Pt0.PolarBy( Ang, W );
  Pt0.Polar( Ang, -W );
  wpt[0].x = (int)(Pt0.x + 0.5);
  wpt[0].y = (int)(Pt0.y + 0.5);
  wpt[1].x = (int)(Pt1.x + 0.5);
  wpt[1].y = (int)(Pt1.y + 0.5);
  wpt[2].x = (int)(Pt.x + 0.5);
  wpt[2].y = (int)(Pt.y + 0.5);
  ::Polygon( hDC, wpt, 3 );
}


