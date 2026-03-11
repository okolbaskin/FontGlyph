/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class CGeLine implementation
********************************************************************/
#include "Geom.h"

static double   _Ax, _Ay, _Bx, _By, _sa;
static double   _Nx, _Ny, _denom, _num, _dval;
static EnPntPos _aclass;

/********************************************************************
* Classify position of point relative to line
*  Pnt - point
* Returns:
*  GE_LEFT  - point is at left side of m_Start-m_End
*  GE_RIGHT - point is at right side of m_Start-m_End
*    Next codes shows that point is on m_Start-m_End direction
*  GE_BEFORE - before m_Start
*  GE_AFTER - after m_End
*  GE_ORIGIN - exactly at m_Start
*  GE_DESTINATION - exactly at m_End
*  GE_BETWEEN - between m_Start and m_End
*********************************************************************/
EnPntPos CGeLine::ClassifyPoint (const CGePoint& Pnt, double LefRigTol) const
{
  if (Pnt.IsEqual( m_Start, g_GeTolerance )){
    return EnPntPos::GE_ORIGIN;
  }
  if (Pnt.IsEqual( m_End, g_GeTolerance )){
    return EnPntPos::GE_DESTINATION;
  }
  _Ax = m_End.x - m_Start.x;
  _Ay = m_End.y - m_Start.y;
  _Bx = Pnt.x - m_Start.x;
  _By = Pnt.y - m_Start.y;
  _sa = (_Ax*_By) - (_Bx*_Ay);
  if (_sa > LefRigTol){
    return EnPntPos::GE_LEFT;
  }
  if (_sa < -LefRigTol){
    return EnPntPos::GE_RIGHT;
  }
  if (((_Ax*_Bx) < 0.0) || ((_Ay*_By) < 0.0)){
    return EnPntPos::GE_BEFORE;
  }
  if (sqrt((_Ax*_Ax) + (_Ay*_Ay)) < sqrt((_Bx*_Bx) + (_By*_By))){
    return EnPntPos::GE_AFTER;
  }
  return EnPntPos::GE_BETWEEN;
}

//-----------------------------------------------
EnPntPos CGeLine::ClassifyPoint2 (const CGePoint& Pnt, double Delta) const
{
  double   Len, Ang, Y;
  CGePoint Pt2;

  if (Pnt.IsEqual( m_Start, Delta )){
    return EnPntPos::GE_ORIGIN;
  }
  if (Pnt.IsEqual( m_End, Delta )){
    return EnPntPos::GE_DESTINATION;
  }
  Ang = m_Start.Angle( m_End );
  Len = m_Start.Distance( m_End );
  Pt2 = Pnt.RotateBy( m_Start, -Ang );
  Y = Pt2.y - m_Start.y;
  if (Delta >= Y){
    if (Y >= -Delta){
      if (Pt2.x < m_Start.x){
        return EnPntPos::GE_BEFORE;
      }
      if (Pt2.x > (m_Start.x + Len)){
        return EnPntPos::GE_AFTER;
      }
      return EnPntPos::GE_BETWEEN;
    }else{
      return EnPntPos::GE_RIGHT;
    }
  }else{
    return EnPntPos::GE_LEFT;
  }
}

/********************************************************************
* Classify position between 2 Infinite Lines
*  Line2 - second line
*  T - (output) parametric coefficient of intersect. point's
*      position along this line (A-0 B-1)
* Returns:
*  GE_PARALLEL - the lines are parallel (T is undefined)
*  GE_COLLINEAR - the lines are at one line direction (T is undefined)
*  GE_SKEW - the lines cross with angle (have intersect. point)
*********************************************************************/
EnIntersec CGeLine::ClassifyLine (const CGeLine& Line2, double* pT) const
{
  _Nx = Line2.m_End.y - Line2.m_Start.y;
  _Ny = Line2.m_Start.x - Line2.m_End.x;
  _denom = (_Nx * (m_End.x - m_Start.x)) + (_Ny * (m_End.y - m_Start.y));
  if ((-g_GeTolClass <= _denom) && (_denom <= g_GeTolClass)){
    // classify position of point A relative to Line2
    _aclass = Line2.ClassifyPoint( m_Start );
    if (_aclass == EnPntPos::GE_LEFT || _aclass == EnPntPos::GE_RIGHT){
      return EnIntersec::GE_PARALLEL;
    }else{
      return EnIntersec::GE_COLLINEAR;
    }
  }
  _num = (_Nx * (m_Start.x - Line2.m_Start.x)) + (_Ny * (m_Start.y - Line2.m_Start.y));
  if (pT){
    *pT = -_num / _denom;
  }
  return EnIntersec::GE_SKEW;
}

