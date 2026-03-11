/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* Defines the entry point for the application.
********************************************************************/
#include <stdio.h>
#include "framework.h"
#include "FontGlyph.h"

const WCHAR* szWindowClass = L"FontGlyph_MainWindow";   // the main window class name

// Global Variables:
CFontGlyph g_App;
WCHAR      g_szLastDir[260];    // last opened directory

// Unicode standard subsets
USUBSET  g_USubSet[NUM_USUBSETS] = {
 {0x0000, 0x007F, 0, 0, L"Basic Latin" }, 
 {0x0080, 0x00FF, 0, 0, L"Latin-1 Supplement" },
 {0x0100, 0x017F, 0, 0, L"Latin Extended-A" },
 {0x0180, 0x024F, 0, 0, L"Latin Extended-B" },
 {0x0250, 0x02AF, 0, 0, L"IPA Extensions" },
 {0x02B0, 0x02FF, 0, 0, L"Spacing Modifier Letters" },
 {0x0300, 0x036F, 0, 0, L"Combining Diacritical" },
 {0x0370, 0x03FF, 0, 0, L"Greek" },
 {0x0400, 0x04FF, 0, 0, L"Cyrillic" },
 {0x0500, 0x052F, 0, 0, L"Cyrillic Supplement" },
 {0x0530, 0x058F, 0, 0, L"Armenian" },
 {0x0590, 0x05FF, 0, 0, L"Hebrew" },
 {0x0600, 0x06FF, 0, 0, L"Arabic" },
 {0x0700, 0x074F, 0, 0, L"Syriac" },
 {0x0750, 0x077F, 0, 0, L"Arabic Supplement" },
 {0x0780, 0x07BF, 0, 0, L"Thaana" },
 {0x07C0, 0x08FF, 0, 0, L"N'Ko" },
 {0x0900, 0x097F, 0, 0, L"Devanagari" },
 {0x0980, 0x09FF, 0, 0, L"Bengali" },
 {0x0A00, 0x0A7F, 0, 0, L"Gurmukhi" },
 {0x0A80, 0x0AFF, 0, 0, L"Gujarati" },
 {0x0B00, 0x0B7F, 0, 0, L"Oriya" },
 {0x0B80, 0x0BFF, 0, 0, L"Tamil" },
 {0x0C00, 0x0C7F, 0, 0, L"Telugu" },
 {0x0C80, 0x0CFF, 0, 0, L"Kannada" },
 {0x0D00, 0x0D7F, 0, 0, L"Malayalam" },
 {0x0D80, 0x0DFF, 0, 0, L"Sinhala" },
 {0x0E00, 0x0E7F, 0, 0, L"Thai" },
 {0x0E80, 0x0EFF, 0, 0, L"Lao" },
 {0x0F00, 0x0FFF, 0, 0, L"Tibetan" },
 {0x1000, 0x109F, 0, 0, L"Myanmar" },
 {0x10A0, 0x10FF, 0, 0, L"Georgian" },
 {0x1100, 0x11FF, 0, 0, L"Hangul Jamo" },
 {0x1200, 0x137F, 0, 0, L"Ethiopic" },
 {0x1380, 0x139F, 0, 0, L"Ethiopic Supplement" },
 {0x13A0, 0x13FF, 0, 0, L"Cherokee" },
 {0x1400, 0x167F, 0, 0, L"Unified Canadian Aboriginal Syllabics" },
 {0x1680, 0x169F, 0, 0, L"Ogham" },
 {0x16A0, 0x16FF, 0, 0, L"Runic" },
 {0x1700, 0x177F, 0, 0, L"Reserve" },
 {0x1780, 0x17FF, 0, 0, L"Khmer" },
 {0x1800, 0x18AF, 0, 0, L"Mongolian" },
 {0x18B0, 0x1DFF, 0, 0, L"Reserve" },
 {0x1E00, 0x1EFF, 0, 0, L"Latin Extended Additional" },
 {0x1F00, 0x1FFF, 0, 0, L"Greek Extended" },
 {0x2000, 0x206F, 0, 0, L"General Punctuation" },
 {0x2070, 0x209F, 0, 0, L"Subscripts and Superscripts" },
 {0x20A0, 0x20CF, 0, 0, L"Currency Symbols" },
 {0x20D0, 0x20FF, 0, 0, L"Combining Diacritical Marks for Symbols" },
 {0x2100, 0x214F, 0, 0, L"Letterlike Symbols" },
 {0x2150, 0x218F, 0, 0, L"Number Forms" },
 {0x2190, 0x21FF, 0, 0, L"Arrows" },
 {0x2200, 0x22FF, 0, 0, L"Mathematical" },
 {0x2300, 0x23FF, 0, 0, L"Miscellaneous" },
 {0x2400, 0x243F, 0, 0, L"Control Pictures" },
 {0x2440, 0x245F, 0, 0, L"Optical Character Recognition" },
 {0x2460, 0x24FF, 0, 0, L"Enclosed Alphanumerics" },
 {0x2500, 0x257F, 0, 0, L"Box Drawing" },
 {0x2580, 0x259F, 0, 0, L"Block Elements" },
 {0x25A0, 0x25FF, 0, 0, L"Geometric Shapes" },
 {0x2600, 0x26FF, 0, 0, L"Miscellaneous Symbols" },
 {0x2700, 0x27BF, 0, 0, L"Dingbats" },
 {0x27C0, 0x27FF, 0, 0, L"Reserve" },
 {0x2800, 0x28FF, 0, 0, L"Braille Patterns" },
 {0x2900, 0x2E7F, 0, 0, L"Reserve" },
 {0x2E80, 0x2EFF, 0, 0, L"CJK Radicals Supplement" },
 {0x2F00, 0x2FDF, 0, 0, L"Kangxi Radicals" },
 {0x2FE0, 0x2FEF, 0, 0, L"Reserve" },
 {0x2FF0, 0x2FFF, 0, 0, L"Ideographic Description Characters" },
 {0x3000, 0x303F, 0, 0, L"CJK Symbols and Punctuation" },
 {0x3040, 0x309F, 0, 0, L"Hiragana" },
 {0x30A0, 0x30FF, 0, 0, L"Katakana" },
 {0x3100, 0x312F, 0, 0, L"Bopomofo" },
 {0x3130, 0x318F, 0, 0, L"Hangul Compatibility Jamo" },
 {0x3190, 0x319F, 0, 0, L"Kanbun" },
 {0x31A0, 0x31BF, 0, 0, L"Bopomofo Extended" },
 {0x31C0, 0x31FF, 0, 0, L"Reserve" },
 {0x3200, 0x32FF, 0, 0, L"Enclosed CJK Letters and Months" },
 {0x3300, 0x33FF, 0, 0, L"CJK Compatibility" },
 {0x3400, 0x4DB5, 0, 0, L"CJK Unified Ideographs Extension A" },
 {0x4DB6, 0x4DFF, 0, 0, L"Reserve" },
 {0x4E00, 0x9FFF, 0, 0, L"CJK Unified Ideographs" },
 {0xA000, 0xA48F, 0, 0, L"Yi Syllables" },
 {0xA490, 0xA4CF, 0, 0, L"Yi Radicals" },
 {0xA4D0, 0xABFF, 0, 0, L"Reserve" },
 {0xAC00, 0xD7A3, 0, 0, L"Hangul Syllables" },
 {0xD7A4, 0xD7FF, 0, 0, L"Reserve" },
 {0xD800, 0xDB7F, 0, 0, L"High Surrogates" },
 {0xDB80, 0xDBFF, 0, 0, L"High Private Use Surrogates" },
 {0xDC00, 0xDFFF, 0, 0, L"Low Surrogates" },
 {0xE000, 0xF8FF, 0, 0, L"Private Use Area" },
 {0xF900, 0xFAFF, 0, 0, L"CJK Compatibility Ideographs" },
 {0xFB00, 0xFB4F, 0, 0, L"Alphabetic Presentation Forms" },
 {0xFB50, 0xFDFF, 0, 0, L"Arabic Presentation Forms-A" },
 {0xFE00, 0xFE1F, 0, 0, L"Reserve" },
 {0xFE20, 0xFE2F, 0, 0, L"Combining Half Marks" },
 {0xFE30, 0xFE4F, 0, 0, L"CJK Compatibility Forms" },
 {0xFE50, 0xFE6F, 0, 0, L"Small Form Variants" },
 {0xFE70, 0xFEFE, 0, 0, L"Arabic Presentation Forms-B" },
 {0xFEFF, 0xFEFF, 0, 0, L"Zero Width No-break Space" },
 {0xFF00, 0xFFEF, 0, 0, L"Halfwidth and Fullwidth Forms" },
 {0xFFF0, 0xFFFF, 0, 0, L"Specials" },
};


