/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* Graphics Window
********************************************************************/
#pragma once
#include "framework.h"


#define CS_MIN_PIXELSIZE  0.0000000001
#define CS_MAX_VIEWPREV   50      // max num of previous views to save
#define CS_OVER_EXTRECT   100.0   // coefficient for m_MaxPixSize calculation

// Point modes ( lcPaint_DrawPoint, lcBlockAddPoint2, LC_PROP_DRW_PDMODE )
#define LC_POINT_PIXEL        0     // Pixel
#define LC_POINT_NONE         1     // None
#define LC_POINT_PLUS         2     // "Plus" sign through the point.
#define LC_POINT_X            3     // "X" through the point
#define LC_POINT_TICK         4     // Vertical tick mark upward from the point.
// additional elements     
#define LC_POINT_CIRCLE       32    // Circle 
#define LC_POINT_SQUARE       64    // Square
#define LC_POINT_RHOMB        128   // Rhomb
#define LC_POINT_FILLED       256   // Filled shape


//=============================================================================
// coordinate convertions between model space (MS) and pixel window space (PS)
//=============================================================================
class CCoordSpace {
  int      m_WndLeft;         // left of PS
  int      m_WndTop;          // top of PS 
  UINT     m_WndWidth;        // width of PS
  UINT     m_WndHeight;       // height of PS
  CGeRect  m_VisibleRect;     // visible rect of a drawing (MS)
  CGeRect  m_ExtentsRect;     // extents rect of a drawing (MS)
  double   m_MinPixSize;
  double   m_MaxPixSize;      // depends on m_ExtentsRect, forbid to zoom more then CS_OVER_EXTRECT*m_ExtentsRect
  double   m_LimPixSize;      // used if m_MaxLimRect is valid
  CGeRect  m_MaxLimRect;
  // calculated view parameters, to fit m_VisibleRect to PS window
  double   m_ViewScale;       // <model units> / <window pixel> = pixel size
  double   m_ViewScaleY;      // <model units> / <window pixel> = pixel size
  double   m_ViewX0;          // view point which match left-top corner of PS
  double   m_ViewY0;
  CGeRect  m_ViewRect;        // MS view rect which match the PS rect
  // view history (for ZoomPrevious)
  CGeRect  m_SView[CS_MAX_VIEWPREV];   // array of saved views    
  UINT     m_NumSView;        // num of saved views
  // level zoom
  CGeRect  m_LevelZoomRect;
  bool     m_bLevelZoomIn;
protected:
  bool    m_bFrozenView;      // if true - can't change view rect
public:
  static double  g_ZoomCoef;  // coefficient for Zoom in/out
  static double  g_PanHCoef;  // coefficient for Pan left/right
  static double  g_PanVCoef;  // coefficient for Pan up/down

public:
    CCoordSpace();

  // set data
  bool SetPSRect     (INT_PTR Lef, INT_PTR Top, INT_PTR Width, INT_PTR Height, bool bUpdate);
  bool SetVisRect    (const CGeRect& VisRect, bool bUpdate);
  void SetExtRect    (const CGeRect& Rect);
  void SetLimRect    (const CGeRect* pRect);
  bool SetMinPixSize (double Val);

  // convert coordinates between MS (View) and PS (Wnd)
  void CoordViewToWnd (double X, double Y, POINT* pPwnd) const;
  void CoordViewToWnd (double X, double Y, int* pXwnd, int* pYwnd) const;
  void CoordViewToWnd (const CGePoint& Pnt, POINT* pPwnd) const;
  void CoordWndToView (int X, int Y, double* pX, double* pY) const;
  void CoordWndToView (const POINT& Pwnd, double* pX, double* pY) const;
  void CoordWndToView (const POINT& Pwnd, CGePoint* pPnt) const;

