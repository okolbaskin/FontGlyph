/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Class COkCtrlGrid - Grid GUI control
********************************************************************/
#ifndef _OK_CTRLGRID_
#define _OK_CTRLGRID_

#ifndef _INC_COMMCTRL
  #include <commctrl.h>
#endif
  #include "ListObj.h"


#define OK_GRID_VAR_TEXT        0
#define OK_GRID_VAR_RTEXT       1    // right aligned text
#define OK_GRID_VAR_LIST        2
#define OK_GRID_VAR_OWNERDRAW   3

#define OK_GRID_GROUPCHANGE     0x100   // allows to assign one value to a column in all selected rows

#define OK_GRID_BUFLEN   2048  // max size of text field


//-----------------------------------------------
class COkTColumn {
  UINT   m_ID;
  WCHAR* m_szName;
  UINT   m_Width;
  UINT   m_VarType;       // one of the LC_GRID_VAR_... + LC_GRID_GROUPCHANGE
  bool   m_bSortable;     // if TRUE, a column can be sorted  
public:
    COkTColumn ();
    COkTColumn (UINT Id, LPCWSTR szName, UINT Width);
    ~COkTColumn ();

  void SetID       (UINT id);
  void SetName     (LPCWSTR szName);
  void SetWidth    (UINT Width);
  void SetVarType  (UINT Type);
  void SetSortable (bool bSort=true);

  UINT ID            () const;
  void GetName       (WCHAR* szBuf) const;
  UINT Width         () const;
  UINT VarType       () const;
  bool IsGroupChange () const;
  bool IsSortable    () const;
};

inline UINT COkTColumn::ID () const {return m_ID;}
inline UINT COkTColumn::Width () const {return m_Width;}
inline UINT COkTColumn::VarType () const {return m_VarType&0xFF;}
inline bool COkTColumn::IsGroupChange () const {return (m_VarType&OK_GRID_GROUPCHANGE)? true : false;}
inline bool COkTColumn::IsSortable () const {return m_bSortable;}

class COkCtrlGrid;

//-----------------------------------------------
class COkTRow : public COkObject {
  bool m_bSelect;
  bool m_bSelect2;
public:
  COLORREF m_AltBkColor;  // alternative background color, if not 0xFFFFFFFF
  COLORREF m_AltFgColor;  // alternative foreground color, if not 0xFFFFFFFF
  bool     m_bReadOnly;   // if TRUE, then the row can not be edited  
public:
    COkTRow (UINT_PTR id=0);

  void SetSelect    (bool b);
  bool IsSelected   () const;
  bool IsSelected2  () const;
  void UpdateSelect ();

  virtual bool GetValue    (const COkCtrlGrid* pGrid, UINT idColumn, WCHAR* szValue, UINT MaxChars) = 0;
  virtual bool SetValue    (const COkCtrlGrid* pGrid, UINT idColumn, LPCWSTR szValue) = 0;
  virtual int  CompareWith (COkTRow* pRow2, UINT idColumn, bool bSortUp) = 0;
};

inline bool COkTRow::IsSelected   () const {return m_bSelect;}
inline bool COkTRow::IsSelected2  () const {return m_bSelect2;}
inline void COkTRow::UpdateSelect () {m_bSelect2=m_bSelect;}