// Forward declarations of functions included in this code module:
void  RegisterClasses (HINSTANCE hInst);
BOOL  InitInstance (HINSTANCE hInst, int);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);


//-----------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInst,
                      _In_opt_ HINSTANCE hPrevInst,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInst);
  UNREFERENCED_PARAMETER(lpCmdLine);

  MSG      Msg;
  HACCEL   hAccelTable;
  INT_PTR  RetVal=0;
  WCHAR    szBuf[260], szPath[260], szFileName[260];

  // Get module version (for messages)
  HRSRC   hRsrc = NULL;
  HGLOBAL hGlb  = NULL;
  BYTE*   pData = NULL;
  VS_FIXEDFILEINFO* pInfo;
  short MS1, MS2, LS1, LS2;
  WCHAR szVer[16];
  wcscpy( szVer, L"?" );
  hRsrc = ::FindResource( hInst, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
  if (hRsrc){
    hGlb = ::LoadResource( hInst, hRsrc );
    if (hGlb){
      pData = (BYTE*)::LockResource( hGlb );
      pInfo = (VS_FIXEDFILEINFO*)(pData+40);
      // version
      MS1 = (short)((pInfo->dwFileVersionMS&0xFFFF0000)>>16);
      MS2 = (short)((pInfo->dwFileVersionMS&0x0000FFFF));
      LS1 = (short)((pInfo->dwFileVersionLS&0xFFFF0000)>>16);
      LS2 = (short)((pInfo->dwFileVersionLS&0x0000FFFF));
      swprintf( szVer, L"%d.%d.%d.%d", MS1, MS2, LS1, LS2 );
      ::FreeResource( hGlb );
    }
  }

  ::GetModuleFileName( hInst, szBuf, 255 );
  wcscpy( szPath, okGetFileDir(szBuf, false) );   // without backslash
  wcscpy( szFileName, okGetFileName(szBuf, false) );  // without extension
  if (g_App.Init( hInst, szVer, szPath, szFileName )){
    if (InitInstance( hInst, nCmdShow )){
      hAccelTable = ::LoadAccelerators( hInst, MAKEINTRESOURCE(IDC_FONTGLYPH) );
      // Main message loop:
      BOOL bRet;

      while( (bRet = ::GetMessage( &Msg, NULL, 0, 0 )) != 0){ 
        if (bRet == -1){
          // handle the error and possibly exit
        }else{
          if (!::TranslateAccelerator( Msg.hwnd, hAccelTable, &Msg )){
            ::TranslateMessage( &Msg );
            ::DispatchMessage( &Msg );
          }
        }
      }
      RetVal = Msg.wParam;
    }
  }
  g_App.End();
  return (int)RetVal;
}

