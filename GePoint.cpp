/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class CGePoint - point in 2D space
********************************************************************/
#include "Geom.h"

static double _Angle  = 0.0;
static double _cosAng = 1.0;
static double _sinAng = 0.0;

//-----------------------------------------------
CGePoint CGePoint::PolarBy (double Angle, double Dist) const
{
  if (_Angle != Angle){
    _Angle  = Angle;
    _cosAng = cos(_Angle);
    _sinAng = sin(_Angle);
  }
  return CGePoint( x+Dist*_cosAng, y+Dist*_sinAng );
}

//-----------------------------------------------
void CGePoint::Polar (double Angle, double Dist)
{
  if (_Angle != Angle){
    _Angle  = Angle;
    _cosAng = cos(_Angle);
    _sinAng = sin(_Angle);
  }
  x += Dist * _cosAng;
  y += Dist * _sinAng;
}

//-----------------------------------------------
CGePoint CGePoint::RotateBy (const CGePoint& Base, double Angle) const
{
  double x1,y1, dx, dy;

  if (_Angle != Angle){
    _Angle  = Angle;
    _cosAng = cos(_Angle);
    _sinAng = sin(_Angle);
  }
  dx = x - Base.x;
  dy = y - Base.y;
  x1 = (dx * _cosAng) - (dy * _sinAng) + Base.x;
  y1 = (dx * _sinAng) + (dy * _cosAng) + Base.y;
  return CGePoint( x1, y1 );
}


//-----------------------------------------------
void CGePoint::Rotate (const CGePoint& Base, double Angle)
{
  double dx, dy;

  if (_Angle != Angle){
    _Angle  = Angle;
    _cosAng = cos(_Angle);
    _sinAng = sin(_Angle);
  }
  dx = x - Base.x;
  dy = y - Base.y;
  x = (dx * _cosAng) - (dy * _sinAng) + Base.x;
  y = (dx * _sinAng) + (dy * _cosAng) + Base.y;
}

//-----------------------------------------------
// Angle from this point to point (0..2PI)
//-----------------------------------------------
double CGePoint::Angle (const CGePoint& Pt) const
{
  double a, b;

  a = Pt.x - x;
  b = Pt.y - y;
  if (a == 0.0) {
    if (b == 0.0){
      return 0.0;
    }
    return (b>0.0)? GE_PI2 : GE_DEG270;
  }else{
    if (b == 0.0){
      return (a>0.0)? 0.0 : GE_PI;
    }
    if (a > 0.0){
      if (b > 0.0){
        return atan( b / a );
      }else{
        return atan( b / a ) + GE_2PI;
      }
    }else{
      return atan( b / a ) + GE_PI;
    }
  }
}

//-----------------------------------------------
// Angle from point 0,0 to this point (0..2PI)
//-----------------------------------------------
double CGePoint::Angle () const
{
  if (x == 0.0) {
    if (y == 0.0){
      return 0.0;
    }
    return (y>0.0)? GE_PI2 : GE_DEG270;
  }else{
    if (y == 0.0){
      return (x>0.0)? 0.0 : GE_PI;
    }
    if (x > 0.0){
      if (y > 0.0){
        return atan( y / x );
      }else{
        return atan( y / x ) + GE_2PI;
      }
    }else{
      return atan( y / x ) + GE_PI;
    }
  }
}


//-----------------------------------------------
// Left side angle this-Pt1-Pt2 (0..2PI)
//-----------------------------------------------
double CGePoint::LeftAngle (const CGePoint& Pt1, const CGePoint& Pt2) const
{
  double Ang;

  Ang = Pt1.Angle(*this) - Pt1.Angle(Pt2);
  if (Ang < 0) {
    Ang += GE_2PI;
  }
  return Ang;
}

//-----------------------------------------------
// Right side angle this-Pt1-Pt2 (0..2PI)
//-----------------------------------------------
double CGePoint::RightAngle (const CGePoint& Pt1, const CGePoint& Pt2) const
{
  double Ang;

  Ang = Pt1.Angle(Pt2) - Pt1.Angle(*this);
  if (Ang < 0){
    Ang += GE_2PI;
  }
  return Ang;
}

