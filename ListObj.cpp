/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
********************************************************************/
#include "ListObj.h"
#include <stdlib.h>
#include <string.h>


//-----------------------------------------------
bool COkID::operator < (const COkID& h) const 
{
  if (m_Id < h.m_Id){
    return true;
  }
  return false;
}

//-----------------------------------------------
bool COkID::operator > (const COkID& h) const 
{
  if (m_Id > h.m_Id){
    return true;
  }
  return false;
}

//-----------------------------------------------
void COkID::SetStr (LPCWSTR szStr)
{
  m_Id = _wtoi64( szStr );
}

//-----------------------------------------------
void COkID::SetStr (LPCSTR szStr)
{
  m_Id = _atoi64( szStr );
}

//-----------------------------------------------
void COkID::ToStr (WCHAR* szID) const
{
  _ui64tow( m_Id, szID, 10 );
}

//-----------------------------------------------
void COkID::ToStr (char* szID) const
{
  _ui64toa( m_Id, szID, 10 );
}




//=============================================================================
COkList::COkList ()
{
  m_pFirst = NULL;
  m_pLast  = NULL;
  m_pCur   = NULL; 
  _p0=0;
  _p1=0;    
  _p2=0;
  _p3=0;
}


//-----------------------------------------------
COkList::~COkList ()
{
  Clear();
}

//-----------------------------------------------
void COkList::Clear ()
{
  if (m_pLast){
    while( m_pLast ){
      m_pCur  = m_pLast;
      m_pLast = m_pLast->m_pPrevObj;
      delete m_pCur;
    }
    m_pFirst = NULL;
    m_pLast  = NULL;
    m_pCur   = NULL; 
  }
}

//-----------------------------------------------
void COkList::Unlink ()
{
  if (m_pLast){
    while( m_pLast ){
      m_pCur  = m_pLast;
      m_pLast = m_pLast->m_pPrevObj;
      m_pCur->m_pNextObj = NULL;
      m_pCur->m_pPrevObj = NULL;
    }
    m_pFirst = NULL;
    m_pLast  = NULL;
    m_pCur   = NULL; 
  }
}


// delete all objects with ID==0
// returns a number of deleted objects
//-----------------------------------------------
UINT_PTR COkList::ClearZeroID ()
{
  UINT_PTR n=0;  // number of deleted items
  m_pCur = m_pFirst;
  while( m_pCur ){
    if (m_pCur->IsZeroID()){
      _p0 = m_pCur->m_pPrevObj;   // previous item
      _p2 = m_pCur->m_pNextObj;   // next item
      if (m_pCur == m_pFirst){
        m_pFirst = _p2;
      }
      if (m_pCur == m_pLast){
        m_pLast = _p0;
      }
      delete m_pCur;
      ++n;
      if (_p0){
        _p0->m_pNextObj = _p2;
      }
      if (_p2){
        _p2->m_pPrevObj = _p0;
      }
      m_pCur = _p2;
    }else{
      m_pCur = m_pCur->m_pNextObj;
    }
  }
  if (m_pFirst){
    m_pCur = m_pFirst;
  }else{
    m_pCur = m_pLast = NULL;
  }
  return n;
}


//-----------------------------------------------
COkObject* COkList::Add (COkObject* pNewObj)
{
  if (m_pLast){
    m_pCur = m_pLast;
    m_pLast->m_pNextObj = pNewObj;
    m_pLast = m_pLast->m_pNextObj;
    m_pLast->m_pPrevObj = m_pCur;
    m_pLast->m_pNextObj = NULL;
    m_pCur = m_pLast;
  }else{
    // the list is empty
    m_pCur = m_pFirst = m_pLast = pNewObj;
    m_pLast->m_pPrevObj = NULL;
    m_pLast->m_pNextObj = NULL;
  }
  return m_pCur;     
}

// add new object into beginning of the list
//-----------------------------------------------
COkObject* COkList::AddFirst (COkObject* pNewObj)
{
  if (m_pFirst){
    m_pCur = m_pFirst;
    m_pFirst->m_pPrevObj = pNewObj;
    pNewObj->m_pPrevObj = NULL;
    pNewObj->m_pNextObj = m_pCur;
    m_pFirst = m_pCur = pNewObj;
  }else{
    // the list is empty
    m_pCur = m_pFirst = m_pLast = pNewObj;
    m_pLast->m_pPrevObj = NULL;
    m_pLast->m_pNextObj = NULL;
  }
  return m_pCur;
}

