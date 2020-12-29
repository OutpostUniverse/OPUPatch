
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

/// Structure used to pass information to ODASL in wplInit.
struct wplOptions {
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

CAPI DLLIMPORT int     CDECL wplInit(wplOptions* pCreateInfo);  ///< Initializes ODASL.
CAPI DLLIMPORT void    CDECL wplExit();                         ///< Uninitializes ODASL.
CAPI DLLIMPORT void    CDECL wplEnable();                       ///< Enables ODASL skinning.
CAPI DLLIMPORT void    CDECL wplDisable();                      ///< Disables ODASL skinning.

// The following APIs are exposed, but not used by OP2Shell.
CAPI DLLIMPORT void    CDECL wplSetPalette(HPALETTE palette);
CAPI DLLIMPORT int     CDECL wplGetSystemMetrics(int nIndex);
CAPI DLLIMPORT BOOL    CDECL wplAdjustWindowRect(RECT* lpRect, DWORD dwStyle, BOOL bMenu);
CAPI DLLIMPORT HBITMAP CDECL wplLoadResourceBitmap(HINSTANCE hInstance, const char* lpName); ///< Gets a resource image.
CAPI DLLIMPORT int     CDECL wplManualDialogSubclass(HWND hDlg);

}