typedef void (CALLBACK* OK_FUN_GRIDDRAWCELL) (HDC hDC, const RECT& rect, COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn);
typedef void (CALLBACK* OK_FUN_GRIDCLICK)  (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, bool* pbEdit);
typedef void (CALLBACK* OK_FUN_GRIDRCLICK) (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn);
typedef void (CALLBACK* OK_FUN_GRIDDBLCLK) (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, bool* pbEdit);
typedef void (CALLBACK* OK_FUN_GRIDAPPEND) (COkCtrlGrid* pGrid);
typedef void (CALLBACK* OK_FUN_GRIDINSERT) (COkCtrlGrid* pGrid, COkTRow* pRow);
typedef void (CALLBACK* OK_FUN_GRIDDELETE) (COkCtrlGrid* pGrid, COkTRow* pRow);
typedef void (CALLBACK* OK_FUN_GRIDENTER)  (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, bool* pbEdit);
typedef bool (CALLBACK* OK_FUN_GRIDKEY)    (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, UINT Key);
typedef void (CALLBACK* OK_FUN_GRIDSELROW) (COkCtrlGrid* pGrid, COkTRow* pRow);
typedef void (CALLBACK* OK_FUN_GRIDLINUM)  (COkCtrlGrid* pGrid, UINT_PTR idColumn, UINT* pNumItems);
typedef void (CALLBACK* OK_FUN_GRIDLIGET)  (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, UINT iItem, LRESULT* pValue, bool* pbCurrent);
typedef void (CALLBACK* OK_FUN_GRIDLISET)  (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, LRESULT Value);
typedef void (CALLBACK* OK_FUN_GRIDLIDRAW) (COkCtrlGrid* pGrid, COkTRow* pRow, UINT idColumn, LRESULT Value, HDC hDC, const RECT& rect, bool bSelected);

//-----------------------------------------------
class COkCtrlGrid {
  HINSTANCE   m_hInst;               // module instance
  HWND        m_hwFrame;
  HWND        m_hwListView;          // handle to "ListView" window
  HWND        m_hwHeader;            // header of the listview
  HWND        m_hwEdit;              // handle to editbox
  HWND        m_hwCbox;              // handle to combobox
  DWORD       m_WndExStyle;
  DWORD       m_WndStyle;
  int         m_iEditItem;
  int         m_iEditColumn;
  RECT        m_EditRect;
  COkTColumn* m_Column;              // array of columns
  int         m_nColumns;            // number of columns
  int         m_MaxColumns;          // max. number of columns
  COkList     m_Rows;                // list of rows
  int         m_nRows;
  COkTRow*    m_pCurRow;             // pointer to current row
  int         m_iCurRow;             // index of current row
  int         m_iCurColumn;          // index of current column
  int         m_iSortColumn;         // index of sorted column
  bool        m_bSortUp;             // TRUE - sortUp FALSE - sortDown
  HBITMAP     m_hBmpSortDown;
  HBITMAP     m_hBmpSortUp;
  COkTRow*    m_pDragRow;            // dragged row
  bool        m_bDragging;
  HWND        m_hDropWnd;
  HWND        m_hDropWndMsg;
  WPARAM      m_DropWParam;
  LPARAM      m_DropLParam;
  HCURSOR     m_hCursorMain;
  HCURSOR     m_hCursorNoDrop;
  HCURSOR     m_hCursorDrop;
  LPARAM      m_Param;               // used by client application to save something

  OK_FUN_GRIDDRAWCELL  m_FireDrawCell;
  OK_FUN_GRIDCLICK     m_FireClick;
  OK_FUN_GRIDRCLICK    m_FireRClick;
  OK_FUN_GRIDDBLCLK    m_FireDblClick;
  OK_FUN_GRIDAPPEND    m_FireAppend;
  OK_FUN_GRIDINSERT    m_FireInsert;
  OK_FUN_GRIDDELETE    m_FireDelete;
  OK_FUN_GRIDENTER     m_FireEnter;
  OK_FUN_GRIDKEY       m_FireKey;
  OK_FUN_GRIDSELROW    m_FireSelRow;
  OK_FUN_GRIDLINUM     m_FireListNum;
  OK_FUN_GRIDLIGET     m_FireListGet;
  OK_FUN_GRIDLISET     m_FireListSet;
  OK_FUN_GRIDLIDRAW    m_FireListDraw;

public:
    COkCtrlGrid (UINT MaxColumns=100);
    ~COkCtrlGrid ();

  void  SetWndStyle   (INT_PTR Style, INT_PTR ExStyle);
  bool  Create        (INT_PTR X, INT_PTR Y, INT_PTR W, INT_PTR H, HWND hwParent, INT_PTR ID, HINSTANCE hInst, bool bSortable);
  HWND  Window        () const;
  void  Resize        (INT_PTR X, INT_PTR Y, INT_PTR W, INT_PTR H);
  void  MoveWindow    (INT_PTR X, INT_PTR Y, INT_PTR W, INT_PTR H) const;
  void  Redraw        () const;
  void  SetFocus      () const;
  void  SetDropWindow (HWND hWnd, HWND hWndMsg, WPARAM wParam, LPARAM lParam);
  void  SetDropCursor (HCURSOR hcurDrop);
  void  SetParam      (LPARAM Param);
  bool  SetVisible    (bool bVisible);
  bool  IsVisible     () const;
  
