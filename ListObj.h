/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Double-linked List of objects
********************************************************************/
#ifndef _OK_LISTOBJ_
#define _OK_LISTOBJ_

#ifndef _OK_BASETYPES_
  #include "okBaseTypes.h"
#endif

class COkList;

//*******************************************************************
// Identifier of object (64-bit)
//*******************************************************************
class COkID {
  UINT64  m_Id;
public: 
    COkID ();
    COkID (UINT64 Id);

  void Init   ();
  void Clear  ();
  void Set    (UINT64 Id);
  void SetInt (UINT_PTR Id);    // obsolete, remains for compatibility
  void SetStr (LPCWSTR szStr);  // as decimal value
  void SetStr (LPCSTR szStr);   // as decimal value
  void Increment ();
  void Decrement ();

  UINT64  Value     () const;
  bool    IsNull    () const;
  bool    IsNotNull () const;
  bool    IsZero    () const;
  void    ToStr (WCHAR* szID) const;   // as decimal value
  void    ToStr (char* szID) const;    // as decimal value

  bool   operator == (const COkID& n) const;
  bool   operator != (const COkID& n) const;
  bool   operator >  (const COkID& n) const;
  bool   operator <  (const COkID& n) const;
};

//-----------------------------------------------
inline        COkID::COkID  () {m_Id=0;}
inline        COkID::COkID  (UINT64 Id) {m_Id=Id;}
inline void   COkID::Init   () {m_Id=0;}
inline void   COkID::Clear  () {m_Id=0;}
inline void   COkID::Set    (UINT64 Id) {m_Id=Id;}
inline void   COkID::SetInt (UINT_PTR Id) {m_Id=Id;}
inline UINT64 COkID::Value  () const {return m_Id;}
inline bool   COkID::IsNull () const {return (m_Id)?false:true;}
inline bool   COkID::IsNotNull () const {return (m_Id)?true:false;}
inline bool   COkID::IsZero () const {return (m_Id)?false:true;}
inline bool   COkID::operator == (const COkID& h) const {return (m_Id==h.m_Id);}
inline bool   COkID::operator != (const COkID& h) const {return (m_Id!=h.m_Id);}
inline void   COkID::Increment () {++m_Id;}
inline void   COkID::Decrement () {if (m_Id>0) --m_Id;}



//*******************************************************************
// Base object class (record of the list)  20/28 bytes
//*******************************************************************
class COkObject {
  WORD        m_ObjType;        // 2b
  WORD        m_ObjCode;        // 2b
  COkID       m_Handle;         // 8b   object's identifier
  COkObject*  m_pNextObj;       // 4/8b pointer to next item
  COkObject*  m_pPrevObj;       // 4/8b pointer to previous item
public:
    COkObject ();
    COkObject (COkID id, UINT_PTR Type, UINT_PTR Code);
    COkObject (COkID id, const COkObject* pObj);
    virtual ~COkObject ();

  void  SetID      (const COkID& id);
  void  SetID      (UINT64 id);
  void  SetHandle  (const COkID& id);
  void  SetHandle  (UINT64 id);
  void  SetZeroID  ();   
  void  SetObjType (UINT_PTR t);
  void  SetObjCode (UINT_PTR t);
  void  InverseObjCode ();

  COkID    ID       () const;
  COkID    Handle   () const;
  UINT_PTR ObjType  () const;
  UINT_PTR ObjCode  () const;
  bool     IsZeroID () const;

  const COkObject* Prev() const {return m_pPrevObj;}
  const COkObject* Next() const {return m_pNextObj;}

  bool IsFirst () const {return m_pPrevObj?false:true;}
  bool IsLast  () const {return m_pNextObj?false:true;}

friend class COkList;
};

//-----------------------------------------------
inline COkObject::COkObject  () {
  m_pNextObj=0; m_pPrevObj=0; m_ObjType=0; m_ObjCode=0;
}
inline COkObject::COkObject  (COkID id, UINT_PTR Type, UINT_PTR Code) {
  m_Handle=id; m_pNextObj=0; m_pPrevObj=0; m_ObjType=(USHORT)Type; m_ObjCode=(USHORT)Code;
}
inline COkObject::COkObject  (COkID id, const COkObject* pObj) {
  m_Handle=id; m_pNextObj=pObj->m_pNextObj; m_pPrevObj=pObj->m_pPrevObj; 
  m_ObjType = pObj->m_ObjType; m_ObjCode = pObj->m_ObjCode;
}
inline          COkObject::~COkObject () {}
inline UINT_PTR COkObject::ObjType    () const {return m_ObjType;}
inline UINT_PTR COkObject::ObjCode    () const {return m_ObjCode;}
inline void     COkObject::SetID      (const COkID& id) {m_Handle=id;}
inline void     COkObject::SetID      (UINT64 id) {m_Handle.Set(id);}
inline void     COkObject::SetHandle  (const COkID& id) {m_Handle=id;}
inline void     COkObject::SetHandle  (UINT64 id) {m_Handle.Set(id);}
inline void     COkObject::SetZeroID  () {m_Handle.Clear();}
inline void     COkObject::SetObjType (UINT_PTR t) {m_ObjType=(USHORT)t;}
inline void     COkObject::SetObjCode (UINT_PTR c) {m_ObjCode=(USHORT)c;}
inline void     COkObject::InverseObjCode () {m_ObjCode=~m_ObjCode;}
inline COkID    COkObject::Handle     () const {return m_Handle;}
inline COkID    COkObject::ID         () const {return m_Handle;}
inline bool     COkObject::IsZeroID   () const {return m_Handle.IsNull();}


// function used to sort objects in a list
typedef int (_stdcall* OK_FUN_SORTLIST) (const COkObject* pObj1, const COkObject* pObj2);