  // get properties
  void     GetPSRect      (RECT* pRect) const;
  int      PSLeft         () const;
  int      PSTop          () const;
  UINT     PSWidth        () const;
  UINT     PSHeight       () const;
  void     GetVisibleRect (CGeRect* pRect) const;
  void     GetExtRect     (CGeRect* pRect) const;
  void     GetViewRect    (CGeRect* pRect) const;
  void     GetLimRect     (CGeRect* pRect) const;
  bool     HasLimRect     () const;
  CGePoint ViewCenter     () const;
  double   ViewHeight     () const;
  double   PixelSize      () const; 
  double   ViewScale      () const; // same as PixelSize()
  bool     Contains  (int X, int y) const;
  bool     Contains  (const POINT& wpt) const;
  bool     Contains  (const CGePoint& Pnt) const;
  bool     Contains  (double X, double Y) const;
 
  // service function to change a MS view in current PS
  void  ZoomIn        (bool bUseBase=false, double Xbase=0.0, double Ybase=0.0);
  void  ZoomOut       (bool bUseBase=false, double Xbase=0.0, double Ybase=0.0);
  void  ZoomScale     (double Scale, bool bUseBase=false, double Xbase=0.0, double Ybase=0.0);
  void  ZoomRect      (const CGeRect& Rect, bool bSaveZoomPrev=false);
  void  ZoomRect      (const CGePoint& Pt0, const CGePoint& Pt1, bool bSaveZoomPrev=false);
  void  ZoomRect      (double Lef, double Bot, double Rig, double Top, bool bSaveZoomPrev=false);
  void  ZoomMove      (double dx, double dy);
  void  ZoomPos       (double Xc, double Yc, double PixSize);
  void  Pan           (double dx, double dy);
  void  Pan           (int dx, int dy);
  void  Pan           (int Direction);
  void  ZoomExtentsCS (bool bWithBasePt);
  void  SetViewCenter (const CGePoint& Pnt);
  bool  SetViewWidth  (double W);
  bool  SetViewHeight (double H);
  bool  SetPixelSize  (double Ps);
  void  Scroll        (bool bVertical, UINT ScrollCode, int PosDelta, bool bSmoothScroll);
  bool  LevelZoomIn   (double Coef, const CGePoint* pCenPt);
  bool  LevelZoomIn   () const;
  bool  LevelZoomOut  ();
  void  LevelZoomClear();
  void  ZoomLimits    ();

  // Zoom Previous
  void  ZoomPrevious ();
  void  SaveForZoomPrev (int Mode=0);
  void  ClearZoomPrev ();
  bool  HasZoomPrev () const;

  // called from Update()
  virtual void OnUpdateCS () = 0;

private:
  void UpdateCS ();
};

inline int      CCoordSpace::PSLeft      () const {return m_WndLeft;}
inline int      CCoordSpace::PSTop       () const {return m_WndTop;}
inline UINT     CCoordSpace::PSWidth     () const {return m_WndWidth;}
inline UINT     CCoordSpace::PSHeight    () const {return m_WndHeight;}
inline void     CCoordSpace::GetVisibleRect (CGeRect* pRect) const {*pRect = m_VisibleRect;}
inline void     CCoordSpace::GetExtRect  (CGeRect* pRect) const {*pRect = m_ExtentsRect;}
inline void     CCoordSpace::GetLimRect  (CGeRect* pRect) const {*pRect = m_MaxLimRect;}
inline bool     CCoordSpace::HasLimRect  () const {return (m_LimPixSize>0.0)?true:false;}
inline CGePoint CCoordSpace::ViewCenter  () const {return m_ViewRect.Center();}
inline double   CCoordSpace::ViewHeight  () const {return m_ViewRect.Height();}
inline double   CCoordSpace::PixelSize   () const {return m_ViewScale;}
inline double   CCoordSpace::ViewScale   () const {return m_ViewScale;}
inline bool     CCoordSpace::HasZoomPrev () const {return m_NumSView?true:false;}
inline bool     CCoordSpace::LevelZoomIn () const {return m_bLevelZoomIn;}

