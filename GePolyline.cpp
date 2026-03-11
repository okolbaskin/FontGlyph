/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class CGePolyline - polyline with vertices (CGeVertex objects)
********************************************************************/
#include "Geom.h"

const UINT_PTR MaxSplineQuality = 50;

//-----------------------------------------------
CGeVertex::CGeVertex () : COkObject (0,0,0)
{
  m_iParam = 0;
  m_fParam = m_fParam2 = 0.0;
  m_Dist = 0.0;
  m_Angle = 0.0;
}

//-----------------------------------------------
CGeVertex::CGeVertex (const CGeVertex* pVerSrc) : COkObject (0,0,0)
{
  m_Pos     = pVerSrc->m_Pos;
  m_iParam  = pVerSrc->m_iParam;
  m_fParam  = pVerSrc->m_fParam;
  m_fParam2 = pVerSrc->m_fParam2;
  m_Dist = 0.0;
  m_Angle = 0.0;
}

//-----------------------------------------------
CGeVertex::CGeVertex (const CGePoint& Pnt, int iPrm, double fPrm, double fPrm2) : COkObject (0,0,0)
{
  m_Pos = Pnt;
  m_iParam = iPrm;
  m_fParam = fPrm;
  m_fParam2 = fPrm2;
  m_Dist = 0.0;
  m_Angle = 0.0;
}

//-----------------------------------------------
CGeVertex::CGeVertex (double X, double Y, double Z) : COkObject (0,0,0)
{
  m_Pos.Set( X, Y );
  m_iParam = 0;
  m_fParam = Z;
  m_fParam2 = 0.0;
  m_Dist = 0.0;
  m_Angle = 0.0;
}



//-----------------------------------------------
CGePolyline::CGePolyline ()
{
  m_bClosed = false;
  m_NumVers = 0;
  m_Zval = 0.0;
  m_bBold = false;
}

//-----------------------------------------------
CGePolyline::CGePolyline (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed)
{
  UINT_PTR i;
  const CGePoint* pPnt;

  pPnt = Pnt;
  for (i=0; i<nPnts; ++i){
    AddVer( *pPnt );
    ++pPnt;
  }
  m_bClosed = bClosed;
  m_NumVers = 0;
  m_Zval = 0.0;
  m_bBold = false;
}

//-----------------------------------------------
CGePolyline::~CGePolyline ()
{
  Clear();
}

//-----------------------------------------------
void CGePolyline::Clear ()
{
  m_Vers.Clear();
  m_NumVers = 0;
  m_bClosed = false;
}

//-----------------------------------------------
CGeVertex* CGePolyline::AddVer (const CGeVertex* pVerSrc)
{
  CGeVertex* pVer;
  CGeVertex* pVerLast;

  if (m_bNoDuplicate){
    pVerLast = GetLastVer();
    if (pVerLast){
      if (pVerLast->m_Pos.IsEqual( pVerSrc->m_Pos, g_GeTolerance )){
        // duplicate point
        return NULL;
      }
    }
  }
  pVer = (CGeVertex*)m_Vers.Add( new CGeVertex(pVerSrc) );
  if (pVer){
    pVer->m_Dist = pVerSrc->m_Dist;
    ++m_NumVers;
  }
  return pVer;
}

//-----------------------------------------------
CGeVertex* CGePolyline::AddVer (const CGePoint& Pnt, int iPrm, double fPrm, double fPrm2)
{
  CGeVertex* pVer;
  CGeVertex* pVerLast;

  if (m_bNoDuplicate){
    pVerLast = GetLastVer();
    if (pVerLast){
      if (pVerLast->m_Pos.IsEqual( Pnt, g_GeTolerance )){
        // duplicate point
        return NULL;
      }
    }
  }
  pVer = (CGeVertex*)m_Vers.Add( new CGeVertex(Pnt,iPrm,fPrm,fPrm2) );
  if (pVer){
    ++m_NumVers;
  }
  return pVer;
}

//-----------------------------------------------
CGeVertex* CGePolyline::AddVer (double X, double Y, double Z)
{
  CGeVertex* pVer;
  CGeVertex* pVerLast;
  CGePoint Pnt;

  if (m_bNoDuplicate){
    pVerLast = GetLastVer();
    if (pVerLast){
      Pnt.Set(X,Y);
      if (pVerLast->m_Pos.IsEqual( Pnt, g_GeTolerance )){
        // duplicate point
        return NULL;
      }
    }
  }
  pVer = (CGeVertex*)m_Vers.Add( new CGeVertex(X,Y,Z) );
  if (pVer){
    ++m_NumVers;
  }
  return pVer;
}

