/********************************************************************
* Project: FontGlyph
* Copyright (c) 2026, Oleg Kolbaskin.
* All rights reserved.
*
* Graphics Window
* Class CCoordSpace
*********************************************************************/
#include <memory.h>
#include "GW.h"

double CCoordSpace::g_ZoomCoef = 1.3;
double CCoordSpace::g_PanHCoef = 0.333;
double CCoordSpace::g_PanVCoef = 0.333;

//-----------------------------------------------
CCoordSpace::CCoordSpace ()
{
  m_MinPixSize = 0.0001;
  m_LimPixSize = 0.0;
  m_MaxLimRect.SetNull();
  SetPSRect( 0, 0, 500, 500, false );  // <window pixel>
  m_VisibleRect.Set( 0.0, 0.0, 100.0, 100.0 );  // <model units>
  SetExtRect( m_VisibleRect );
  m_ViewScale = 100.0 / 500.0;     // <model units> / <window pixel> = pixel size
  m_ViewScaleY = m_ViewScale;
  m_ViewX0 = 0.0;         
  m_ViewY0 = 100.0;
  m_ViewRect = m_VisibleRect;
  LevelZoomClear();
  memset( m_SView, 0, sizeof(m_SView) );
  m_NumSView = 0;
  m_bFrozenView = false;
}

//-----------------------------------------------
bool CCoordSpace::SetPSRect (INT_PTR Lef, INT_PTR Top, INT_PTR W, INT_PTR H, bool bUpdate)
{
  double Wlim, Hlim, Ratio, Rat2;

  if (10<=W && W<=1000000000 && 10<=H && H<=1000000000){
    m_WndLeft = (int)Lef;
    m_WndTop = (int)Top;
    m_WndWidth = (UINT)W;
    m_WndHeight = (UINT)H;
    if (m_MaxLimRect.IsNotNull()){
      Wlim = m_MaxLimRect.Width();
      Hlim = m_MaxLimRect.Height();
      Ratio = (double)m_WndWidth / (double)m_WndHeight;
      Rat2 = Wlim / Hlim;
      if (Ratio > Rat2){
        Wlim = Hlim * Ratio;
      }else{
        Hlim = Wlim / Ratio;
      }
      m_LimPixSize = Wlim / m_WndWidth;
    }
    if (bUpdate){
      UpdateCS();
    }
    return true;
  }
  return false;
}


//-----------------------------------------------
bool CCoordSpace::SetVisRect (const CGeRect& VisRect, bool bUpdate)
{
  CGeRect Rect;
  int W, H;

  if (m_bFrozenView){
    return false;
  }
  if (VisRect.IsNull()){
    // view extents
    Rect = m_ExtentsRect;
  }else{
    // rect width and height by pixels
    W = (int)(VisRect.Width() / PixelSize());
    H = (int)(VisRect.Height() / PixelSize());
    if (W<3 || H<3){
      // too small view
      return false;
    }
    Rect = VisRect;
  }
  if (Rect.IsNotNull()){
    m_VisibleRect = Rect;
    if (bUpdate){
      UpdateCS();
    }
    return true;
  }
  return false;
}

//-----------------------------------------------
void CCoordSpace::SetExtRect  (const CGeRect& Rect) 
{
  double PixSize, cfx, cfy;
  m_ExtentsRect = Rect;
  // correct m_MaxPixSize
  cfx = Rect.Width() / m_WndWidth;
  cfy = Rect.Height() / m_WndHeight;
  PixSize = __max( cfx, cfy );
  if (PixSize > 0.0){
    m_MaxPixSize = PixSize * CS_OVER_EXTRECT;
  }
}

