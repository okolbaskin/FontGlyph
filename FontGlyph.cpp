/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* class CFontGlyph
********************************************************************/
#include "framework.h"
#include "FontGlyph.h"
#include <stdio.h>
#include <shlobj_core.h>
#include <commctrl.h>
#include <commdlg.h>

extern WCHAR  g_szLastDir[];    // last used directory (for Export)
extern USUBSET  g_USubSet[];

//-----------------------------------------------
void CFontGlyph::FileOpenTTF (LPCWSTR szFontName, LPCWSTR szFileName, int Cmd)
{
  CHOOSEFONTW cf;            // common dialog box structure
  // Initialize CHOOSEFONT
  memset( &cf, 0, sizeof(CHOOSEFONTW) );
  cf.lStructSize = sizeof (CHOOSEFONTW);
  cf.hwndOwner = m_hwMain;
  cf.hInstance = m_hInst;
  cf.lpLogFont = &m_LF;
  cf.Flags = CF_TTONLY | CF_SCREENFONTS | CF_EFFECTS;
  cf.Flags|=CF_INITTOLOGFONTSTRUCT;
  
  m_LF.lfHeight = 20; 
  if (szFontName){
    wcsncpy( m_LF.lfFaceName, szFontName, LF_FACESIZE-1 );
    m_LF.lfFaceName[LF_FACESIZE-1] = 0;
    switch( Cmd ){
      case TTF_REGULAR:
        m_LF.lfWeight = FW_NORMAL;
        m_LF.lfItalic = false;
        break;
      case TTF_BOLD:
        m_LF.lfWeight = FW_BOLD;
        m_LF.lfItalic = false;
        break;
      case TTF_ITALIC:
        m_LF.lfWeight = FW_NORMAL;
        m_LF.lfItalic = true;
        break;
      case TTF_BOLDITALIC:
        m_LF.lfWeight = FW_BOLD;
        m_LF.lfItalic = true;
        break;
    }
    goto m1;
  }
  if (::ChooseFont( &cf )){
m1:
    ClearFontData();
    m_bFontLoaded = true;
    wcscpy( m_szFontName, m_LF.lfFaceName );
    wcscpy( m_szFileName, STR_SYSFONT );
    m_LF.lfHeight = GLYPH_SIZE; 
    UpdateSubsets();
    m_Info.SetFileName( szFileName );
    m_Info.SetFontName( m_szFontName, Cmd );
    m_FontMode = Cmd;
    m_Info.SetNumChars( m_nChars, 0 );
    ::InvalidateRect( m_hwMain, NULL, true );
  }
}

//-----------------------------------------------
bool CFontGlyph::GetUnicodeRanges ()
{
  bool bRet = false;
  UINT        i, j, k, n, nrg;
  HFONT       hFont;
  HGDIOBJ     hPrevFont;
  TEXTMETRICW tm;
  DWORD       cb, cbRet;
  DWORD       dwSize = sizeof(GLYPHSET);
  BYTE*       Buf = NULL;
  GLYPHSET*   Pgs = NULL;
  WCRANGE*    pRange = NULL;
  HDC         hDC = ::CreateDC( L"DISPLAY", NULL, NULL, NULL );

  memset( m_UChar, 0, sizeof(UCCHAR)*65536 );
  m_nChars = 0;
  m_nSbarLines = 0;
  if (!hDC){
    ::MessageBox( m_hwMain, L"Can't create DISPLAY DC", APP_TITLE, MB_OK|MB_ICONEXCLAMATION );
    return false;
  }
  hFont = ::CreateFontIndirect( &m_LF );
  if (hFont == NULL){
    ::DeleteDC( hDC );
    return false;
  }
  hPrevFont = ::SelectObject( hDC, hFont );
  ::GetTextMetrics( hDC, &tm );
  m_FontHeight = tm.tmAscent - tm.tmInternalLeading;

  // First call with NULL to get required buffer size in bytes
  cb = ::GetFontUnicodeRanges( hDC, NULL );
  if (cb > dwSize) {
    // Allocate buffer and retrieve GLYPHSET
    Buf = (BYTE*)calloc( 1, cb );
    if (Buf){
      Pgs = (GLYPHSET*)Buf;
      cbRet = ::GetFontUnicodeRanges( hDC, Pgs );
      if (cbRet == cb){
        m_nChars = Pgs->cGlyphsSupported;
        k = 0;
        for (i=0; i<Pgs->cRanges; ++i){
          pRange = Pgs->ranges + i;
          j = pRange->wcLow;
          nrg = pRange->cGlyphs;  // number or glyphs in the range
          n = j + nrg - 1;
          for (; j<=n; ++j){
            if (k >= m_nChars){
              ::MessageBox( m_hwMain, L"Exceeding of max characters", L"Error", MB_OK );
              goto m1;
            }
            m_UChar[j].bExist = true;
            ++k;
          }
        }
        bRet = true;
      }
m1:
      free( Buf ); 
      m_nSbarLines = (m_nChars / NUM_CHARSROW) + 1;   // number of lines in the m_hListChars listbox
    }else{
      ::MessageBox( m_hwMain, L"Allocation failed", APP_TITLE, MB_OK|MB_ICONEXCLAMATION );

    }
  }
  ::SelectObject( hDC, hPrevFont );
  ::DeleteObject( hFont );
  ::DeleteDC( hDC );
  return bRet;
}

