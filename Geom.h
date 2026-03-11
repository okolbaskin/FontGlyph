/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Classes for 2D geometric shapes
********************************************************************/
#ifndef _OK_GEOM_
#define _OK_GEOM_

#ifndef _INC_MATH
  #include <math.h>
#endif
#ifndef _INC_STDLIB
  #include <stdlib.h>
#endif
#ifndef _OK_LISTOBJ_
  #include "ListObj.h"
#endif


#define GE_FLT_MAX  3.4E+38
#define GE_DBL_MAX  1.7E+308

// angles in radians
//            3,14159265358979323846 2643383279 5028841971 6939937510 5820974944 5923078164 0628620899 8628034825 3421170679
#define GE_PI  3.14159265358979323846   // 180
#define GE_PI2 1.57079632679489661923   //  90  (GE_PI / 2.0)
#define GE_PI4 0.785398163397448309615  //  45  (GE_PI / 4.0)
#define GE_2PI 6.28318530717958647692   // 360  (GE_PI+GE_PI) 

// aka degrees
#define GE_DEG1    GE_PI / 180.0
#define GE_DEG2    GE_PI / 90.0
#define GE_DEG3    GE_PI / 60.0
#define GE_DEG4    GE_PI / 45.0
#define GE_DEG5    GE_PI / 36.0
#define GE_DEG10   GE_PI / 18.0
#define GE_DEG20   GE_PI /  9.0
#define GE_DEG22_5 GE_PI /  8.0
#define GE_DEG30   GE_PI /  6.0
#define GE_DEG36   GE_PI /  5.0
#define GE_DEG45   GE_PI4
#define GE_DEG90   GE_PI2
#define GE_DEG135  (GE_PI2+GE_PI4)
#define GE_DEG180  GE_PI
#define GE_DEG225  (GE_PI+GE_PI4)
#define GE_DEG270  (GE_PI+GE_PI2)
#define GE_DEG315  (GE_2PI-GE_PI4)
#define GE_DEG360  GE_2PI


// Radian -> Degree
#define GE_RAD_TO_DEG   57.2957795130823208768
// Degree -> Radian
#define GE_DEG_TO_RAD    0.01745329251994329577

// Intersection results
enum class EnIntersec{
  GE_COLLINEAR=0, 
  GE_PARALLEL=1, 
  GE_SKEW=2, 
  GE_SKEW_CROSS=3, 
  GE_SKEW_NO_CROSS=4
};

// Type of point's position relative to line
enum class EnPntPos{
  GE_LEFT=0, 
  GE_RIGHT=1, 
  GE_BEFORE=2, 
  GE_AFTER=3, 
  GE_BETWEEN=4, 
  GE_ORIGIN=5, 
  GE_DESTINATION=6
};

// Polyline fitting types
#define GE_PLFIT_NONE         0      // linear
#define GE_PLFIT_QUAD         1      // Quadratic B-spline
#define GE_PLFIT_CUBIC        2      // Cubic B-spline
#define GE_PLFIT_LINQUAD      3      // mixed linear and quadratic segments


class CGeLine;

#ifndef _GE_GLOBALS
  extern double g_GeTolerance;
  extern double g_GeTolClass;   // tolerance for CGeLine::ClassifyLine() and CGeLine::ClassifyPoint()
  extern double g_GeMiterLimit;
#endif

//=============================================================================
//
//  CGePoint_
//
//=============================================================================
class CGePoint {
public:
  double x;
  double y;
public:
    CGePoint ();
    CGePoint (const CGePoint& Pnt);
    CGePoint (double x, double y);

  void  Set (double x, double y);
  void  Set (const CGePoint& Pnt);
  bool  IsEqual (const CGePoint& Pnt, double Delta=g_GeTolerance) const;
  bool  IsNotEqual (const CGePoint& Pnt, double Delta=g_GeTolerance) const;

  // Functions/binary-operators that return points or sizes
  CGePoint  MoveBy      (double dx, double dy) const;
  CGePoint  MoveBy      (const CGePoint& Off) const;
  CGePoint  PolarBy     (double Angle, double Dist) const;
  CGePoint  RotateBy    (const CGePoint& Base, double Angle) const;
  CGePoint  ScaleBy     (const CGePoint& Base, double ScaleX, double ScaleY) const;
  CGePoint  ScaleBy     (const CGePoint& Base, double Scal) const;

