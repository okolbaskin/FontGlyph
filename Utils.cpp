/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Simple functions
********************************************************************/
#include "framework.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commdlg.h>


//-----------------------------------------------
// Delete last blank in the specified text string
//-----------------------------------------------
void okDelLastBlanks (WCHAR* szText)
{
  INT_PTR i = wcslen(szText);
  if (i > 0){
    --i;
    for (; i>=0; --i){
      if (szText[i] == L' '){
        szText[i] = 0;
      }else{
        break;
      }
    }
  }
}


typedef BOOL (WINAPI *tfTransparentBlt)(HDC hdcDest, int Xdest, int Ydest, int Wdest, int Hdest, HDC hdcSrc, int Xsrc, int Ysrc, int Wsrc, int Hsrc, COLORREF TrColor);

//-----------------------------------------------
BOOL okTransparentBlt (HDC hdcDest, int Xdest, int Ydest, int Wdest, int Hdest,
                       HDC hdcSrc, int Xsrc, int Ysrc, int Wsrc, int Hsrc, COLORREF TrColor)
{
  BOOL ret = FALSE;
  HINSTANCE hInst = ::LoadLibrary( L"msimg32.dll" );

  if (hInst){
    tfTransparentBlt pfTransparentBlt;
    pfTransparentBlt = (tfTransparentBlt)::GetProcAddress( hInst, "TransparentBlt" );
    if (pfTransparentBlt){
      ret = (*pfTransparentBlt)( hdcDest, Xdest, Ydest, Wdest, Hdest, hdcSrc, Xsrc, Ysrc, Wsrc, Hsrc, TrColor);
    }
    ::FreeLibrary( hInst );
  }
  return ret;
}

//-----------------------------------------------
// Draw a resource bitmap on DC
// TrColor - transparent color of the bitmap
//-----------------------------------------------
void okDrawBitmap (HINSTANCE hInst, HDC hDC, int Xdest, int Ydest, int IdBmp, int BmpW, int BmpH, COLORREF TrColor)
{
  HBITMAP  hbm, oldbm;
  HDC      hmdc;
  BOOL     ret = FALSE;

  if (IdBmp){
    hbm = ::LoadBitmap( hInst, MAKEINTRESOURCE(IdBmp) );
    hmdc = ::CreateCompatibleDC( hDC );
    oldbm = (HBITMAP)::SelectObject( hmdc, hbm );
    if (TrColor != 0xFFFFFFFF){
      ret = okTransparentBlt( hDC, Xdest, Ydest, BmpW, BmpH, hmdc, 0,0, BmpW, BmpH, TrColor );
    }
    if (ret == FALSE){
      ::BitBlt( hDC, Xdest, Ydest, BmpW, BmpH, hmdc, 0, 0, SRCCOPY ); 
    }
    ::SelectObject( hmdc, oldbm );
    ::DeleteDC( hmdc );
    ::DeleteObject( hbm );
  }
}



//-----------------------------------------------
// Call dialog "Save File" and 
// returns file name of selected file
//-----------------------------------------------

UINT_PTR CALLBACK SaveFileHook (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  INT_PTR   idCtrl;
  NMHDR*    pnmh;
  RECT  Rect;
  int   DlgW, DlgH, x0, y0;
  HWND  hwParent = ::GetParent( hDlg );

  switch (uMsg) {
    case WM_INITDIALOG:
      ::GetWindowRect( hwParent, &Rect );
      DlgW = Rect.right - Rect.left + 1;
      DlgH = Rect.bottom - Rect.top + 1;
      x0 = (::GetSystemMetrics(SM_CXSCREEN)-DlgW)>>1;
      y0 = (::GetSystemMetrics(SM_CYSCREEN)-DlgH)>>1;
      ::SetWindowPos( hwParent, HWND_TOP, x0, y0, DlgW, DlgH, SWP_SHOWWINDOW ); //SWP_NOMOVE );
      return 0;

    case WM_NOTIFY:
      idCtrl = (int) wParam; 
      pnmh = (LPNMHDR) lParam; 
      switch( pnmh->code ){
        case CDN_INITDONE:
          break;
        case CDN_SELCHANGE:
          break;
      }
      return 0;
  }
  return 0;
}

