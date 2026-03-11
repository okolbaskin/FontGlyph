/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* class CFontGlyph
********************************************************************/
#pragma once

#include "framework.h"
#include "resource.h"


#define GLYPH_SIZE      10000
#define NUM_USUBSETS    102
#define NUM_CHARSROW    6     // number of characters in one row of the m_hListChars listbox
#define MAX_FONTS       2000  // max number of fonts in the m_hListFonts listbox

#define ID_LIST_FONTS   701
#define ID_LIST_SUBSETS 702
#define ID_LIST_CHARS   703

#define ID_SETHEIGHT    251
#define ID_SETWIDTH     252
#define ID_ZOOMFONT     253
#define ID_ZOOMEXT      254
#define ID_ZOOMRECT     255
#define ID_ZOOMPREV     256

#define SUBSETS_Y    0
#define CAD_Y0       175
#define BOTTOM_GAP   0

#define TTF_REGULAR      1
#define TTF_BOLD         2
#define TTF_ITALIC       3
#define TTF_BOLDITALIC   4

#define FONT_TYPE_TTF   0    // imported from Windows TTF

// Application GUI strings 
#define STR_SHOWPOINTS  L"Display control points" 
#define STR_SHOWNUMS    L"Display points numbers"
#define STR_SHOWLINES   L"Display control lines"
#define STR_SHOWCHARBOX L"Display ascent/descent rect"
#define STR_USUBSET     L"Unicode subset"
#define STR_USTART      L"Start"
#define STR_UEND        L"End"
#define STR_UNCODES     L"Available"
#define STR_UNCHARS     L"Symbols"
#define STR_FONTNAME    L"Font Name :"
#define STR_FONTFILE    L"Font File :"
#define STR_FONTCHARS   L"Amount of Symbols :"
#define STR_SYSFONT     L"system font"
#define STR_CHARCODE    L"Symbol Code :"
#define STR_USUBSET2    L"Unicode subset :"
#define STR_UNKNOWN     L"unknown"
#define STR_CURSORXY    L"Cursor Pos :"
#define STR_CHARWIDTH   L"Symbol Width :"
#define STR_NOSELCHARS  L"No selected symbols"
#define STR_FONTCREATED L"Font created successfully"
#define STR_ERRLOADFILE L"Can't open the file\n%s"
#define STR_FILEEXIST   L"The file\n'%s'\nalready exist, overwrite it?"
#define STR_CHARPOINTS  L"Points :"
#define STR_CHARPOINTS2 L"%d  (in %d polylines)"
#define STR_FILELOADED  L"The file is already opened"


//-----------------------------------------------
struct USUBSET {        // Unicode subset
  DWORD Start;          // start code
  DWORD End;            // end code 
  DWORD nUsedCodes;     // number of used characters in loaded font
  DWORD nSelCodes;      // number of selected characters (not used)
  WCHAR szDesc[48];     // description of the subset
};

//-----------------------------------------------
struct UCCHAR {
  bool bExist;
  int  iSubset;    // index of unicode subset
  int  iSubChar;   // index of the char inside the subset
};

//-----------------------------------------------
struct GLYPHPOINT {
  short x = 0;
  short y = 0;
  BYTE  bFix = 0;
};

//-----------------------------------------------
class CSubsetRow : public COkTRow {
  int   m_iRec;  // index of a row in the g_USubSet table
public:
    CSubsetRow (int i) {m_iRec=i;}

  virtual bool GetValue    (const COkCtrlGrid* pGrid, UINT idColumn, WCHAR* szValue, UINT MaxChars);
  virtual bool SetValue    (const COkCtrlGrid* pGrid, UINT idColumn, LPCWSTR szValue) {return false;}
  virtual int  CompareWith (COkTRow* pRow2, UINT idColumn, bool bSortUp);

  USUBSET* UnicodeSubset() const;
};


//-----------------------------------------------
class CFontName {
  WCHAR* m_szName;
  WCHAR* m_szFile;
  int    m_nChars;
  int    m_Type;     // FONT_TYPE_TTF and others
public:
    CFontName ();
    ~CFontName ();
  void    Set      (LPCWSTR szName, LPCWSTR szFileName, int Type);
  LPCWSTR Name     () const {return m_szName;}
  LPCWSTR FileName () const {return m_szFile;}
  int     NumChars () const {return m_nChars;}
  int     Type     () const {return m_Type;}
};