  // Functions/assignement-operators that modify this point
  void      Move        (double dx, double dy);
  void      Move        (const CGePoint& Off);
  void      Polar       (double Angle, double Dist);
  void      Rotate      (const CGePoint& Base, double Angle);
  void      Scale       (const CGePoint& Base, double ScaleX, double ScaleY);
  void      Scale       (const CGePoint& Base, double Scal);

  bool      operator == (const CGePoint& Pt) const;
  bool      operator != (const CGePoint& Pt) const;
  CGePoint  operator +  (const CGePoint& Pt) const;
  CGePoint  operator -  (const CGePoint& Pt) const;
  bool      operator <  (const CGePoint& Pt) const;
  bool      operator >  (const CGePoint& Pt) const;

  // Vector length
  double Length () const;

  // Distance from this point to other point
  double Distance (const CGePoint& Pnt) const;
  // Distance from this point to other point along the angle direction
  double Distance (const CGePoint& Pnt, double Angle) const;

  // Shortest distance from this point to a line
  double Distance (const CGeLine& Line, bool bApparent) const;

  // Angle from this point to 'Pnt' (0..2PI)
  double Angle (const CGePoint& Pnt) const;
  // Angle from point 0,0 to this point (0..2PI)
  double Angle () const;

  // Left side angle this-p1-p2 (0..2PI)
  double LeftAngle  (const CGePoint& Pnt1, const CGePoint& Pnt2) const;
  // Right side angle this-p1-p2 (0..2PI)
  double RightAngle (const CGePoint& Pnt1, const CGePoint& Pnt2) const;

  // Left side center angle this-p1-p2 (0..2PI)
  double LeftCenAngle  (const CGePoint& Pnt1, const CGePoint& Pnt2) const;
  // Right side center angle this-p1-p2 (0..2PI)
  double RightCenAngle (const CGePoint& Pnt1, const CGePoint& Pnt2) const;
  // Center angle of bigger lef/rig angle this-p1-p2 (0..2PI)
  double CenterAngle   (const CGePoint& Pnt1, const CGePoint& Pnt2, bool* pLeft) const;

  // deviation angle from this-p1 direction (0..PI) + to left side, - to right side
  double TurnAngle (const CGePoint& Pnt1, const CGePoint& Pnt2) const;

  // Get center point between this point to other point
  CGePoint Center (const CGePoint& Pnt) const;

  // Get middle point between this point to other point
  CGePoint Middle (const CGePoint& Pnt, double Coef) const;

  // Get center of circle that goes through 3 points
//  bool GetCenter (const CGePoint& Pt1, const CGePoint& Pt2, CGePoint* pCenter, double* pRadius) const;

  // Makes perpendicular line from this point to the specified Line
  EnPntPos GetPerpendicular (const CGeLine& Line, CGePoint* pInterPt) const;

  EnPntPos Classify (const CGePoint& Pt0, const CGePoint& Pt1) const;