//-----------------------------------------------
void RegisterClasses (HINSTANCE hInst)
{
  WNDCLASSEXW wcex;

  wcex.cbSize         = sizeof(WNDCLASSEXW); 
  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = (WNDPROC)WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInst;
  wcex.hIcon          = ::LoadIcon( hInst, (LPCWSTR)IDI_FONTGLYPH );
  wcex.hCursor        = ::LoadCursor( NULL, IDC_ARROW );
  wcex.hbrBackground  = NULL; //(HBRUSH)(COLOR_BTNFACE+1); //WINDOW+1);
  wcex.lpszMenuName   = (LPCWSTR)IDC_FONTGLYPH;
  wcex.lpszClassName  = szWindowClass;
  wcex.hIconSm        = ::LoadIcon(wcex.hInstance, (LPCWSTR)IDI_SMALL);

  ::RegisterClassEx(&wcex);
}


//-----------------------------------------------
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//-----------------------------------------------
BOOL InitInstance (HINSTANCE hInst, int nCmdShow)
{
  HWND hWnd;

  RegisterClasses( hInst );

  COkCtrlDiv::m_Color    = ::GetSysColor( COLOR_BTNFACE );
  COkCtrlDiv::m_Size     = 3;   
  COkCtrlDiv::m_hCurHorz = ::LoadCursor( hInst, MAKEINTRESOURCE(CUR_DIVHORZ) );
  COkCtrlDiv::m_hCurVert = ::LoadCursor( hInst, MAKEINTRESOURCE(CUR_DIVVERT) );

  hWnd = ::CreateWindow( szWindowClass, APP_TITLE, WS_OVERLAPPEDWINDOW, // | WS_CLIPCHILDREN ,
                          CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);
  if (!hWnd){
    return FALSE;
  }
  if (g_App.Create( hWnd, &nCmdShow ) == FALSE){
    return FALSE;
  }
  ::ShowWindow( hWnd, nCmdShow );
  ::UpdateWindow( hWnd );
  return TRUE;
}