inline void CCoordSpace::CoordViewToWnd (double X, double Y, POINT* pPwnd) const {
  pPwnd->x = m_WndLeft + (LONG)((X - m_ViewX0) / m_ViewScale + 0.5);
  pPwnd->y = m_WndTop  + (int)((m_ViewY0 - Y) / m_ViewScaleY + 0.5);
}
inline void CCoordSpace::CoordViewToWnd (double X, double Y, int* pXwnd, int* pYwnd) const {
  *pXwnd = m_WndLeft + (LONG)((X - m_ViewX0) / m_ViewScale + 0.5);
  *pYwnd = m_WndTop  + (int)((m_ViewY0 - Y) / m_ViewScaleY + 0.5);
}
inline void CCoordSpace::CoordViewToWnd (const CGePoint& Pnt, POINT* pPwin) const {
  pPwin->x = m_WndLeft + (int)((Pnt.x - m_ViewX0) / m_ViewScale + 0.5);
  pPwin->y = m_WndTop  + (int)((m_ViewY0 - Pnt.y) / m_ViewScaleY + 0.5);
}
inline void CCoordSpace::CoordWndToView (int Xwnd, int Ywnd, double* pX, double* pY) const {
  *pX = (((double)Xwnd - m_WndLeft) * m_ViewScale) + m_ViewX0;
  *pY = m_ViewY0 - ((double)Ywnd - m_WndTop) * m_ViewScaleY;
}
inline void CCoordSpace::CoordWndToView (const POINT& Pwin, double* pX, double* pY) const {
  *pX = (((double)Pwin.x - m_WndLeft) * m_ViewScale) + m_ViewX0;
  *pY = m_ViewY0 - ((double)Pwin.y - m_WndTop) * m_ViewScaleY;
}
inline void CCoordSpace::CoordWndToView (const POINT& Pwin, CGePoint* pPnt) const {
  pPnt->x = (((double)Pwin.x - m_WndLeft) * m_ViewScale) + m_ViewX0;
  pPnt->y = m_ViewY0 - (((double)Pwin.y - m_WndTop) * m_ViewScaleY);
}

inline void CCoordSpace::ZoomIn (bool bUseBase, double Xbase, double Ybase){
  ZoomScale( g_ZoomCoef, bUseBase, Xbase, Ybase );
}
inline void CCoordSpace::ZoomOut (bool bUseBase, double Xbase, double Ybase){
  ZoomScale( 1.0/g_ZoomCoef, bUseBase, Xbase, Ybase );
}
inline void CCoordSpace::ZoomRect (const CGePoint& Pt0, const CGePoint& Pt1, bool bSaveZoomPrev){
  ZoomRect( CGeRect(Pt0,Pt1), bSaveZoomPrev );
}
inline void CCoordSpace::ZoomRect (double Lef, double Bot, double Rig, double Top, bool bSaveZoomPrev){
  ZoomRect( CGeRect(Lef,Bot,Rig,Top), bSaveZoomPrev );
}
inline void CCoordSpace::ZoomMove (double dx, double dy) {
  Pan( dx, dy );
}
inline void CCoordSpace::Pan (int dx, int dy) {
  Pan( dx*m_ViewScale, dy*m_ViewScaleY );
}



typedef int (CALLBACK* F_GWEVENT) (int EventType, HANDLE Prm1);

// GW event types 
#define GW_EVENT_PAINT      1
#define GW_EVENT_MOUSEMOVE  2
#define GW_EVENT_RBDOWN     3
#define GW_EVENT_SNAP       4

//-----------------------------------------------------------------------------
// LcWindow
//-----------------------------------------------------------------------------
class LcWindow : public CCoordSpace {
  HINSTANCE   m_hInst;            // module instance
  HWND        m_hWnd;             // window handler
  POINT       m_FrameOrig;        // window origin in screen coordinates (left, top)
  SIZE        m_FrameSize;        // window height and width in screen coordinates
  UINT        m_WndStyle;         // GWL_STYLE window style
  HBITMAP     m_hWndBitmap;       // OnPaint bitmap
  HBITMAP     m_hWndBitmap2;      // used to clone m_hWndBitmap
  BITMAPINFO  m_Bmi;              // BITMAPINFO of m_hWndBitmap
  RGBQUAD     m_Colors[256];
  BYTE*       m_Bits;             // bits of m_hWndBitmap
  HDC         m_hPaintDC;

  // parameters used to work with window scrollers 
  int         m_ScrollRangeX;
  int         m_ScrollRangeY;
  int         m_ScrollUnitX;
  int         m_ScrollUnitY;
  int         m_ScrollPosX;
  int         m_ScrollPosY;
  bool        m_bSteadyScrollBars;   // if true - scrollbars always exist, if false - only if within drawing's extents