//-----------------------------------------------
// Get file directory from full pathname
// The output directory includes leading drive symbol
// and trailing backslash (\)
// If bIncludeSlash is FALSE then trailing backslash will not be included
//-----------------------------------------------
LPCWSTR _GetFileDir (LPCWSTR szFileName, bool bIncludeBackslash)
{
  static WCHAR szBuf[256];
  const WCHAR*  ch = L"\\//";
  size_t  i, len;

  wcscpy( szBuf, szFileName );
  len = wcslen( szBuf );
  for (i=len-1; i>0; --i){
    if (szBuf[i]==ch[0] || szBuf[i]==ch[1]){
      if (bIncludeBackslash){
        szBuf[i+1] = 0;
      }else{
        szBuf[i] = 0;
      }
      return szBuf;
    }
  }
  szBuf[0] = 0;
  return szBuf;
}


//-----------------------------------------------
bool okGetSaveFileName (HWND hParent, LPCWSTR szTitle, LPCWSTR szFilter, LPCWSTR szDefExt, 
                        WCHAR* szDir, WCHAR* szFileName)
{
  WCHAR fibuf[2048]; 
  OPENFILENAME data;
  DWORD ErrCode;

  memset( fibuf, 0, sizeof(fibuf) );
  okMakeFilterPair( szFilter, 2040, fibuf);

  memset( &data, 0, sizeof(data) );
  data.lStructSize       = sizeof(data);
  data.hwndOwner         = hParent;
  data.hInstance         = NULL;
  data.lpstrFilter       = fibuf;  // filter pairs buffer
  data.lpstrCustomFilter = NULL;
  data.nMaxCustFilter    = 0;
  data.nFilterIndex      = 0;
  data.lpstrFile         = szFileName;
  data.nMaxFile          = 256;
  data.lpstrFileTitle    = NULL;
  data.nMaxFileTitle     = 0;
  data.lpstrInitialDir   = szDir;
  if (szTitle){
    data.lpstrTitle      = (szTitle[0])? szTitle : NULL;
  }
  data.Flags             = OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_HIDEREADONLY |
                           OFN_EXTENSIONDIFFERENT | OFN_OVERWRITEPROMPT; // | OFN_ENABLESIZING;
  data.Flags            |= (OFN_ENABLEHOOK | OFN_EXPLORER);
  data.lpfnHook          = SaveFileHook;

  // default extention
  data.lpstrDefExt = szDefExt;

  BOOL ret = ::GetSaveFileName( &data );
  if (ret==TRUE){
    if (szDir){
      wcscpy( szDir, _GetFileDir( data.lpstrFile, true ) );
    }
  }else{
    ErrCode = ::CommDlgExtendedError();
  }
  return (ret==TRUE)?true:false;
}

//-----------------------------------------------
// Make filter pairs for OPENFILENAME structure
//-----------------------------------------------
void okMakeFilterPair (LPCWSTR filter, int filterSize, WCHAR* fibuf)
{
  INT_PTR code, i, j;
  WCHAR szExt[256];
  bool bExt = false;

  j = 0;
  memset( szExt, 0, sizeof(szExt) );
  for (i=0; i<filterSize; ++i){
    code = filter[i];
    if (code == 0){
      fibuf[i] = 0;
      ++i;
      fibuf[i] = 0;
      break;
    }
    if (code == L'|'){
      fibuf[i] = 0;
    }else{
      fibuf[i] = (wchar_t)code;
    }
  }
}


