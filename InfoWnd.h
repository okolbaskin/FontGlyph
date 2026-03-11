/********************************************************************
* Project: LCFont
* Copyright (c) 2009-2022, Oleg Kolbaskin.
* All rights reserved.
*
* "Info" window
********************************************************************/
#ifndef _VC_INFOWND_
#define _VC_INFOWND_

struct USUBSET;

//-----------------------------------------------
class InfoWnd {
  HINSTANCE m_hInst;
  HWND   m_hwFrame;     // 
  HWND   m_hwFileName1;
  HWND   m_hwFileName2;
  HWND   m_hwFontName1;
  HWND   m_hwFontName2;
  HWND   m_hwNChars1;
  HWND   m_hwNChars2;
  HWND   m_hwHorLine;
  HWND   m_hwCharCode1;
  HWND   m_hwCharCode2;
  HWND   m_hwSubset1;
  HWND   m_hwSubset2;
  HWND   m_hwCoords1;
  HWND   m_hwCoords2;
  HWND   m_hwCharW1;
  HWND   m_hwCharW2;
  HWND   m_hwCharPts1;
  HWND   m_hwCharPts2;
  HWND   m_hwChkPoints;
  HWND   m_hwChkNums;
  HWND   m_hwChkLines;
  HWND   m_hwChkRect;
public:
    InfoWnd();
    ~InfoWnd();

  HWND  Create     (HINSTANCE hInst, HWND hwParent);
  void  Resize     (int X, int Y, int W, int H);
  void  Invalidate ();

  int   WinProcFrame (UINT Msg, WPARAM wParam, LPARAM lParam);

  void  SetFileName    (LPCWSTR szFileName) const;
  void  SetFontName    (LPCWSTR szFontName, int Mode);
  void  SetNumChars    (int nChars, int nSelChars) const;
  void  ShowCtrlPoints (bool bShow) const;
  void  ShowCtrlNums   (bool bShow) const;
  void  ShowCtrlLines  (bool bShow) const;
  void  ShowCharBox    (bool bShow) const;
  void  SetCharCode    (int Code, LPCWSTR szName) const;
  void  SetSubset      (const USUBSET* pUS) const;
  void  SetCoords      (double x, double y, LPCWSTR szSnapInfo) const;
  void  SetCharWidth   (double w1, double w2) const;
  void  SetNumPoints   (int nPoints, int nPlines) const;

private:
  void    RegisterClass () const;
};



#endif  // _VC_INFOWND_

