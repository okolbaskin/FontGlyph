/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
// include file for standard system include files,
// or project specific include files
********************************************************************/
#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "targetver.h"

// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Application Header Files
#include "OkBaseTypes.h"
#include "ListObj.h"
#include "CtrlGrid.h"
#include "CtrlDiv.h"
#include "Geom.h"
#include "InfoWnd.h"
#include "GW.h"



#define APP_TITLE        L"FontGlyph"
#define APP_COPYRIGHT    L"2026, Oleg Kolbaskin"
#define APP_WEBSITE      L"www.kolbasoft.com"
#define APP_WEBSITELINK  L"https://www.kolbasoft.com"


/********************************************************************
* Simple functions
********************************************************************/

void okDelLastBlanks (WCHAR* szText); 
void okDrawBitmap (HINSTANCE hInst, HDC hDC, int Xdest, int Ydest, int IdBmp, int BmpW, int BmpH, COLORREF TrColor);
bool okGetSaveFileName (HWND hParent, LPCWSTR szTitle, LPCWSTR szFilter, LPCWSTR szDefExt, WCHAR* szDir, WCHAR* szFileName);
void okMakeFilterPair (LPCWSTR filter, int filterSize, WCHAR* fibuf);
void okDrawBitmap  (HINSTANCE hInst, HDC hDC, int Xdest, int Ydest, int IdBmp, int BmpW, int BmpH, COLORREF TrColor=0xFFFFFFFF);
void okNumToStr (double Val, WCHAR* szBuf, UINT_PTR MaxDec=0, bool bTrimZero=true, bool bRemainPoint=false, UINT_PTR Separator='.', UINT_PTR TypeChar='f');
LPCWSTR okGetFileDir (LPCWSTR szFileName, bool bIncludeBackslash=true);
LPCWSTR okGetFileName (LPCWSTR szFileName, bool bWithExtention=true);

//-----------------------------------------------
// Converts UTF-8 string into Unicode string
//-----------------------------------------------
UINT_PTR okUTF8toUnicode (const char* szInStr, WCHAR* szOutStr, UINT_PTR nMaxChars);
UINT_PTR okUTF8toUnicode (const WCHAR* szInStr, WCHAR* szOutStr, UINT_PTR nMaxChars);

//-----------------------------------------------
// Converts Unicode string into UTF-8 string
//-----------------------------------------------
UINT_PTR okUnicodeToUTF8 (LPCWSTR szInStr, char* szOutStr, UINT_PTR nMaxChars);
UINT_PTR okUnicodeToUTF8 (LPCWSTR szInStr, WCHAR* szOutStr, UINT_PTR nMaxChars);

// Write variable to dialog's edit/text window
bool _cdecl okVarToDialog (HWND hwDlg, INT_PTR Id, LPCWSTR szFmt, ...);

// Read variable from dialog's edit window
bool okDialogToVar (HWND hwDlg, INT_PTR Id, INT_PTR ns, LPCWSTR szFmt, LPVOID pVar);
