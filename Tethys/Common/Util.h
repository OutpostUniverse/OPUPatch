
#pragma once

#include "Tethys/Common/Types.h"

#include <cstdlib>
#include <cstdio>
#if defined(_WIN32)
# include <intrin.h>
#endif

namespace TethysUtil {

/// Returns the size in elements of a C-style array.
template <typename T, size_t N>  constexpr size_t ArrayLen(const T (&array)[N]) { return N; }

/// Tests if any bit flag is set in mask.
template <typename T1, typename T2> bool BitFlagTest(T1 mask, T2 flag) { return (mask & flag) != 0; }

/// Tests if all bit flags are set in mask.
template <typename T1, typename T2> bool BitFlagsTest(T1 mask, T2 flags) { return (mask & flags) == flags; }

/// Sets or unsets a bit flag.
template <typename T1, typename T2> void SetBitFlag(T1& out, T2 flag, bool on){ out ^= (out ^ (on ? flag : 0)) & flag; }

/// Iterates forward on a bitmask.  Found bit must be masked out after each call in a loop.
inline bool GetNextBit(
  uint32*  pIndex,
  uint32   mask)
{
  bool result = false;

#if defined(_WIN32) && defined(_M_IX86)
  *pIndex = _tzcnt_u32(mask);
#elif defined(_WIN32)
  _BitScanForward(reinterpret_cast<unsigned long*>(pIndex), mask);
#elif defined(__GNUC__)
  *pIndex = __builtin_ctz(mask);
#else
  if (mask != 0) {
    uint32 index = 0;
    for (; (mask & 1) == 0; mask >>= 1, ++index);
    *pIndex = index;
  }
#endif

  if (mask != 0) {
    result = (mask != 0);
  }

  return result;
}

} // TethysUtil