// Determine number of used characters in each Unicode subset
// and update the "Subsets" listbox
//-----------------------------------------------
void CFontGlyph::UpdateSubsets ()
{
  CSubsetRow* pRow;
  UINT i, j, k, start, end;

  m_Subsets.ClearRows();
  if (GetUnicodeRanges() == false){
    return;
  }
  m_pGWnd->m_IntVal = 0;
  for (i=0; i<NUM_USUBSETS; ++i){
    g_USubSet[i].nUsedCodes = 0;
    start = g_USubSet[i].Start;
    end = g_USubSet[i].End;
    k = 0;
    for (j=start; j<=end; ++j){
      m_UChar[j].iSubset = i;
      if (m_UChar[j].bExist){
        m_UChar[j].iSubChar = k;
        ++k;
        ++(g_USubSet[i].nUsedCodes);
      }
    }
    if (g_USubSet[i].nUsedCodes > 0){
      pRow = (CSubsetRow*)m_Subsets.AddRow( new CSubsetRow(i) );
      pRow->SetID( i );
    }
  }
  m_Subsets.SortColumn( 2, true );  // sort by "Start" code
  m_Subsets.SelectColumn( 0 );
  m_Subsets.Update(); 
  pRow = (CSubsetRow*)m_Subsets.GetFirstRow();
  if (pRow){
    m_Subsets.SelectRow( pRow );
  }
}

//-----------------------------------------------
bool CFontGlyph::GetGlyph_TTF (UINT uChar)
{
  bool        bRet = false;
  TEXTMETRICW tm;
  HFONT       hFont;
  HGDIOBJ     hPrevFont;
  HDC         hDC = ::CreateDC( L"DISPLAY", NULL, NULL, NULL );

  if (!hDC){
    ::MessageBox( m_hwMain, L"Can't create DISPLAY DC", APP_TITLE, MB_OK|MB_ICONEXCLAMATION );
    return false;
  }
  m_LF.lfHeight = GLYPH_SIZE;
  hFont = ::CreateFontIndirect( &m_LF );
  if (hFont == NULL){
    ::DeleteDC( hDC );
    ::MessageBox( m_hwMain, L"Can't create HFONT", APP_TITLE, MB_OK|MB_ICONEXCLAMATION );
    return false;
  }
  hPrevFont = ::SelectObject( hDC, hFont );
  ::GetTextMetrics( hDC, &tm );
  m_CharAscent  = tm.tmAscent;
  m_CharDescent = tm.tmDescent;
  m_CharIntLead = tm.tmInternalLeading;
  m_CharExtLead = tm.tmExternalLeading;
  bRet = GetGlyphPoints( uChar, hDC );
  if (bRet){
    UpdateCharRect();
  }else{
    ::MessageBox( m_hwMain, L"Can't get glyph outline", APP_TITLE, MB_OK|MB_ICONEXCLAMATION );
  }
  ::SelectObject( hDC, hPrevFont );
  ::DeleteObject( hFont );
  ::DeleteDC( hDC );
  return bRet;
}