//-----------------------------------------------
CGeVertex* CGePolyline::AddVer0 ()
{
  CGeVertex* pVer0;
  CGeVertex* pVer;
  CGeVertex* pVerLast;

  pVerLast = GetLastVer();
  if (pVerLast){
    pVer0 = (CGeVertex*)(m_Vers.GetFirst());
    if (pVerLast->m_Pos.IsEqual( pVer0->m_Pos, g_GeTolerance )){
      return NULL;
    }
    pVer = (CGeVertex*)m_Vers.Add( new CGeVertex(pVer0->m_Pos) );
    if (pVer){
      SetClosed( false );
      ++m_NumVers;
    }
    return pVer;
  }
  return NULL;
}

//-----------------------------------------------
UINT_PTR CGePolyline::AddVers (const CGePoint* Pnt, UINT_PTR nPnts)
{
  UINT_PTR   i, nAdded;
  CGeVertex* pVer;
  CGeVertex* pVerLast;
  const CGePoint* pPnt = Pnt;

  nAdded = 0;
  if (nPnts){
    for (i=0; i<nPnts; ++i){
      if (m_bNoDuplicate){
        pVerLast = GetLastVer();
        if (pVerLast){
          if (pVerLast->m_Pos.IsEqual( *pPnt, g_GeTolerance )){
            // duplicate point
            ++pPnt;
            continue;
          }
        }
      }
      pVer = (CGeVertex*)m_Vers.Add( new CGeVertex( *pPnt ));
      if (pVer){
        ++m_NumVers;
        ++nAdded;
      }
      ++pPnt;
    }
  }
  return nAdded;
}

//-----------------------------------------------
void CGePolyline::SetVers (const CGePoint* Pnt, UINT_PTR nPnts, bool bClosed, bool bBackOrder)
{
  UINT_PTR i, j;
  const CGePoint* pPnt;

  pPnt = Pnt;
  Clear();
  if (nPnts >= 2){
    if (bBackOrder){
      if (bClosed){
        AddVer( *Pnt );
        j = nPnts - 1;
        pPnt = Pnt + j;
        for (i=0; i<nPnts; ++i){
          AddVer( *pPnt );
          --pPnt;
        }
      }else{
        j = nPnts - 1;
        pPnt = Pnt + j;
        for (i=0; i<nPnts; ++i){
          AddVer( *pPnt );
          --pPnt;
        }
      }
    }else{
      pPnt = Pnt;
      for (i=0; i<nPnts; ++i){
        AddVer( *pPnt );
        ++pPnt;
      }
    }
    if (nPnts > 2){
      m_bClosed = bClosed;
    }
  }
}

//-----------------------------------------------
void CGePolyline::CopyFrom (const CGePolyline& Pline2)
{
  CGeVertex* pVer2;

  Clear();
  pVer2 = Pline2.GetFirstVer();
  while( pVer2 ){
    AddVer( pVer2 );
    pVer2 = Pline2.GetNextVer( pVer2 );
  }
  m_bBold = Pline2.m_bBold;
  m_Zval = Pline2.m_Zval;
  SetClosed( Pline2.IsClosed() );
}

//-----------------------------------------------
void CGePolyline::Interpolate (int FitType, int Quality)
{
  switch( FitType ){
    case GE_PLFIT_QUAD:    Gen_BSpline2( Quality ); break;
    case GE_PLFIT_CUBIC:   Gen_BSpline3( Quality ); break;
    case GE_PLFIT_LINQUAD: Gen_LinQuad( Quality ); break;
  }
}

//-----------------------------------------------
void CGePolyline::Gen_BSpline2 (int Quality)
{
}

//-----------------------------------------------
void CGePolyline::Gen_BSpline3 (int Quality)
{
}