  const CGePoint& Point () const;
  const CGePoint& Pnt () const;

};
inline CGePoint CGePoint::MoveBy (double dx, double dy) const {return CGePoint(x+dx, y+dy);}
inline CGePoint CGePoint::MoveBy (const CGePoint& Off) const {return CGePoint(x+Off.x, y+Off.y);}
inline CGePoint CGePoint::ScaleBy (const CGePoint& Base, double Scx, double Scy) const {
  return CGePoint( Base.x + (x - Base.x) * Scx, Base.y + (y - Base.y) * Scy );
}
inline CGePoint CGePoint::ScaleBy (const CGePoint& Base, double Sc) const {
  return CGePoint( Base.x + (x - Base.x) * Sc, Base.y + (y - Base.y) * Sc );
}
inline void CGePoint::Move (double dx, double dy) {x+=dx; y+=dy;}
inline void CGePoint::Move (const CGePoint& Off)  {x+=Off.x; y+=Off.y;}
inline void CGePoint::Scale (const CGePoint& Base, double Scx, double Scy) {
  x = Base.x + (x - Base.x) * Scx;
  y = Base.y + (y - Base.y) * Scy;
}
inline void CGePoint::Scale (const CGePoint& Base, double Sc) {
  x = Base.x + (x - Base.x) * Sc;
  y = Base.y + (y - Base.y) * Sc;
}
inline bool     CGePoint::operator == (const CGePoint& Pt) const {return (x==Pt.x && y==Pt.y);}
inline bool     CGePoint::operator != (const CGePoint& Pt) const {return (x!=Pt.x || y!=Pt.y);}
inline CGePoint CGePoint::operator + (const CGePoint& Pt) const {return CGePoint(x+Pt.x, y+Pt.y);}
inline CGePoint CGePoint::operator - (const CGePoint& Pt) const {return CGePoint(x-Pt.x, y-Pt.y);}
inline bool     CGePoint::operator < (const CGePoint& Pt) const {return (x<Pt.x || (x==Pt.x && y<Pt.y));}
inline bool     CGePoint::operator > (const CGePoint& Pt) const {return (x>Pt.x || (x==Pt.x && y>Pt.y));}
inline CGePoint CGePoint::Center (const CGePoint& Pt) const {return CGePoint( (x+Pt.x)/2.0, (y+Pt.y)/2.0 );}
inline double   CGePoint::Length () const {return _hypot( x, y );}
inline double   CGePoint::Distance  (const CGePoint& Pnt) const {return _hypot( x-Pnt.x, y-Pnt.y );}
inline const CGePoint& CGePoint::Point () const {return *this;}
inline const CGePoint& CGePoint::Pnt () const {return *this;}


// Inline functions
//-------------------------------------------------------------------
inline      CGePoint::CGePoint () : x(0.0), y(0.0) {}
inline      CGePoint::CGePoint (const CGePoint& src) : x(src.x), y(src.y) {}
inline      CGePoint::CGePoint (double xx, double yy) : x(xx), y(yy) {}
inline void CGePoint::Set (double xx, double yy) {x=xx; y=yy;}
inline void CGePoint::Set (const CGePoint& Pnt) {x = Pnt.x; y=Pnt.y;}
inline bool CGePoint::IsEqual(const CGePoint& Pt2, double Delta) const {
  if ((x-Delta <= Pt2.x) && (Pt2.x <= x+Delta) && (y-Delta <= Pt2.y) && (Pt2.y <= y+Delta)) {
    return true;
  }
  return false;
}
inline bool CGePoint::IsNotEqual(const CGePoint& Pt2, double delta) const {
  if ((x-delta <= Pt2.x) && (Pt2.x <= x+delta) && (y-delta <= Pt2.y) && (Pt2.y <= y+delta)) {
    return false;
  }
  return true;
}



//=============================================================================
//
//  CGeLine_
//
//=============================================================================
class CGeLine {
public:
  CGePoint m_Start;
  CGePoint m_End;
public:
    CGeLine ();
    CGeLine (const CGeLine& Line);
    CGeLine (const CGePoint& Pnt1, const CGePoint& Pnt2);
    CGeLine (double x0, double y0, double x1, double y1);

  CGePoint Start        () const;
  CGePoint End          () const;
  double   dx           () const;
  double   dy           () const;
  CGePoint Center       () const;
  double   Length       () const;
  double   Angle        () const;   // Angle from Start to End (0..2PI)

  // modify geometry by properties
  void  Set        (double x1, double y1, double x2, double y2);
  void  Set        (const CGeLine& line);
  void  Set        (const CGePoint& Pt0, const CGePoint& Pt1);
  void  Set        (const CGePoint* Pnts);
  void  SetStart   (double x, double y);
  void  SetStart   (const CGePoint& Pnt);
  void  SetStartX  (double x);
  void  SetStartY  (double y);
  void  SetEnd     (double x, double y);
  void  SetEnd     (const CGePoint& Pnt);
  void  SetEnd     (const CGePoint* Pnt, UINT_PTR nPnts, UINT_PTR* piSel);
  void  SetEndPolar(double Angle, double Dist);
  void  SetEndX    (double x);
  void  SetEndY    (double y);
  void  SetLength  (double Len);
  void  SetAngle   (double Ang);