  CGePoint    m_CursorPoint;       // cursor coordinates (Model Space)
  POINT       m_CursorWPt;         // cursor PS coordinates
  POINT       m_MovePrevWPt;       // prev. point of a cursor's movement
  CGePoint    m_ClickPoint;        // point coordinates (MS) of last LButtonDown
  POINT       m_ClickWPt;          // cursor PS coordinates of last LButtonDown
//  WCHAR       m_szCursorText[256]; // cursor tooltip text
  bool        m_bHasFocus;
  bool        m_bMouseLeave;       // TRUE if cursor is out of window
  bool        m_bPanMode;          // TRUE while user is moving view in a window 
  int         m_ZoomFrame;         // command Zoom by drawn rectangle
  CGePoint    m_ZoomFramePt0;      // first point of zoom frame

  F_GWEVENT m_pEventProc;      // event procedure
  HANDLE    m_EventPrm1;       // event parameter 1

public:
  HCURSOR   m_hCurMain;     // main cursor
  COLORREF  m_ColorBack;         // window background
  COLORREF  m_ColorCursor;       // window cursor
  COLORREF  m_ColorFore;         // foreground color
  bool      m_bShowSysCursor;    // visibility of an arrow cursor (Windows defined)
  bool      m_bShowCrosshair;    // visibility of a crosshair cursor
  int       m_CrossSize;         // size of a crosshair cursor, % of window, if negative - size in pixels
  bool      m_bShowPickbox;      // visibility of cursor pickbox
  int       m_PickBoxSize;       // half-size of a selecting square
  bool      m_bDrawCenter;       // crosshair at the center of a window
  int       m_IntVal;            // application defined integer value

  static HCURSOR  g_hCurPan1;     // pan start cursor
  static HCURSOR  g_hCurPan2;     // pan move cursor
  static HCURSOR  g_hCurZoom;     // realtime zoom cursor
  static HCURSOR  g_hCurArrow;    // Standard arrow
  static HCURSOR  g_hCurCross;    // Crosshair
  static HCURSOR  g_hCurFinger;   // Hand
  static HCURSOR  g_hCurHelp;     // Arrow and question mark
  static HCURSOR  g_hCurNO;       // Slashed circle
  static HCURSOR  g_hCurWait;     // Hourglass

  static int   g_PanStep;         // Minimal step, pixels
  static int   g_PickBoxSize;     // half-size of a selecting square
  static bool  g_bPntPixSize;     // (for negative point size) if TRUE then point size is measured in pixels, FALSE - % of window height or g_PntWndHeight
  static UINT  g_PntWndHeight;    // if not 0.0 then used instead of window height to calculete point size (g_bPntPixSize==false)

public:
    LcWindow (HINSTANCE hInst, HWND hWndParent, int Style);
    ~LcWindow ();

  UINT  Width   () const;
  UINT  Height  () const;
  HWND  GetHWND () const;
  HDC   GetDC   () const;

  bool WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void Resize          (int x, int y, int Width, int Height);
  void SetFocus        () const;
  void UpdateWindow    () const;
  void UpdateCursor    () const;
  bool SetMainCursor   (HCURSOR hCur);
  void Invalidate      () const;
  void RePaint         (HDC hDC=0) const;
  void ZoomExtents     (bool bWithBasePt=false);
  void ZoomFrame       ();
  void UpdateScrollers ();
  bool HasFocus        () const;
  bool IsPanning       () const;
  void GetCursorCoords (POINT* pWinPt, CGePoint* pDrwPt) const;
  void GetClickCoords  (POINT* pWinPt, CGePoint* pDrwPt) const;
  void GetCursorRect   (CGeRect* pRect) const;
  void SetCursorCoords (const CGePoint& Pnt);

  void DrawCursor     (HDC hDC, int x, int y, WPARAM Flags, COLORREF Color);
  void DrawCrosshair  (HDC hDC);
  void DrawWndCenter  (HDC hDC, COLORREF Color);