  void  OnEvent_DrawCell (OK_FUN_GRIDDRAWCELL pFunc);
  void  OnEvent_Click    (OK_FUN_GRIDCLICK pFunc);
  void  OnEvent_RClick   (OK_FUN_GRIDRCLICK pFunc);
  void  OnEvent_DblClk   (OK_FUN_GRIDDBLCLK pFunc);
  void  OnEvent_Append   (OK_FUN_GRIDAPPEND pFunc);
  void  OnEvent_Insert   (OK_FUN_GRIDINSERT pFunc);
  void  OnEvent_Delete   (OK_FUN_GRIDDELETE pFunc);
  void  OnEvent_Enter    (OK_FUN_GRIDENTER  pFunc);
  void  OnEvent_Key      (OK_FUN_GRIDKEY    pFunc);
  void  OnEvent_SelRow   (OK_FUN_GRIDSELROW pFunc);
  void  OnEvent_ListNum  (OK_FUN_GRIDLINUM  pFunc);
  void  OnEvent_ListGet  (OK_FUN_GRIDLIGET  pFunc);
  void  OnEvent_ListSet  (OK_FUN_GRIDLISET  pFunc);
  void  OnEvent_ListDraw (OK_FUN_GRIDLIDRAW pFunc);

  bool     AddColumn    (UINT Id, LPCWSTR szName, UINT Width, UINT VarType, bool bSortable=true);
  COkTRow* AddRow       (COkTRow* pRowObject);
  bool     DeleteRow    (COkTRow* pRowObject);
  void     ClearColumns ();
  void     ClearRows    ();
  void     StartEdit    ();  // at current field

  INT_PTR  ActiveRow    () const;
  INT_PTR  ActiveColumn () const;

  UINT_PTR NumRows      (bool bCount=false) const;
  COkTRow* GetFirstRow  () const;
  COkTRow* GetLastRow   () const;
  COkTRow* GetNextRow   (const COkTRow* p) const;
  COkTRow* GetPrevRow   (const COkTRow* p) const;
  COkTRow* GetRow       (INT_PTR index) const;
  COkTRow* GetRow       () const;
  COkTRow* GetRowByItem (INT_PTR iItem) const;
  COkTRow* GetRowWithID (COkID Id) const;
  COkTRow* GetDragRow   () const;
  INT_PTR  GetRowIndex  (const COkTRow* p) const;

  INT_PTR     NumColumns () const;
  COkTColumn* GetColumn  (INT_PTR index) const;

  // work with the "ListView" Windows control
  void     Update        ();
  bool     SelectRow     (COkTRow* pRow);
  bool     SelectRow0    ();
  void     SelectRows    (bool bSel=true);
  void     UnselectRows  ();
  bool     SelectColumn  (int index);
  void     Unsort        ();
  void     SortColumn    (int index, bool bSortUp);
  bool     IsSortUp      () const;
  bool     IsSortDown    () const;
  bool     IsSorted      () const;
  bool     IsUnsorted    () const;
  UINT     SortColumnID  () const;
  bool     IsEdit        () const;
  bool     IsCbox        () const;

  LRESULT   WinProc (UINT Msg, WPARAM wParam, LPARAM lParam);
  HINSTANCE Inst () const;
  LPARAM    Param () const;
private:
  void   OnDrawRaw   (DRAWITEMSTRUCT* pDis);
  void   OnDrawListItem (DRAWITEMSTRUCT* pDis);
  void   OnKeyDown   (WPARAM KeyCode, LPARAM KeyData);
  void   OnBeginDrag (LPNMLISTVIEW pLV);
  void   OnMouseMove (int Xcur, int Ycur) const;
  void   OnLButtonUp (int Xcur, int Ycur);

  void   StartEdit   (UINT iItem, UINT iColumn);
  void   StopEdit    (bool bSave);
  void   Cbox_Create (bool bOpen=false);
  void   Cbox_Select ();
};

