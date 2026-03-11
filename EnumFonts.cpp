/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Fonts array
*********************************************************************/
#include "framework.h"
#include "FontGlyph.h"
#include <shlobj_core.h>

static int _FontType = FONT_TYPE_TTF;  // used to sort font names

int GetNextNameValue (HKEY key, LPCWSTR subkey, WCHAR* szName, WCHAR* szData);
static int _cdecl CmpFontNames (const void* e1, const void* e2);


//-----------------------------------------------
bool CFontGlyph::EnumTTFonts ()
{
  WCHAR  szFontName[MAX_PATH];
  WCHAR  szFontFile[MAX_PATH];
  WCHAR  szSubKey[MAX_PATH];
  WCHAR  szDirFonts[MAX_PATH];
  WCHAR  szBuf[MAX_PATH+MAX_PATH];
  WCHAR* psz = NULL;
  int    Step;
  HKEY   Key;

  m_nFonts = 0;
  _FontType = FONT_TYPE_TTF;
  ::SendMessage( m_hListFonts, LB_RESETCONTENT, 0, 0 );
  // collect Windows TrueType fonts
  wcscpy( szSubKey, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts" );
  if (::SHGetSpecialFolderPath( NULL, szDirFonts, CSIDL_FONTS, false ) == false){
    szDirFonts[0] = 0;
  }
  for (Step=0; Step<2; ++Step){
    if (Step == 0){
      Key = HKEY_LOCAL_MACHINE;
    }else{
      Key = HKEY_CURRENT_USER;
    }
    wcscpy( szSubKey, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts" );
    while( GetNextNameValue( Key, szSubKey, szFontName, szBuf ) == ERROR_SUCCESS ){
      szSubKey[0] = 0;	// this will get next value, same key
      if (Step == 0){
        swprintf( szFontFile, L"%s\\%s", szDirFonts, szBuf ); 
      }else{
        wcscpy( szFontFile, szBuf );
      }
      psz = wcsstr( szFontName, L"(TrueType)" );
      if (psz){
        psz[0] = 0;
        ParseFontName( szFontName, szFontFile );
      }
    }
    GetNextNameValue( Key, NULL, NULL, NULL );	// close the registry key
  }
  qsort( m_FontName, m_nFonts, sizeof(CFontName), CmpFontNames );
  ::SendMessage( m_hListFonts, LB_SETCURSEL, 0, 0 );
  ClearFontData();
  return true;
}

//-----------------------------------------------
bool CFontGlyph::AddFontName (LPCWSTR szFontName, LPCWSTR szFontFile)
{
  int i=0;
  if (m_nFonts < MAX_FONTS){
    for (i=0; i<m_nFonts; ++i){
      if (wcscmp( m_FontName[i].Name(), szFontName ) == 0){
        return true;
      }
    }
    m_FontName[m_nFonts].Set( szFontName, szFontFile, _FontType ); 
    ++m_nFonts;
    ::SendMessage( m_hListFonts, LB_ADDSTRING, 0, (LPARAM)L"!" );
    return true;
  }
  return false;
}

//-----------------------------------------------
bool CFontGlyph::SelectFontName (LPCWSTR szFontName)
{
  int i;
  for (i=0; i<m_nFonts; ++i){
    if (wcscmp( m_FontName[i].Name(), szFontName ) == 0){
      m_iFont = i;
      ::SendMessage( m_hListFonts, LB_SETCURSEL, i, 0 );
      return true;
    }
  }
  m_iFont = 0;
  ::SendMessage( m_hListFonts, LB_SETCURSEL, m_iFont, 0 );
  wcscpy( m_szFontName, m_FontName[m_iFont].Name() );
  return false;
}



//-----------------------------------------------
void CFontGlyph::ParseFontName (WCHAR* szFontName, LPCWSTR szFontFile)
{
  WCHAR* pszName = NULL;
  WCHAR* psz;
  WCHAR  szFontName2[256];

  psz = wcsstr( szFontName, L" & " );
  if (psz){
    okDelLastBlanks( szFontName );
    pszName = szFontName;
m1:
    psz[0] = 0;
    NormalizeFontName( pszName, szFontName2, 256 );
    AddFontName( szFontName2, szFontFile );
    pszName = psz + 3;
    psz = wcsstr( pszName, L" & " );
    if (psz){
      goto m1;
    }else{
      NormalizeFontName( pszName, szFontName2, 256 );
      AddFontName( szFontName2, szFontFile );
    }
  }else{
    NormalizeFontName( szFontName, szFontName2, 256 );
    AddFontName( szFontName2, szFontFile );
  }
}

//-----------------------------------------------
void CFontGlyph::NormalizeFontName (LPCWSTR szName, WCHAR* szOutName, INT_PTR OutSize) const
{
  INT_PTR i, j, len;
  WCHAR   szFontName[256];
  WCHAR*  psz;
  WCHAR*  szName2;

  memset( szOutName, 0, sizeof(WCHAR)*OutSize );
  wcscpy( szFontName, szName );
  szName2 = wcschr( szFontName, L' ' );  // first blank character
  if (szName2 == NULL){
    wcscpy( szOutName, szFontName );
    return;
  }

  psz = wcsstr( szName2, L"Extra Bold" );
  if (psz){
    for (i=0; i<10; ++i) psz[i]=32;
  }

  psz = wcsstr( szName2, L"Bold" );
  if (psz){
    for (i=0; i<4; ++i) psz[i]=32;
  }

  psz = wcsstr( szName2, L"Italic" );
  if (psz){
    for (i=0; i<6; ++i) psz[i]=32;
  }

  // remove multiple spaces
  len = wcslen( szFontName );
  for (i=j=0; i<len; ++i){
    if (szFontName[i] == 32){
      if ((szFontName[i+1] != 32) && (szFontName[i+1] != 0)){
        szOutName[j] = szFontName[i];
        ++j;
      }
    }else{
      szOutName[j] = szFontName[i];
      ++j;
    }
  }
}



//-----------------------------------------------
int GetNextNameValue (HKEY key, LPCWSTR subkey, WCHAR* szName, WCHAR* szData)
{
  static HKEY hkey = NULL;
  static DWORD dwIndex = 0;
  int retval;

  if ((subkey == NULL) && (szName == NULL) && (szData == NULL)){
    // closing key
    if (hkey){
      ::RegCloseKey( hkey );
    }
    hkey = NULL;
    return ERROR_SUCCESS;
  }

  if (subkey && (subkey[0] != 0)){
    retval = ::RegOpenKeyEx( key, subkey, 0, KEY_READ, &hkey );   // KEY_ALL_ACCESS
    if (retval != ERROR_SUCCESS){
      // TRACE(_T("RegOpenKeyEx failed\n"));
      return retval;
    }
    dwIndex = 0;
  }else{
    ++dwIndex;
  }

  if ((szName == NULL) || (szData == NULL)){
    return ERROR_SUCCESS;
  }
  szName[0] = 0;
  szData[0] = 0;

  WCHAR szValueName[MAX_PATH];
	DWORD dwValueNameSize = MAX_PATH - 1;
	WCHAR szValueData[MAX_PATH];
	DWORD dwValueDataSize = sizeof(szValueData)-2;
	DWORD dwType = 0;

  retval = ::RegEnumValue( hkey, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, (BYTE*)szValueData, &dwValueDataSize );
  if (retval == ERROR_SUCCESS){
    wcscpy( szName, szValueName );
    wcscpy( szData, (WCHAR*)szValueData );
  }else{
    // TRACE(_T("RegEnumKey failed\n"));
  }
  return retval;
}

//-----------------------------------------------
static int _cdecl CmpFontNames (const void* e1, const void* e2)
{
  CFontName* pFont1 = (CFontName*)e1; 
  CFontName* pFont2 = (CFontName*)e2;
  if (pFont1->Type() == pFont2->Type()){
    return wcscmp(pFont1->Name(),pFont2->Name());
  }else{
    if (pFont1->Type() == FONT_TYPE_TTF){
      return 1;
    }else{
      return -1;
    }
    return 0;
  }
}




//-----------------------------------------------
CFontName::CFontName ()
{
  m_szName = NULL;
  m_szFile = NULL;
  m_nChars = 0;
  m_Type   = FONT_TYPE_TTF;
}
//-----------------------------------------------
CFontName::~CFontName ()
{
  if (m_szName){
    free( m_szName );
    m_szName = NULL;
  }
  if (m_szFile){
    free( m_szFile );
    m_szFile = NULL;
  }
}

//-----------------------------------------------
void CFontName::Set (LPCWSTR szName, LPCWSTR szFileName, int Type)
{
  UINT_PTR nc;

  if (m_szName){
    free( m_szName );
    m_szName = NULL;
  }
  m_nChars = (int)wcslen( szName );
  if (m_nChars > 0){
    m_szName = (WCHAR*)calloc( m_nChars+1, sizeof(WCHAR) );
    if (m_szName){
      wcscpy( m_szName, szName );
    }
  }else{
    m_nChars = 1;
    m_szName = (WCHAR*)calloc( 2, sizeof(WCHAR) );
    if (m_szName){
      m_szName[0] = L'?';
    }
  }
  m_Type = Type;

  if (m_szFile){
    free( m_szFile );
  }
  nc = wcslen( szFileName );
  if (nc > 0){
    m_szFile = (WCHAR*)calloc( nc+1, sizeof(WCHAR) );
    if (m_szFile){
      wcscpy( m_szFile, szFileName );
    }
  }else{
    m_szFile = (WCHAR*)calloc( 2, sizeof(WCHAR) );
    if (m_szFile){
      m_szFile[0] = L'?';
    }
  }
}