//-----------------------------------------------
void CCoordSpace::SetLimRect (const CGeRect* pRect)
{
  double W, H, Ratio, Rat2;

  if (m_WndWidth==0 || m_WndHeight==0){
    goto m1;
  }
  if (pRect){
    m_MaxLimRect = *pRect;
    W = m_MaxLimRect.Width();
    H = m_MaxLimRect.Height();
  }else{
    goto m1;
  }
  if ((W > 0.0) && (H > 0.0)){
    Ratio = (double)m_WndWidth / (double)m_WndHeight;
    Rat2 = W / H;
    if (Ratio > Rat2){
      W = H * Ratio;
    }else{
      H = W / Ratio;
    }
    m_LimPixSize = W / m_WndWidth;
    UpdateCS();
  }else{
m1:
    m_MaxLimRect.SetNull();
    m_LimPixSize = 0.0;
  }
}

//-----------------------------------------------
bool CCoordSpace::SetMinPixSize (double Val)
{
  if (Val >= CS_MIN_PIXELSIZE){
    m_MinPixSize = Val;
    return true;
  }
  return false;
}


//-----------------------------------------------
// Must be always called after a view or window size was changed
// m_VisibleRect must be already defined
// This function defines m_ViewScale, m_ViewX0, m_ViewY0, m_ViewRect
// using m_VisibleRect
//-----------------------------------------------
void CCoordSpace::UpdateCS ()
{
  double   W, H;
  double   VisWidth, VisHeight, VisCenterX, VisCenterY;
  double   LimL, LimB, LimR, LimT, LimW, LimH, dx, dy;
  double   ViewL, ViewB, ViewR, ViewT, ViewW, ViewH;
  double   Delta = 0.0000001;
  CGePoint Pt0;

  VisWidth   = m_VisibleRect.Width();
  VisHeight  = m_VisibleRect.Height();
  VisCenterX = m_VisibleRect.Center().x;
  VisCenterY = m_VisibleRect.Center().y;
  m_ViewScale = __max( VisWidth / m_WndWidth, VisHeight / m_WndHeight );

  if ((m_ViewScale+Delta) < m_MinPixSize){
    m_ViewScale = m_MinPixSize;
    // correct m_VisibleRect
    VisWidth  = m_WndWidth * m_ViewScale;
    VisHeight = m_WndHeight * m_ViewScale;
    Pt0.Set( m_ViewX0, m_ViewY0 );
    m_VisibleRect.Set( Pt0, VisWidth, VisHeight );
  }else{
    if (m_LimPixSize > 0.0){
      if ((m_ViewScale-Delta) > m_LimPixSize){
        m_ViewScale = m_LimPixSize;
        // correct m_VisibleRect
        VisWidth  = m_WndWidth * m_ViewScale;
        VisHeight = m_WndHeight * m_ViewScale;
        Pt0.Set( m_ViewX0, m_ViewY0 );
        m_VisibleRect.Set( Pt0, VisWidth, VisHeight );
      }
    }
  }

  W = (m_WndWidth * m_ViewScale) / 2.0;
  H = (m_WndHeight * m_ViewScale) / 2.0;
  m_ViewX0 = VisCenterX - W;
  m_ViewY0 = VisCenterY + H;
  m_ViewRect.Set( m_ViewX0, VisCenterY-H, VisCenterX+W, m_ViewY0 );

  if (m_LimPixSize > 0.0){
    // correct view by limits
    LimL = m_MaxLimRect.Left();
    LimB = m_MaxLimRect.Bottom();
    LimR = m_MaxLimRect.Right();
    LimT = m_MaxLimRect.Top();
    LimW = m_MaxLimRect.Width();
    LimH = m_MaxLimRect.Height();
    ViewL = m_ViewRect.Left();
    ViewB = m_ViewRect.Bottom();
    ViewR = m_ViewRect.Right();
    ViewT = m_ViewRect.Top();
    ViewW = m_ViewRect.Width();
    ViewH = m_ViewRect.Height();
    dx = dy = 0.0;
    if (LimW <= ViewW){
      if (LimL < ViewL){
        dx = LimL - ViewL;
      }else{
        if (ViewR < LimR){
          dx = LimR - ViewR;
        }
      }
    }else{
      if (LimR < ViewR){
        dx = LimR - ViewR;
      }else{
        if (ViewL < LimL){
          dx = LimL - ViewL;
        }
      }
    }
    if (LimH <= ViewH){
      if (LimB < ViewB){
        dy = LimB - ViewB;
      }else{
        if (ViewT < LimT){
          dy = LimT - ViewT;
        }
      }
    }else{
      if (LimT < ViewT){
        dy = LimT - ViewT;
      }else{
        if (ViewB < LimB){
          dy = LimB - ViewB;
        }
      }
    }
    m_VisibleRect.Move( dx, dy );
    m_ViewX0 += dx;
    m_ViewY0 += dy;
    m_ViewRect.Move( dx, dy );
  }
  m_ViewScaleY = m_ViewScale; // * 1.5;
  OnUpdateCS();  // virtual 
}


