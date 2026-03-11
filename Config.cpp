/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* class COkConfig - used to store app config values
********************************************************************/
#include <stdio.h>
#include "framework.h"

#define MAX_CFGITEMS  1000

//*****************************************************************************
CConfigItem::CConfigItem ()
{
  m_szKey = NULL;
  m_szValue = NULL;
}


//-------------------------------------
CConfigItem::~CConfigItem ()
{
  if (m_szKey){
    delete[] m_szKey;
  }
  if (m_szValue){
    delete[] m_szValue;
  }
}


//-------------------------------------
void CConfigItem::Set (LPCWSTR szKey, LPCWSTR szValue)
{
  size_t len;

  len = wcslen( szKey ) + 2;
  if (m_szKey){
    delete[] m_szKey;
  }
  m_szKey = new WCHAR[len];
  memset( m_szKey, 0, sizeof(WCHAR)*len );
  wcscpy( m_szKey, szKey );

  len = wcslen( szValue ) + 2;
  if (m_szValue){
    delete[] m_szValue;
  }
  m_szValue = new WCHAR[len];
  memset( m_szValue, 0, sizeof(WCHAR)*len );
  wcscpy( m_szValue, szValue );
}


//-------------------------------------
void CConfigItem::Get (WCHAR* szKey, WCHAR* szValue) const
{
  wcscpy( szKey, m_szKey );
  wcscpy( szValue, m_szValue );
}


//-------------------------------------
bool CConfigItem::GetValue (LPCWSTR szKey, WCHAR* szValue, UINT_PTR nMaxChars) const
{
  UINT_PTR k;
  if (m_szKey && m_szValue && (nMaxChars >= 4)){
    if (_wcsicmp( m_szKey, szKey ) == 0){
      k = nMaxChars - 1;
      wcsncpy( szValue, m_szValue, k );
      szValue[k] = 0;
      return true;
    }
  }
  szValue[0] = 0;
  return false;
}




//*******************************************************************
COkConfig::COkConfig ()
{
  memset( m_szFileName, 0, sizeof(m_szFileName) );
  m_Item = new CConfigItem[MAX_CFGITEMS+1];
  m_nItems = 0;
}


//-------------------------------------
COkConfig::~COkConfig ()
{
  Clear();
}

//-------------------------------------
void COkConfig::Clear()
{
  if (m_Item){
    delete[] m_Item;
    m_Item = NULL;
  }
  m_nItems = 0;
}


//-------------------------------------
void COkConfig::SetFileName (LPCWSTR szFileName)
{
  memset( m_szFileName, 0, sizeof(m_szFileName) );
  wcsncpy( m_szFileName, szFileName, 255 );
}


//-------------------------------------
bool COkConfig::Read (LPCWSTR szFileName, const char* szSignature)
{
  char   szBufA[320];
  WCHAR  szBuf[320];
  WCHAR* pd;
  WCHAR  divider = L'=';
  WCHAR  endstr = L'\n';
  int    nWChars, err, ok=1;
  size_t len;
  bool   bFirst = true;
  FILE*  df;

  m_nItems = 0;
  if (szFileName){
    df = _wfopen( szFileName, L"rt" );
  }else{
    df = _wfopen( m_szFileName, L"rt" );
  }
  if (df){
    while( ok ){
      if (fgets( szBufA, 310, df ) == NULL){
        break;
      }
      if (bFirst){
        bFirst = false;
        if (szSignature){
          len = strlen( szSignature );
          if (strncmp( szBufA, szSignature, len ) == 0){
            continue;
          }
          return false;  // wrong signature
        }
      }
      szBuf[0] = 0;
      nWChars = ::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szBufA, -1, szBuf, 310 );
      szBuf[310-1] = 0;
      if (nWChars == 0){
        err = ::GetLastError();
        return false;
      }

      pd = wcschr( szBuf, endstr );
      if (pd){
        pd[0] = 0;
        pd = wcschr( szBuf, divider );
        if (pd){
          pd[0] = 0;
          m_Item[m_nItems].Set( szBuf, pd+1 );
          ++m_nItems;
          if (m_nItems >= MAX_CFGITEMS){
            break;
          }
        }
      }
    }
    fclose( df );
    return true;
  }   
  return false;
}


//-------------------------------------
bool COkConfig::GetValue (LPCWSTR szKey, WCHAR* szValue, UINT_PTR nMaxChars) const
{
  UINT_PTR i;

  for (i=0; i<m_nItems; ++i){
    if (m_Item[i].GetValue( szKey, szValue, nMaxChars )){
      return true;
    }
  }
  szValue[0] = 0;
  return false;  // key not found
}


//-------------------------------------
bool COkConfig::GetValueBool (LPCWSTR szKey, bool bDefVal) const
{
  WCHAR szValue[32];
  int   iv;
  if (GetValue( szKey, szValue, 30 )){
    iv = _wtoi( szValue );
    return (iv)? true : false;
  }
  return bDefVal;
}

//-------------------------------------
int COkConfig::GetValueInt (LPCWSTR szKey, int DefVal, int MinVal, int MaxVal) const
{
  WCHAR szValue[32];
  int   iv;
  if (GetValue( szKey, szValue, 30 )){
    iv = _wtoi( szValue );
    if (MinVal < MaxVal){
      if (MinVal <= iv && iv <= MaxVal){
        return iv;
      }
    }else{
      return iv;
    }
  }
  return DefVal;
}

//-------------------------------------
double COkConfig::GetValueDbl (LPCWSTR szKey, double DefVal, double MinVal, double MaxVal) const
{
  WCHAR  szValue[32];
  double dv;
  if (GetValue( szKey, szValue, 30 )){
    dv = wcstod( szValue, NULL );
    if (MinVal < MaxVal){
      if (MinVal <= dv && dv <= MaxVal){
        return dv;
      }
    }else{
      return dv;
    }
  }
  return DefVal;
}