/********************************************************************
* Convert numerical value to string
*   Val - input value
*   szBuf - output string
*   MaxDec - max number of decimal digits, max - 8
*   bTrimZero - if true then last zeros will be deleted
*********************************************************************/
void okNumToStr (double Val, WCHAR* szBuf, UINT_PTR MaxDec, bool bTrimZero, bool bRemainPoint, UINT_PTR Separator, UINT_PTR TypeChar)
{
  WCHAR    szFmt[16];
  INT_PTR  i, n;

  if (MaxDec==0){
    if (Val>=0){
      i = (INT_PTR)(Val + 0.5);
    }else{
      i = (INT_PTR)(Val - 0.5);
    }
#if _WIN64
    swprintf(szBuf, L"%I64d", i);
#else
    swprintf(szBuf, L"%d", i);
#endif
  }else{
    if (MaxDec > 8){
      MaxDec = 8;
    }
#if _WIN64
    swprintf( szFmt, L"%%.%I64d%c", MaxDec, (int)TypeChar );
#else
    swprintf(szFmt, L"%%.%d%c", MaxDec, TypeChar);
#endif
    swprintf( szBuf, szFmt, Val );
    if (bTrimZero){
      n = wcslen( szBuf );
      for (i=n-1; i>0; --i){
        if (szBuf[i] == L'0'){
          szBuf[i] = 0;
        }else{
          if (szBuf[i] == L'.'){
            if (bRemainPoint){
              // remain point and 1 digit
              szBuf[i+1] = L'0';
              szBuf[i+2] = 0;
            }else{
              szBuf[i] = 0;
            }
          }
          break;
        }
      }
    }else{
      
    }
    // replace separator
    i = 0;
    while( szBuf[i] ){
      if (szBuf[i] == L'.' || szBuf[i] == L','){
        szBuf[i] = (WCHAR)Separator;
      }
      ++i;
    }
  }
}


//-----------------------------------------------
// Get file directory from full pathname
// The output directory includes leading drive symbol
// and trailing backslash (\)
// If bIncludeSlash is FALSE then trailing backslash will not be included
//-----------------------------------------------
LPCWSTR okGetFileDir (LPCWSTR szFileName, bool bIncludeBackslash)
{
  static WCHAR szBuf[260];
  const WCHAR*  ch = L"\\//";
  INT_PTR i, len;

  wcsncpy( szBuf, szFileName, 258 );
  szBuf[258] = 0;
  len = wcslen( szBuf );
  if (len > 1){
    for (i=len-1; i>=0; --i){
      if (szBuf[i]==ch[0] || szBuf[i]==ch[1]){
        if (bIncludeBackslash){
          szBuf[i+1] = 0;
        }else{
          szBuf[i] = 0;
        }
        return szBuf;
      }
    }
  }
  szBuf[0] = 0;
  return szBuf;
}


//-----------------------------------------------
// Get file name with/without extention from full pathname
//-----------------------------------------------
LPCWSTR okGetFileName (LPCWSTR szFileName, bool bWithExtention)
{
  static WCHAR szBuf[300];
  const WCHAR*   ch = L"/\\.";
  WCHAR*   pDot = NULL;
  INT_PTR  i, j, len;

  if (wcschr( szFileName, ch[0] )==NULL){
    if (wcschr( szFileName, ch[1] )==NULL){
      // divider not found 
      wcscpy( szBuf, szFileName );
      if (bWithExtention == false){
        // delete extention
        j = 0;
        len = wcslen( szBuf );
        for (i=len-1; i>=0 && j<5; --i){
          if (szBuf[i] == ch[2]){
            szBuf[i] = 0;
            break;
          }
          ++j;
        }
      }
      return szBuf;
    }
  }
  len = wcslen( szFileName );
  for (i=len-1; i>=0; --i){
    if ((szFileName[i] == ch[0]) || (szFileName[i] == ch[1])){
      j=0;
      ++i;
      while( szFileName[i] != 0 ){
        if (szFileName[i]==ch[2]){
          pDot = szBuf + j;
        }
        szBuf[j] = szFileName[i];
        ++j;
        ++i;
      }
      szBuf[j] = 0;
      if (bWithExtention == false){
        // delete extention
        if (pDot){
          pDot[0] = 0;
        }
      }
      return szBuf;
    }
  }
  szBuf[0]=0;
  return szBuf;
}