//-----------------------------------------------
void CCoordSpace::GetPSRect (RECT* pRect) const
{
  pRect->left   = m_WndLeft;
  pRect->top    = m_WndTop;
  pRect->right  = m_WndLeft + m_WndWidth;
  pRect->bottom = m_WndTop  + m_WndHeight;
}

//-----------------------------------------------
void CCoordSpace::GetViewRect (CGeRect* pRect) const 
{
  *pRect = m_ViewRect;
}

//-----------------------------------------------
bool CCoordSpace::Contains (int X, int Y) const
{
  int Rig = m_WndLeft + m_WndWidth - 1;
  int Bot = m_WndTop + m_WndHeight - 1;
  if (m_WndLeft<=X && X<=Rig){
    if (m_WndTop<=Y && Y<=Bot){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------
bool CCoordSpace::Contains (const POINT& wpt) const
{
  int Rig = m_WndLeft + m_WndWidth - 1;
  int Bot = m_WndTop + m_WndHeight - 1;
  if (m_WndLeft<=wpt.x && wpt.x<=Rig){
    if (m_WndTop<=wpt.y && wpt.y<=Bot){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------
bool CCoordSpace::Contains (const CGePoint& Pnt) const
{
  return m_ViewRect.Contains( Pnt );
}
//-----------------------------------------------
bool CCoordSpace::Contains (double X, double Y) const
{
  return m_ViewRect.Contains( X, Y );
}


//-----------------------------------------------
void CCoordSpace::ZoomScale (double Scale, bool bUseBase, double Xbase, double Ybase)
{
  CGeRect  Rect;
  CGePoint VisCenter;
  double   VisWidth, VisHeight;
  double   dLef, dRig, dBot, dTop;
  double   Sc2;

  if (Scale > 0.0){
    Sc2 = m_ViewScale / Scale;
    if (Sc2 < m_MinPixSize){
      Scale = m_ViewScale / m_MinPixSize;
    }else{
      if (m_LimPixSize > 0.0){
        if (Sc2 > m_LimPixSize){
          Scale = m_ViewScale / m_LimPixSize;
        }
      }else{
        if (Sc2 > m_MaxPixSize){
          Scale = m_ViewScale / m_MaxPixSize;
        }
      }
    }
    Rect = m_VisibleRect;
    VisCenter = Rect.Center();
    VisWidth  = Rect.Width();
    VisHeight = Rect.Height();
    if (bUseBase == false){
      Xbase = VisCenter.x;
      Ybase = VisCenter.y;
    }
    dLef = Xbase - m_ViewRect.Left();
    dRig = m_ViewRect.Right() - Xbase;
    dBot = Ybase - m_ViewRect.Bottom();
    dTop = m_ViewRect.Top() - Ybase;
    dLef /= Scale;
    dRig /= Scale;
    dBot /= Scale;
    dTop /= Scale;
    Rect.Set( Xbase - dLef, Ybase - dBot, Xbase + dRig, Ybase + dTop );
    VisCenter = Rect.Center();
    VisWidth /= Scale;
    VisHeight /= Scale;
    Rect.Set( VisCenter, VisWidth, VisHeight );
    SetVisRect( Rect, true );
  }
}

//-----------------------------------------------
void CCoordSpace::ZoomRect (const CGeRect& Rect, bool bSaveZoomPrev)
{
  if (bSaveZoomPrev){
    SaveForZoomPrev();
  }
  SetVisRect( Rect, true );
}

//-----------------------------------------------
void CCoordSpace::ZoomPos (double Xc, double Yc, double PixSize)
{
  CGeRect Rect;
  double  W, H;
  if (PixSize < m_MinPixSize){
    // only set position, size remains the same
    Rect = m_VisibleRect;
    Rect.SetCenter( CGePoint(Xc,Yc) );
  }else{
    if (PixSize > m_MaxPixSize){
      // only set position, size remains the same
      Rect = m_VisibleRect;
      Rect.SetCenter( CGePoint(Xc,Yc) );
    }else{
      W = m_WndWidth * PixSize;
      H = m_WndHeight * PixSize;
      Rect.Set( CGePoint(Xc,Yc), W, H );
    }
  }
  SetVisRect( Rect, true );
}

//-----------------------------------------------
void CCoordSpace::Pan (double dx, double dy)
{
  CGeRect Rect = m_VisibleRect;
  Rect.Move( dx, dy );
  SetVisRect( Rect, true );
}

//-----------------------------------------------
void CCoordSpace::Pan (int Direction)
{
  double dx = m_ViewRect.Width() * g_PanHCoef;
  double dy = m_ViewRect.Height() * g_PanVCoef;
  switch( Direction ){
    case 0:  Pan( -dx, 0.0 ); break;   // LEFT
    case 1:  Pan( 0.0,  dy ); break;   // UP 
    case 2:  Pan(  dx, 0.0 ); break;   // RIGHT
    case 3:  Pan( 0.0, -dy ); break;   // DOWN
  }
}

//-----------------------------------------------
void CCoordSpace::ZoomExtentsCS (bool bWithBasePt)
{
  double dval;
  CGeRect Rect = m_ExtentsRect;
  if (Rect.IsNotNull()){
    Rect.Scale( 1.01, true );
    dval = m_LimPixSize;
    m_LimPixSize = 0.0;
    SetVisRect( Rect, true );
    m_LimPixSize = dval;
  }
}

//-----------------------------------------------
void CCoordSpace::ZoomLimits ()
{
  CGeRect Rect = m_MaxLimRect;
  if (Rect.IsNotNull()){
    SetVisRect( Rect, true );
  }
}

//-----------------------------------------------
void CCoordSpace::SetViewCenter (const CGePoint& Pnt)
{
  CGePoint Pt0 = m_ViewRect.Center();
  double dx = Pnt.x - Pt0.x;
  double dy = Pnt.y - Pt0.y;
  Pan( dx, dy );
}

//-----------------------------------------------
bool CCoordSpace::SetViewWidth (double W)
{
  double Scale;
  if (W > 0){
    Scale = m_ViewRect.Width() / W;
    ZoomScale( Scale );
    return true;
  }
  return false;
}

//-----------------------------------------------
bool CCoordSpace::SetViewHeight (double H)
{
  double Scale;
  if (H > 0){
    Scale = m_ViewRect.Height() / H;
    ZoomScale( Scale );
    return true;
  }
  return false;
}

//-----------------------------------------------
bool CCoordSpace::SetPixelSize (double Ps)
{
  double Scale;
  if (Ps > 0.0){
    Scale = m_ViewScale / Ps;
    ZoomScale( Scale );
    return true;
  }
  return false;
}

// used with scrollbars
//-----------------------------------------------
void CCoordSpace::Scroll (bool bVertical, UINT ScrollCode, int PDelta, bool bSmoothScroll)
{
  double LineStep = 16 * m_ViewScale;  // 16 pixels
  double PageStep = 64 * m_ViewScale;  // 64 pixels
  double Step;
  double PosDelta = PDelta;

  if (bVertical){
    switch( ScrollCode ){
      case SB_LINEUP:     Pan( 0.,  LineStep ); break;
      case SB_LINEDOWN:   Pan( 0., -LineStep ); break;
      case SB_PAGEUP:     Pan( 0.,  PageStep ); break;
      case SB_PAGEDOWN:   Pan( 0., -PageStep ); break;
      case SB_THUMBTRACK:
        if (!bSmoothScroll){
          break;
        }
      case SB_THUMBPOSITION:
        Step = -PosDelta * m_ViewScale;
        Pan( 0., Step );
        break;
      case SB_TOP:
        break;
      case SB_BOTTOM:
        break;
    }
  }else{
    switch( ScrollCode ){
      case SB_LINERIGHT:  Pan(  LineStep,  0. ); break;
      case SB_LINELEFT:   Pan( -LineStep,  0. ); break;
      case SB_PAGERIGHT:  Pan(  PageStep,  0. ); break;
      case SB_PAGELEFT:   Pan( -PageStep,  0. ); break;
      case SB_THUMBTRACK:
        if (!bSmoothScroll){
          break;
        }
      case SB_THUMBPOSITION:
        Step = PosDelta * m_ViewScale;
        Pan( Step,  0. );
        break;
      case SB_RIGHT:
        break;
      case SB_LEFT:
        break;
    }
  }
}

//-----------------------------------------------
bool CCoordSpace::LevelZoomIn (double Coef, const CGePoint* pCenPt)
{
  CGePoint Pnt;
  CGeRect  Rect;
  double   W, H;

  if ((2.0 <= Coef) && (Coef <= 100.0)){
    if (m_bLevelZoomIn == false){
      m_bLevelZoomIn = true;
      GetViewRect( &m_LevelZoomRect );
      if (pCenPt){
        Pnt = *pCenPt;
      }else{
        Pnt = m_LevelZoomRect.Center();
      }
      W = m_LevelZoomRect.Width() / Coef;
      H = m_LevelZoomRect.Height() / Coef;
      Rect.Set( Pnt, W, H );
      ZoomRect( Rect );
      return true;
    }
  }
  return false;
}

//-----------------------------------------------
bool CCoordSpace::LevelZoomOut ()
{
  if (m_bLevelZoomIn){
    ZoomRect( m_LevelZoomRect );
    LevelZoomClear();
    return true;
  }
  return false;
}

//-----------------------------------------------
void CCoordSpace::LevelZoomClear ()
{
  m_LevelZoomRect.SetNull();
  m_bLevelZoomIn = false;
}


//-----------------------------------------------
void CCoordSpace::ZoomPrevious ()
{
  INT_PTR i;
  CGeRect Rect;

  if (m_NumSView > 0){
    i = --m_NumSView;
    Rect = m_SView[i];
    SetVisRect( Rect, true );
  }
}


//-----------------------------------------------
void CCoordSpace::SaveForZoomPrev (int Mode)
{
  INT_PTR i;
  static int b2=0;

  if (m_bLevelZoomIn){
    return;
  }
  if (m_NumSView > 0){
    if (Mode != 0){
      if (Mode == b2){
        return;
      }
      b2 = Mode;
    }
    i = m_NumSView - 1;
    if (m_SView[i] == m_ViewRect){
      // the same view as the last added, don't add it
      return;
    }
  }

  i = -1;
  if (m_NumSView < CS_MAX_VIEWPREV){
    ++m_NumSView; 
    i = m_NumSView - 1;
  }else{
    if (m_NumSView == CS_MAX_VIEWPREV){
      // no free place to add view, shift the array to beginning
      // make free the last item
      for (i=1; i<(int)m_NumSView; ++i){
        m_SView[i-1] = m_SView[i];
      }
      i = m_NumSView - 1;
    }
  }
  if (i >= 0){
    m_SView[i] = m_ViewRect;
  }
  if (Mode == 0){
    b2 = 0;
  }
}

//-----------------------------------------------
void CCoordSpace::ClearZoomPrev ()
{
  memset( m_SView, 0, sizeof(m_SView) );
  m_NumSView = 0;
}