// inserts object pObj after pObjPrev
// if pObjPrev==NULL, then pObj will be first object in a list
//-----------------------------------------------
COkObject* COkList::InsertAfter (COkObject* pObjPrev, COkObject* pNewObj)
{
  if (pObjPrev){
    if (pObjPrev->m_pNextObj){
      pObjPrev->m_pNextObj->m_pPrevObj = pNewObj;
      pNewObj->m_pNextObj = pObjPrev->m_pNextObj;
      pNewObj->m_pPrevObj = pObjPrev;
      pObjPrev->m_pNextObj = pNewObj;
      m_pCur = pNewObj;
    }else{
      // add to end
      return Add( pNewObj );
    }
  }else{
    // insert pObj at the beginning
    return AddFirst( pNewObj );
  }
  return m_pCur;
}

// inserts object pObj before pObj0
// if pObj0==NULL, then pObj will be last object in a list
//-----------------------------------------------
COkObject* COkList::InsertBefore (COkObject* pObj0, COkObject* pNewObj)
{
  if (pObj0){
    if (pObj0->m_pPrevObj){
      pNewObj->m_pPrevObj = pObj0->m_pPrevObj;
      pNewObj->m_pNextObj = pObj0;
      pObj0->m_pPrevObj->m_pNextObj = pNewObj;
      pObj0->m_pPrevObj = pNewObj;
      m_pCur = pNewObj;
    }else{
      // add to the beginning
      return AddFirst( pNewObj );
    }
  }else{
    // insert pObj at the end
    return Add( pNewObj );
  }
  return m_pCur;
}

//-----------------------------------------------
COkObject* COkList::Replace (COkObject* pObj, COkObject* pNewObj)
{
  if (m_pFirst == pObj){
    m_pFirst = pNewObj;
  }
  if (m_pLast == pObj){
    m_pLast = pNewObj;
  }
  if (m_pCur == pObj){
    m_pCur = pNewObj;
  }
  _p0 = pObj->m_pPrevObj;   // previous item
  _p2 = pObj->m_pNextObj;   // next item
  pNewObj->m_pPrevObj = _p0;
  pNewObj->m_pNextObj = _p2;
  if (_p0){
    _p0->m_pNextObj = pNewObj;
  }
  if (_p2){
    _p2->m_pPrevObj = pNewObj;
  }
  delete pObj;
  return pNewObj;
}

//-----------------------------------------------
// Swaps elements p2 with previous (p2->m_pPrevObj)
//-----------------------------------------------
void COkList::SwapRecs (COkObject* p2) 
{
  _p1 = p2->m_pPrevObj;
  if (_p1 && p2){
    _p0 = _p1->m_pPrevObj;
    _p3 = p2->m_pNextObj;

    p2->m_pPrevObj = _p1->m_pPrevObj;
    _p1->m_pNextObj = p2->m_pNextObj;
    p2->m_pNextObj = _p1;
    _p1->m_pPrevObj = p2;

    if (_p0){
      _p0->m_pNextObj = p2;
    }else{
      m_pFirst = p2;
    }
    if (_p3){
      _p3->m_pPrevObj = _p1;
    }else{
      m_pLast = _p1;
    }
  }
}


//-----------------------------------------------
// Specify object's handle and return pointer to this object or NULL
//-----------------------------------------------
COkObject* COkList::GetWithID (COkID id) const
{
  COkObject* pObj = GetFirst();
  while( pObj ){
    if (pObj->ID() == id){
      return pObj;  // object is found
    }
    pObj = GetNext( pObj );
  }
  return NULL;  // not found
}

//-----------------------------------------------
bool COkList::Find (const COkObject* p) const
{
  COkObject* pObj = GetFirst();
  while( pObj ){
    if (pObj == p){
      return true;  // object is found
    }
    pObj = GetNext( pObj );
  }
  return false;  // not found
}

//-----------------------------------------------
bool COkList::SetCurrent (COkObject* pObj)
{
  if (Find( pObj )){
    m_pCur = pObj;
    return true;
  }
  return false;  // object is found
}


