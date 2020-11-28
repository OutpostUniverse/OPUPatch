
#pragma once

#include "Tethys/Common/Types.h"
#include "Tethys/Common/Memory.h"

#if defined(_WIN32)
# include <intrin.h>
#elif defined(__GNUC__)
# include <cpuid.h>
#endif


inline HINSTANCE g_hInst = nullptr;  // HINSTANCE of this DLL.  DllMain must set this during DLL_PROCESS_ATTACH!


/// Returns true if the virtual machine hypervisor bit of cpuid is set.
inline bool IsVirtualMachine() {
  uint32 out[4] = { };
#if defined(_WIN32)
  __cpuid(reinterpret_cast<int*>(&out[0]), 0x1);
#elif defined(__GNUC__)
  __get_cpuid(0x1, &out[0], &out[1], &out[2], &out[3]);
#endif
  return (out[2] & (1u << 31)) != 0;
}


// Printfs a debug message.  You need to include stdio.h and windows.h to use this.
#if _DEBUG || ENABLE_DEBUG_MSG
# define DEBUG_MSG(format, ...)  \
  { char dbgStr[1024]; snprintf(dbgStr, sizeof(dbgStr), format "\n", __VA_ARGS__); OutputDebugStringA(dbgStr); } (void)0
#else
# define DEBUG_MSG(format, ...)
#endif
