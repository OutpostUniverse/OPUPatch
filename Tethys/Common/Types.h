
/// Common platform type definitions.
// ** TODO remove SWIG/CLIF stuff

#pragma once

#include <cstdint>

// Pull in windows headers (or fakes for CLIF)
#ifndef SWIG
# include "Tethys/Common/WinTypes.h"
#endif  // !SWIG

namespace Tethys::TethysAPI{};

/// Convenience alias for Tethys::TethysAPI nested namespace.
namespace TethysAPI = Tethys::TethysAPI;

namespace Tethys {

using ibool   = int;
using int8    = int8_t;
using int16   = int16_t;
using int32   = int32_t;
using int64   = int64_t;
using uint8   = uint8_t;
using uint16  = uint16_t;
using uint32  = uint32_t;
using uint64  = uint64_t;
using uintptr = uintptr_t;

// Defines for function calling conventions
#ifndef CDECL
# ifdef SWIG
#  define CDECL
# else
#  define CDECL __cdecl
# endif  // SWIG
#endif  // CDECL

#ifndef STDCALL
# ifdef SWIG
#  define STDCALL
# else
#  define STDCALL __stdcall
# endif  // SWIG
#endif  // STDCALL

#ifndef FASTCALL
# ifdef SWIG
#  define FASTCALL
# else
#  define FASTCALL __fastcall
# endif  // SWIG
#endif  // FASTCALL

#ifndef THISCALL
# ifdef SWIG
#  define THISCALL
# else
#  define THISCALL __thiscall
# endif  // SWIG
#endif  // THISCALL

#ifndef DLLIMPORT
# ifdef SWIG
#  define DLLIMPORT
# else
#  define DLLIMPORT __declspec(dllimport)
# endif  // SWIG
#endif  // DLLIMPORT

#ifndef DLLEXPORT
# ifdef SWIG
#  define DLLEXPORT
# else
#  define DLLEXPORT __declspec(dllexport)
# endif  // SWIG
#endif  // DLLEXPORT

#ifndef CAPI
# define CAPI extern "C"
#endif  // CAPI

#ifdef BUILDING_OP2API_STUBS

# ifndef OP2API
#  define OP2API DLLEXPORT
# endif  // OP2API

#else  // BUILDING_OP2API_STUBS

# ifndef OP2API
#  define OP2API DLLIMPORT
# endif  // OP2API

#endif  // BUILDING_OP2API_STUBS

#ifndef DLLAPI
# define DLLAPI CAPI DLLEXPORT
#endif  // DLLAPI

#ifndef PACKED
# ifdef SWIG
#  define PACKED
# else
#  ifdef _MSC_VER
#   define PACKED __declspec(align(1))
#  else
#   define PACKED __attribute__((packed))
#  endif  // _MSC_VER
# endif  // SWIG
#endif  // PACKED

#ifndef BEGIN_PACKED
# if defined(SWIG) || !defined(_MSC_VER)
#  define BEGIN_PACKED
# else
#  define BEGIN_PACKED __pragma(pack(push, 1))
# endif  // SWIG || !_MSC_VER
#endif  // BEGIN_PACKED

#ifndef END_PACKED
# if defined(SWIG) || !defined(_MSC_VER)
#  define END_PACKED
# else
#  define END_PACKED __pragma(pack(pop))
# endif  // SWIG || !_MSC_VER
#endif  // END_PACKED

} // Tethys
