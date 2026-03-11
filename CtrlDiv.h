/********************************************************************
* Project: Divider.Lib
* Copyright (C) 2009-2024, Oleg Kolbaskin
* All rights reserved.
*
* Class COkCtrlDiv 
********************************************************************/
#ifndef _OK_CTRLDIV_
#define _OK_CTRLDIV_

#define MSG_DIV_MOVED  15097   // wParam of WM_USER sent to parent window

//-----------------------------------------------
class COkCtrlDiv {
  bool     m_bEnable;
  bool     m_bVertical;   // true - vertical, false - horizontal divider
  HWND     m_hwParent;    // handle to a parent window
  HWND     m_hWnd;        // handle of control window
  int      m_Left;        // left coord of the control
  int      m_Top;         // top coord of the control
  UINT     m_Width;       // width of the control
  UINT     m_Height;      // height of the control
  HDC      m_hDrawDC;
  HCURSOR  m_hCursor;     // cursor for a divider
  bool     m_bDrag;
  POINT    m_Point;
  POINT    m_PrevPt;
  int*     m_pPos;        // divider's position on a parent window
  bool     m_bIncrease;
  bool     m_bRigLine;
public:
  static COLORREF m_Color;      // window background
  static COLORREF m_Color2;     // line color
  static UINT     m_Size;       // divider's thickness
  static HCURSOR  m_hCurHorz;   // cursor for a horizontal divider
  static HCURSOR  m_hCurVert;   // cursor for a vertical divider
public:
    COkCtrlDiv();
    ~COkCtrlDiv();

  HWND    CreateHor   (HINSTANCE hInst, HWND hwParent, int* pPos, bool bInc);
  HWND    CreateVer   (HINSTANCE hInst, HWND hwParent, int* pPos, bool bInc, bool bRLine=false);
  void    Resize      (int X, int Y, UINT Length);
  void    SetPosition (int Pos);
  UINT    Width       () const;
  UINT    Height      () const;
  void    SetEnabled  (bool b=true);
  bool    IsEnabled   () const;

  LRESULT WinProc (UINT Msg, WPARAM wParam, LPARAM lParam);
private:
  HWND Create        (HINSTANCE hInst, HWND hwParent, int Type, int* pPos, bool bInc);
  bool RegisterClass (HINSTANCE hInst) const;
  void OnPaint       (HDC hDC) const;
  void OnMouseMove   (WPARAM Flags, int x, int y);
  void OnLButtonDown (WPARAM Flags, int x, int y);
  void OnLButtonUp   (WPARAM Flags, int x, int y);
  void DrawXorLine   (int x, int y, UINT width, UINT height) const;
};

inline UINT COkCtrlDiv::Width  () const {return m_Width;}
inline UINT COkCtrlDiv::Height () const {return m_Height;}
inline bool COkCtrlDiv::IsEnabled () const {return m_bEnable;}


#endif    // _OK_CTRLDIV_