//-----------------------------------------------
bool CFontGlyph::GetGlyphPoints (UINT uChar, HDC hDC)
{
  MAT2             _mat2;  // transformation matrix structure
  GLYPHMETRICS     gm;     // address of structure for metrics
  double           dx=0, Delta=0.0001;
  int              off=0, off2;
  int              i, cb, n;
  GLYPHPOINT*      pVer = m_Ver;
  TTPOLYGONHEADER* pgn;
  TTPOLYCURVE*     pc;

  _mat2.eM11.value=1;
  _mat2.eM11.fract=0;
  _mat2.eM12.value=0;
  _mat2.eM12.fract=0;
  _mat2.eM21.value=0;
  _mat2.eM21.fract=0;
  _mat2.eM22.value=1;
  _mat2.eM22.fract=0;
  // Get glyph metrics 
  n = ::GetGlyphOutline( hDC, uChar, GGO_METRICS, &gm, 0, NULL, &_mat2 );
  if (n != GDI_ERROR){
    m_W2 = abs(gm.gmCellIncX);         // distance to next character cell 
//    m_W1 = gm.gmBlackBoxX;
    m_W1 = (WORD)(gm.gmptGlyphOrigin.x + gm.gmBlackBoxX);
    m_H  = (WORD)(gm.gmptGlyphOrigin.y);   // height of character cell 
    m_GlyphOrigin = gm.gmptGlyphOrigin;
    m_GlyphSize.x = gm.gmBlackBoxX;
    m_GlyphSize.y = gm.gmBlackBoxY;
    // get size of outline buffer 
    n = ::GetGlyphOutline( hDC, uChar, GGO_NATIVE, &gm, 0, NULL, &_mat2 );
    if ((n != GDI_ERROR) && (n < 16300)){
      // get outline's data 
      ::GetGlyphOutline( hDC, uChar, GGO_NATIVE, &gm, n, m_Buf, &_mat2 );
      if (m_H == 0){
        m_H = (WORD)(gm.gmptGlyphOrigin.y);   // height of character cell 
      }
      // convert outline points to char polyline
      m_nVers = 0;   // counter for points of char polyline
      m_nPL   = 0;    // counter for char polylines
      m_PLSize[m_nPL] = 0;  // array of char polylines sizes (points)
      while( off < n ){
        pgn = (TTPOLYGONHEADER*)(m_Buf+off);
        cb = pgn->cb;
        pVer->x = pgn->pfxStart.x.value;
        pVer->y = pgn->pfxStart.y.value;
        pVer->bFix = 1;
        ++pVer;
        ++m_nVers;
        ++(m_PLSize[m_nPL]);
        off2 = sizeof(TTPOLYGONHEADER);
        while( off2 < cb ){
          pc = (TTPOLYCURVE*)(m_Buf+off+off2);
          for (i=0; i<pc->cpfx; ++i){
            pVer->x = pc->apfx[i].x.value;
            pVer->y = pc->apfx[i].y.value;
            if (pc->wType==TT_PRIM_QSPLINE){
              if (i==(pc->cpfx-1)){
                pVer->bFix = 1;
              }else{
                pVer->bFix = 0;
              }
            }else{
              pVer->bFix = 1;
            }
            ++pVer;
            ++m_nVers;
            ++(m_PLSize[m_nPL]);
          }
          off2 += sizeof(TTPOLYCURVE) + sizeof(POINTFX) * (pc->cpfx - 1);
        }
        ++m_nPL;
        m_PLSize[m_nPL] = 0;
        off+=off2;
      }
      return true;
    }
  }
  return false;
}

//-----------------------------------------------
void CFontGlyph::UpdateCharRect ()
{
  CGeRect Rect;
  UINT    i, ht;
  GLYPHPOINT* pVer = m_Ver;

  m_pGWnd->ClearZoomPrev();
  m_CharRect.SetNull();
  if (m_nVers > 0){
    m_CharRect.Set( CGePoint(pVer->x,pVer->y), 0.0 ); 
    ++pVer;
    for (i=1; i<m_nVers; ++i){
      m_CharRect.UpdateExtentsBy( pVer->x, pVer->y );
      ++pVer;
    }
  }
  if (m_CharRect.IsNull()){
    Rect.Set( 0, 0, m_W2, m_FontHeight );
  }else{
    Rect = m_CharRect;
  }
  Rect.Scale( 1.05, true );
  m_pGWnd->SetExtRect( Rect );
  if (m_pGWnd->m_IntVal == 0) {
    m_pGWnd->m_IntVal = 1;
    ht = m_CharDescent + m_CharAscent;
    Rect.Set( 0, -m_CharDescent, m_W2, ht );
    Rect.Scale( 1.01, true );
    m_pGWnd->ZoomRect( Rect );
  }
}