//-----------------------------------------------
// Converts UTF-8 string into Unicode string
//-----------------------------------------------
UINT_PTR okUTF8toUnicode (const char* szInStr, WCHAR* szOutStr, UINT_PTR nMaxChars)
{
  UINT_PTR nOutChars = 0;
  UINT_PTR i = 0;
  UINT_PTR len = strlen( szInStr );
  BYTE     Code, Code2, Code3;
  WCHAR    UniCode;

  szOutStr[0] = 0;
  if (len >= nMaxChars){
    len = nMaxChars - 1;
  }
  for (; i<len; ++i){
    Code = szInStr[i];
    if (Code & 0x80){   // 10000000
      if ((Code & 0xE0) == 0xC0){  // 11100000  110 00000
        Code2 = szInStr[i+1];
        if ((Code2 & 0xC0) == 0x80){  // 11000000  10000000
          // 0080-07FF  Code=110xxxxx Code2=10xxxxxx    
          UniCode = (Code&0x1F)<<6;
          UniCode += Code2&0x3F;
          szOutStr[nOutChars] = UniCode;
          ++nOutChars;
          ++i;
        }
      }else{
        if ((Code & 0xF0) == 0xE0){
          Code2 = szInStr[i+1];
          if ((Code2 & 0xC0) == 0x80){
            Code3 = szInStr[i+2];
            if ((Code3 & 0xC0) == 0x80){
              // 0800-FFFF  1110xxxx 10xxxxxx 10xxxxxx
              UniCode = (Code&0x1F)<<12;
              UniCode += (Code2&0x3F)<<6;
              UniCode += Code3&0x3F;
              szOutStr[nOutChars] = UniCode;
              ++nOutChars;
              ++i;
              ++i;
            }
          }
        }
      }
    }else{
      // 0xxxxxxx -> 0000-007F
      szOutStr[nOutChars] = Code;;
      ++nOutChars;
    }
  }
  szOutStr[nOutChars] = 0;
  return nOutChars;
}

//-----------------------------------------------
UINT_PTR okUTF8toUnicode (const WCHAR* szInStr, WCHAR* szOutStr, UINT_PTR nMaxChars)
{
  UINT_PTR Ret = 0;
  UINT_PTR i;
  char* szStr = NULL;

  szOutStr[0] = 0;
  if (nMaxChars > 4){
    szStr = (char*)malloc( sizeof(char)*(nMaxChars+1) );
    if (szStr){
      for (i=0; i<nMaxChars; ++i){
        szStr[i] = (char)(szInStr[i]);
        if (szStr[i] == 0){
          break;
        }
      }
      Ret = okUTF8toUnicode( szStr, szOutStr, nMaxChars );
      free( szStr );
    }
  }
  return Ret;
}