  EnPntPos   ClassifyPoint   (const CGePoint& Pnt, double LefRigTol=0.0) const;
  EnPntPos   ClassifyPoint2  (const CGePoint& Pnt, double Delta) const;
  EnIntersec ClassifyLine    (const CGeLine& Line2, double* pT) const;
  EnIntersec GetIntersection (const CGeLine& Line2, CGePoint* pInterPt, CGePoint* pNextPt=NULL, double* pRightAngle=NULL, double Zero1=0.0, double One1=1.0, double Zero2=0.0, double One2=1.0) const;

};

//-----------------------------------------------
inline CGeLine::CGeLine () : m_Start(0.0,0.0), m_End(1.0,0.0) {}
inline CGeLine::CGeLine (const CGeLine& Line) {Set(Line);}
inline CGeLine::CGeLine (const CGePoint& Pt0, const CGePoint& Pt1) : m_Start(Pt0), m_End(Pt1) {}
inline CGeLine::CGeLine (double x0, double y0, double x1, double y1) : m_Start(x0,y0), m_End(x1,y1) {}
inline void CGeLine::Set (double x0, double y0, double x1, double y1) {
  m_Start.Set( x0, y0 );
  m_End.Set( x1, y1 );
}
inline void CGeLine::Set (const CGeLine& Line) {
  m_Start = Line.m_Start; 
  m_End = Line.m_End;
}
inline void CGeLine::Set (const CGePoint& Pt0, const CGePoint& Pt1) {
  m_Start = Pt0; 
  m_End = Pt1;
}
inline void CGeLine::Set (const CGePoint* Pnts) {
  m_Start = *Pnts; 
  m_End = *(Pnts+1);
}
inline void     CGeLine::SetStart  (double x, double y) {m_Start.Set(x,y);}
inline void     CGeLine::SetStartX (double x) {m_Start.x=x;}
inline void     CGeLine::SetStartY (double y) {m_Start.y=y;}
inline void     CGeLine::SetStart  (const CGePoint& Pt) {m_Start=Pt;}
inline void     CGeLine::SetEnd    (double x, double y) {m_End.Set(x,y);}
inline void     CGeLine::SetEndX   (double x) {m_End.x=x;}
inline void     CGeLine::SetEndY   (double y) {m_End.y=y;}
inline void     CGeLine::SetEnd    (const CGePoint& Pt) {m_End=Pt;}

inline CGePoint CGeLine::Center    () const {return m_Start.Center( m_End );}
inline double   CGeLine::dx        () const {return (m_End.x - m_Start.x);}
inline double   CGeLine::dy        () const {return (m_End.y - m_Start.y);}
inline double   CGeLine::Length    () const {return m_Start.Distance( m_End );}
inline CGePoint CGeLine::Start     () const {return m_Start;}
inline CGePoint CGeLine::End       () const {return m_End;}
inline double   CGeLine::Angle () const {return m_Start.Angle( m_End );}


//=============================================================================
//
//  CGeRect_
//
//=============================================================================
class CGeRect {
  double m_Left;
  double m_Bottom;
  double m_Right;
  double m_Top;
public:
    CGeRect ();
    CGeRect (double Lef, double Bot, double Rig, double Top);
    CGeRect (const CGeRect& Rect);
    CGeRect (const CGePoint& Pnt1, const CGePoint& Pnt2);
    CGeRect (const CGePoint& Center, double Width, double Height);
    CGeRect (const CGePoint& Center, double Delta);

  void Set (double Lef, double Bot, double Rig, double Top);
  void Set (const CGePoint& Pnt1, const CGePoint& Pnt2);
  void Set (const CGePoint& Center, double Width, double Height);
  void Set (const CGePoint& Center, double Delta);
  bool SetWidth  (double W, bool bCentered);
  bool SetHeight (double H, bool bCentered);
  void SetCenter (const CGePoint& Center);
  void SetSize   (double Width, double Height);
  void SetNull   ();
  bool IsNull    () const;
  bool IsNotNull () const;
  double   Width  () const;
  double   Height () const;
  CGePoint Center () const;
  CGePoint LefBot () const;
  CGePoint LefTop () const;
  CGePoint RigTop () const;
  CGePoint RigBot () const;
  CGePoint LefMid () const;
  CGePoint RigMid () const;
  CGePoint MidBot () const;
  CGePoint MidTop () const;
  double   Left   () const;
  double   Bottom () const;
  double   Right  () const;
  double   Top    () const;
  void     GetPoints   (CGePoint* pPnt) const;