//-----------------------------------------------
// Specify object's index in a list and return a pointer to this object or NULL
//-----------------------------------------------
COkObject* COkList::GetWithIndex (UINT_PTR index) const
{
  UINT_PTR i = 0;
  COkObject* pObj = GetFirst();
  while( pObj ){
    if (i == index){
      return pObj;  // object is found
    }
    ++i;
    pObj = GetNext( pObj );
  }
  return NULL;  // not found
}


//-----------------------------------------------
INT_PTR COkList::GetIndexOf (const COkObject* p) const
{
  INT_PTR index = 0;
  COkObject* pObj = GetFirst();
  if (p){
    while( pObj ){
      if (pObj == p){
        return index;
      }
      pObj = GetNext( pObj );
      ++index;
    }
  }
  return -1;  // not found
}

//-----------------------------------------------
COkID COkList::GetMaxID () const
{
  COkID IDmax;
  COkObject* pObj = GetFirst();
  while( pObj ){
    if (IDmax < pObj->ID()){
      IDmax = pObj->ID();
    }
    pObj = GetNext( pObj );
  }
  return IDmax;
}


//-----------------------------------------------
// Returns number of objects in the list
//-----------------------------------------------
UINT_PTR COkList::NumRecs () const
{
  UINT_PTR n=0;
  COkObject* pObj = m_pFirst;
  while( pObj ){
    ++n;
    pObj = pObj->m_pNextObj;
  }
  return n;
}

//-----------------------------------------------
bool COkList::HasNumRecs (UINT_PTR nRecs) const
{
  UINT_PTR n=0;
  COkObject* pObj = m_pFirst;
  if (pObj){
    if (nRecs == 0){
      return false;
    }
    while( pObj ){
      ++n;
      pObj = pObj->m_pNextObj;
      if (pObj == NULL){
        if (n == nRecs){
          return true;
        }
      }
    }
  }else{
    if (nRecs == 0){
      return true;
    }
  }
  return false;
}


// returns a number of deleted objects
//-----------------------------------------------
UINT_PTR COkList::RemoveID (COkID Id)
{
  UINT_PTR n=0;  // number of deleted items
  m_pCur = m_pFirst;
  while( m_pCur ){
    if (m_pCur->ID() == Id){
      _p0 = m_pCur->m_pPrevObj;   // previous item
      _p2 = m_pCur->m_pNextObj;   // next item
      if (m_pCur == m_pFirst){
        m_pFirst = _p2;
      }
      if (m_pCur == m_pLast){
        m_pLast = _p0;
      }
      delete m_pCur;
      ++n;
      if (_p0){
        _p0->m_pNextObj = _p2;
      }
      if (_p2){
        _p2->m_pPrevObj = _p0;
      }
      m_pCur = _p2;
    }else{
      m_pCur = m_pCur->m_pNextObj;
    }
  }
  if (m_pFirst){
    m_pCur = m_pFirst;
  }else{
    m_pCur = m_pLast = NULL;
  }
  return n;
}


//-----------------------------------------------
bool COkList::Remove (const COkObject* pObj)
{
  bool bRet = false;
  m_pCur = m_pFirst;
  while( m_pCur ){
    if (m_pCur == pObj){
      _p0 = m_pCur->m_pPrevObj;   // previous item
      _p2 = m_pCur->m_pNextObj;   // next item
      if (m_pCur==m_pFirst){
        m_pFirst = _p2;
      }
      if (m_pCur==m_pLast){
        m_pLast = _p0;
      }
      delete m_pCur;
      if (_p0){
        _p0->m_pNextObj = _p2;
        m_pCur = _p0;
      }
      if (_p2){
        _p2->m_pPrevObj = _p0;
        m_pCur = _p2;
      }
      bRet = true;
      break;
    }else{
      m_pCur = m_pCur->m_pNextObj;
    }
  }
  if (m_pFirst == NULL){
    m_pCur = m_pLast = NULL;
  }
  return bRet;
}