// clear data of currently loaded font
//-----------------------------------------------
void CFontGlyph::ClearFontData ()
{
  if (m_UChar){
    memset( m_UChar, 0, sizeof(UCCHAR)*65536 ); 
  }
  m_nChars = 0;
  m_nVers = 0;
  m_nPL = 0;
  m_Subsets.ClearRows();
  m_bFontLoaded = false;
  ::SendMessage( m_hListChars, LB_RESETCONTENT, 0, 0 );
  ::InvalidateRect( m_hwMain, NULL, true );
  m_bNoZoom = true;
}


//-----------------------------------------------
bool CFontGlyph::SelectChar (DWORD Code)
{
  UCCHAR* pUChar = m_UChar + Code;
  USUBSET* pUSubset;
  CSubsetRow* pRow;
  int i;

  if (pUChar->bExist){
    pRow = (CSubsetRow*)m_Subsets.GetFirstRow();
    while( pRow ){
      i = (int)pRow->ID().Value();
      if (i == pUChar->iSubset){
        m_Subsets.SelectRow( pRow );
        m_iCurSubChar = pUChar->iSubChar;     // index of current char in the Unicode subset
        pUSubset = pRow->UnicodeSubset();
        i = m_iCurSubChar / NUM_CHARSROW;
        m_CharCode = Code;
        GetGlyph_TTF( m_CharCode );
        ::SendMessage( m_hListChars, LB_SETCURSEL, i, 0 );
        ::InvalidateRect( m_hListChars, NULL, false );
        UpdateInfoChar();
        return true;
      }
      pRow = (CSubsetRow*)m_Subsets.GetNextRow( pRow );
    }
  }
  // symbol not found in the font
  return false;
}



//-----------------------------------------------
void CFontGlyph::UpdateInfoChar ()
{
  CSubsetRow* pRow = (CSubsetRow*)m_Subsets.GetRow();  // current Unicode subset
  m_Info.SetSubset( pRow->UnicodeSubset() );
  m_Info.SetCharCode( m_CharCode, NULL );
  m_Info.SetCharWidth( m_W1/m_FontHeight, m_W2/m_FontHeight );
  m_Info.ShowCharBox( m_bShowCharBox );
  m_Info.ShowCtrlPoints( m_bShowCtrlPoints );
  m_Info.ShowCtrlNums( m_bShowCtrlNums );
  m_Info.ShowCtrlLines( m_bShowCtrlLines );
  m_Info.SetNumPoints( m_nVers, m_nPL );
  m_Info.Invalidate();
  m_pGWnd->Invalidate();
}

//-----------------------------------------------
int CFontGlyph::GetCharCode (CSubsetRow* pRow, int iSubsetPos) const
{
  int Code = -1;
  DWORD start, end, i;
  CSubsetRow* pSubsetRow = pRow; 
  if (pSubsetRow == NULL){
    pSubsetRow = (CSubsetRow*)m_Subsets.GetRow();  // current subset row
  }
  if (pSubsetRow){
    start = pSubsetRow->UnicodeSubset()->Start;
    end = pSubsetRow->UnicodeSubset()->End;
    for (i=start; i<=end; ++i){
      if (m_UChar[i].bExist){
        if (m_UChar[i].iSubChar == iSubsetPos){
          Code = i;
          break;
        }
      }
    }
  }
  return Code;
}


