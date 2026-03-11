/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class COkCtrlDiv
********************************************************************/
#include "framework.h"

static WCHAR szClassDivider[32];
static LRESULT CALLBACK ProcDivider (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

COLORREF COkCtrlDiv::m_Color  = ::GetSysColor( COLOR_BTNFACE );
COLORREF COkCtrlDiv::m_Color2 = ::GetSysColor( COLOR_3DDKSHADOW );
UINT     COkCtrlDiv::m_Size = 3;
HCURSOR  COkCtrlDiv::m_hCurHorz = 0;
HCURSOR  COkCtrlDiv::m_hCurVert = 0;


//-----------------------------------------------
COkCtrlDiv::COkCtrlDiv ()
{
  m_bEnable  = true;
  m_bVertical = false;
  m_hwParent = NULL;
  m_hWnd     = NULL;
  m_Left     = 0;
  m_Top      = 0;
  m_Width    = 0;
  m_Height   = 0; 
  m_hDrawDC  = NULL;
  m_hCursor  = NULL;
  m_bDrag    = false;
  m_Point.x  = 0;
  m_Point.y  = 0;
  m_PrevPt.x = m_PrevPt.y = -111;
  m_pPos     = NULL;
  m_bIncrease= true;
  m_bRigLine = false;
}

//-----------------------------------------------
COkCtrlDiv::~COkCtrlDiv ()
{
}

//-----------------------------------------------
HWND COkCtrlDiv::Create (HINSTANCE hInst, HWND hwParent, int Type, int* pPos, bool bInc)
{
  DWORD ExStyle = 0;
  DWORD Style = WS_CHILD | WS_VISIBLE;

  if (::IsWindow(m_hWnd)){
    return m_hWnd;    // already exist
  }
  m_hwParent = hwParent;
  if (Type == 0){
    m_bVertical = false;
    if (m_hCurHorz){
      m_hCursor = m_hCurHorz;
    }else{
      m_hCursor = ::LoadCursor( NULL, IDC_SIZENS );
    }
  }else{
    m_bVertical = true;
    if (m_hCurVert){
      m_hCursor = m_hCurVert;
    }else{
      m_hCursor = ::LoadCursor( NULL, IDC_SIZEWE );
    }
  }
  m_pPos = pPos;
  m_bIncrease = bInc;
  m_Left = m_Top = m_Width = m_Height = 0;
  RegisterClass( hInst );
  m_hWnd = ::CreateWindowEx( ExStyle, szClassDivider, L"", Style, 0, 0, 100, 100, m_hwParent, 0, hInst, NULL );
  if (m_hWnd){
    m_Width = 100;
    m_Height = 100;
    ::SetWindowLongPtr( m_hWnd, GWLP_USERDATA, (LONG_PTR)this );
    ::ShowWindow( m_hWnd, SW_SHOW );
    return m_hWnd;
  }
  return NULL;
}


//-----------------------------------------------
HWND COkCtrlDiv::CreateHor (HINSTANCE hInst, HWND hwParent, int* pPos, bool bInc) 
{
  return Create(hInst,hwParent,0,pPos,bInc);
}


//-----------------------------------------------
HWND COkCtrlDiv::CreateVer (HINSTANCE hInst, HWND hwParent, int* pPos, bool bInc, bool bRLine) 
{
  m_bRigLine = bRLine;
  return Create(hInst,hwParent,1,pPos,bInc);
}


//-----------------------------------------------
void COkCtrlDiv::Resize (int X, int Y, UINT Length)
{
  int W, H;
  if (::IsWindow( m_hWnd )){
    if (m_bVertical){
      W = m_Size;
      H = Length;
    }else{
      W = Length;
      H = m_Size;
    }
    ::MoveWindow( m_hWnd, X, Y, W, H, true );
    m_Left   = X;
    m_Top    = Y;
    m_Width  = W;
    m_Height = H;
  }
}

//-----------------------------------------------
void COkCtrlDiv::SetPosition (int Pos)
{
  int X, Y, W, H;
  if (::IsWindow( m_hWnd )){
    if (m_bVertical){
      X = m_Left = Pos;
      Y = m_Top;
      W = m_Size;
      H = m_Height;
    }else{
      X = m_Left;
      Y = m_Top = Pos;
      W = m_Width;
      H = m_Size;
    }
    ::MoveWindow( m_hWnd, X, Y, W, H, true );
  }
}


//-----------------------------------------------
bool COkCtrlDiv::RegisterClass (HINSTANCE hInst) const
{
  WNDCLASSEXW wc;
  memset( &wc, 0, sizeof(WNDCLASSEXW) );
  // Check if the class already registered
  wcscpy( szClassDivider, L"COkCtrlDivClass" );
  BOOL gc = ::GetClassInfoEx( hInst, szClassDivider, &wc );
  if (gc){
    return true;
  }else{
    wc.lpszClassName = szClassDivider;
    wc.hInstance     = hInst;
    wc.lpfnWndProc   = (WNDPROC)ProcDivider;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL; 
    wc.cbClsExtra    = NULL;
    wc.cbWndExtra    = NULL;
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.hIconSm       = NULL;
    if (::RegisterClassEx( &wc )){
      return true;
    }
  }
  return false;
}


// Window procedure
//-----------------------------------------------
LRESULT CALLBACK ProcDivider (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  COkCtrlDiv* prbp = (COkCtrlDiv*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
  if (prbp){
    return prbp->WinProc( Msg, wParam, lParam );
  }
  return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}


//-----------------------------------------------
LRESULT COkCtrlDiv::WinProc (UINT Msg, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  switch (Msg) {
    case WM_PAINT:     // Paint the window's client area.
      ::BeginPaint( m_hWnd, &ps );
      OnPaint( ps.hdc ); 
      ::EndPaint( m_hWnd, &ps );
      return 0;
    case WM_MOUSEMOVE:
      OnMouseMove( wParam, LOWORD(lParam), HIWORD(lParam) );
      break;
    case WM_LBUTTONDOWN:
      OnLButtonDown( wParam, LOWORD(lParam), HIWORD(lParam) );
      break;
    case WM_LBUTTONUP:
      OnLButtonUp( wParam, LOWORD(lParam), HIWORD(lParam) );
      break;
    default:
      break;
  }
  return ::DefWindowProc( m_hWnd, Msg, wParam, lParam );
}


//-----------------------------------------------
void COkCtrlDiv::OnPaint (HDC hOutDC) const
{
  POINT   pnt[2];
  HPEN    hPen;
  HGDIOBJ hPrevPen;
  // paint the window background (clear bitmap)
  HBRUSH  hBrush  = ::CreateSolidBrush( m_Color );
  HGDIOBJ hPrevBrush = ::SelectObject( hOutDC, hBrush );
  ::PatBlt( hOutDC, 0,0, m_Width, m_Height, PATCOPY );
  ::SelectObject( hOutDC, hPrevBrush );
  ::DeleteObject( hBrush );
  if (m_bRigLine){
    hPen = CreatePen( PS_SOLID, 1, m_Color2 );
    hPrevPen = ::SelectObject( hOutDC, hPen );
    pnt[0].x = (LONG)(m_Width - 1);
    pnt[0].y = 0;
    pnt[1].x = (LONG)(m_Width - 1);
    pnt[1].y = (LONG)(m_Height - 1);
    ::Polyline( hOutDC, pnt, 2 );
    ::SelectObject( hOutDC, hPrevPen );
    ::DeleteObject( hPen );
  }
}


//-----------------------------------------------
void COkCtrlDiv::OnMouseMove (WPARAM /*Flags*/, int x, int y)
{
  POINT Pt;
  int   delta;

  if (m_bEnable){
    ::SetCursor( m_hCursor );
    if (m_bDrag){
      if (m_bVertical){
        delta = x - m_Point.x;
      }else{
        delta = y - m_Point.y;
      }
      if (delta > 32767){
        delta -= 65536;
      }
      if (m_bVertical){
        Pt.x = (LONG)(m_Left + delta);
        Pt.y = (LONG)m_Top;
      }else{
        Pt.x = (LONG)m_Left;
        Pt.y = (LONG)(m_Top + delta);
      }
      ::ClientToScreen( m_hwParent, &Pt );
      if ((m_PrevPt.x != -111) && (m_PrevPt.y != -111)){
        DrawXorLine( m_PrevPt.x, m_PrevPt.y, m_Width, m_Height ); 
      }
      DrawXorLine( Pt.x, Pt.y, m_Width, m_Height ); 
      m_PrevPt = Pt;
    }
  }
}


//-----------------------------------------------
void COkCtrlDiv::OnLButtonDown (WPARAM /*Flags*/, int x, int y)
{
  if (m_bEnable){
    m_bDrag = true;
    m_Point.x = (LONG)x;
    m_Point.y = (LONG)y;
    m_PrevPt.x = m_PrevPt.y = -111;
    ::SetCapture( m_hWnd );
    m_hDrawDC = ::GetDC( 0 );
  }
}


//-----------------------------------------------
void COkCtrlDiv::OnLButtonUp (WPARAM /*Flags*/, int x, int y)
{
  int delta;
  if (m_bDrag && m_bEnable){
    if (m_bVertical){
      delta = x - m_Point.x;
    }else{
      delta = y - m_Point.y;
    }
    if (delta > 32767){
      delta -= 65536;
    }
    if (*m_pPos > 0){
      if (m_bIncrease){
        *m_pPos += delta;
      }else{
        *m_pPos -= delta;
      }
    }
    m_bDrag = false;
    ::ReleaseDC( 0, m_hDrawDC );
    ::ReleaseCapture();
    ::PostMessage( m_hwParent, WM_USER, MSG_DIV_MOVED, 0 );
  }
}


//-----------------------------------------------
void COkCtrlDiv::DrawXorLine (int x, int y, UINT width, UINT height) const
{
  // Raw bits for bitmap - enough for an 8x8 monochrome image
  static WORD _dotPatternBmp1[] = 
  {
    0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
  };
  HBITMAP hbm;
  HBRUSH  hbr;
  HANDLE  hbrushOld;

  // Create a patterned bitmap to draw the borders
  hbm = ::CreateBitmap( 8, 8, 1, 1, _dotPatternBmp1 );
  hbr = ::CreatePatternBrush( hbm );
  hbrushOld = ::SelectObject( m_hDrawDC, hbr );

  ::PatBlt( m_hDrawDC, x, y, width, height, PATINVERT);

  // Clean up
  ::SelectObject( m_hDrawDC, hbrushOld );
  ::DeleteObject( hbr );
  ::DeleteObject( hbm );
}


//-----------------------------------------------
void COkCtrlDiv::SetEnabled (bool b)
{
  m_bEnable = b;
}


    