inline HWND      COkCtrlGrid::Window       () const {return m_hwFrame;}
inline COkTRow*  COkCtrlGrid::GetFirstRow  () const {return (COkTRow*)m_Rows.GetFirst();}
inline COkTRow*  COkCtrlGrid::GetLastRow   () const {return (COkTRow*)m_Rows.GetLast();}
inline COkTRow*  COkCtrlGrid::GetNextRow   (const COkTRow* p) const {return (COkTRow*)m_Rows.GetNext(p);}
inline COkTRow*  COkCtrlGrid::GetPrevRow   (const COkTRow* p) const {return (COkTRow*)m_Rows.GetPrev(p);}
inline COkTRow*  COkCtrlGrid::GetRow       () const {return m_pCurRow;}
inline COkTRow*  COkCtrlGrid::GetDragRow   () const {return m_pDragRow;}
inline INT_PTR   COkCtrlGrid::ActiveRow    () const {return m_iCurRow;}
inline INT_PTR   COkCtrlGrid::ActiveColumn () const {return m_iCurColumn;}
inline bool      COkCtrlGrid::IsSortUp     () const {return m_bSortUp;}
inline bool      COkCtrlGrid::IsSortDown   () const {return !m_bSortUp;}
inline bool      COkCtrlGrid::IsSorted     () const {return (m_iSortColumn>=0)?true:false;}
inline bool      COkCtrlGrid::IsUnsorted   () const {return (m_iSortColumn<0)?true:false;}
inline HINSTANCE COkCtrlGrid::Inst         () const {return m_hInst;}
inline LPARAM    COkCtrlGrid::Param        () const {return m_Param;}
inline void      COkCtrlGrid::Resize       (INT_PTR X, INT_PTR Y, INT_PTR W, INT_PTR H) {MoveWindow(X,Y,W,H);} 
inline INT_PTR   COkCtrlGrid::NumColumns   () const {return m_nColumns;}
inline UINT_PTR  COkCtrlGrid::NumRows      (bool bCount) const {return (bCount)?m_Rows.NumRecs():m_nRows;}


//-----------------------------------------------
void    LView_SetItemText     (HWND hwndLV, WPARAM i, int iSubItem, WCHAR* pszText);
void    LView_DeleteAllItems  (HWND hwndLV);
void    LView_DeleteItem      (HWND hwndLV, WPARAM i);
HWND    LView_GetHeader       (HWND hwndLV);
void    LView_GetSubItemRect  (HWND hwndLV, WPARAM iItem, int iSubItem, int code, RECT* prc);
BOOL    LView_GetColumn       (HWND hwndLV, WPARAM iCol, LV_COLUMN* pcol);
LRESULT LView_GetColumnWidth  (HWND hwndLV, WPARAM iCol );
BOOL    LView_GetItem         (HWND hwndLV, LV_ITEM* pitem);
LRESULT LView_GetItemCount    (HWND hwndLV);
LRESULT LView_GetItemState    (HWND hwndLV, WPARAM i, LPARAM mask);
BOOL    LView_SetColumn       (HWND hwndLV, WPARAM iCol, const LV_COLUMN* pcol);
LRESULT LView_SetExtendedListViewStyle (HWND hwndLV, DWORD Style);
void    LView_SetItemState    (HWND hwndLV, WPARAM i, UINT data, UINT mask);
void    LView_EnsureVisible   (HWND hwndLV, WPARAM i);
BOOL    LView_SortItems       (HWND hwndLV, PFNLVCOMPARE pfnCompare, LPARAM lPrm);
LRESULT LView_SubItemHitTest  (HWND hwndLV, LPLVHITTESTINFO plvhti);
LRESULT LView_InsertColumn    (HWND hwndLV, WPARAM iCol, const LV_COLUMN* pcol);
LRESULT LView_InsertItem      (HWND hwndLV, const LV_ITEM* pitem);

#ifdef _GRID_CPP_

  #define ITEM_HEIGHT  18
  #define ID_LISTVIEW  101
  #define ID_EDITBOX   102
  #define ID_CBOX      103

#endif 




#endif   // _OK_CTRLGRID_

