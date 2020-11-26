
// ** TODO remove CLIF stuff

#pragma once

#if CLIF
// "Fake" Windows types that are only used in the processing of headers for
// CLIF, rather than including the "real" Windows headers.
//
// This file contains the bare minimum of Windows types in order for header
// parsing to work.

// Basic types.
// These are all defined as int even though the real definitions may or may not
// be unsigned, due to Python unsigned conversion behavior rejecting negative
// numbers.
typedef int BOOL, DWORD, INT_PTR, LONG, LPARAM, LRESULT, UINT, ULONG, WPARAM, COLORREF;
/*
typedef unsigned int DWORD;
typedef int  INT_PTR;
typedef long LONG;
typedef long LPARAM;
typedef long LRESULT;
typedef unsigned int  UINT;
typedef unsigned long WPARAM;
typedef unsigned int  COLORREF;
*/

// Windows handles are opaque integers, typically represented as void*.
// However, when STRICT is defined, they are declared as pointers to empty
// structs (e.g. HWND__).
// To make things more confusing, CLIF seems to treat different handle types
// differently (i.e. double-indirection on random types but not all of them).
# define WINDOWS_HANDLE(T) struct T { void* p; }

WINDOWS_HANDLE(HBITMAP);
WINDOWS_HANDLE(HBRUSH);
WINDOWS_HANDLE(HCURSOR);
WINDOWS_HANDLE(HHOOK);
WINDOWS_HANDLE(HICON);
WINDOWS_HANDLE(HINSTANCE);
WINDOWS_HANDLE(HMENU);
WINDOWS_HANDLE(HPALETTE);
WINDOWS_HANDLE(HWND);

struct POINT {
  LONG x;
  LONG y;
};

struct MSG {
  HWND   hwnd;
  UINT   message;
  WPARAM wParam;
  LPARAM lParam;
  DWORD  time;
  POINT  pt;
  DWORD  lPrivate;
};

struct RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
};

struct GUID {
  uint32 data1;
  uint16 data2;
  uint16 data3;
  uint8  data4[8];
};

# define MAX_PATH 260

#else // !CLIF
// If not generating CLIF code, include the real Windows headers.
# include <windows.h>
# include <commctrl.h>
#endif  // CLIF