//-----------------------------------------------
// Left side center angle this-Pt1-Pt2 (0..2PI)
//-----------------------------------------------
double CGePoint::LeftCenAngle (const CGePoint& Pt1, const CGePoint& Pt2) const
{
  double Ang1, Ang2, Ang;

  Ang1 = Pt1.Angle(*this);
  Ang2 = Pt1.Angle(Pt2);
  Ang = Ang1 - Ang2;
  if (Ang < 0){
    Ang += GE_2PI;
  }
  Ang2 += Ang/2.0;
  if (Ang2 > GE_2PI){
    Ang2 -= GE_2PI;
  }
  return Ang2;
}

//-----------------------------------------------
// Right side center angle this-Pt1-Pt2 (0..2PI)
//-----------------------------------------------
double CGePoint::RightCenAngle (const CGePoint& Pt1, const CGePoint& Pt2) const
{
  double Ang1, Ang2, Ang;

  Ang1 = Pt1.Angle(Pt2);
  Ang2 = Pt1.Angle(*this);
  Ang = Ang1 - Ang2;
  if (Ang < 0){
    Ang += GE_2PI;
  }
  Ang1 -= Ang/2.0;
  if (Ang1 < 0.0){
    Ang1 += GE_2PI;
  }
  return Ang1;
}

//-----------------------------------------------
// Center angle of bigger lef/rig angle this-Pt1-Pt2 (0..2PI)
//-----------------------------------------------
double CGePoint::CenterAngle (const CGePoint& Pt1, const CGePoint& Pt2, bool* pLeft) const
{
  double Ang1, Ang2, Ang, AngLef, AngRig;

  Ang1 = Pt1.Angle(*this);
  Ang2 = Pt1.Angle(Pt2);
  AngLef = Ang1 - Ang2;
  AngRig = Ang2 - Ang1;
  if (AngLef < 0){
    AngLef += GE_2PI;
  }
  if (AngRig < 0){
    AngRig += GE_2PI;
  }
  if (AngLef > AngRig){
    *pLeft = true;
    Ang = Ang2 + AngLef/2.0;
    if (Ang > GE_2PI){
      Ang -= GE_2PI;
    }
  }else{
    *pLeft = false;
    Ang = Ang2 - AngRig/2.0;
    if (Ang < 0.0){
      Ang += GE_2PI;
    }
  }
  return Ang;
}


//-----------------------------------------------
// Deviation angle from this-Pt1 direction (0..PI) + to left side, - to right side
//-----------------------------------------------
double CGePoint::TurnAngle (const CGePoint& Pt1, const CGePoint& Pt2) const
{
  double Ang;

  Ang = LeftAngle( Pt1, Pt2 );
  if (Ang <= GE_DEG180){
    Ang = GE_DEG180 - Ang;
  }else{
    Ang = -(Ang - GE_DEG180);
  }
  return Ang;
}

//-----------------------------------------------
// Get middle point between this point to other point
//-----------------------------------------------
CGePoint CGePoint::Middle (const CGePoint& Pnt, double Coef) const
{
  double Xm, Ym, dX, dY;

  dX = Pnt.x - x;
  dY = Pnt.y - y;
  Xm = x + (dX * Coef);
  Ym = y + (dY * Coef);
  return CGePoint(Xm,Ym);
}

/*

//-----------------------------------------------
// Get center of circle that goes through 3 points
//-----------------------------------------------
bool CGePoint::GetCenter (const CGePoint& Pt1, const CGePoint& Pt2, CGePoint* pCenter, double* pRadius) const
{
  CGeArc Arc;

  if (Arc.Set3P( *this, Pt1, Pt2 )){
    *pCenter = Arc.Center();
    *pRadius = Arc.Radius();
    return true;
  }
  return false;
}
*/