  void UpdateExtentsBy (double X, double Y);
  void UpdateExtentsBy (const CGePoint& Pt);
  void UpdateExtentsBy (const CGeRect& rect);
  void    Scale   (double Coef, bool bByCenter);
  CGeRect ScaleBy (double Coef, bool bByCenter) const;
  void    Move    (double dx, double dy);
  void    Move    (const CGePoint& Off);
  CGeRect MoveBy  (double dx, double dy) const;
  CGeRect MoveBy  (const CGePoint& Off) const;
  void    Inflate (double Delta);

  bool  Contains  (const CGePoint& Pnt) const;
  bool  Contains  (const CGePoint& Pnt, double Delta) const;
  bool  NotContains (const CGePoint& Pnt) const;
  bool  NotContains (const CGePoint& Pnt, double Delta) const;
  bool  Contains  (double X, double Y) const;

  bool  operator == (const CGeRect& rect2) const;

private:
  void Normalize ();
};

//-----------------------------------------------
inline CGeRect::CGeRect () {m_Left=m_Bottom=m_Right=m_Top=0.0;}
inline CGeRect::CGeRect (double lef, double bot, double rig, double top) {Set(lef,bot,rig,top);}
inline CGeRect::CGeRect (const CGeRect& rect) {m_Left=rect.m_Left; m_Bottom=rect.m_Bottom; m_Right=rect.m_Right; m_Top=rect.m_Top;}
inline CGeRect::CGeRect (const CGePoint& Center, double Wid, double Hei) {Set(Center,Wid,Hei);}
inline CGeRect::CGeRect (const CGePoint& Center, double Delta) {Set(Center,Delta);}
inline CGeRect::CGeRect (const CGePoint& pnt11, const CGePoint& pnt22) {Set(pnt11,pnt22);}
inline void CGeRect::Set (double lef, double bot, double rig, double top){
  if (lef < rig){
    m_Left = lef;
    m_Right = rig;
  }else{
    m_Left = rig;
    m_Right = lef;
  }
  if (bot < top){
    m_Bottom = bot;
    m_Top = top;
  }else{
    m_Bottom = top;
    m_Top = bot;
  }
}
inline void CGeRect::Set (const CGePoint& Pt1, const CGePoint& Pt2) {
  m_Left = Pt1.x;  m_Bottom = Pt1.y;
  m_Right = Pt2.x; m_Top = Pt2.y;
  Normalize();
}
inline void CGeRect::SetNull   () {Set(0.,0.,0.,0.);}
inline bool CGeRect::IsNull () const {
  return ((m_Left==0.0) && (m_Bottom==0.0) && (m_Right==0.0) && (m_Top==0.0))? true : false;
}
inline bool CGeRect::IsNotNull () const {
  return ((m_Left==0.0) && (m_Bottom==0.0) && (m_Right==0.0) && (m_Top==0.0))? false: true;
}
inline void CGeRect::UpdateExtentsBy (double X, double Y){
  if (m_Left  > X) m_Left  =X;
  if (m_Right < X) m_Right =X;
  if (m_Bottom> Y) m_Bottom=Y;
  if (m_Top   < Y) m_Top   =Y;
}
inline void CGeRect::UpdateExtentsBy (const CGePoint& Pt) {
  if (m_Left  > Pt.x) m_Left  =Pt.x;
  if (m_Right < Pt.x) m_Right =Pt.x;
  if (m_Bottom> Pt.y) m_Bottom=Pt.y;
  if (m_Top   < Pt.y) m_Top   =Pt.y;
}
inline void CGeRect::UpdateExtentsBy (const CGeRect& rect) {
  if (m_Left  > rect.m_Left)   m_Left  =rect.m_Left;
  if (m_Right < rect.m_Right)  m_Right =rect.m_Right;
  if (m_Bottom> rect.m_Bottom) m_Bottom=rect.m_Bottom;
  if (m_Top   < rect.m_Top)    m_Top   =rect.m_Top;
}
inline double   CGeRect::Width  () const {return m_Right - m_Left;}
inline double   CGeRect::Height () const {return m_Top - m_Bottom;}
inline CGePoint CGeRect::Center () const {return CGePoint( (m_Right+m_Left)/2.0, (m_Top+m_Bottom)/2.0 );}
inline CGePoint CGeRect::LefBot () const {return CGePoint( m_Left, m_Bottom );}
inline CGePoint CGeRect::LefTop () const {return CGePoint( m_Left, m_Top );}
inline CGePoint CGeRect::RigTop () const {return CGePoint( m_Right, m_Top );}
inline CGePoint CGeRect::RigBot () const {return CGePoint( m_Right, m_Bottom );}
inline CGePoint CGeRect::LefMid () const {return CGePoint( m_Left, (m_Top+m_Bottom)/2.0 );}
inline CGePoint CGeRect::RigMid () const {return CGePoint( m_Right, (m_Top+m_Bottom)/2.0 );}
inline CGePoint CGeRect::MidBot () const {return CGePoint( (m_Right+m_Left)/2.0, m_Bottom );}
inline CGePoint CGeRect::MidTop () const {return CGePoint( (m_Right+m_Left)/2.0, m_Top );}
inline double   CGeRect::Left   () const {return m_Left;}
inline double   CGeRect::Bottom () const {return m_Bottom;}
inline double   CGeRect::Right  () const {return m_Right;}
inline double   CGeRect::Top    () const {return m_Top;}
inline void CGeRect::Move (double dx, double dy) {m_Left+=dx; m_Bottom+=dy; m_Right+=dx; m_Top+=dy;}
inline void CGeRect::Move (const CGePoint& Off) {m_Left+=Off.x; m_Bottom+=Off.y; m_Right+=Off.x; m_Top+=Off.y;}
inline CGeRect CGeRect::MoveBy (const CGePoint& Off) const {return MoveBy(Off.x,Off.y);}
inline void CGeRect::Inflate (double Delta) {m_Left-=Delta; m_Bottom-=Delta; m_Right+=Delta; m_Top+=Delta;}
inline bool CGeRect::operator == (const CGeRect& r2) const {
  return (r2.m_Right==m_Right && r2.m_Left==m_Left && r2.m_Bottom==m_Bottom && r2.m_Top==m_Top)?true:false;
}
inline bool CGeRect::Contains (const CGePoint& Pnt) const {   // false - outside or on-border
  return ((m_Left < Pnt.x) && (Pnt.x < m_Right) && (m_Bottom < Pnt.y) && (Pnt.y < m_Top))? true : false;
}
inline bool CGeRect::NotContains (const CGePoint& Pnt) const {   // true - outside
  return ((m_Left < Pnt.x) && (Pnt.x < m_Right) && (m_Bottom < Pnt.y) && (Pnt.y < m_Top))? false : true;
}
inline bool CGeRect::Contains (double X, double Y) const {   // false - outside or on-border
  return ((m_Left < X) && (X < m_Right) && (m_Bottom < Y) && (Y < m_Top))? true : false;
}