/********************************************************************
* Get relative position between 2 lines
*  Line2 - other line
*  pInterPt - (out) intersection point, 
*         if NULL - the point will not be calculated
* Returns:
*  GE_PARALLEL      - the lines are parallel (pInterPt is undefined)
*  GE_COLLINEAR     - the lines are at one line direction (pInterPt is undefined)
*  GE_SKEW_NO_CROSS - intersect. point is outside of lines 
*  GE_SKEW_CROSS    - lines has intersect. point inside of both lines
*********************************************************************/
EnIntersec CGeLine::GetIntersection (const CGeLine& Line2, CGePoint* pInterPt, CGePoint* pNextPt, double* pRightAngle, double Zero1, double One1, double Zero2, double One2) const
{
  double t, ang, ang2;
  EnIntersec crossType;

  crossType = ClassifyLine( Line2, &t );
  if (crossType == EnIntersec::GE_PARALLEL){
    if (pInterPt){
      pInterPt->x = pInterPt->y = 0.0;
    }
    return EnIntersec::GE_PARALLEL;  // there are no intersect. point
  }

  if (m_End.IsEqual( Line2.m_Start, g_GeTolerance )){
    ang = m_Start.RightAngle( m_End, Line2.m_End );
    if (pRightAngle){
      *pRightAngle = ang;
    }
    if (pInterPt){
      *pInterPt = m_End;
    }
    if (pNextPt){
      *pNextPt = Line2.m_End;
    }
    if (crossType == EnIntersec::GE_COLLINEAR){
      if ((GE_DEG180-0.01 < ang) && (ang < GE_DEG180+0.01)){
        return EnIntersec::GE_SKEW_CROSS;
      }
      return EnIntersec::GE_COLLINEAR;  // there are no intersect. point
    }
    return EnIntersec::GE_SKEW_CROSS;
  }

  if (m_End.IsEqual( Line2.m_End, g_GeTolerance )){
    ang = m_Start.RightAngle( m_End, Line2.m_Start );
    if (pRightAngle){
      *pRightAngle = ang;
    }
    if (pInterPt){
      *pInterPt = m_End;
    }
    if (pNextPt){
      *pNextPt = Line2.m_Start;
    }
    if (crossType == EnIntersec::GE_COLLINEAR){
      if ((GE_DEG180-0.01 < ang) && (ang < GE_DEG180+0.01)){
        return EnIntersec::GE_SKEW_CROSS;
      }
      return EnIntersec::GE_COLLINEAR;  // there are no intersect. point
    }
    return EnIntersec::GE_SKEW_CROSS;
  }
  if (crossType == EnIntersec::GE_COLLINEAR){
    return EnIntersec::GE_COLLINEAR;  // there are no intersect. point
  }

  if (pInterPt){
    // intersection point
    pInterPt->x = m_Start.x + (t * (m_End.x - m_Start.x));
    pInterPt->y = m_Start.y + (t * (m_End.y - m_Start.y));
  }
  if ((t < Zero1) || (t > One1)){
    return EnIntersec::GE_SKEW_NO_CROSS;  // intersect. point is outside of lines 
  }
  Line2.ClassifyLine( *this, &t );
  if ((Zero2 <= t) && (t <= One2)){
    if (pNextPt){
      if (t < 0.0001){
        ang = GE_DEG360;
      }else{
        if (pInterPt){
          ang = m_Start.RightAngle( *pInterPt, Line2.m_Start );
        }else{
          ang = 0.0;
        }
      }
      if (t > 0.9999){
        ang2 = GE_DEG360;
      }else{
        if (pInterPt){
          ang2 = m_Start.RightAngle( *pInterPt, Line2.m_End );
        }else{
          ang2 = 0.0;
        }
      }
      if (ang < ang2){
        *pNextPt = Line2.m_Start;
        if (pRightAngle){
          *pRightAngle = ang;
        }
      }else{
        *pNextPt = Line2.m_End;
        if (pRightAngle){
          *pRightAngle = ang2;
        }
      }
    }
    return EnIntersec::GE_SKEW_CROSS;     // lines has intersect. point
  }else{
    return EnIntersec::GE_SKEW_NO_CROSS;  // intersect. point is outside of lines
  }
}





//-----------------------------------------------
void CGeLine::SetEnd (const CGePoint* Pnt, UINT_PTR nPnts, UINT_PTR* piSel)
{
  UINT_PTR i, j;
  double d, dmin;

  j = 0;
  dmin = 1000000000000.0;
  for (i=0; i<nPnts; ++i){
    d = m_Start.Distance( Pnt[i] );
    if (dmin > d){
      dmin = d;
      j = i;
    }
  }
  m_End = Pnt[j];
  if (piSel){
    *piSel = j;
  }
}


// set end point by angle & length
//-----------------------------------------------
void CGeLine::SetEndPolar (double Angle, double Len)
{
  m_End = m_Start.PolarBy( Angle, Len );
}

//-----------------------------------------------
void CGeLine::SetLength (double Len)
{
  if (Len > 0.0){
    m_End = m_Start.PolarBy( Angle(), Len );
  }else{
    if (Len < 0.0){
      m_Start = m_End.PolarBy( Angle(), Len );
    }
  }
}

//-----------------------------------------------
void CGeLine::SetAngle (double Ang)
{
  m_End = m_Start.PolarBy( Ang, Length() );
}