//-----------------------------------------------
// Distance from this point to other point along the angle direction
//-----------------------------------------------
double CGePoint::Distance (const CGePoint& pnt, double Angle) const
{
  double x2, dx, dy, Dist;

  dx = pnt.x - x;
  dy = pnt.y - y;
  Dist = _hypot( dx, dy );
  _cosAng = cos(-Angle);
  _sinAng = sin(-Angle);
  x2 = (dx * _cosAng) - (dy * _sinAng);
  if (x2 < 0.0){
    Dist = -Dist;
  }
  return Dist;
}


// Shortest distance from this point to a line
//-----------------------------------------------
double CGePoint::Distance (const CGeLine& line, bool bApparent) const
{
  CGePoint InterPt;
  EnPntPos res;

  res = GetPerpendicular( line, &InterPt );
  if (bApparent){
    return Distance( InterPt );
  }
  if (res == EnPntPos::GE_BEFORE){
    return Distance( line.Start() );
  }
  if (res == EnPntPos::GE_AFTER){
    return Distance( line.End() );
  }
  return Distance( InterPt );
}


/********************************************************************
* Makes perpendicular line from this point to the specified line (Line)
*  pInterPt - (out) intersection point, 
*         if NULL - the point will not be calculated
* Returns:
*  position of the intersection point (pInterPt) on the line (Line)
*  GE_BETWEEN     - between the line's start and end points
*  GE_BEFORE      - before the line's start point
*  GE_AFTER       - after the line's end point
*  GE_ORIGIN      - exactly on the line's start point
*  GE_DESTINATION - exactly on the line's end point
*********************************************************************/
EnPntPos CGePoint::GetPerpendicular (const CGeLine& Line, CGePoint* pInterPt) const
{
  const double Zero = 0.0;  //-0.00000001;
  const double One  = 1.0;  // 1.00000001;
  double   t;
  CGeLine  Line2;
  CGePoint Pt2;

  Pt2 = PolarBy( Line.Angle()+GE_DEG90, 1000000000.0 );
  Line2.SetStart( x, y );
  Line2.SetEnd( Pt2 );
  if (Line.ClassifyLine( Line2, &t ) == EnIntersec::GE_SKEW){
    if (pInterPt){
      // intersection point
      pInterPt->x = Line.Start().x + t * (Line.End().x - Line.Start().x);
      pInterPt->y = Line.Start().y + t * (Line.End().y - Line.Start().y);
    }
    if (Zero < t && t < One){
      return EnPntPos::GE_BETWEEN;
    }
    if (t < Zero){
      return EnPntPos::GE_BEFORE;
    }
    if (One < t){
      return EnPntPos::GE_AFTER;
    }
    if (t == Zero){
      return EnPntPos::GE_ORIGIN;
    }
    if (t == One){
      return EnPntPos::GE_DESTINATION;
    }
  }
  if (pInterPt){
    pInterPt->x = pInterPt->y = 0;
  }
  return EnPntPos::GE_BETWEEN;
}

//-----------------------------------------------
EnPntPos CGePoint::Classify (const CGePoint& Pt0, const CGePoint& Pt1) const
{
  CGePoint Pt2, a, b;
  double sa;

  Pt2 = *this;
  a = Pt1 - Pt0;
  b = Pt2 - Pt0;
  sa = (a.x * b.y) - (b.x * a.y);
  if (sa > 0.0){
    return EnPntPos::GE_LEFT;
  }
  if (sa < 0.0){
    return EnPntPos::GE_RIGHT;
  }
  if ((a.x*b.x<0.0) || (a.y*b.y<0.0)){
    return EnPntPos::GE_AFTER;
  }
  if (a.Length() < b.Length()){
    return EnPntPos::GE_BEFORE;
  }
  if (Pt0 == Pt1){
    return EnPntPos::GE_ORIGIN;
  }
  if (Pt1 == Pt2){
    return EnPntPos::GE_DESTINATION;
  }
  return EnPntPos::GE_BETWEEN;
}