// Remove first object from the list
//-----------------------------------------------
bool COkList::RemoveFirst ()
{
  if (m_pFirst){
    _p0 = m_pFirst;
    if (m_pLast == m_pFirst){
      m_pLast = NULL;
      m_pFirst = NULL;
      m_pCur = NULL;
    }else{
      m_pFirst = m_pFirst->m_pNextObj;
      m_pFirst->m_pPrevObj = NULL;
      if (m_pCur == _p0){
        m_pCur = m_pFirst;
      }
    }
    delete _p0;
    return true;
  }
  return false;
}

// Remove last object from the list
//-----------------------------------------------
bool COkList::RemoveLast ()
{
  if (m_pLast){
    _p0 = m_pLast;
    if (m_pLast == m_pFirst){
      m_pLast = NULL;
      m_pFirst = NULL;
      m_pCur = NULL;
    }else{
      m_pLast = m_pLast->m_pPrevObj;
      m_pLast->m_pNextObj = NULL;
      if (m_pCur == _p0){
        m_pCur = m_pLast;
      }
    }
    delete _p0;
    return true;
  }
  return false;
}

//-----------------------------------------------
void COkList::SortByFunc (OK_FUN_SORTLIST pFuncCmp)
{
  COkObject* a1;
  COkObject* a2;
  COkObject* b1;
  COkObject* b2;
  int i=0, j=0;

  if (m_pFirst == NULL){
    return;
  }
  if (m_pFirst->m_pNextObj == NULL){
    return;
  }
  a1 = m_pFirst;
  a2 = a1->m_pNextObj;
  while( a2 ){
    ++i;
    if ((*pFuncCmp)(a1,a2) > 0){
      SwapRecs( a2 );
      b2 = a2;
      b1 = b2->m_pPrevObj;
      j = 0;
      while( b1 ){
        ++j;
        if ((*pFuncCmp)(b1,b2) > 0){
          SwapRecs( b2 );
          b1 = b2->m_pPrevObj;
          continue;
        }
        b2 = b1;
        b1 = b2->m_pPrevObj;
      }
      a2 = a1->m_pNextObj;
      continue;
    }
    a1 = a2;
    a2 = a1->m_pNextObj;
  }
}


//-----------------------------------------------
void COkList::SortByID ()
{
  COkObject* a1;
  COkObject* a2;
  COkObject* b1;
  COkObject* b2;
  int i=0, j=0;

  a1 = m_pFirst;
  a2 = a1->m_pNextObj;
  while( a2 ){
    ++i;
    if (a1->ID() > a2->ID()){
      SwapRecs( a2 );
      b2 = a2;
      b1 = b2->m_pPrevObj;
      j = 0;
      while( b1 ){
        ++j;
        if (b1->ID() > b2->ID()){
          SwapRecs( b2 );
          b1 = b2->m_pPrevObj;
          continue;
        }
        b2 = b1;
        b1 = b2->m_pPrevObj;
      }
      a2 = a1->m_pNextObj;
      continue;
    }
    a1 = a2;
    a2 = a1->m_pNextObj;
  }
}


//-----------------------------------------------
bool COkList::Move (COkObject* pObj, COkList& List2)
{
  m_pCur = m_pFirst;
  while( m_pCur ){
    if (m_pCur == pObj){
      _p0 = m_pCur->m_pPrevObj;   // previous item
      _p2 = m_pCur->m_pNextObj;   // next item
      if (m_pCur==m_pFirst){
        m_pFirst = _p2;
      }
      if (m_pCur==m_pLast){
        m_pLast = _p0;
      }
      if (_p0){
        _p0->m_pNextObj = _p2;
      }
      if (_p2){
        _p2->m_pPrevObj = _p0;
      }
      // add object to other list
      if (List2.m_pLast){
        List2.m_pCur = List2.m_pLast;
        List2.m_pLast->m_pNextObj = pObj;
        List2.m_pLast = List2.m_pLast->m_pNextObj;
        List2.m_pLast->m_pPrevObj = List2.m_pCur;
        List2.m_pLast->m_pNextObj = NULL;
        List2.m_pCur = List2.m_pLast;
      }else{
        // the other list is empty
        List2.m_pCur = List2.m_pFirst = List2.m_pLast = pObj;
        List2.m_pLast->m_pPrevObj = NULL;
        List2.m_pLast->m_pNextObj = NULL;
      }
      m_pCur = m_pFirst;
      return true;
    }else{
      m_pCur = m_pCur->m_pNextObj;
    }
  }
  m_pCur = m_pFirst;
  return false;
}