static BOOL CALLBACK ProcCharcode (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
static POINT _DlgPos;
static WCHAR _szValDec[16];
static WCHAR _szValHex[16];

//-----------------------------------------------
void CFontGlyph::FindSymbol ()
{
  INT_PTR RetVal;
  bool bFound = false;
  int  Code;

  memset( _szValDec, 0, sizeof(_szValDec) );
  memset( _szValHex, 0, sizeof(_szValHex) );
  ::GetCursorPos( &_DlgPos );
  RetVal = ::DialogBox( m_hInst, MAKEINTRESOURCEW(IDD_SELECTCHAR), m_hwMain, (DLGPROC)ProcCharcode );
  if (RetVal == IDOK){
    if (_szValDec[0]) {  // decimal code is entered
      Code = _wtoi( _szValDec );
      if ((Code > 0) && (Code <= 65535)){
        bFound = SelectChar( Code );
      }
    }
    if (!bFound && _szValHex[0]) {  // hex code is entered
      // convert hex string to decimal code
      Code = (int)wcstoul(_szValHex, NULL, 16 );
      if ((Code > 0) && (Code <= 65535)){
        bFound = SelectChar( Code );
      }
    }
    if (bFound){
      ZoomCharCell( false );
    }else{
      ::MessageBox( m_hwMain, L"Character not found in the font", L"Find symbol", MB_OK | MB_ICONINFORMATION );
    }
  }
  m_pGWnd->SetFocus();
}

//-----------------------------------------------
void CFontGlyph::ZoomCharCell (bool bSaveZoomPrev) const
{
  CGeRect Rect;

  Rect.Set( 0, -m_CharDescent, m_W2, m_CharDescent + m_CharAscent );
  Rect.Scale( 1.01, true );
  m_pGWnd->ZoomRect( Rect, bSaveZoomPrev );
}

//-----------------------------------------------
static BOOL CALLBACK ProcCharcode (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
    case WM_INITDIALOG:
      ::SetWindowPos( hDlg, HWND_TOP, _DlgPos.x, _DlgPos.y, 0,0, SWP_NOSIZE );
      okVarToDialog( hDlg, EDIT_CODE_DEC, L"%s", _szValDec );
      okVarToDialog( hDlg, EDIT_CODE_HEX, L"%s", _szValHex );
      return TRUE;

    case WM_CLOSE:
      ::EndDialog( hDlg, 0 );
      return TRUE;

    case WM_COMMAND:
      switch (LOWORD(wParam)) {

        case IDOK:
          okDialogToVar( hDlg, EDIT_CODE_DEC, 4, L"%s", _szValDec );
          okDialogToVar( hDlg, EDIT_CODE_HEX, 4, L"%s", _szValHex );
          ::EndDialog( hDlg, IDOK );
          return TRUE;

        case IDCANCEL:
          ::EndDialog( hDlg, 0 );
          return TRUE;

      }
      break;
  }
  return FALSE;
}


//-----------------------------------------------
void CFontGlyph::Export () const
{
  FILE*  df;
  int    iPath, j, npt, k;
  double x, y;
  WCHAR  szFilter[512], szFName[256];
  char   szBufA[128];
  bool   bFontFilled = true;
  bool   bFontTTF = true;

  if (m_nVers >= 2 && m_nPL >= 1){
    wcscpy( szFilter, L"TTF symbol data  (*.txt)|*.txt|" );
    swprintf( szFName, L"%s %d.txt", m_szFontName, m_CharCode );
    if (okGetSaveFileName( m_hwMain, NULL, szFilter, L"txt", g_szLastDir, szFName )){
      df = _wfopen( szFName, L"wt" );
      if (df){
//        okUnicodeToUTF8( m_szFontName, szBufA, 120 );
        ::WideCharToMultiByte( CP_ACP, 0, m_szFontName, -1, szBufA,  120, NULL, NULL );
        fprintf( df, "FONTNAME=%s\nCHARCODE=%d\n", szBufA, m_CharCode ); 
        fprintf( df, "POINTS=%d\nPATHS=%d\n", m_nVers, m_nPL );
        k = 0;
        for (iPath=0; iPath<m_nPL; ++iPath){
          npt = m_PLSize[iPath];
          fprintf( df, "PATHSIZE=%d\n", npt );
          for (j=0; j<npt; ++j){
            x = m_Ver[k].x / m_FontHeight;
            y = m_Ver[k].y / m_FontHeight;
            fprintf( df, "%.4f %.4f %d\n", x, y, m_Ver[k].bFix?1:0 );
            ++k;
          }
        }
        fclose( df );
        swprintf( szFilter, L"Current symbol has been saved to file\n%s", szFName );
        ::MessageBox( m_hwMain, szFilter, L"Export symbol", MB_OK|MB_ICONINFORMATION );
      }
    }
    m_pGWnd->SetFocus();
  }
}