//-----------------------------------------------
class CFontGlyph {
  HINSTANCE    m_hInst;           // application instance
  WCHAR        m_szVersion[16];   // application version
  WCHAR        m_szPath[260];     // application directory path
  WCHAR        m_szExeName[260];  // application filename
  COkConfig    m_Cfg;
  HWND         m_hwMain;
  int          m_WndWidth;
  int          m_WndHeight;
  HWND         m_hListChars;      // symbols previews  
  HWND         m_hListFonts;      // List of fonts
  CFontName    m_FontName[MAX_FONTS];  // Fonts array to fill m_hListFonts
  int          m_nFonts;          // number of fonts in the m_FontName array
  int          m_iFont;           // index of currently selected font
  InfoWnd      m_Info;            // Information window
  LcWindow*    m_pGWnd;           // Graphics window
  HWND         m_hPicChar[48];    // symbols previews  
  WCHAR        m_szFileName[256];
  WCHAR        m_szFontName[256];
  int          m_FontMode;        // TTF_REGULAR and others
  UINT         m_nChars;          // number of chars
  UCCHAR*      m_UChar;           // unicode array
  int          m_CharCode;        // code of selected char (also index in the 'm_UChar' array)
  int          m_iCurSubChar;     // index of current char in the range subset
  LOGFONT      m_LF;
  bool         m_bFontLoaded;
  int          m_nSbarLines;      // number of lines for scrollbar
  COkCtrlGrid  m_Subsets;         // Unicode subsets grid
  COkCtrlDiv   m_DivVer1;         // vert. divider "Fonts - Ranges"
  COkCtrlDiv   m_DivVer2;         // vert. divider "Ranges - CAD window"
  COkCtrlDiv   m_DivHorz;         // horz. divider "Ranges - Chars"
  int          m_CellSize;        // cell size in the "Chars" listbox
  WCHAR        m_szSnapInfo[64];  // information of snapped point
  bool         m_bNoZoom;         // invalid zoom, need to set
  BYTE*        m_Buf;             // buffer for glyph outline data

  // parameters of the currently selected char
  WORD         m_W1;              // character width (by right extent of its polylines)
  WORD         m_W2;              // character width with blank space
  WORD         m_H;
  WORD         m_nPL;             // number of polylines in a glyph
  WORD*        m_PLSize;          // sizes for each polyline
  GLYPHPOINT*  m_Ver;             // points of glyph polylines
  WORD         m_nVers;           // number of points in glyph polylines
  CGeRect      m_CharRect;        // rect of char extentions
  int          m_CharAscent;
  int          m_CharDescent;
  int          m_CharIntLead;
  int          m_CharExtLead;
  POINT        m_GlyphOrigin;
  POINT        m_GlyphSize;
  double       m_FontHeight;
  double       m_CursorX;
  double       m_CursorY;

  // to be saved in the app config
  int       m_SubsetLef;          // X of the left  of the "Ranges" window
  int       m_SubsetRig;          // X of the right of the "Ranges" window
  int       m_SubsetBot;          // Y of the bottom of the "Ranges" window
  bool      m_bMaximized;        // app is maximized at start
  bool      m_bShowCharBox;      // display black box
  bool      m_bShowCtrlPoints;   // display control points
  bool      m_bShowCtrlLines;    // display lines between control points
  bool      m_bShowCtrlNums;     // display numbers of control points
public:
    CFontGlyph ();
    ~CFontGlyph ();

  HINSTANCE Instance () const;
  LPCWSTR   Version  () const;
  LPCWSTR   Path     () const;

  bool Init (HINSTANCE hInst, LPCWSTR szVer, LPCWSTR szAppDir, LPCWSTR szExeName);
  void End  ();
  bool Create   (HWND hwParent, int* pCmdShow);
  void Resize   (int SizeType, int W, int H, bool bFromEvent);
  void Update   (int WndW=0, int WndH=0);
  void ToggleShowPoints ();
  void ToggleShowNums ();
  void ToggleShowLines ();
  void ToggleShowBox ();

  void DrawItem (int id, DRAWITEMSTRUCT& dis);

  bool EnumTTFonts ();
  void Dg_About    ();
  int  CellSize    () const {return m_CellSize;}
  void Export      () const;
  void FindSymbol  ();
  void ZoomCharCell(bool bSaveZoomPrev) const;


  void OnSelectSubset (CSubsetRow* pRow);
  void OnSelectChar   ();
  void OnSelectFont   (int Cmd=0);
  int  OnGWndEvent    (int EventType);
private:
  int  OnMouseMove ();
  int  OnRButtonDown ();
  int  OnMouseSnap ();

  void MemAlloc ();
  void MemFree ();
  void InitConfig ();
  void ConfigLoad (LPCWSTR szFileName);
  void ConfigSave ();
  void ClearFontData  ();
  void FileOpenTTF    (LPCWSTR szFontName, LPCWSTR szFileName, int Cmd);
  bool GetUnicodeRanges ();
  void UpdateSubsets  ();
  void UpdateInfoChar ();
  bool SelectChar     (DWORD Code);
  void DrawCharsLine  (DRAWITEMSTRUCT& dis);
  void DrawCharCell   (HDC hDC, RECT rect, int CharCode, bool bCurrent); 
  void NormalizeFontName  (LPCWSTR szName, WCHAR* szOutName, INT_PTR OutSize) const;
  int  GetCharCode (CSubsetRow* pRow, int iSubsetPos) const;

  bool GetGlyph_TTF   (UINT uChar);
  bool GetGlyphPoints (UINT uChar, HDC hDC);
//  void GenDrawing     ();
  int  DrawGlyph ();

  bool AddFontName    (LPCWSTR szFontName, LPCWSTR szFontFile);
  bool SelectFontName (LPCWSTR szFontName);
  void ParseFontName  (WCHAR* szFontName, LPCWSTR szFontFile);
  void DrawFontsLine  (DRAWITEMSTRUCT& dis) const;
  void UpdateCharRect ();
};


//-----------------------------------------------
inline HINSTANCE CFontGlyph::Instance () const {return m_hInst;}
inline LPCWSTR   CFontGlyph::Version() const {return m_szVersion;}
inline LPCWSTR   CFontGlyph::Path() const {return m_szPath;}


