/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class CGeRect implementation
********************************************************************/
#include "Geom.h"

double g_GeTolerance  = 0.0000001;
double g_GeTolClass   = 0.0000001;   // tolerance for CGeLine::ClassifyLine() and CGeLine::ClassifyPoint()
double g_GeMiterLimit = 10.0;

bool CGePolyline::m_bNoDuplicate = true;


//-----------------------------------------------
void CGeRect::Set (const CGePoint& Center, double Width, double Height)
{
  double dx = Width / 2.0;
  double dy = Height / 2.0;
  m_Left   = Center.x - dx;
  m_Bottom = Center.y - dy;
  m_Right  = Center.x + dx;
  m_Top    = Center.y + dy;
}

//-----------------------------------------------
void CGeRect::Set (const CGePoint& Center, double Delta)
{
  m_Left   = Center.x - Delta;
  m_Bottom = Center.y - Delta;
  m_Right  = Center.x + Delta;
  m_Top    = Center.y + Delta;
}

//-----------------------------------------------
bool CGeRect::SetWidth (double w, bool bCenter)
{
  double xc, w2;
  if (w > 0.0){
    if (bCenter){
      xc = (m_Right + m_Left) / 2.0;
      w2 = w / 2.0;
      m_Left = xc - w2;
      m_Right = xc + w2;
    }else{
      m_Right = m_Left + w;
    }
    return true;
  }
  return false;
}

//-----------------------------------------------
bool CGeRect::SetHeight (double h, bool bCenter)
{
  double yc, h2;
  if (h > 0.){
    if (bCenter){
      yc = (m_Top + m_Bottom) / 2.0;
      h2 = h / 2.0;
      m_Bottom = yc - h2;
      m_Top = yc + h2;
    }else{
      m_Top = m_Bottom + h;
    }
    return true;
  }
  return false;
}

//-----------------------------------------------
void CGeRect::SetCenter (const CGePoint& CenPt)
{
  CGePoint Pt0 = Center();
  double   dx = CenPt.x - Pt0.x;
  double   dy = CenPt.y - Pt0.y;
  m_Left   += dx;
  m_Bottom += dy;
  m_Right  += dx;
  m_Top    += dy;
}

//-----------------------------------------------
void CGeRect::SetSize (double W, double H)
{
  CGePoint Pt0 = Center();
  double   dx = W / 2.0;
  double   dy = H / 2.0;
  m_Left   = Pt0.x - dx;
  m_Bottom = Pt0.y - dy;
  m_Right  = Pt0.x + dx;
  m_Top    = Pt0.y + dy;
}

//-----------------------------------------------
void CGeRect::Normalize ()
{
  double val;
  if (m_Left > m_Right){
    val = m_Left;
    m_Left = m_Right;
    m_Right = val;
  }
  if (m_Bottom > m_Top){
    val = m_Bottom;
    m_Bottom = m_Top;
    m_Top = val;
  }
}


//-----------------------------------------------
void CGeRect::GetPoints (CGePoint* pPnt) const
{
  pPnt[0] = LefBot();
  pPnt[1] = LefTop();
  pPnt[2] = RigTop();
  pPnt[3] = RigBot();
}

//-----------------------------------------------
void CGeRect::Scale (double Coef, bool bByCenter)
{
  CGePoint CenPt;
  double   dx, dy;

  if (Coef > 0.0){
    if (bByCenter){
      CenPt = Center();
      dx = (Width() / 2.0) * Coef;
      dy = (Height() / 2.0) * Coef;
      m_Left   = CenPt.x - dx;
      m_Bottom = CenPt.y - dy;
      m_Right  = CenPt.x + dx;
      m_Top    = CenPt.y + dy;
    }else{
      m_Left *= Coef;
      m_Bottom *= Coef;
      m_Right *= Coef;
      m_Top *= Coef;
    }
  }
}

//-----------------------------------------------
CGeRect CGeRect::ScaleBy (double Coef, bool bByCenter) const
{
  CGeRect Rect = *this;
  Rect.Scale( Coef, bByCenter );
  return Rect;
}

//-----------------------------------------------
CGeRect CGeRect::MoveBy  (double dx, double dy) const
{
  CGeRect Rect;
  Rect.m_Left   = m_Left + dx; 
  Rect.m_Bottom = m_Bottom + dy; 
  Rect.m_Right  = m_Right + dx; 
  Rect.m_Top    = m_Top + dy;
  return Rect;
}

//-----------------------------------------------
bool CGeRect::Contains (const CGePoint& Pnt, double Delta) const
{
  static double L, B, R, T;
  L = m_Left - Delta; 
  B = m_Bottom - Delta; 
  R = m_Right + Delta; 
  T = m_Top + Delta;
  if ((L < Pnt.x) && (Pnt.x < R) && (B < Pnt.y) && (Pnt.y < T)){
    return true;
  }
  return false;
}

//-----------------------------------------------
bool CGeRect::NotContains (const CGePoint& Pnt, double Delta) const
{
  static double L, B, R, T;
  L = m_Left - Delta; 
  B = m_Bottom - Delta; 
  R = m_Right + Delta; 
  T = m_Top + Delta;
  if ((L < Pnt.x) && (Pnt.x < R) && (B < Pnt.y) && (Pnt.y < T)){
    return false;
  }
  return true;
}