//=============================================================================
//
//  CGePolyline_
//
//=============================================================================

//-----------------------------------------------
class CGeVertex : public COkObject {
public:
  CGePoint m_Pos;     // position
  int      m_iParam;
union {
  double   m_fParam;
  double   m_Z;
};
union {
  double   m_fParam2;
  double   m_Bulge;
};
  double   m_Dist;  // distance from beginning of polyline
  double   m_Angle;
public:
    CGeVertex ();
    CGeVertex (const CGeVertex* pVerSrc);
    CGeVertex (const CGePoint& Pnt, int iPrm=0, double fPrm=0.0, double fPrm2=0.0);
    CGeVertex (double X, double Y, double Z=0.0);
  double X () const {return m_Pos.x;}
  double Y () const {return m_Pos.y;}
  double Z () const {return m_fParam;}
  void   SetZ (double Z) {m_fParam=Z;}
  const CGeVertex* PrevVer() const {return (CGeVertex*)Prev();}
  const CGeVertex* NextVer() const {return (CGeVertex*)Next();}
};

//-----------------------------------------------
class CGePolyline {
  COkList  m_Vers;       // list of vertices (CGeVertex objects)
  UINT_PTR m_NumVers;    // number of polyline points
  bool     m_bClosed;
public:
  double   m_Zval;       // used for isolines (COkTIN::GenIsolines)
  bool     m_bBold;      // used to mark bold isoline
  static bool m_bNoDuplicate;  // don't add duplicate point
public:
    CGePolyline ();
    CGePolyline (const CGePoint* Pnts, UINT_PTR nPnts, bool bClosed);
    ~CGePolyline ();

