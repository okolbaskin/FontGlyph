/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Dialog "About"
*********************************************************************/
#include "framework.h"
#include "FontGlyph.h"
#include <shellapi.h>

extern CFontGlyph g_App;

#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))

static LRESULT CALLBACK ProcAbout (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT SetLinkColor (HDC hDC, HWND hwCtrl);
static void OnMouseMove (HWND hDlg, int X, int Y);
static void OnClick (HWND hDlg, int X, int Y);

static HWND _hwLink;
static bool _bOnLink;

//-----------------------------------------------
void CFontGlyph::Dg_About ()
{
  _hwLink = 0;
  _bOnLink = false;
  ::DialogBox( m_hInst, MAKEINTRESOURCEW(IDD_ABOUTBOX), m_hwMain, (DLGPROC)ProcAbout );
  m_pGWnd->SetFocus();
}


// Message handler for about box.
//-----------------------------------------------
static LRESULT CALLBACK ProcAbout (HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  WCHAR szBuf[128], szAppName[32];
  RECT  rect;
  POINT p;
  HICON hIconSmall;

  switch( Msg ){
    case WM_INITDIALOG:
      ::GetWindowRect( hDlg, &rect );
      p.x = (::GetSystemMetrics(SM_CXSCREEN)-(rect.right-rect.left))>>1;
      p.y = (::GetSystemMetrics(SM_CYSCREEN)-(rect.bottom-rect.top))>>1;
      ::SetWindowPos( hDlg, HWND_TOP, p.x, p.y, 0,0, SWP_NOSIZE );
      hIconSmall = (HICON)::LoadImage( g_App.Instance(), MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
      ::SendMessage( hDlg, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall );
      // set strings
      ::SetWindowText( hDlg, APP_TITLE );
#ifdef _WIN64
      swprintf( szAppName, L"%s (x64)", APP_TITLE );
#else
      swprintf( szAppName, L"%s", APP_TITLE );
#endif
      swprintf( szBuf, L"%s  ver. %s", szAppName, g_App.Version() );
      ::SetDlgItemText( hDlg, DT_VEC_VERSION, szBuf );
      swprintf( szBuf, L"Copyright (c) %s", APP_COPYRIGHT );
      ::SetDlgItemText( hDlg, DT_VEC_COPYRIGHT, szBuf );
      ::SetDlgItemText( hDlg, DT_VEC_RIGHTS, L"All Rights Reserved" );
      swprintf( szBuf, L"Website:  %s", APP_WEBSITE );
      ::SetDlgItemText( hDlg, DT_VEC_WEBSITE, szBuf );
      _hwLink = ::GetDlgItem( hDlg, DT_VEC_WEBSITE );
      return TRUE;

    case WM_CTLCOLORSTATIC:
      return SetLinkColor( (HDC)wParam, (HWND)lParam );

    case WM_MOUSEMOVE:
      OnMouseMove( hDlg, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
      return true;

    case WM_LBUTTONDOWN:
      OnClick( hDlg, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
      return true;

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
        ::EndDialog( hDlg, LOWORD(wParam) );
        return TRUE;
      }
      break;
  }
  return FALSE;
}

//-----------------------------------------------
LRESULT SetLinkColor (HDC hDC, HWND hwCtrl)
{
  if (hwCtrl == _hwLink){
    ::SetTextColor( hDC, ::GetSysColor(COLOR_HOTLIGHT) );
    ::SetBkMode( hDC, TRANSPARENT );
    return (LRESULT)::GetSysColorBrush(COLOR_BTNFACE);
  }
  return 0;
}

//-----------------------------------------------
void OnMouseMove (HWND hDlg, int X, int Y)
{
  HCURSOR hCurFinger;
  RECT  Rect;
  POINT Pmin, Pmax;
  int   dx;

  _bOnLink = false;
  if (_hwLink){
    ::GetWindowRect( _hwLink, &Rect );
    dx = (Rect.right - Rect.left) / 8;
    Pmin.x = Rect.left + dx;
    Pmin.y = Rect.top;
    Pmax.x = Rect.right - dx;
    Pmax.y = Rect.bottom;
    ::ScreenToClient( hDlg, &Pmin );
    ::ScreenToClient( hDlg, &Pmax );
    if (Pmin.x<X && X<Pmax.x && Pmin.y<Y && Y<Pmax.y){
      hCurFinger = ::LoadCursor( NULL, IDC_HAND );
      ::SetCursor( hCurFinger );
      _bOnLink = true;
    }
  }
}

//-----------------------------------------------
void OnClick (HWND hDlg, int X, int Y)
{
  if (_bOnLink){
    ::ShellExecute( hDlg, L"open", APP_WEBSITELINK, 0, 0, SW_SHOWNORMAL );
  }
}