//-----------------------------------------------
// Converts Unicode string into UTF-8 string
//-----------------------------------------------
UINT_PTR okUnicodeToUTF8 (LPCWSTR szInStr, char* szOutStr, UINT_PTR nMaxChars)
{
  UINT_PTR nMax;
  UINT_PTR nOutChars = 0;
  WCHAR    Code = 1;
  WCHAR    b;
  UINT_PTR i = 0;

  szOutStr[0] = 0;
  if (nMaxChars > 2){
    nMax = nMaxChars - 1;
    while( Code ){
      Code = szInStr[i];
      ++i;
      if (Code == 0){
mExit:
        szOutStr[nOutChars] = 0;
        break;
      }
      if (Code & 0xFF80){
        if (Code >= 0x0800){
          // range 0x0800 - 0xFFFF
          if ((nOutChars + 2) < nMax){
            b = 0x00E0 | ((Code & 0xF000)>>12);
            szOutStr[nOutChars] = (BYTE)b;
            ++nOutChars;
            b = 0x0080 | ((Code & 0x0FC0)>>6);
            szOutStr[nOutChars] = (BYTE)b;
            ++nOutChars;
            b = 0x0080 | (Code & 0x003F);
            szOutStr[nOutChars] = (BYTE)b;
            ++nOutChars;
          }else{
            goto mExit;
          }
        }else{
          // range 0x0080 - 0x07FF
          if ((nOutChars + 1) < nMax){
            b = 0x00C0 | ((Code & 0x07C0)>>6);
            szOutStr[nOutChars] = (BYTE)b;
            ++nOutChars;
            b = 0x0080 | (Code & 0x003F);
            szOutStr[nOutChars] = (BYTE)b;
            ++nOutChars;
          }else{
            goto mExit;
          }
        }
      }else{
        // range 0x0000 - 0x007F
        if (nOutChars < nMax){
          szOutStr[nOutChars] = Code & 0x007F;
          ++nOutChars;
        }else{
          // reaches limit of output string size
          goto mExit;
        }
      }
    }
  }
  return nOutChars;
}

// same, but output string has WCHAR type
//-----------------------------------------------
UINT_PTR okUnicodeToUTF8 (LPCWSTR szInStr, WCHAR* szOutStr, UINT_PTR nMaxChars)
{
  UINT_PTR Len = 0;
  UINT_PTR i;
  char     szBuf1[1024];
  char*    szBuf2 = NULL;
  char*    psz = szBuf1;

  szOutStr[0] = 0;
  if (nMaxChars < 1024){
    psz = szBuf1;
  }else{
    szBuf2 = (char*)malloc( sizeof(char)*(nMaxChars+2) );
    psz = szBuf2;
  }
  Len = okUnicodeToUTF8( szInStr, psz, nMaxChars );
  if (Len > 0){
    for (i=0; i<Len; ++i){
      szOutStr[i] = psz[i];
    }
    szOutStr[i] = 0;
  }
  if (szBuf2){
    free( szBuf2 );
  }
  return Len;
}


#define LENDLGBUF 32032   // buffer size for dialog variable

//-----------------------------------------------
// Write variable to dialog's edit/text window
//-----------------------------------------------
bool _cdecl okVarToDialog (HWND dgItem, INT_PTR Id, LPCWSTR szFmt, ...)
{
  bool res = false;
  wchar_t* szBuf = (wchar_t*)calloc( LENDLGBUF, sizeof(wchar_t) );
  if (szBuf){
    va_list  argptr;	 // pointer to list of arguments
    va_start( argptr, szFmt );
    if (vswprintf( szBuf, szFmt, argptr ) > 0){
      res = true;
    }
    ::SetDlgItemText( dgItem, (int)Id, szBuf );
    va_end( argptr );
    free( szBuf );
  }
  return res;
}

//-----------------------------------------------
// Read variable from dialog's edit window
//-----------------------------------------------
bool okDialogToVar (HWND hDlg, INT_PTR Id, INT_PTR ns, LPCWSTR szFmt, LPVOID pVar)
{
  bool     res = false;
  INT_PTR  n;
  wchar_t* szBuf = (wchar_t*)calloc( ns+8, sizeof(wchar_t) );
  if (szBuf){
    ::GetDlgItemText( hDlg, (int)Id, szBuf, (int)(ns+1) );
    if (szFmt[1] == L's'){
      wcscpy( (wchar_t*)pVar, szBuf );
      res = true;
    }else{
      n = swscanf( szBuf, szFmt, pVar );
      if (n > 0){
        res = true;
      }else{
        n = swscanf( L"0", szFmt, pVar );
        res = false;
      }
    }
    free( szBuf );
  }
  return res;
}