  void       Clear       ();
  CGeVertex* AddVertex   ();
  CGeVertex* AddVer      (const CGeVertex* pVerSrc);
  CGeVertex* AddVer      (const CGePoint& Pnt, int iPrm=0, double fPrm=0.0, double fPrm2=0.0);
  CGeVertex* AddVer      (double X, double Y, double Z=0.0);
  CGeVertex* AddVer0     ();
  UINT_PTR   AddVers     (const CGePoint* Pnt, UINT_PTR nPnts);
  void       SetPoints   (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed=false, bool bBackOrder=false);
  void       SetVers     (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed=false, bool bBackOrder=false);
  void       CopyFrom    (const CGePolyline& Pline);
  void       SetClosed   (bool bClosed=true);
  void       Interpolate (int FitType, int Quality); 

  bool    IsClosed     () const;
  bool    IsNotClosed  () const;

  UINT_PTR   CountVers   ();
  UINT_PTR   CountPoints ();
  UINT_PTR   NumVers     () const;
  UINT_PTR   NumPoints   () const;
  CGeVertex* GetFirstVer () const;
  CGeVertex* GetNextVer  (const CGeVertex* p) const;
  CGeVertex* GetLastVer  () const;
  CGeVertex* GetPrevVer  (const CGeVertex* p) const;

private:
  void Gen_BSpline2 (int Quality);
  void Gen_BSpline3 (int Quality);
  void Gen_LinQuad  (int Quality);
};


//-----------------------------------------------
inline CGeVertex* CGePolyline::AddVertex () {return (CGeVertex*)(m_Vers.Add( new CGeVertex() ));}
inline UINT_PTR   CGePolyline::CountVers   () {return m_NumVers=m_Vers.NumRecs();}
inline UINT_PTR   CGePolyline::CountPoints () {return m_NumVers=m_Vers.NumRecs();}
inline UINT_PTR   CGePolyline::NumVers     () const {return m_NumVers;}
inline UINT_PTR   CGePolyline::NumPoints   () const {return m_NumVers;}
inline CGeVertex* CGePolyline::GetFirstVer () const {return (CGeVertex*)m_Vers.GetFirst();}
inline CGeVertex* CGePolyline::GetNextVer  (const CGeVertex* p) const {return (CGeVertex*)m_Vers.GetNext(p);}
inline CGeVertex* CGePolyline::GetLastVer  () const {return (CGeVertex*)m_Vers.GetLast();}
inline CGeVertex* CGePolyline::GetPrevVer  (const CGeVertex* p) const {return (CGeVertex*)m_Vers.GetPrev(p);}
inline void       CGePolyline::SetClosed   (bool b) {m_bClosed=b;}
inline bool       CGePolyline::IsClosed    () const {return m_bClosed;}
inline bool       CGePolyline::IsNotClosed () const {return !m_bClosed;}
inline void       CGePolyline::SetPoints   (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed, bool bBackOrder) {SetVers(Pnt,nPnts,bClosed,bBackOrder);}


//=============================================================================
//
//  CListPlines_  - list of polylines
//
//=============================================================================

//-----------------------------------------------
class CItemPline : public COkObject, public CGePolyline {
  INT_PTR m_Type;
public:
  BYTE m_Flags;
public:
    CItemPline (const CGePolyline* pPline, INT_PTR Type=0);
    CItemPline (const CGeLine& Line, INT_PTR Type=0);
    CItemPline (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed, INT_PTR Type=0);
    CItemPline (const CGePoint& Pt1, const CGePoint& Pt2, double Width, INT_PTR Type=0);
    CItemPline (INT_PTR Type);
    CItemPline (const CItemPline* pItem2);

