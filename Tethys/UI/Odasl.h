
#pragma once

#include "Tethys/Common/Library.h"

namespace Tethys::Odasl {

/// Structure used to pass information to ODASL in wplInit.
struct WplInfo {
	uint32    structSize;   ///< [60]
	uint32    field_04;     ///< Flags?  [87]
	HINSTANCE hAppInst;     ///< OS handle to OP2Shell.dll.
	HINSTANCE hResDllInst;  ///< OS handle to op2shres.dll.
	uint32    startResId;   ///< Starting resource ID  [200]
	uint32    field_14;     ///< Palette?  [0]
	uint32    field_18;     ///< More flags?  [0]
	uint32    field_1C;     ///< Size scale? Large values make windows huge  [96]
	uint32    field_20[7];  ///< [{0}]
};

/// Get the ODASL Library object.  Loads odasl.dll if needed.
inline const TethysUtil::Library& GetOdasl() { static TethysUtil::Library odaslLib("odasl.dll");  return odaslLib; }

inline int  STDCALL wplInit(WplInfo* pInfo) { return GetOdasl().Get<&wplInit>(__func__)(pInfo); } ///< Initialize ODASL.
inline void STDCALL wplExit()               { return GetOdasl().Get<&wplExit>(__func__)(); }      ///< Deinit ODASL.

inline void STDCALL wplEnable()  { return GetOdasl().Get<&wplEnable>(__func__)();  } ///< Begin paint UI with skinning.
inline void STDCALL wplDisable() { return GetOdasl().Get<&wplDisable>(__func__)(); } ///< End paint UI with skinning.

// The following APIs are exported, but only used internally within ODASL, not directly by Outpost2 or OP2Shell.
inline void    STDCALL wplSetPalette(HPALETTE hPalette) { return GetOdasl().Get<&wplSetPalette>(__func__)(hPalette); }
inline HBITMAP STDCALL wplLoadResourceBitmap(HINSTANCE hInstance, const char* lpName)
	{ return GetOdasl().Get<&wplLoadResourceBitmap>(__func__)(hInstance, lpName); }
inline int     STDCALL wplManualDialogSubclass(HWND hDlg)
	{ return GetOdasl().Get<&wplManualDialogSubclass>(__func__)(hDlg); }

inline int  STDCALL wplGetSystemMetrics(int nIndex) { return GetOdasl().Get<&wplGetSystemMetrics>(__func__)(nIndex); }
inline BOOL STDCALL wplAdjustWindowRect(RECT* lpRect, DWORD dwStyle, BOOL bMenu)
	{ return GetOdasl().Get<&wplAdjustWindowRect>(__func__)(lpRect, dwStyle, bMenu); }

} // Tethys::Odasl
