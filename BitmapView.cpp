
/********************************************************************
* Project: Bitmap View
* Copyright (C) 2015-2024, Oleg Kolbaskin
* All rights reserved.
*
* Class CBitmapView implementation
********************************************************************/
#include "CtrlGrid.h"
#include "okDib.h"

#ifdef _BITMAPVIEW

static wchar_t* szClassBView = L"BitmapViewWnd";

//void _SetKbHook (BOOL b, CBitmapView* pBView);

//-----------------------------------------------
LRESULT CALLBACK ProcBView (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  CBitmapView* pBView = (CBitmapView*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
  if (pBView){
    return pBView->WinProc( Msg, wParam, lParam );
  }
  return ::DefWindowProc( hWnd, (UINT)Msg, wParam, lParam);
}


//*******************************************************************
CBitmapView::CBitmapView ()
{
  bool bTopDown = false;
  int  WS = ::GetSystemMetrics( SM_CXSCREEN );
  int  HS = ::GetSystemMetrics( SM_CYSCREEN );

  m_hInst   = NULL;
  m_hwParent = NULL;
  m_hwFrame = NULL;
  m_FrameWidth = 0;
  m_FrameHeight = 0;
  m_bViewAll = true;
  m_pBmp = NULL;
  m_BmpW = 0;
  m_BmpH = 0;
  m_LineBytes = 0;
  memset( m_szInfo, 0, sizeof(m_szInfo) );
  m_Scale = 1;
  m_BmpX0 = 0;
  m_BmpY0 = 0;
  m_pFireEvent = NULL;
/*
  memset( &m_Bmi, 0, sizeof(BITMAPINFO) );
  memset( m_Colors, 0, sizeof(m_Colors) );
  m_Bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  hDC = ::GetDC(0);
  m_hWndBitmap = ::CreateCompatibleBitmap( hDC, WS, HS );
  n = ::GetDIBits( hDC, m_hWndBitmap, 0, HS, NULL, &m_Bmi, DIB_RGB_COLORS );
  if (n > 0){
    n = m_Bmi.bmiHeader.biSizeImage;
    m_Bits = new BYTE[n];   // used to clone m_hWndBitmap into m_hWndBitmap2
  }else{
    m_Bits = NULL;
  }
*/
  m_pDib = new COkDib;
  m_pDib->Create( WS, HS, 24, NULL, NULL, bTopDown );
}

//-----------------------------------------------
CBitmapView::~CBitmapView ()
{
/*
  if (m_hWndBitmap){
    ::DeleteObject( m_hWndBitmap );
  }
  if (m_Bits){
    delete[] m_Bits;
  }
*/
  delete m_pDib;
}

//-----------------------------------------------
void CBitmapView::SetBitmap (const BYTE* pBitmap, int W, int H)
{
  int w2;
  if ((W >= 100) && (H >= 100)){
    m_pBmp = pBitmap;
    m_BmpW = W;
    m_BmpH = H;
    m_LineBytes = m_BmpW >> 3;
    w2 = m_LineBytes << 3;
    if (w2 < m_BmpW){
      ++m_LineBytes;
    }
  }else{
    m_pBmp = NULL;
    m_BmpW = 0;
    m_BmpH = 0;
  }
  m_Scale = 1;
  m_BmpX0 = 0;
  m_BmpY0 = 0;
}

//-----------------------------------------------
void CBitmapView::SetEventProc (F_BVIEWEVENT pFunc)
{
  m_pFireEvent = pFunc;
}


//-----------------------------------------------
bool CBitmapView::Create (int X, int Y, int W, int H, HWND hwParent, INT_PTR ID, HINSTANCE hInst)
{
  bool  bOK = false;
  DWORD ExStyle = 0; //WS_EX_CLIENTEDGE;
  DWORD Style = WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE;
  WNDCLASSEXW wc;
  BOOL  gc;

  m_hInst = hInst;
  m_hwParent = hwParent;
  memset( &wc, 0, sizeof(WNDCLASSEX) );
  // Check if the class is already registered
  gc = ::GetClassInfoEx( hInst, szClassBView, &wc );
  if (gc){
    bOK = true;
  }else{
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpszClassName = szClassBView;
//    wc.style         = CS_OWNDC | CS_BYTEALIGNCLIENT;
    wc.hInstance     = hInst;
    wc.lpfnWndProc   = (WNDPROC)ProcBView;
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
//    ExStyle = 0; //m_WndExStyle;
//    Style = WS_CHILD | WS_VISIBLE;
//    Style |= WS_BORDER;
/*
    if (m_WndStyle & WS_BORDER){
      Style |= WS_BORDER;
    }
*/
    m_hwFrame = ::CreateWindowEx( ExStyle, szClassBView, L"", Style, X, Y, W, H, 
                                  hwParent, (HMENU)ID, hInst, NULL );
    if (m_hwFrame){
      ::SetWindowLongPtr( m_hwFrame, GWLP_USERDATA, (LONG_PTR)this );


      return true;
    }
  }
  return false;
}


//-----------------------------------------------
void CBitmapView::MoveWindow (int X, int Y, int W, int H)
{
  if (m_hwFrame){
    ::MoveWindow( m_hwFrame, X, Y, W, H, false );
    m_FrameWidth = W;
    m_FrameHeight = H;
    UpdateScrollers();
  }
}


//-----------------------------------------------
void CBitmapView::Redraw () const
{
  ::InvalidateRect( m_hwFrame, NULL, true );
}


//-----------------------------------------------
void CBitmapView::SetFocus () const
{
  ::SetFocus( m_hwFrame );
}

//-----------------------------------------------
void CBitmapView::UpdateInfo (int Xcur, int Ycur)
{
  int Scale[]={100,200,400,600,800};
  int i = m_Scale - 1;
  WCHAR szX[16], szY[16];

  _itow( Scale[i], m_szInfo, 10 );
  if ((Xcur < 0) || (Ycur < 0)){
    wcscat( m_szInfo, L"% " );
  }else{
    _itow( Xcur, szX, 10 );
    _itow( Ycur, szY, 10 );
    wcscat( m_szInfo, L"%     Pos: " );
    wcscat( m_szInfo, szX );
    wcscat( m_szInfo, L", " );
    wcscat( m_szInfo, szY );
  }
  if (m_pFireEvent){
    (m_pFireEvent)(this);
  }
}

//-----------------------------------------------
void CBitmapView::PanView (int Dir, int Step)
{
  int DotSize;
  int WndW = m_FrameWidth;
  int WndH = m_FrameHeight;
  int Delta, NumDots, Xmax, Ymax;
  double Coef;

  switch( m_Scale ){
    case 2:  DotSize=2; break;
    case 3:  DotSize=4; break;
    case 4:  DotSize=6; break;
    case 5:  DotSize=8; break;
    default: DotSize=1; break;
  }

  if (Dir == -VK_RIGHT){
    Coef = (double)Step / (double)m_FrameWidth;
    Delta = (int)((Coef * m_BmpW) + 0.5);
    m_BmpX0 += Delta;
    if (Step > 0){
      NumDots = WndW / DotSize;  // number of bitmap dots in window row
      goto m4;
    }else{
      goto m3;
    }
  }else
  if (Dir == -VK_DOWN){
    Coef = (double)Step / (double)m_FrameHeight;
    Delta = (int)((Coef * m_BmpH) + 0.5);
    m_BmpY0 += Delta;
    if (Step > 0){
      NumDots = WndH / DotSize;  // number of bitmap dots in window row
      goto m2;
    }else{
      goto m1;
    }
  }

  switch( Dir ){
    case VK_UP:
      NumDots = WndH / DotSize;  // number of bitmap dots in window row
      if (Step == 1){
        Delta = NumDots / 10;
      }else{
        Delta = NumDots * 4 / 5;
      }
      m_BmpY0 -= Delta;
m1:
      if (m_BmpY0 < 0){
        m_BmpY0 = 0;
      }
      break;

    case VK_DOWN:
      NumDots = WndH / DotSize;  // number of bitmap dots in window row
      if (Step == 1){
        Delta = NumDots / 10;
      }else{
        Delta = NumDots * 4 / 5;
      }
      m_BmpY0 += Delta;
m2:
      Ymax = m_BmpY0 + NumDots;
      if (Ymax >= m_BmpH){
        m_BmpY0 = m_BmpH - NumDots;
        m_BmpY0 >>= 3;
        m_BmpY0 <<= 3;
        m_BmpY0 += 8;
      }
      break;

    case VK_LEFT:
      NumDots = WndW / DotSize;  // number of bitmap dots in window row
      if (Step == 1){
        Delta = NumDots / 10;
      }else{
        Delta = NumDots * 4 / 5;
      }
      if (Delta <= 15){
        Delta = 8;
      }else{
        Delta >>= 3;  // /8
        Delta <<= 3;  // *8
      }
      m_BmpX0 -= Delta;
m3:
      if (m_BmpX0 < 0){
        m_BmpX0 = 0;
      }
      break;

    case VK_RIGHT:
      NumDots = WndW / DotSize;  // number of bitmap dots in window row
      if (Step == 1){
        Delta = NumDots / 10;
      }else{
        Delta = NumDots * 4 / 5;
      }
      if (Delta <= 15){
        Delta = 8;
      }else{
        Delta >>= 3;  // /8
        Delta <<= 3;  // *8
      }
      m_BmpX0 += Delta;
m4:
      Xmax = m_BmpX0 + NumDots;
      if (Xmax >= m_BmpW){
        m_BmpX0 = m_BmpW - NumDots;
        m_BmpX0 >>= 3;
        m_BmpX0 <<= 3;
        m_BmpX0 += 8;
      }
      break;
  }
  UpdateScrollers();
  Redraw();
}


//-----------------------------------------------
void CBitmapView::OnPaint (HDC hOutDC, RECT Rect)
{
  const BYTE* pLine0 = NULL;
  const BYTE* pLine;
  int      i, j, k, WndX, WndY, WndW, WndH, BmpX, BmpY;
  int      DotSize;
  BYTE     Mask, Byt;
  HGDIOBJ  hPrevObj;
/*
  HGDIOBJ  hPrevBrush;
  HBRUSH   hBrushB, hBrushW, hBrushA;
  COLORREF ColorB = RGB(0,0,0);
  COLORREF ColorW = RGB(255,255,255);
  COLORREF ColorA = RGB(160,160,160);
*/
  HDC       hDC=0;
  HBITMAP   hWndBitmap;
  OK_PIXPOS Pos;

  WndW = m_FrameWidth  = Rect.right  - Rect.left + 1;
  WndH = m_FrameHeight = Rect.bottom - Rect.top  + 1;

  // create memory DC
  hWndBitmap = ::CreateCompatibleBitmap( hOutDC, m_FrameWidth, m_FrameHeight );
  hDC = ::CreateCompatibleDC( hOutDC );
  hPrevObj = ::SelectObject( hDC, hWndBitmap );
/*
  hBrushB = ::CreateSolidBrush( ColorB );
  hBrushW = ::CreateSolidBrush( ColorW );
  hBrushA = ::CreateSolidBrush( ColorA );
  hPrevBrush = ::SelectObject( hDC, hBrushA );
  ::PatBlt( hDC, 0,0, WndW, WndH, PATCOPY );
*/
  m_pDib->SetPixels24( 160, 160, 160 );
  if (m_pBmp == NULL){
    goto mEnd;
  }
  switch( m_Scale ){
    case 2:  DotSize=2; break;
    case 3:  DotSize=4; break;
    case 4:  DotSize=6; break;
    case 5:  DotSize=8; break;
    default: DotSize=1; break;
  }
  m_pDib->SetPixPos_First( &Pos );
  pLine0 = m_pBmp + ((INT_PTR)m_BmpY0 * m_LineBytes);
  BmpY = m_BmpY0;
  k = 0;
  for (WndY=0; WndY<WndH; ){
    pLine = pLine0 + (m_BmpX0>>3);
    WndX = 0;
    BmpX = m_BmpX0;
    for(;;){
      Byt = *pLine;
      Mask = 0x80;
      for (i=0; i<8; ++i){
        if (Byt & Mask){
          for (j=0; j<DotSize; ++j){
            ++WndX;
            m_pDib->SetPixel24( &Pos, 0,0,0 );
            m_pDib->SetPixPos_Next24( &Pos );
          }
        }else{
          for (j=0; j<DotSize; ++j){
            ++WndX;
            m_pDib->SetPixel24( &Pos, 255,255,255 );
            m_pDib->SetPixPos_Next24( &Pos );
          }
        }
        Mask>>=1;
        ++BmpX;
        if (WndX >= WndW){
          goto mNextLine;
        }
        if (BmpX >= m_BmpW){
          goto mNextLine;
        }
      }
      ++pLine;
    }
mNextLine:
    ++WndY;
    m_pDib->SetPixPos_NextLine( &Pos );
    ++k;
    if (k == DotSize){
      k = 0;
      pLine0 += m_LineBytes;
      ++BmpY;
      if (BmpY >= m_BmpH){
        break;
      }
    }
  }
mEnd:
/*
  ::SelectObject( hDC, hPrevBrush );
  ::DeleteObject( hBrushB );
  ::DeleteObject( hBrushW );
  ::DeleteObject( hBrushA );
*/
/*
  m_pDib->SetPixel24( 0,0, 255,0,0 );
  m_pDib->SetPixel24( 1,1, 255,0,0 );
  m_pDib->SetPixel24( 2,2, 255,0,0 );
  m_pDib->SetPixel24( 3,3, 255,0,0 );
  m_pDib->SetPixel24( 4,3, 255,0,0 );
  m_pDib->SetPixel24( 5,3, 255,0,0 );
  m_pDib->SetPixel24( 10,4, 255,0,0 );
*/
  m_pDib->DrawBitBlt( hDC, m_FrameWidth, m_FrameHeight );

  // copy drawing's view from a memory to a window
  ::BitBlt( hOutDC, 0, 0, m_FrameWidth, m_FrameHeight, hDC, 0,0, SRCCOPY );

  ::SelectObject( hDC, hPrevObj );
  ::DeleteObject( hWndBitmap );
  // delete memory DC
  ::DeleteDC( hDC );
}

//-----------------------------------------------
void CBitmapView::OnKeyDown (WPARAM KeyCode, LPARAM /*KeyData*/)
{
  switch( KeyCode ){
    case VK_TAB:
      m_bViewAll = !m_bViewAll;
      break;
    case VK_UP:
      PanView( VK_UP );
      break;
    case VK_DOWN:
      PanView( VK_DOWN );
      break;
    case VK_LEFT:
      PanView( VK_LEFT );
      break;
    case VK_RIGHT:
      PanView( VK_RIGHT );
      break;
    case VK_PRIOR:
      if (m_Scale > 1){
        m_Scale--;
        UpdateScrollers();
        Redraw();
        UpdateInfo();
      }
      break;
    case VK_NEXT:
      if (m_Scale < 5){
        ++m_Scale;
        UpdateScrollers();
        Redraw();
        UpdateInfo();
      }
      break;
  }
}

//-----------------------------------------------
void CBitmapView::OnMouseMove (int Xcur, int Ycur)
{
  int x, y;

  switch( m_Scale ){
    case 2:  // 200%
      x = Xcur/2;
      y = Ycur/2;
      break;
    case 3:  // 400%
      x = Xcur/4;
      y = Ycur/4;
      break;
    case 4:  // 600%
      x = Xcur/6;
      y = Ycur/6;
      break;
    case 5:  // 800%
      x = Xcur/8;
      y = Ycur/8;
      break;
    default:  // 100%
      x = Xcur;
      y = Ycur;
      break;
  }
  x += m_BmpX0;
  y += m_BmpY0;
  UpdateInfo( x, y );
}

//-----------------------------------------------
void CBitmapView::OnLButtonDown (int Xcur, int Ycur)
{
  Xcur = Ycur;
  ::SetFocus( m_hwFrame ); 
}

//-----------------------------------------------
void CBitmapView::OnRButtonDown (int Xcur, int Ycur)
{
  UINT  Tpm = TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN | TPM_LEFTBUTTON;
  UINT  Cmd;
  POINT wpt;
  int   i, Flags[5];
  HMENU hMenu = ::CreatePopupMenu();
  
  for (i=0; i<5; ++i){
    Flags[i] = MF_STRING;
    if (m_Scale == i+1){
      Flags[i] |= MF_CHECKED;
    }
  }
  ::AppendMenu( hMenu, Flags[0], 101, L"100%" );
  ::AppendMenu( hMenu, Flags[1], 102, L"200%" );
  ::AppendMenu( hMenu, Flags[2], 103, L"400%" );
  ::AppendMenu( hMenu, Flags[3], 104, L"600%" );
  ::AppendMenu( hMenu, Flags[4], 105, L"800%" );
  // display menu
  wpt.x = Xcur;
  wpt.y = Ycur;
  ::ClientToScreen( m_hwFrame, &wpt );
  Cmd = ::TrackPopupMenu( hMenu, Tpm, wpt.x, wpt.y, 0, m_hwFrame, NULL );
  switch( Cmd ){
    case 101: m_Scale=1; break;
    case 102: m_Scale=2; break;
    case 103: m_Scale=3; break;
    case 104: m_Scale=4; break;
    case 105: m_Scale=5; break;
  }
  UpdateScrollers();
  Redraw();
  UpdateInfo();
}

//-----------------------------------------------
void CBitmapView::OnHScroll (WPARAM SBCode, UINT Pos)
{
  int PosDelta=0;
  if (SBCode == SB_THUMBPOSITION || SBCode == SB_THUMBTRACK){
    PosDelta = Pos - m_ScrollPosX;
  }
  Scroll( false, SBCode, PosDelta );
}

//-----------------------------------------------
void CBitmapView::OnVScroll (WPARAM SBCode, UINT Pos)
{
  int PosDelta=0;
  if (SBCode == SB_THUMBPOSITION || SBCode == SB_THUMBTRACK){
    PosDelta = Pos - m_ScrollPosY;
  }
  Scroll( true, SBCode, PosDelta );
}


//-----------------------------------------------
void CBitmapView::UpdateScrollers ()
{
  SCROLLINFO si;
//  CGeRect    Ext;        // extents rect included View rect and drawing extents DwgExt
//  double     Scale=m_ViewScale;   // <model units>/<pixel>
  int        RangeX, RangeY, UnitX, UnitY, PosX, PosY, DotSize;

  switch( m_Scale ){
    case 2:  DotSize=2; break;
    case 3:  DotSize=4; break;
    case 4:  DotSize=6; break;
    case 5:  DotSize=8; break;
    default: DotSize=1; break;
  }
  RangeX = m_BmpW;
  RangeY = m_BmpH;
  UnitX  = m_FrameWidth / DotSize;
  UnitY  = m_FrameHeight / DotSize;
  // horizontal scroller position 
  PosX = m_BmpX0;
  PosY = m_BmpY0;
  // Ajust scrollbars in a window
  memset( &si, 0, sizeof(si) );
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
//  if (m_bSteadyScrollBars){
    si.fMask |= SIF_DISABLENOSCROLL;
//  }
//  if (m_WndStyle & WS_HSCROLL){
    si.nMin = 1;
    si.nMax = RangeX;
    si.nPage = UnitX;
    si.nPos = PosX;
    ::SetScrollInfo( m_hwFrame, SB_HORZ, &si, TRUE );
//  }
//  if (m_WndStyle & WS_VSCROLL){
    si.nMin = 1;
    si.nMax = RangeY;
    si.nPage = UnitY;
    si.nPos = PosY;
    ::SetScrollInfo( m_hwFrame, SB_VERT, &si, TRUE );
//  }
  m_ScrollRangeX = RangeX;
  m_ScrollRangeY = RangeY;
  m_ScrollUnitX  = UnitX ;
  m_ScrollUnitY  = UnitY ;
  m_ScrollPosX   = PosX  ;
  m_ScrollPosY   = PosY  ;
}

//-----------------------------------------------
void CBitmapView::Scroll (bool bVertical, WPARAM ScrollCode, int PDelta)
{
  //double Step;
//  double PosDelta = PDelta;

  if (bVertical){
    switch( ScrollCode ){
      case SB_LINEUP:     PanView( VK_UP, 1 ); break;
      case SB_LINEDOWN:   PanView( VK_DOWN, 1 ); break;
      case SB_PAGEUP:     PanView( VK_UP,  2 ); break;
      case SB_PAGEDOWN:   PanView( VK_DOWN, 2 ); break;
      case SB_THUMBTRACK:
//        if (!g_bSmoothScroll){
          break;
//        }
      case SB_THUMBPOSITION:
        PanView( -VK_DOWN, PDelta );
        break;
      case SB_TOP:
        break;
      case SB_BOTTOM:
        break;
    }
  }else{
    switch( ScrollCode ){
      case SB_LINERIGHT:  PanView( VK_RIGHT, 1 ); break;
      case SB_LINELEFT:   PanView( VK_LEFT, 1 ); break;
      case SB_PAGERIGHT:  PanView( VK_RIGHT, 2 ); break;
      case SB_PAGELEFT:   PanView( VK_LEFT, 2 ); break;
      case SB_THUMBTRACK:
//        if (!g_bSmoothScroll){
          break;
//        }
      case SB_THUMBPOSITION:
        PanView( -VK_RIGHT, PDelta );
        break;
      case SB_RIGHT:
        break;
      case SB_LEFT:
        break;
    }
  }
//  UpdateScrollers();
//  Redraw();
}

//=======================================================================================
LRESULT CBitmapView::WinProc (UINT Msg, WPARAM wParam, LPARAM lParam)
{
  int x, y;
//  DRAWITEMSTRUCT* pDis;
  PAINTSTRUCT ps;
  HDC hDC;


  switch( Msg ){
    case WM_PAINT:
      hDC = ::BeginPaint( m_hwFrame, &ps);
      OnPaint( hDC, ps.rcPaint );
      ::EndPaint( m_hwFrame, &ps );
//      pDis = (DRAWITEMSTRUCT*)lParam;
//      OnDraw( pDis );
      break;

    case WM_SIZE:
      UpdateScrollers();
      break;

    case WM_KEYDOWN:
      OnKeyDown( wParam, lParam ); 
      break;

    case WM_MOUSEMOVE:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      OnMouseMove( x, y );
      break;

    case WM_LBUTTONDOWN:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      OnLButtonDown( x, y );
      break;

    case WM_RBUTTONDOWN:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      OnRButtonDown( x, y );
      break;

    case WM_HSCROLL:  
      OnHScroll( LOWORD(wParam), HIWORD(wParam) );  
      break;

    case WM_VSCROLL:  
      OnVScroll( LOWORD(wParam), HIWORD(wParam) );  
      break;
  }
  return ::DefWindowProc( m_hwFrame, Msg, wParam, lParam );
}

/*
static HHOOK        _hHook = NULL;
static CBitmapView* _pBView = NULL;


//*******************************************************************
//  HOOK PROCEDURE
//*******************************************************************
static LRESULT CALLBACK BViewHookProc (
  int code,        // hook code
  WPARAM wParam,   // virtual-key code
  LPARAM lParam)   // keystroke-message information
{
  if (code < 0){   
    // do not process message 
    return ::CallNextHookEx( _hHook, code, wParam, lParam ); 
  }
  if (_pBView){
//    _pBView->WinProc( WM_KEYDOWN, wParam, lParam );
    _pBView->OnKeyDown( wParam, lParam ); 
//    return true;
  }
  return ::CallNextHookEx( _hHook, code, wParam, lParam ); 
}

//-----------------------------------------------
void CBitmapView::SetKbHook (BOOL b)
{
  DWORD idThread=0;
  if (b){
    // set message hook
    idThread = ::GetCurrentThreadId();
    if (_hHook){
      ::UnhookWindowsHookEx( _hHook );
    }
    _hHook = ::SetWindowsHookEx( WH_KEYBOARD, (HOOKPROC)BViewHookProc, m_hInst, idThread ); 
    _pBView = this;
  }else{
    // disable the hook
    if (_hHook){
      ::UnhookWindowsHookEx( _hHook );
      _hHook = NULL;
      _pBView = NULL;
    }
  }
}
*/

#endif   // _BITMAPVIEW