  bool     IsValid  () const;
  INT_PTR  Type     () const;
  void     SetType  (INT_PTR T);
  bool     IsMarker (int* pMark) const;
  CGePolyline* GePline() const;
};
inline INT_PTR CItemPline::Type () const {return m_Type;}
inline void    CItemPline::SetType (INT_PTR T) {m_Type=T;}
inline CGePolyline* CItemPline::GePline() const {return (CGePolyline*)this;}


//-----------------------------------------------
class CListPlines {
  COkList m_Plines;     // list of polylines (CItemPline objects)
public:
    CListPlines ();
    ~CListPlines ();

  void        Clear      ();
  CItemPline* Add        (INT_PTR Type=0);
  CItemPline* Add        (const CGePolyline* pPline, INT_PTR Type=0);
  CItemPline* Add        (const CGeLine& Line, INT_PTR Type=0);
  CItemPline* Add        (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed, INT_PTR Type=0);
  CItemPline* Add        (const CItemPline* pItem2);
  UINT_PTR    AddPlines  (const CListPlines& Plines, bool bClear=false, bool bSetType=true, INT_PTR Type=0);
  void        SetType    (INT_PTR Type);
  UINT_PTR    RemoveID   (COkID Id);
  bool        Remove     (const COkObject* pObj);
  bool        RemoveFirst();
  bool        RemoveLast ();
  bool        DeleteLast ();
  UINT_PTR    CopyFrom   (const CListPlines& Plines);

  bool        IsEmpty    () const;
  CItemPline* GetFirst   () const;
  CItemPline* GetNext    (const CItemPline* p) const;
  CItemPline* GetLast    () const;
  CItemPline* GetPrev    (const CItemPline* p) const;
  UINT_PTR    NumPlines  () const;
  UINT_PTR    NumVers    () const;
  UINT_PTR    NumPoints  () const;

  void        Scale      (const CGePoint& Pt0, double ScaleX, double ScaleY);
  void        Rotate     (const CGePoint& Pt0, double Angle);
  void        Move       (double dx, double dy);
  void        Move       (const CGePoint& Off);
};

inline bool        CListPlines::IsEmpty  () const {return (m_Plines.GetFirst())?false:true;}
inline CItemPline* CListPlines::GetFirst () const {return (CItemPline*)m_Plines.GetFirst();}
inline CItemPline* CListPlines::GetNext  (const CItemPline* p) const {return (CItemPline*)m_Plines.GetNext(p);}
inline CItemPline* CListPlines::GetLast  () const {return (CItemPline*)m_Plines.GetLast();}
inline CItemPline* CListPlines::GetPrev  (const CItemPline* p) const {return (CItemPline*)m_Plines.GetPrev(p);}
inline UINT_PTR    CListPlines::NumPlines () const {return m_Plines.NumRecs();}
inline UINT_PTR    CListPlines::NumPoints () const {return NumVers();}
inline UINT_PTR    CListPlines::RemoveID    (COkID id) {return m_Plines.RemoveID(id);}
inline bool        CListPlines::Remove      (const COkObject* pObj) {return m_Plines.Remove(pObj);}
inline bool        CListPlines::RemoveFirst () {return m_Plines.RemoveFirst();}
inline bool        CListPlines::RemoveLast  () {return m_Plines.RemoveLast();}
inline bool        CListPlines::DeleteLast () {return m_Plines.RemoveLast();}
inline void        CListPlines::Move (const CGePoint& Off) {Move(Off.x,Off.y);}
inline UINT_PTR    CListPlines::CopyFrom (const CListPlines& Plines) {return AddPlines( Plines, true, false );}


/********************************************************************
* Generates quadratic B-spline curve between 
* control points A,B,C
*   nLines - number of lines per curve 
*   GenVer - pointer to array of result points (without last point C)
*   pnGenVer - pointer to number of points in GenVer
*   MaxGenVer - max number of points in GenVer
*********************************************************************/
bool geGenSegmentBSpline2 (const CGePoint& A, const CGePoint& B, const CGePoint& C, UINT_PTR nLines,
                           CGePoint* GenVer, UINT_PTR* pnGenVer, UINT_PTR MaxGenVer, double* pLen=NULL);

#endif  // _OK_GEOM_



