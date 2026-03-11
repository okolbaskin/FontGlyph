/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* "Info Panel"
********************************************************************/
#include "framework.h"
#include "FontGlyph.h"

#define CHK_POINTS   371
#define CHK_NUMS     372
#define CHK_LINES    373
#define CHK_CHARBOX  374

const WCHAR* szClassFrameWnd = L"LCFontInfoWnd";
static LRESULT CALLBACK ProcFrameWnd (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

extern CFontGlyph g_App;

//-----------------------------------------------
InfoWnd::InfoWnd ()
{
  m_hInst = NULL;
  m_hwFrame = NULL;
  m_hwFileName1 = NULL;
  m_hwFileName2 = NULL;
  m_hwFontName1 = NULL;
  m_hwFontName2 = NULL;
  m_hwNChars1 = NULL;
  m_hwNChars2 = NULL;
  m_hwHorLine = NULL;
  m_hwCharCode1 = NULL;
  m_hwCharCode2 = NULL;
  m_hwSubset1 = NULL;
  m_hwSubset2 = NULL;
  m_hwCoords1 = NULL;
  m_hwCoords2 = NULL;
  m_hwCharW1 = NULL;
  m_hwCharW2 = NULL;
  m_hwCharPts1 = NULL;
  m_hwCharPts2 = NULL;
  m_hwChkPoints = NULL;
  m_hwChkNums = NULL;
  m_hwChkLines = NULL;
  m_hwChkRect = NULL;
}


//-----------------------------------------------
InfoWnd::~InfoWnd ()
{
}


//-----------------------------------------------
void InfoWnd::RegisterClass () const
{
  BOOL gc;
  WNDCLASSEXW wc;
  memset( &wc, 0, sizeof(WNDCLASSEXW) );
  // Check if the class already registered
  gc = ::GetClassInfoEx( m_hInst, szClassFrameWnd, &wc );
  if (!gc){
    wc.lpszClassName = szClassFrameWnd;
//    wc.style         = CS_OWNDC | CS_BYTEALIGNCLIENT;
    wc.hInstance     = m_hInst;
    wc.lpfnWndProc   = (WNDPROC)ProcFrameWnd;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.cbClsExtra    = NULL;
    wc.cbWndExtra    = NULL;
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.hIconSm       = NULL;
    ::RegisterClassEx( &wc );
  }
}


// Window procedure
//-----------------------------------------------
LRESULT CALLBACK ProcFrameWnd (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  InfoWnd* ptr = (InfoWnd*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
  if (ptr){
    return ptr->WinProcFrame( Msg, wParam, lParam );
  }
  return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}


//-----------------------------------------------
int InfoWnd::WinProcFrame (UINT Msg, WPARAM wParam, LPARAM lParam)
{
  int   id;

  switch (Msg) {
    case WM_COMMAND:
      id = LOWORD(wParam);
      switch( id ) {
        case CHK_POINTS:
          g_App.ToggleShowPoints();
          return TRUE;
        case CHK_NUMS:
          g_App.ToggleShowNums();
          return TRUE;
        case CHK_LINES:
          g_App.ToggleShowLines();
          return TRUE;
        case CHK_CHARBOX:
          g_App.ToggleShowBox();
          return TRUE;
      }
      break;
  }
  return (int)::DefWindowProc( m_hwFrame, Msg, wParam, lParam );
}


//-----------------------------------------------
HWND InfoWnd::Create (HINSTANCE hInst, HWND hwParent)
{
  UINT  ExStyle, Style;

  m_hInst = hInst;
  RegisterClass ();
  ExStyle = WS_EX_CLIENTEDGE; //WS_EX_STATICEDGE; //WINDOWEDGE; //
  Style = WS_CHILD | WS_VISIBLE | WS_DLGFRAME;
  m_hwFrame = ::CreateWindowEx( ExStyle, szClassFrameWnd, L"", Style,
                                 0, 0, 100, 100, hwParent, 0, m_hInst, NULL );
  if (m_hwFrame == NULL){
    return NULL;
  }
  ::SetWindowLongPtr( m_hwFrame, GWLP_USERDATA, (LONG_PTR)this );

  //--- Font File
  m_hwFileName1 = ::CreateWindowEx( 0, L"STATIC", STR_FONTFILE, WS_CHILD | WS_VISIBLE, 
                                     0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwFileName1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  m_hwFileName2 = ::CreateWindowEx( 0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_READONLY, 
                                     0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwFileName2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- Font Name
  m_hwFontName1 = ::CreateWindowEx( 0, L"STATIC", STR_FONTNAME, WS_CHILD | WS_VISIBLE, 
                                     0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwFontName1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwFontName2 = ::CreateWindowEx( 0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_READONLY, 
                                     0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwFontName2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- Amount of characters in the font
  m_hwNChars1 = ::CreateWindowEx( 0, L"STATIC", STR_FONTCHARS, WS_CHILD | WS_VISIBLE, 
                                   0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwNChars1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwNChars2 = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, // | WS_BORDER, 
                                   0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwNChars2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- horizontal line
  m_hwHorLine = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
                                     0,100,300,20, m_hwFrame, NULL, m_hInst, NULL ); 

  //--- Char Code
  m_hwCharCode1 = ::CreateWindowEx( 0, L"STATIC", STR_CHARCODE, WS_CHILD | WS_VISIBLE, 
                                     0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCharCode1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwCharCode2 = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 
                                     0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCharCode2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- Unicode subset
  m_hwSubset1 = ::CreateWindowEx( 0, L"STATIC", STR_USUBSET2, WS_CHILD | WS_VISIBLE, // | WS_BORDER, 
                                   0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwSubset1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwSubset2 = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE,  //| WS_BORDER, 
                                   0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwSubset2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- Cursor coordinates
  m_hwCoords1 = ::CreateWindowEx( 0, L"STATIC", STR_CURSORXY, WS_CHILD | WS_VISIBLE, // | WS_BORDER, 
                                   0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCoords1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwCoords2 = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, //  | WS_BORDER, 
                                   0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCoords2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- Symbol width
  m_hwCharW1 = ::CreateWindowEx( 0, L"STATIC", STR_CHARWIDTH, WS_CHILD | WS_VISIBLE, // | WS_BORDER, 
                                  0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCharW1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwCharW2 = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, //  | WS_BORDER, 
                                  0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCharW2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- Points and polylines in a symbol
  m_hwCharPts1 = ::CreateWindowEx( 0, L"STATIC", STR_CHARPOINTS, WS_CHILD | WS_VISIBLE, // | WS_BORDER, 
                                    0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCharPts1, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );
  m_hwCharPts2 = ::CreateWindowEx( 0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, //  | WS_BORDER, 
                                    0,0,100,20, m_hwFrame, NULL, m_hInst, NULL ); 
  ::SendMessage( m_hwCharPts2, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- checkbox "Display control points"
  Style = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX  | BS_TOP; // | WS_BORDER;
  m_hwChkPoints = ::CreateWindowEx( 0, L"BUTTON", STR_SHOWPOINTS, Style, 
	                                  300,10, 200,20, m_hwFrame, (HMENU)CHK_POINTS, m_hInst, NULL);
  ::SendMessage( m_hwChkPoints, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- checkbox "Display control points"
  Style = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX  | BS_TOP; // | WS_BORDER;
  m_hwChkNums = ::CreateWindowEx( 0, L"BUTTON", STR_SHOWNUMS, Style, 
	                                300,10, 200,20, m_hwFrame, (HMENU)CHK_NUMS, m_hInst, NULL);
  ::SendMessage( m_hwChkNums, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- checkbox "Display control points"
  Style = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX  | BS_TOP; // | WS_BORDER;
  m_hwChkLines = ::CreateWindowEx( 0, L"BUTTON", STR_SHOWLINES, Style, 
	                                 300,10, 200,20, m_hwFrame, (HMENU)CHK_LINES, m_hInst, NULL);
  ::SendMessage( m_hwChkLines, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  //--- checkbox "Display symbol rectangle "
  Style = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX  | BS_TOP;
  m_hwChkRect = ::CreateWindowEx( 0, L"BUTTON", STR_SHOWCHARBOX, Style, 
	                                 300,10, 200,20, m_hwFrame, (HMENU)CHK_CHARBOX, m_hInst, NULL);
  ::SendMessage( m_hwChkRect, WM_SETFONT, (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), false );

  return m_hwFrame;
}

//-----------------------------------------------
void InfoWnd::Resize (int X, int Y, int W, int H)
{
  int x1, x2, x3, w1, w2, w2b, w3;
  if (::IsWindow(m_hwFrame)){
    ::MoveWindow( m_hwFrame, X, Y, W, H, true );
    x1 = 5;
    w1 = 118;
    x2 = 125;
    w2 = W - x2 - x1 - x1 - 5;
    x3 = W / 2;
    if (x3 < 250){
      if (W > 300){
        x3 = 250;
      }
    }
    w2b = x3 - x2 - 10;
    w3 = W / 2 - 15;
    ::MoveWindow( m_hwFileName1,  x1,   4,  w1, 15, true );
    ::MoveWindow( m_hwFileName2,  x2,   4,  w2, 15, true );
    ::MoveWindow( m_hwFontName1,  x1,  22,  w1, 15, true );
    ::MoveWindow( m_hwFontName2,  x2,  22,  w2, 15, true );
    ::MoveWindow( m_hwNChars1  ,  x1,  40,  w1, 15, true );
    ::MoveWindow( m_hwNChars2  ,  x2,  40,  70, 15, true );
    ::MoveWindow( m_hwHorLine  ,   1,  60,   W,  2, true );
    ::MoveWindow( m_hwCharCode1,  x1,  70,  w1, 15, true );
    ::MoveWindow( m_hwCharCode2,  x2,  70,  w2, 15, true );
    ::MoveWindow( m_hwSubset1  ,  x1,  88,  w1, 15, true );
    ::MoveWindow( m_hwSubset2  ,  x2,  88,  w2, 15, true );
    ::MoveWindow( m_hwCharW1   ,  x1, 106,  w1, 15, true );
    ::MoveWindow( m_hwCharW2   ,  x2, 106,  w2, 15, true );
    ::MoveWindow( m_hwCharPts1 ,  x1, 124,  w1, 15, true );
    ::MoveWindow( m_hwCharPts2 ,  x2, 124, w2b, 15, true );
    ::MoveWindow( m_hwCoords1  ,  x1, 142,  w1, 15, true );
    ::MoveWindow( m_hwCoords2  ,  x2, 142, w2b, 15, true );
    ::MoveWindow( m_hwChkPoints,  x3,  88,  w3, 15, true );
    ::MoveWindow( m_hwChkNums  ,  x3, 106,  w3, 15, true );
    ::MoveWindow( m_hwChkLines ,  x3, 124,  w3, 15, true );
    ::MoveWindow( m_hwChkRect  ,  x3, 142,  w3, 15, true );
  }
}


//-----------------------------------------------
void InfoWnd::Invalidate ()
{
  ::InvalidateRect( m_hwFrame, NULL, false );
}


//-----------------------------------------------
void InfoWnd::SetFileName (LPCWSTR szFileName) const
{
  ::SetWindowText( m_hwFileName2, szFileName );
}


//-----------------------------------------------
void InfoWnd::SetFontName (LPCWSTR szFontName, int Mode)
{
  WCHAR szBuf[256];
  wcscpy( szBuf, szFontName );
  switch( Mode ){
    case TTF_BOLD:
      wcscat_s( szBuf, 254, L"  (Bold)" );
      break;
    case TTF_ITALIC:
      wcscat_s( szBuf, 254, L"  (Italic)" );
      break;
    case TTF_BOLDITALIC:
      wcscat_s( szBuf, 254, L"  (Bold Italic)" );
      break;
  }
  ::SetWindowText( m_hwFontName2, szBuf );
}

//-----------------------------------------------
void InfoWnd::SetNumChars (int nChars, int nSelChars) const
{
  WCHAR szText[64];
  swprintf( szText, L"%d", nChars );
  ::SetWindowText( m_hwNChars2, szText );
}

//-----------------------------------------------
void InfoWnd::SetNumPoints (int nPoints, int nPlines) const
{
  WCHAR szText[64];
  swprintf( szText, STR_CHARPOINTS2, nPoints, nPlines );  // "%d  (in %d polylines)"
  ::SetWindowText( m_hwCharPts2, szText );
}

//-----------------------------------------------
void InfoWnd::ShowCtrlPoints (bool bShow) const
{
  ::SendMessage( m_hwChkPoints, BM_SETCHECK, bShow?BST_CHECKED:BST_UNCHECKED, 0 );
  ::EnableWindow( m_hwChkNums, bShow );
  ::EnableWindow( m_hwChkLines, bShow );
}

//-----------------------------------------------
void InfoWnd::ShowCtrlNums (bool bShow) const
{
  ::SendMessage( m_hwChkNums, BM_SETCHECK, bShow?BST_CHECKED:BST_UNCHECKED, 0 );
}

//-----------------------------------------------
void InfoWnd::ShowCtrlLines (bool bShow) const
{
  ::SendMessage( m_hwChkLines, BM_SETCHECK, bShow?BST_CHECKED:BST_UNCHECKED, 0 );
}

//-----------------------------------------------
void InfoWnd::ShowCharBox (bool bShow) const
{
  ::SendMessage( m_hwChkRect, BM_SETCHECK, bShow?BST_CHECKED:BST_UNCHECKED, 0 );
}

//-----------------------------------------------
void InfoWnd::SetCharCode (int Code, LPCWSTR szName) const
{
  WCHAR szText[256];
  if (Code >= 0){
    if (szName){
      swprintf( szText, L"%04X  (%d)  %s", Code, Code, szName );
    }else{
      swprintf( szText, L"%04X  (%d)", Code, Code );
    }
  }else{
    wcscpy( szText, STR_UNKNOWN );
  }
  ::SetWindowText( m_hwCharCode2, szText );
}

//-----------------------------------------------
void InfoWnd::SetSubset (const USUBSET* pUS) const
{
  WCHAR szText[256];
  if (pUS){
    swprintf( szText, L"%s  (%04X - %04X)", pUS->szDesc, pUS->Start, pUS->End );
  }else{
    wcscpy( szText, STR_UNKNOWN );
  }
  ::SetWindowText( m_hwSubset2, szText );
}

//-----------------------------------------------
void InfoWnd::SetCoords (double x, double y, LPCWSTR szSnapInfo) const
{
  WCHAR szText[64], szX[32], szY[32];
  okNumToStr( x, szX, 4, true, true );
  okNumToStr( y, szY, 4, true, true );
  swprintf( szText, L"%s,  %s  %s", szX, szY, szSnapInfo );
  ::SetWindowText( m_hwCoords2, szText );
}

//-----------------------------------------------
void InfoWnd::SetCharWidth (double w1, double w2) const
{
  WCHAR szText[64], szW1[32], szW2[32];
  okNumToStr( w1, szW1, 4, true, true );
  okNumToStr( w2, szW2, 4, true, true );
  swprintf( szText, L"%s  (%s)", szW1, szW2 );
  ::SetWindowText( m_hwCharW2, szText );
}