//-----------------------------------------------
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//-----------------------------------------------
LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int   wmId, wmEvent, prm;
  RECT* pRect;
  MEASUREITEMSTRUCT* pMis;

  switch( message ){
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 
      // Parse the menu selections:
      switch( wmId ){
        case ID_FILE_FINDSYMBOL:
          g_App.FindSymbol();
          break;
        case ID_FILE_EXPORTSYMBOL:
          g_App.Export();
          break;

        case ID_TOOLS_EXPORT:
          g_App.Export();
          break;

        case IDM_ABOUT:
          g_App.Dg_About();
          break;

        case IDM_EXIT:
          ::SendMessage( hWnd, WM_CLOSE, 0,0 );
          break;

        case ID_LIST_CHARS:
          prm = HIWORD(wParam);
          switch( prm ){
            case LBN_SELCHANGE:
              g_App.OnSelectChar();
              break;
          }
          break;

        case ID_LIST_FONTS:
          prm = HIWORD(wParam);
          switch( prm ){
            case LBN_SELCHANGE:
              g_App.OnSelectFont();
              break;
          }
          break;

        case TTF_REGULAR:
        case TTF_BOLD:
        case TTF_ITALIC:
        case TTF_BOLDITALIC:
          g_App.OnSelectFont( wmId );
          break;

        default:
          return ::DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;

    case WM_KEYDOWN:
//      g_App.OnKeyDown( wParam, lParam );
      ::DefWindowProc(hWnd, message, wParam, lParam);
      break;

    case WM_SIZING:
      pRect = (RECT*)lParam;
      if ((pRect->right - pRect->left) < 500){
        pRect->right = pRect->left + 500;
      }
      if ((pRect->bottom - pRect->top) < 400){
        pRect->bottom = pRect->top + 400;
      }
      break;

    case WM_SIZE:
      g_App.Resize( (int)wParam, LOWORD(lParam), HIWORD(lParam), true );
      return DefWindowProc(hWnd, message, wParam, lParam);

    case WM_MEASUREITEM:
      if (wParam == ID_LIST_CHARS){
        pMis = (MEASUREITEMSTRUCT*)lParam;
        pMis->itemHeight = g_App.CellSize();
      }
      break;

    case WM_DRAWITEM:
      g_App.DrawItem( (int)wParam, *((LPDRAWITEMSTRUCT)lParam) );
      break;

    case WM_USER:
      if (wParam == MSG_DIV_MOVED){
        g_App.Update();
      }
      break;

    case WM_DESTROY:
      ::PostQuitMessage(0);
      break;
    default:
      return ::DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}



//-----------------------------------------------
CFontGlyph::CFontGlyph ()
{
  m_hInst = NULL;
  m_hwMain      = NULL;
  m_WndWidth    = 0;
  m_WndHeight   = 0;
  m_hListChars  = NULL;
  m_hListFonts  = NULL;
  m_nFonts      = 0;
  m_iFont       = 0;
  m_pGWnd = NULL; 
  memset( m_szFontName, 0, sizeof(m_szFontName) );
  memset( m_szFileName, 0, sizeof(m_szFileName) );
  memset( m_szVersion, 0, sizeof(m_szVersion) );
  memset( m_szPath, 0, sizeof(m_szPath) );
  m_nChars = 0;
  wcscpy( m_szFileName, L"File" );
  m_FontMode = TTF_REGULAR;
  memset( &m_LF, 0, sizeof(m_LF) );
  m_LF.lfHeight         = GLYPH_SIZE;
  m_LF.lfWidth          = 0;
  m_LF.lfEscapement     = 0;
  m_LF.lfOrientation    = 0;
  m_LF.lfWeight         = FW_NORMAL;
  m_LF.lfItalic         = FALSE;
  m_LF.lfUnderline      = FALSE;
  m_LF.lfStrikeOut      = FALSE;
  m_LF.lfCharSet        = DEFAULT_CHARSET;  
  m_LF.lfOutPrecision   = OUT_TT_ONLY_PRECIS;
  m_LF.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
  m_LF.lfQuality        = DRAFT_QUALITY;
  m_LF.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
  m_bFontLoaded = false;
  m_UChar = NULL; 
  m_CharCode = 0;
  m_iCurSubChar = 0;
  m_nSbarLines = 0;
  m_CellSize = 10;
  m_bNoZoom = true;

  memset( m_hPicChar, 0, sizeof(m_hPicChar) );   // symbols previews  
  m_W1 = 0;
  m_W2 = 0;
  m_H = 0;
  m_nPL = 0;
  m_nVers = 0;
  m_Buf    = NULL;
  m_PLSize = NULL;
  m_Ver    = NULL;
  m_FontHeight = GLYPH_SIZE;
  m_CursorX = 0.0;
  m_CursorY = 0.0;
  memset( m_szSnapInfo, 0, sizeof(m_szSnapInfo) );
  InitConfig ();
}

//-----------------------------------------------
CFontGlyph::~CFontGlyph ()
{
  if (m_pGWnd){
    delete m_pGWnd;
    m_pGWnd = NULL;
  }
  MemFree();
}


//-----------------------------------------------
bool CFontGlyph::Init (HINSTANCE hInst, LPCWSTR szVer, LPCWSTR szAppDir, LPCWSTR szExeName)
{
  COkConfig Cfg;

  m_hInst = hInst;
  wcsncpy( m_szVersion, szVer, 15 );
  wcsncpy( m_szPath, szAppDir, 255 );
  wcsncpy( m_szExeName, szExeName, 255 );
  MemAlloc();
  wcscpy( g_szLastDir, m_szPath );
  return true;
}

//-----------------------------------------------
void CFontGlyph::End ()
{
  ConfigSave();
  MemFree();
}

//-----------------------------------------------
void CFontGlyph::MemAlloc ()
{
  m_UChar  = (UCCHAR*)calloc( 65536, sizeof(UCCHAR) );
  m_Buf    = (BYTE*)calloc( 16384, 1 );
  m_PLSize = (WORD*)calloc( 200, sizeof(WORD) );
  m_Ver    = (GLYPHPOINT*)calloc( 8192, sizeof(GLYPHPOINT) );
}

//-----------------------------------------------
void CFontGlyph::MemFree ()
{
  if (m_UChar){
    free( m_UChar );
    m_UChar = NULL;
  }
  if (m_Buf){ 
    free( m_Buf );
    m_Buf = NULL;
  }
  if (m_PLSize){
    free( m_PLSize );
    m_PLSize = NULL;
  }
  if (m_Ver){
    free( m_Ver ); 
    m_Ver = NULL; 
  }
  delete m_pGWnd;
  m_pGWnd = NULL;
}

//-----------------------------------------------
void CFontGlyph::InitConfig ()
{
  m_SubsetLef   = 200;
  m_SubsetRig   = 660;
  m_SubsetBot   = 280;
  m_bMaximized = false;
  m_bShowCharBox = true;
  m_bShowCtrlPoints = false;
  m_bShowCtrlLines = false;
  m_bShowCtrlNums = false;
  memset( m_szFontName, 0, sizeof(m_szFontName) );
  m_FontMode = TTF_REGULAR;
  m_CharCode = 0;
}


//-----------------------------------------------
void CFontGlyph::ConfigLoad (LPCWSTR szFileName)
{
  int    ivar;
  WCHAR  szBuf[128];

  m_Cfg.SetFileName( szFileName );
  InitConfig();
  // load config values
  if (m_Cfg.Read()){
    m_bMaximized    = m_Cfg.GetValueBool( L"MAXIMIZED", m_bMaximized );
    m_SubsetLef  = m_Cfg.GetValueInt( L"USLEFT", m_SubsetLef );
    m_SubsetRig  = m_Cfg.GetValueInt( L"USRIGHT", m_SubsetRig );
    if ((m_SubsetLef+150) > m_SubsetRig){
      m_SubsetLef = 150;
      m_SubsetRig = 310;
    }
    m_SubsetBot  = m_Cfg.GetValueInt( L"USBOTTOM", m_SubsetBot );
    ivar = m_Cfg.GetValueInt( L"USCOLW1", m_Subsets.GetColumn(0)->Width() );
    m_Subsets.GetColumn(0)->SetWidth( ivar );
    ivar = m_Cfg.GetValueInt( L"USCOLW2", m_Subsets.GetColumn(1)->Width() );
    m_Subsets.GetColumn(1)->SetWidth( ivar );
    ivar = m_Cfg.GetValueInt( L"USCOLW3", m_Subsets.GetColumn(2)->Width() );
    m_Subsets.GetColumn(2)->SetWidth( ivar );
    ivar = m_Cfg.GetValueInt( L"USCOLW4", m_Subsets.GetColumn(3)->Width() );
    m_Subsets.GetColumn(3)->SetWidth( ivar );
    ivar = m_Cfg.GetValueInt( L"USCOLW5", m_Subsets.GetColumn(4)->Width() );
    m_Subsets.GetColumn(4)->SetWidth( ivar );
    ivar  = m_Cfg.GetValueInt( L"SHOWPOINTS", m_bShowCtrlPoints );
    m_bShowCtrlPoints = (ivar)? true : false;
    ivar  = m_Cfg.GetValueInt( L"SHOWNUMS", m_bShowCtrlNums );
    m_bShowCtrlNums = (ivar)? true : false;
    ivar  = m_Cfg.GetValueInt( L"SHOWLINES", m_bShowCtrlLines );
    m_bShowCtrlLines = (ivar)? true : false;
    ivar  = m_Cfg.GetValueInt( L"SHOWBOX", m_bShowCharBox );
    m_bShowCharBox = (ivar)? true : false;
    if (m_Cfg.GetValue( L"FONTNAME", szBuf, 125 )){
      szBuf[125] = 0;
      memset( m_szFontName, 0, sizeof(m_szFontName) );
      okUTF8toUnicode( szBuf, m_szFontName, 125 );
    }
    m_FontMode = m_Cfg.GetValueInt( L"FONTMODE", m_FontMode );
    m_CharCode = m_Cfg.GetValueInt( L"FONTCHAR", m_CharCode );
  }
}


//-----------------------------------------------
void CFontGlyph::ConfigSave ()
{
  char  szBuf[256];
  FILE* df = _wfopen( m_Cfg.FileName(), L"wt" );
  if (df){
    fprintf( df, "MAXIMIZED=%d\n" , m_bMaximized? 1 : 0 ); 
    fprintf( df, "USLEFT=%d\n"    , m_SubsetLef );
    fprintf( df, "USRIGHT=%d\n"   , m_SubsetRig );
    fprintf( df, "USBOTTOM=%d\n"  , m_SubsetBot );
    fprintf( df, "USCOLW1=%d\n"   , m_Subsets.GetColumn(0)->Width() );
    fprintf( df, "USCOLW2=%d\n"   , m_Subsets.GetColumn(1)->Width() );
    fprintf( df, "USCOLW3=%d\n"   , m_Subsets.GetColumn(2)->Width() );
    fprintf( df, "USCOLW4=%d\n"   , m_Subsets.GetColumn(3)->Width() );
    fprintf( df, "USCOLW5=%d\n"   , m_Subsets.GetColumn(4)->Width() );
    fprintf( df, "SHOWPOINTS=%d\n", m_bShowCtrlPoints );
    fprintf( df, "SHOWNUMS=%d\n"  , m_bShowCtrlNums );
    fprintf( df, "SHOWLINES=%d\n" , m_bShowCtrlLines );
    fprintf( df, "SHOWBOX=%d\n"   , m_bShowCharBox );
    okUnicodeToUTF8( m_szFontName, szBuf, 250 );
    fprintf( df, "FONTNAME=%s\n"  , szBuf );
    fprintf( df, "FONTMODE=%d\n"  , m_FontMode );
    fprintf( df, "FONTCHAR=%d\n"  , m_CharCode );
    fclose( df );
  }
}


//-----------------------------------------------
static void CALLBACK OnGridSelRow (COkCtrlGrid* pGrid, COkTRow* pRow)
{
  if (pRow){
    g_App.OnSelectSubset( (CSubsetRow*)pRow );
  }
}
// Graphics window event procedure
//-----------------------------------------------
int CALLBACK GWndEvent (int EventType, HANDLE Prm1)
{
  CFontGlyph* pApp = (CFontGlyph*)Prm1;
  return pApp->OnGWndEvent( EventType );
}

//-----------------------------------------------
bool CFontGlyph::Create (HWND hwParent, int* pCmdShow)
{
  bool  bOK;
  DWORD Style=0, ExStyle=0;
  WCHAR szBuf[260];
  int   cc;

  m_hwMain = hwParent;
  InitCommonControls();

  // Unicode subsets grid
  m_Subsets.ClearColumns();
  m_Subsets.AddColumn( 0, STR_USUBSET, 170, OK_GRID_VAR_TEXT );
  m_Subsets.AddColumn( 1, STR_UNCHARS, 70, OK_GRID_VAR_TEXT );
  m_Subsets.AddColumn( 2, STR_USTART, 68, OK_GRID_VAR_TEXT );
  m_Subsets.AddColumn( 3, STR_UEND, 50, OK_GRID_VAR_TEXT, false );
  m_Subsets.AddColumn( 4, STR_UNCODES, 70, OK_GRID_VAR_TEXT );
  m_Subsets.OnEvent_SelRow( OnGridSelRow );
  bOK = m_Subsets.Create( 0, 0, 300, 200, m_hwMain, ID_LIST_SUBSETS, m_hInst, true );
  if (bOK == false){
    return false;
  }

  // Load the application configuration parameters
  swprintf( szBuf, L"%s\\%s.cfg", m_szPath, m_szExeName );
  ConfigLoad( szBuf );

  // Fonts list
  ExStyle = WS_EX_CLIENTEDGE;
  Style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_NODATA | LBS_NOTIFY;
  m_hListFonts = ::CreateWindowEx( ExStyle, L"LISTBOX", L"Fonts", Style, 
	                                 0,0, 300,200, m_hwMain, (HMENU)ID_LIST_FONTS, m_hInst, NULL);
  if (m_hListFonts == NULL){
    return false;
  }

  // Chars list
  ExStyle = WS_EX_CLIENTEDGE;
  Style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_NODATA | LBS_NOTIFY;
  m_hListChars = ::CreateWindowEx( ExStyle, L"LISTBOX", L"Chars", Style, 
	                                  0,0, 300,200, m_hwMain, (HMENU)ID_LIST_CHARS, m_hInst, NULL);
  if (m_hListChars == NULL){
    return false;
  }

  // "Info" window
  m_Info.Create( m_hInst, m_hwMain );
  m_Info.ShowCharBox( m_bShowCharBox );
  m_Info.ShowCtrlPoints( m_bShowCtrlPoints );
  m_Info.ShowCtrlNums( m_bShowCtrlNums );
  m_Info.ShowCtrlLines( m_bShowCtrlLines );

  // vert. divider between Files list and "Unicode Subsets" list
  m_DivVer1.CreateVer( m_hInst, m_hwMain, &m_SubsetLef, true );

  // vert. divider between Subsets list and CAD window
  m_DivVer2.CreateVer( m_hInst, m_hwMain, &m_SubsetRig, true );

  // horz. divider between Subsets list and Chars list
  m_DivHorz.CreateHor( m_hInst, m_hwMain, &m_SubsetBot, true );
  // Graphics window
  m_pGWnd = new LcWindow( m_hInst, m_hwMain, WS_BORDER );
  if (m_pGWnd){
    m_pGWnd->SetEventProc( GWndEvent, (HANDLE)this );
    m_pGWnd->m_ColorBack = RGB(245,255,250);
    m_pGWnd->m_ColorFore = RGB(255,200,100);
    m_pGWnd->m_ColorCursor = ::GetSysColor(COLOR_BTNTEXT);
    if (m_bShowCtrlPoints){
      m_pGWnd->m_bShowPickbox = true;
    }else{
      m_pGWnd->m_bShowPickbox = false;
    }
  }
  EnumTTFonts();
  SelectFontName( m_szFontName );
  cc = m_CharCode;
  FileOpenTTF( m_FontName[m_iFont].Name(), m_FontName[m_iFont].FileName(), m_FontMode );
  if (cc){
    m_pGWnd->m_IntVal = 0;
    if (SelectChar( cc )){
      ZoomCharCell( false );
    }
  }

  if (m_bMaximized){
    *pCmdShow = SW_SHOWMAXIMIZED;
  }
  return true;
}


//-----------------------------------------------
void CFontGlyph::Resize (int SizeType, int Wmain, int Hmain, bool bFromEvent)
{
  int  x, y, w, h;
  RECT rc;
  int  DIV_SIZE = COkCtrlDiv::m_Size;

  if (SizeType == SIZE_MINIMIZED){
    return;
  }
  if (Wmain==0 && Hmain==0){
    return;
  }else{
    m_WndWidth  = Wmain;
    m_WndHeight = Hmain;
  }

  Update( m_WndWidth, m_WndHeight );

  // "List of Fonts" position
  x = 0;
  y = SUBSETS_Y;
  w = m_SubsetLef;
  h = m_WndHeight;
  ::MoveWindow( m_hListFonts, x, y, w, h, false );

  // "List of Unicode subsets" position
  x = m_SubsetLef + DIV_SIZE;
  y = SUBSETS_Y;
  w = m_SubsetRig - x;
  h = m_SubsetBot - y;
  m_Subsets.MoveWindow( x, y, w, h );

  // "List of Chars" position
  x = m_SubsetLef + DIV_SIZE;
  y = m_SubsetBot + DIV_SIZE;
  w = m_SubsetRig - x;
  h = m_WndHeight - BOTTOM_GAP - y;
  ::MoveWindow( m_hListChars, x, y, w, h, false );
  ::GetClientRect( m_hListChars, &rc );
  m_CellSize = rc.right / NUM_CHARSROW;
  ::SendMessage( m_hListChars, LB_SETITEMHEIGHT, 0, m_CellSize );

  // "Symbil info" window position
  x = m_SubsetRig + DIV_SIZE;
  y = SUBSETS_Y;
  w = m_WndWidth - x;
  h = CAD_Y0 - y; // - DIV_SIZE;
  m_Info.Resize( x, y, w, h );


  // Graphics window position
  x = m_SubsetRig + DIV_SIZE;
  y = CAD_Y0;
  w = m_WndWidth - x;
  h = m_WndHeight - BOTTOM_GAP - y;
  m_pGWnd->Resize(x, y, w, h);

  // vert. divider between Files list and Subsets list
  x = m_SubsetLef;
  y = SUBSETS_Y;
  h = m_WndHeight - SUBSETS_Y - y + 1;
  m_DivVer1.Resize( x, y, h );

  // vert. divider between Subsets list and CAD window
  x = m_SubsetRig;
  y = SUBSETS_Y;
  h = m_WndHeight - SUBSETS_Y - y + 1;
  m_DivVer2.Resize( x, y, h );

  // horz. divider between Subsets list and Chars list
  x = m_SubsetLef;
  y = m_SubsetBot;
  w = m_SubsetRig - x + 1;
  m_DivHorz.Resize( x, y, w );

  if (bFromEvent){
    // save the state in configuration
    m_bMaximized = (SizeType==SIZE_MAXIMIZED)? true : false;
  }
}


//-----------------------------------------------
void CFontGlyph::Update (int WndW, int WndH)
{
  RECT rect;
  int  W, H, x1, x2;
  bool bResize = false;

  if (WndW > 0 && WndH > 0){
    W = WndW;
    H = WndH;
  }else{
    ::GetClientRect( m_hwMain, &rect );
    W = rect.right;
    H = rect.bottom;
    bResize = true;
  }

  if (m_SubsetLef < 150){
    m_SubsetLef = 150;
  }else{
    if (m_SubsetLef > W/2){
      m_SubsetLef = W/2;
    }
  }
  x1 = m_SubsetLef + 150;
  x2 = W*2/3;
  if (m_SubsetRig < x1){
    m_SubsetRig = x1;
  }else{
    if (m_SubsetRig > x2){
      m_SubsetRig = x2;
    }
  }
  if (m_SubsetBot < 250){
    m_SubsetBot = 250;
  }else{
    if (m_SubsetBot > H*2/3){
      m_SubsetBot = H*2/3;
    }
  }
  if (bResize){
    Resize( -1, W, H, false );
    ::InvalidateRect( m_hwMain, NULL, true );
  }
}


//-----------------------------------------------
void CFontGlyph::ToggleShowPoints ()
{
  m_bShowCtrlPoints = !m_bShowCtrlPoints;
  m_Info.ShowCtrlPoints( m_bShowCtrlPoints );
  m_pGWnd->m_bShowPickbox = m_bShowCtrlPoints;
  m_pGWnd->Invalidate();
  m_pGWnd->SetFocus();
}

//-----------------------------------------------
void CFontGlyph::ToggleShowNums ()
{
  m_bShowCtrlNums = !m_bShowCtrlNums;
  m_Info.ShowCtrlNums( m_bShowCtrlNums );
  m_pGWnd->Invalidate();
  m_pGWnd->SetFocus();
}

//-----------------------------------------------
void CFontGlyph::ToggleShowLines ()
{
  m_bShowCtrlLines = !m_bShowCtrlLines;
  m_Info.ShowCtrlLines( m_bShowCtrlLines );
  m_pGWnd->Invalidate();
  m_pGWnd->SetFocus();
}


//-----------------------------------------------
void CFontGlyph::ToggleShowBox ()
{
  m_bShowCharBox = !m_bShowCharBox;
  m_Info.ShowCharBox( m_bShowCharBox );
  m_pGWnd->Invalidate();
  m_pGWnd->SetFocus();
}