// move all objects from List2 to the end of this list
//-----------------------------------------------
void COkList::Append (COkList& List2)
{
  if (m_pLast){
    m_pCur = m_pLast;
    m_pCur->m_pNextObj = List2.m_pFirst;
    m_pCur->m_pNextObj->m_pPrevObj = m_pCur;
    m_pLast = List2.m_pLast;
  }else{
    // the list is empty
    m_pFirst = List2.m_pFirst;
    m_pLast = List2.m_pLast;
  }
  m_pCur = m_pLast;
  List2.m_pFirst = NULL;
  List2.m_pLast = NULL;
  List2.m_pCur = NULL; 
}


// replace all objects in this list with the objects from List2
// must be replaced objects with the same ID
// All objects in the List2 have the same ID
//-----------------------------------------------
void COkList::Replace (COkList& List2)
{
  COkID Id = List2.GetFirst()->ID();
  RemoveID( Id );
  if (m_pFirst){
    if (_p0 && _p2){
      _p0->m_pNextObj = List2.m_pFirst;
      _p0->m_pNextObj->m_pPrevObj = _p0;
      List2.m_pLast->m_pNextObj = _p2;
      _p2->m_pPrevObj = List2.m_pLast;
    }else{
      if (_p0 == NULL){
        // put List2 at the start of this list
        m_pFirst = List2.m_pFirst;
        List2.m_pLast->m_pNextObj = _p2;
        _p2->m_pPrevObj = List2.m_pLast;
      }else{
        if (_p2 == NULL){
          // put List2 at the end of this list
          _p0->m_pNextObj = List2.m_pFirst;
          _p0->m_pNextObj->m_pPrevObj = _p0;
          m_pLast = List2.m_pLast;
        }
      }
    }
  }else{
    // list is empty
    m_pFirst = List2.m_pFirst;
    m_pLast = List2.m_pLast;
  }
  m_pCur = m_pLast;
  List2.m_pFirst = NULL;
  List2.m_pLast = NULL;
  List2.m_pCur = NULL; 
}



// first entity is a bottom
// last entity is a top
// This function moves pObj to the end of the list
//-----------------------------------------------
bool COkList::ObjToTop (COkObject* pObj)
{
  if (pObj == NULL){
    return false;
  }
  _p1 = pObj->m_pNextObj;
  if (_p1 == NULL){
    // the object is on-top, therefore don't need to move it
    return true;
  }
  _p0 = pObj->m_pPrevObj;
  if (_p0 == NULL){   // pObj is the first object in the list
    m_pFirst = _p1;
    _p1->m_pPrevObj = NULL;
  }else{
    _p0->m_pNextObj = _p1;
    _p1->m_pPrevObj = _p0;
  }
  // move the object to the end of the list
  pObj->m_pNextObj = NULL;
  pObj->m_pPrevObj = m_pLast;
  m_pLast->m_pNextObj = pObj;
  m_pLast = pObj;
  m_pCur = m_pLast;
  return true;
}


// This function moves pObj to the start of the list
//-----------------------------------------------
bool COkList::ObjToBottom (COkObject* pObj)
{
  if (pObj == NULL){
    return false;
  }
  _p0 = pObj->m_pPrevObj;
  if (_p0 == NULL){
    // the object is on-bottom, therefore don't need to move it
    return true;
  }
  _p1 = pObj->m_pNextObj;
  if (_p1 == NULL){ // pObj is the last object in the list
    m_pLast = _p0;
    _p0->m_pNextObj = NULL;
  }else{
    _p1->m_pPrevObj = _p0;
    _p0->m_pNextObj = _p1;
  }
  // add the object to the start of the list
  pObj->m_pPrevObj = NULL;
  pObj->m_pNextObj = m_pFirst;
  m_pFirst->m_pPrevObj = pObj;
  m_pFirst = pObj; 
  m_pCur = m_pFirst;
  return true;
}