// Quality - number of lines per spline segment
//-----------------------------------------------
void CGePolyline::Gen_LinQuad (int Quality)
{
  CGeVertex* pFirst = GetFirstVer();
  CGeVertex* pLast = GetLastVer();
  CGeVertex* pVer = pFirst;
  CGeVertex* pVerPrev;
  CGeVertex* pVerNext;
  CGePolyline GenPline;
  CGePoint  A, B, C, Pt0;
  CGePoint  GenPt[MaxSplineQuality+4];
  CGePoint* pGenPt = NULL;
  UINT_PTR  nGen, i;
  bool      bClose = true;
  BOOL      bFixPrev, bFixNext;
  bool      bClosed = IsClosed();

  if (!bClosed){
    // for 
    pFirst->m_iParam = 1;
    pLast->m_iParam = 1;
  }
  if (m_NumVers == 2){
    GenPline.AddVer( pFirst->m_Pos );
    GenPline.AddVer( pLast->m_Pos );
    return;
  }
  pVerPrev = NULL;
  while( pVer ){
    pVerNext = GetNextVer( pVer );
    if (bClosed){
      if (pVerPrev == NULL){
        pVerPrev = pLast;
        if (pVer->m_iParam){
          Pt0 = pVer->m_Pos;
        }else{
          if (pVerPrev->m_iParam){
            bClose = false;
          }else{
            Pt0 = pLast->m_Pos.Center( pFirst->m_Pos );
          }
        }
      }
      if (pVerNext == NULL){
        pVerNext = pFirst;
      }
    }
    if (pVerPrev && pVerNext){
      if (pVer->m_iParam){
        GenPline.AddVer( pVer->m_Pos );
      }else{
        if (pVerPrev == pFirst){
          bFixPrev = true;
        }else{
          bFixPrev = (pVerPrev->m_iParam==0)?false:true;
        }
        if (bFixPrev){
          A = pVerPrev->m_Pos;
        }else{
          A = pVerPrev->m_Pos.Center( pVer->m_Pos );
        }
        B = pVer->m_Pos;
        if (pVerNext == pLast){
          bFixNext = true;
        }else{
          bFixNext = (pVerNext->m_iParam==0)?false:true;
        }
        if (bFixNext){
          C = pVerNext->m_Pos;
        }else{
          C = pVer->m_Pos.Center( pVerNext->m_Pos );
        }
        nGen = 0;
        geGenSegmentBSpline2( A, B, C, Quality, GenPt, &nGen, MaxSplineQuality+4 );
        pGenPt = GenPt;
        for (i=0; i<nGen; ++i){
          GenPline.AddVer( *pGenPt );  
          ++pGenPt;
        }
      }
    }else{
      if (pVerPrev == NULL){
        GenPline.AddVer( pVer->m_Pos );
      }
      if (pVerNext == NULL){
        GenPline.AddVer( pVer->m_Pos );
      }
    }
    pVerPrev = pVer;
    pVer = GetNextVer( pVer );
  }
  CopyFrom( GenPline );
}

  /********************************************************************
* Generates quadratic B-spline curve between control points A,B,C
*   nLines - number of lines per curve
*   GenVer - pointer to array of result points (point C is not added)
*   pnGenVer - pointer to number of points in GenVer
*   MaxGenVer - max number of points in GenVer
*********************************************************************/
bool geGenSegmentBSpline2 (const CGePoint& A, const CGePoint& B, const CGePoint& C, UINT_PTR nLines,
                            CGePoint* GenVer, UINT_PTR* pnGenVer, UINT_PTR MaxGenVer, double* pLen)
{
  double    t, t2, cf1, cf2, cf3;
  UINT_PTR  i, iVer;
  CGePoint* pGenVer;
  CGePoint  PrevPos;

  if (pLen){
    *pLen = 0.0;
  }
  iVer = *pnGenVer;
  if ((iVer + nLines) < MaxGenVer){
    pGenVer = GenVer + iVer;
    *pGenVer = A;
    PrevPos = A;
    ++pGenVer;
    ++iVer;
    for (i=1; i<nLines; ++i){
      t = (double)i / (double)nLines;
      t2 = 1.0 - t;
      cf1 = t2 * t2;
      cf2 = 2.0 * t * t2;
      cf3 = t * t;
      pGenVer->x = (cf1 * A.x) + (cf2 * B.x) + (cf3 * C.x);
      pGenVer->y = (cf1 * A.y) + (cf2 * B.y) + (cf3 * C.y);
      if (pLen){
        *pLen += PrevPos.Distance( *pGenVer );
        PrevPos = *pGenVer;
      }
      ++iVer;
      ++pGenVer;
    }
    if (pLen){
      *pLen += PrevPos.Distance( C );
      PrevPos = *pGenVer;
    }
    *pnGenVer = iVer;
    return true;
  }else{
    // no memory for all result points
    return false;
  }
}