//*******************************************************************
// List of COkObject class objects
//*******************************************************************
class COkList {
  COkObject* m_pFirst;      // pointer to first object of the list
  COkObject* m_pLast;       // pointer to last object in the list
  COkObject* m_pCur;        // pointer to current object

  COkObject* _p0;    // temporary variables used for sorting and deleting objects
  COkObject* _p1;
  COkObject* _p2;
  COkObject* _p3;
public:
    COkList ();
    virtual ~COkList ();

  void       Clear        ();
  UINT_PTR   ClearZeroID  ();
  COkObject* Add          (COkObject* pNewObj);
  COkObject* AddFirst     (COkObject* pNewObj);
  COkObject* InsertBefore (COkObject* pObj, COkObject* pNewObj);
  COkObject* InsertAfter  (COkObject* pObj, COkObject* pNewObj);
  COkObject* Replace      (COkObject* pObj, COkObject* pNewObj);
  UINT_PTR   RemoveID     (COkID Id);
  bool       Remove       (const COkObject* pObj);
  bool       RemoveFirst  ();
  bool       RemoveLast   ();
  bool       Move         (COkObject* pObj, COkList& List2);
  COkObject* GetFirst     () const;
  COkObject* GetSecond    () const;
  COkObject* GetLast      () const;
  COkObject* GetPrev      (const COkObject* p) const;
  COkObject* GetNext      (const COkObject* p) const;
  bool       IsFirst      (const COkObject* p) const;
  bool       IsLast       (const COkObject* p) const;
  bool       IsEmpty      () const;
  bool       IsNotEmpty   () const;
  UINT_PTR   NumRecs      () const;
  bool       HasNumRecs   (UINT_PTR nRecs) const;
  COkObject* Find         (COkID Id) const;
  bool       Find         (const COkObject* p) const;
  COkObject* GetWithID    (COkID Id) const;
  COkObject* GetWithIndex (UINT_PTR index) const;
  COkObject* GetCurrent   () const;
  bool       SetCurrent   (COkObject* p);
  INT_PTR    GetIndexOf   (const COkObject* p) const;
  COkID      GetMaxID     () const;
  void       SortByFunc   (OK_FUN_SORTLIST pFuncCmp);
  void       SortByID     ();
  void       Reverse      ();
  void       ShiftForward ();
  // Swaps element p2 with previous (p2->pPrev)
  void       SwapRecs     (COkObject* p2);

  // first entity is a bottom, last entity is a top
  bool       ObjToTop     (COkObject* pObj);
  bool       ObjToBottom  (COkObject* pObj);
  bool       ObjToAbove   (COkObject* pObj, COkObject* pRefObj);
  bool       ObjToUnder   (COkObject* pObj, COkObject* pRefObj);
  bool       ObjSwap      (COkObject* pObj1, COkObject* pObj2);
  bool       TailToAbove  (COkObject* pObj, COkObject* pRefObj);

  void       Append       (COkList& List2);
  void       Replace      (COkList& List2);

  void  Unlink();
};

//-----------------------------------------------
inline COkObject* COkList::GetFirst  () const {return m_pFirst;}
inline COkObject* COkList::GetLast   () const {return m_pLast;}
inline COkObject* COkList::GetPrev   (const COkObject* p) const {return p->m_pPrevObj;}
inline COkObject* COkList::GetNext   (const COkObject* p) const {return p->m_pNextObj;}
inline COkObject* COkList::Find      (COkID Id) const {return GetWithID(Id);}
inline COkObject* COkList::GetCurrent() const {return m_pCur;}
inline bool       COkList::IsEmpty   () const {return (m_pFirst)? false : true;}
inline bool       COkList::IsNotEmpty() const {return (m_pFirst)? true : false;}
inline bool       COkList::IsFirst   (const COkObject* p) const {return (p==m_pFirst)?true:false;}
inline bool       COkList::IsLast    (const COkObject* p) const {return (p==m_pLast)?true:false;}






/********************************************************************
* Project: Config.Lib
* Copyright (C) 2009-2024, Oleg Kolbaskin
* All rights reserved.
*
* class COkConfig - used to store app config values
********************************************************************/

//-------------------------------------
class CConfigItem {
  WCHAR* m_szKey;
  WCHAR* m_szValue;
public:
    CConfigItem ();
    ~CConfigItem ();

  void    Set      (LPCWSTR szKey, LPCWSTR szValue);
  void    Get      (WCHAR* szKey, WCHAR* szValue) const;
  LPCWSTR Key      () const {return m_szKey;}
  LPCWSTR Value    () const {return m_szValue;}
  bool    GetValue (LPCWSTR szKey, WCHAR* szValue, UINT_PTR nMaxChars) const;
};


//-------------------------------------
class COkConfig {
  WCHAR        m_szFileName[256];
  CConfigItem* m_Item;
  UINT_PTR     m_nItems;    
public:
    COkConfig ();
    ~COkConfig ();

  void    SetFileName (LPCWSTR szFileName);
  LPCWSTR FileName    () const;

  void    Clear        ();
  bool    Read         (LPCWSTR szFileName=0, const char* szSignature=0);
  bool    GetValue     (LPCWSTR szKey, WCHAR* szValue, UINT_PTR nMaxChars) const;
  bool    GetValueBool (LPCWSTR szKey, bool DefVal) const;
  int     GetValueInt  (LPCWSTR szKey, int DefVal, int MinVal=0, int MaxVal=0) const;
  double  GetValueDbl  (LPCWSTR szKey, double DefVal, double MinVal=0.0, double MaxVal=0.0) const;
};

inline LPCWSTR COkConfig::FileName () const {return m_szFileName;}


#endif  // _OK_LISTOBJ_