//-----------------------------------------------
bool COkList::ObjToAbove (COkObject* pObj, COkObject* pRefObj)
{
  if (pObj == NULL || pRefObj == NULL || pObj == pRefObj){
    return false;
  }
  _p2 = pRefObj->m_pNextObj;
  if (_p2 == NULL){
    // pRefObj is the last object
    return ObjToTop( pObj );
  }
  if (_p2 == pObj){
    // already above
    return false;
  }
  _p0 = pObj->m_pPrevObj;
  _p1 = pObj->m_pNextObj;
  if (_p0){
    if (_p1){
      _p0->m_pNextObj = _p1;
      _p1->m_pPrevObj = _p0;
    }else{
      // pObj is the last object
      m_pLast = _p0;
      _p0->m_pNextObj = NULL;
    }
  }else{
    // pObj is the first object
    m_pFirst = _p1;
    _p1->m_pPrevObj = NULL;
  }
  pRefObj->m_pNextObj = pObj;
  pObj->m_pPrevObj = pRefObj;
  pObj->m_pNextObj = _p2;
  _p2->m_pPrevObj = pObj;
  m_pCur = pObj; 
  return true;
}

//-----------------------------------------------
bool COkList::ObjToUnder (COkObject* pObj, COkObject* pRefObj)
{
  if (pObj == NULL || pRefObj == NULL || pObj == pRefObj){
    return false;
  }
  _p2 = pRefObj->m_pPrevObj;
  if (_p2 == NULL){
    // pRefObj is the first object
    return ObjToBottom( pObj );
  }
  if (_p2 == pObj){
    // already under
    return false;
  }
  _p0 = pObj->m_pPrevObj;
  _p1 = pObj->m_pNextObj;
  if (_p0){
    if (_p1){
      _p0->m_pNextObj = _p1;
      _p1->m_pPrevObj = _p0;
    }else{
      // pObj is the last object
      m_pLast = _p0;
      _p0->m_pNextObj = NULL;
    }
  }else{
    // pObj is the first object
    m_pFirst = _p1;
    _p1->m_pPrevObj = NULL;
  }

  pRefObj->m_pPrevObj = pObj;
  pObj->m_pNextObj = pRefObj;
  pObj->m_pPrevObj = _p2;
  _p2->m_pNextObj = pObj;
  m_pCur = pObj; 
  return true;
}


//-----------------------------------------------
bool COkList::ObjSwap (COkObject* pObj1, COkObject* pObj2)
{
  m_pCur = m_pFirst;
  while( m_pCur ){
    if (m_pCur == pObj1){
      m_pCur = m_pFirst;
      while( m_pCur ){
        if (m_pCur == pObj2){
          if (pObj1->m_pNextObj == pObj2){
            _p0 = pObj1->m_pPrevObj;
            _p1 = pObj2->m_pNextObj;
            pObj2->m_pPrevObj = _p0;
            pObj2->m_pNextObj = pObj1;
            pObj1->m_pPrevObj = pObj2;
            pObj1->m_pNextObj = _p1;
            if (_p0){
              _p0->m_pNextObj = pObj2;
            }
            if (_p1){
              _p1->m_pPrevObj = pObj1;
            }
            goto m1;
          }
          if (pObj2->m_pNextObj == pObj1){
            _p0 = pObj2->m_pPrevObj;
            _p1 = pObj1->m_pNextObj;
            pObj1->m_pPrevObj = _p0;
            pObj1->m_pNextObj = pObj2;
            pObj2->m_pPrevObj = pObj1;
            pObj2->m_pNextObj = _p1;
            if (_p0){
              _p0->m_pNextObj = pObj1;
            }
            if (_p1){
              _p1->m_pPrevObj = pObj2;
            }
            goto m1;
          }
          _p0 = pObj1->m_pPrevObj;
          _p1 = pObj1->m_pNextObj;
          _p2 = pObj2->m_pPrevObj;
          _p3 = pObj2->m_pNextObj;
          pObj1->m_pPrevObj = _p2;
          pObj1->m_pNextObj = _p3;
          pObj2->m_pPrevObj = _p0;
          pObj2->m_pNextObj = _p1;
          if (_p0){
            _p0->m_pNextObj = pObj2;
          }
          if (_p1){
            _p1->m_pPrevObj = pObj2;
          }
          if (_p2){
            _p2->m_pNextObj = pObj1;
          }
          if (_p3){
            _p3->m_pPrevObj = pObj1;
          }
          goto m1;
        }
        m_pCur = m_pCur->m_pNextObj;
      }
      break;
    }
    m_pCur = m_pCur->m_pNextObj;
  }
  m_pCur = m_pFirst;
  return false;
m1:
  if (m_pFirst == pObj1){
    m_pFirst = pObj2;
  }else{
    if (m_pFirst == pObj2){
      m_pFirst = pObj1;
    }
  }
  if (m_pLast == pObj1){
    m_pLast = pObj2;
  }else{
    if (m_pLast == pObj2){
      m_pLast = pObj1;
    }
  }
  m_pCur = m_pFirst;
  return true;
}