  void DrawPixel    (HDC hDC, double X, double Y, COLORREF Color) const;
  void DrawBox      (HDC hDC, double X, double Y, COLORREF Color) const;
  void DrawPoint    (HDC hDC, const CGePoint& Pos, int Mode, double Size, HBRUSH hBrush=0) const;
  void DrawDot      (HDC hDC, const CGePoint& Pos) const;
  void DrawDot      (HDC hDC, double X, double Y) const;
  void DrawLine     (HDC hDC, double x1, double y1, double x2, double y2) const;
  void DrawLine     (HDC hDC, const CGePoint& Pt1, const CGePoint& Pt2) const;
  void DrawRect     (HDC hDC, double Lef, double Bot, double Rig, double Top) const;
  void DrawRect     (HDC hDC, const CGeRect& Rect) const;
  bool DrawXline    (HDC hDC, const CGePoint& Pt0, double Angle, bool bRay) const;
  void DrawPolylines  (HDC hDC, const CListPlines& Glyph, bool bFill, bool bContour);

  void SetEventProc (F_GWEVENT pProc, HANDLE Prm1);

private:
  void OnPaint         ();
  void OnMouseMove     (WPARAM Flags, short x, short y, bool bOnPaint=false);
  void OnMouseLeave    ();
  void OnMouseWheel    (WORD Flags, short zDelta, short x, short y);
  void OnLButtonDown   (WPARAM Flags, short x, short y);
  void OnLButtonUp     (WPARAM Flags, short x, short y);
  void OnLButtonDblClk (WPARAM Flags, short x, short y);
  void OnRButtonDown   (WPARAM Flags, short x, short y);
  void OnRButtonUp     (WPARAM Flags, short x, short y);
  void OnMButtonDown   (WPARAM Flags, short x, short y);
  void OnMButtonUp     (WPARAM Flags, short x, short y);
  void OnMButtonDblClk (WPARAM Flags, short x, short y);
  void OnKeyDown       (WPARAM VirtKey, LPARAM Flags);
  void OnHScroll       (WORD SBCode, WORD Pos);
  void OnVScroll       (WORD SBCode, WORD Pos);
  void OnSetFocus      ();
  void OnKillFocus     ();
  void CreateWndBitmap ();
  void PanRT_Start (short x, short y);
  void PanRT_End   ();
  void InvalidateClickPoint ();
  virtual void OnUpdateCS ();  // from class CCoordSpace
};

inline HWND LcWindow::GetHWND () const {return m_hWnd;}
inline HDC  LcWindow::GetDC () const {return m_hPaintDC;}
inline UINT LcWindow::Width   () const {return PSWidth();}
inline UINT LcWindow::Height  () const {return PSHeight();}
inline bool LcWindow::HasFocus   () const {return m_bHasFocus;}
inline bool LcWindow::IsPanning  () const {return m_bPanMode;}
inline void LcWindow::DrawRect (HDC hDC, const CGeRect& Rect) const {
  DrawRect( hDC, Rect.Left(), Rect.Bottom(), Rect.Right(), Rect.Top() );
}



// Draw a dot
void okDrawDot (HDC hDC, POINT Pt0);

// Draw a cross
void okDrawCross (HDC hDC, POINT Pt0, UINT HalfSize);

// Draw an X cross
void okDrawX (HDC hDC, POINT Pt0, UINT HalfSize);

// Draw a square
void okDrawSquare (HDC hDC, POINT Pt, UINT HalfSize, bool bFill=false);

// Draw a rhomb
void okDrawRhomb (HDC hDC, POINT Pt0, UINT HalfSize, bool bFill=false);

// Draw a circle
void okDrawCircle (HDC hDC, POINT Pt0, UINT R, bool bFill=false);

// Draw a rectangle
void okDrawRect (HDC hDC, const RECT& Rect, bool bFill=false);

// Draw a rectangle by width line
void okDrawWidRect (HDC hDC, const RECT& Rect);

// Draw a Text (instead of ::TextOut)
void okDrawText (HDC hDC, int x, int y, LPCWSTR szText);

void okDrawArrow (HDC hDC, POINT Ptw, double Ang, int Len, int HalfWidth);
void okDrawArrow (HDC hDC, POINT Ptw0, POINT Ptw1, int Len, int HalfWidth);
