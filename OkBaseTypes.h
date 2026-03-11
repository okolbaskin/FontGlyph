/********************************************************************
* Project: FontGlyph
* Copyright (C) 2026, Oleg Kolbaskin
* All rights reserved.
*
* Base types 
********************************************************************/
#ifndef _OK_BASETYPES_
#define _OK_BASETYPES_

#if defined(_WIN64)
    typedef __int64 INT_PTR;
    typedef unsigned __int64 UINT_PTR;
    typedef __int64 LONG_PTR;
    typedef unsigned __int64 ULONG_PTR;
#else
    typedef int INT_PTR;
    typedef unsigned int UINT_PTR;
    typedef long LONG_PTR;
    typedef unsigned long ULONG_PTR;
#endif

typedef unsigned char    UCHAR;
typedef unsigned char    BYTE;
typedef wchar_t          WCHAR;
#ifndef _WINDOWS_
  typedef const char*      LPCSTR;
  typedef const wchar_t*   LPCWSTR;
#endif

typedef unsigned short   WORD;
typedef unsigned short   USHORT;

typedef int              BOOL;
typedef int              INT32;
typedef int              INT;
typedef long             LONG;
typedef unsigned int     UINT32;
typedef unsigned int     UINT;
typedef unsigned long    DWORD;
typedef unsigned long    COLORREF;

typedef __int64          INT64;
typedef unsigned __int64 UINT64;

typedef UINT_PTR         WPARAM;
typedef LONG_PTR         LPARAM;
typedef LONG_PTR         LRESULT;
typedef ULONG_PTR        DWORD_PTR;
typedef void*            HANDLE;

struct WPOINT
{
  LONG  x;
  LONG  y;
};


#define LOWORD(l)      ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)      ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)      ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)      ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#define RGB(r,g,b)     ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) (LOBYTE(rgb))
#define GetGValue(rgb) (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) (LOBYTE((rgb)>>16))


#define NULL  0
#define CALLBACK __stdcall


#endif // _OK_BASETYPES_