// objects from pObj to last object move after pRefObj
//-----------------------------------------------
bool COkList::TailToAbove (COkObject* pObj, COkObject* pRefObj)
{
  if (pObj == NULL || pRefObj == NULL || pObj == pRefObj){
    return false;
  }
  _p0 = pObj->m_pPrevObj;
  if (_p0 == pRefObj){
    // don't need to change the list
    return true;
  }
  _p2 = pRefObj->m_pNextObj;
  pRefObj->m_pNextObj = pObj;
  pObj->m_pPrevObj = pRefObj;
  m_pLast->m_pNextObj = _p2;
  _p2->m_pPrevObj = m_pLast;
  _p0->m_pNextObj = NULL;
  m_pLast = _p0;
  m_pCur = pObj; 
  return true;
}


struct COkObjectPtr {
  COkObject* m_pObj;
};

// reverse the list
//-----------------------------------------------
void COkList::Reverse ()
{
  COkObjectPtr* Buf;
  COkObject*    pObj;
  UINT_PTR      nRecs = NumRecs();
  UINT_PTR      i = 0;
  UINT_PTR      n2 = nRecs - 1;

  if (nRecs > 1){
    Buf = (COkObjectPtr*)calloc( nRecs+1, sizeof(COkObjectPtr) );
    if (Buf == NULL){
      return;
    }
    pObj = m_pLast;
    while( pObj ){
      Buf[i].m_pObj = pObj;
      ++i;
      pObj = pObj->m_pPrevObj;
    }
    // make reversed list
    for (i=0; i<nRecs; ++i){
      m_pCur = Buf[i].m_pObj;
      if (i > 0){
        m_pCur->m_pPrevObj = Buf[i-1].m_pObj;
      }else{
        m_pCur->m_pPrevObj = NULL;
      }
      if (i < n2){
        m_pCur->m_pNextObj = Buf[i+1].m_pObj;
      }else{
        m_pCur->m_pNextObj = NULL;
      }
    }
    m_pCur = m_pFirst = Buf[0].m_pObj;
    m_pLast = Buf[n2].m_pObj;
    free( Buf );
  }
}

// move first item into back
//-----------------------------------------------
void COkList::ShiftForward ()
{
  if (m_pFirst == NULL){
    return;
  }
  _p0 = m_pFirst; 
  _p1 = m_pFirst->m_pNextObj;
  if (_p1 == NULL){
    // only one record in the list
    return;
  }
  _p2 = m_pLast; 
  if (_p1 == m_pLast){
    // only two records in the list, swap them
    m_pFirst = _p2;
    m_pLast  = _p0;
    m_pFirst->m_pPrevObj = NULL;
    m_pFirst->m_pNextObj = m_pLast;
    m_pLast->m_pPrevObj = m_pFirst;
    m_pLast->m_pNextObj = NULL;
    m_pCur = m_pFirst;
    return;
  }
  if ((m_pCur == m_pFirst) || (m_pCur == m_pLast)){
    m_pCur = NULL;
  }
  _p1->m_pPrevObj = NULL;
  m_pFirst = _p1;
  _p2->m_pNextObj = _p0;
  _p0->m_pPrevObj = _p2;
  _p0->m_pNextObj = NULL;
  m_pLast = _p0;
  if (m_pCur == NULL){
    m_pCur = m_pFirst;
  }
}


// get a pointer to second item of the list
//-----------------------------------------------
COkObject* COkList::GetSecond () const
{
  COkObject* pRec;
  if (m_pFirst){
    pRec = m_pFirst->m_pNextObj;
  }else{
    pRec = NULL;
  }
  return pRec;
}
