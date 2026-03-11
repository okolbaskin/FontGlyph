/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class CListPlines - list of polylines (CGePolyline objects)
********************************************************************/
#include "Geom.h"

#define CODE_ITEM_PLINE  11373

//=============================================================================
CItemPline::CItemPline (const CGePolyline* pPline, INT_PTR Typ) 
  : COkObject (0,0,CODE_ITEM_PLINE), CGePolyline()
{
  CopyFrom( *pPline );
  m_Type = Typ;
  m_Flags = 0;
}
//-----------------------------------------------
CItemPline::CItemPline (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed, INT_PTR Typ)
  : COkObject (0,0,CODE_ITEM_PLINE), CGePolyline()
{
  SetVers( Pnt, nPnts, bClosed );
  m_Type = Typ;
  m_Flags = 0;
}
//-----------------------------------------------
CItemPline::CItemPline (const CGeLine& Line, INT_PTR Typ)
  : COkObject (0,0,CODE_ITEM_PLINE), CGePolyline()
{
  AddVer( Line.m_Start );
  AddVer( Line.m_End );
  m_Type = Typ;
  m_Flags = 0;
}
//-----------------------------------------------
CItemPline::CItemPline (const CGePoint& Pt1, const CGePoint& Pt2, double Width, INT_PTR Typ)
  : COkObject (0,0,CODE_ITEM_PLINE), CGePolyline()
{
  double Ang;
  double W2;
  CGePoint Pnt;

  m_Type = Typ;
  m_Flags = 0;
  if (Width > 0.0){
    Ang = Pt1.Angle( Pt2 ) + GE_DEG90;
    W2 = Width / 2.0;
    Pnt = Pt1.PolarBy( Ang, -W2 );
    AddVer( Pnt );
    Pnt = Pt1.PolarBy( Ang, W2 );
    AddVer( Pnt );
    Pnt = Pt2.PolarBy( Ang, W2 );
    AddVer( Pnt );
    Pnt = Pt2.PolarBy( Ang, -W2 );
    AddVer( Pnt );
    SetClosed( true );
  }
}


//-----------------------------------------------
CItemPline::CItemPline (INT_PTR Typ)
  : COkObject (0,0,CODE_ITEM_PLINE), CGePolyline()
{
  m_Type = Typ;
  m_Flags = 0;
}
//-----------------------------------------------
CItemPline::CItemPline (const CItemPline* pItem2)
  : COkObject (0,0,CODE_ITEM_PLINE), CGePolyline()
{
  CopyFrom( *pItem2 );
  m_Type = pItem2->m_Type;
  m_Flags = pItem2->m_Flags;
}

//-----------------------------------------------
bool CItemPline::IsValid () const
{
  return (ObjCode()==CODE_ITEM_PLINE)?true:false;
}
//-----------------------------------------------
bool CItemPline::IsMarker (int* pMark) const
{
  if (m_Type >= 1000){
    *pMark = (int)m_Type; 
    return true;
  }
  return false;
}


//=============================================================================
CListPlines::CListPlines () //bool bClosed)
{
}

//-----------------------------------------------
CListPlines::~CListPlines ()
{
  Clear();
}

//-----------------------------------------------
void CListPlines::Clear ()
{
  m_Plines.Clear();
}

//-----------------------------------------------
CItemPline* CListPlines::Add (INT_PTR Typ)
{
  CItemPline* pItem;
  pItem = (CItemPline*)(m_Plines.Add( new CItemPline(Typ) ));
  return pItem;
}

//-----------------------------------------------
CItemPline* CListPlines::Add (const CGePolyline* pPlineSrc, INT_PTR Typ)
{
  CItemPline* pItem;
  pItem = (CItemPline*)(m_Plines.Add( new CItemPline(pPlineSrc,Typ) ));
  return pItem;
}

//-----------------------------------------------
CItemPline* CListPlines::Add (const CGeLine& Line, INT_PTR Typ)
{
  CItemPline* pItem;
  pItem = (CItemPline*)(m_Plines.Add( new CItemPline(Line,Typ) ));
  return pItem;
}

//-----------------------------------------------
CItemPline* CListPlines::Add (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed, INT_PTR Typ)
{
  CItemPline* pItem;
  pItem = (CItemPline*)(m_Plines.Add( new CItemPline(Pnt,nPnts,bClosed,Typ) ));
  return pItem;
}

// add a polyline as a copy from another CListPlines object
// pItem2 - a polyline from another CListPlines object
//-----------------------------------------------
CItemPline* CListPlines::Add (const CItemPline* pItem2)
{
  CItemPline* pItem;
  pItem = (CItemPline*)(m_Plines.Add( new CItemPline(pItem2,pItem2->Type()) ));
  return pItem;
}

//-----------------------------------------------
UINT_PTR CListPlines::AddPlines (const CListPlines& Plines, bool bClear, bool bSetType, INT_PTR Type)
{
  UINT_PTR nPlines = 0;
  CItemPline* pItem;

  if (bClear){
    Clear();
  }
  pItem = Plines.GetFirst();
  while( pItem ){
    if (bSetType){
      m_Plines.Add( new CItemPline(pItem,Type) );
    }else{
      m_Plines.Add( new CItemPline(pItem) );
    }
    ++nPlines;
    pItem = Plines.GetNext( pItem );
  }
  return nPlines;
}

//-----------------------------------------------
UINT_PTR CListPlines::NumVers () const 
{
  static UINT_PTR nVers;
  UINT_PTR    n;
  CItemPline* pItem = GetFirst();

  nVers = 0;
  while( pItem ){
    n = pItem->NumVers();
    nVers += n;
    pItem = GetNext( pItem );
  }
  return nVers;
}
