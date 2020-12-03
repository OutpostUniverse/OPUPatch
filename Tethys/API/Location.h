
#pragma once

#include "Tethys/Common/Memory.h"

/// Bitfields typically representing pixel X/Y coordinates for path finding waypoints.
struct Waypoint {
  uint32 x : 15;  ///< In pixels (max = 1024 tiles)
  uint32 y : 14;  ///< In pixels (max = 512 tiles)
  uint32   :  3;
};

/// Compactified struct typically representing a rectangular tile area on the map.
struct PackedMapRect {
  uint16 x1;
  uint16 y1;
  uint16 x2;
  uint16 y2;
};


/// Struct typically representing tile X/Y coordinates on the map.
struct LOCATION : public OP2Class<LOCATION> {
public:
  constexpr LOCATION()                     : x(INT_MIN), y(INT_MIN) { }
  constexpr LOCATION(int tileX, int tileY) : x(tileX),   y(tileY)   { }

  constexpr bool operator==(const LOCATION& other) const { return (x == other.x) && (y == other.y); }
  constexpr operator bool()                        const { return (x != INT_MIN) && (y != INT_MIN); }

  LOCATION& Add(const LOCATION& vector) { Thunk<0x475A30, void(const LOCATION&)>(vector); return *this; }
  static LOCATION FASTCALL Difference(const LOCATION& a, const LOCATION& b)
    { return OP2Thunk<0x4759D0, &$::Difference>(a, b); }

  LOCATION& operator+=(const LOCATION& vector)       { return Add(vector);                      }
  LOCATION   operator+(const LOCATION& vector) const { return LOCATION(*this).Add(vector);      }
  LOCATION   operator-(const LOCATION& vector) const { return Difference(vector, *this);        }
  LOCATION   operator+(int scalar)             const { return *this + LOCATION(scalar, scalar); }
  LOCATION   operator-(int scalar)             const { return *this - LOCATION(scalar, scalar); }

  LOCATION& Clip() { Thunk<0x475960>();  return *this; }  ///< Wraps X coordinate around the map, clips Y to edge

  int Norm() { return Thunk<0x401E50, &$::Norm>(); }  ///< ftol(sqrt(x*x + y*y) + 0.5)

  constexpr int   GetPixelX(bool centered = true) const { return (x * 32) + (centered ? 16 : 0); }
  constexpr int   GetPixelY(bool centered = true) const { return (y * 32) + (centered ? 16 : 0); }
  constexpr POINT GetPixel(bool xCentered = true, bool yCentered = true) const
    { return { GetPixelX(xCentered), GetPixelY(yCentered) }; }

  constexpr Waypoint AsWaypoint(bool xCentered = true, bool yCentered = true) const
    { return { uint32(GetPixelX(xCentered)), uint32(GetPixelY(yCentered)) }; }

public:
  int x;
  int y;
};
using Location = LOCATION;


/// Struct typically representing a rectangular tile area on the map.
struct MAP_RECT : public OP2Class<MAP_RECT> {
public:
  constexpr MAP_RECT() : x1(INT_MIN), y1(INT_MIN), x2(INT_MIN), y2(INT_MIN) { }
  constexpr MAP_RECT(int leftTile, int topTile, int rightTile, int bottomTile)
    : x1(leftTile), y1(topTile), x2(rightTile), y2(bottomTile) { }
  constexpr MAP_RECT(const Location& topLeftTile, const Location& bottomRightTile)
    : x1(topLeftTile.x), y1(topLeftTile.y), x2(bottomRightTile.x), y2(bottomRightTile.y) { }

  constexpr bool operator==(const MAP_RECT& other) const
    { return (x1 == other.x1) && (y1 == other.y1) && (x2 == other.x2) && (y2 == other.y2); }
  constexpr operator bool() const { return (x1 != INT_MIN) && (y1 != INT_MIN) && (x2 != INT_MIN) && (y2 != INT_MIN); }

  int      Width()  const { return Thunk<0x475AA0, &$::Width>();  }
  int      Height() const { return Thunk<0x475AE0, &$::Height>(); }
  Location Size()   const { return Thunk<0x475C70, &$::Size>();   }

  /// Checks if the point is in the rect  [handles x wrap around for rect coordinates]
  int      Check(const Location& ptToCheck) { return Thunk<0x475D50, &$::Check>(ptToCheck); }
  Location RandPt() const                   { return Thunk<0x475CC0, &$::RandPt>();         }

  MAP_RECT& Clip()                      { Thunk<0x475AF0>();                             return *this; }
  MAP_RECT& Inflate(int wide, int high) { Thunk<0x475A60, void(int, int)>(wide, high);   return *this; }
  MAP_RECT& Offset(int right, int down) { Thunk<0x475BD0, void(int, int)>(right, down);  return *this; }
  MAP_RECT& FromPtSize(const Location& a, const Location& b)
    { Thunk<0x475C10, void(const Location&, const Location&)>(a, b);  return *this; }

  constexpr RECT GetPixels(bool centered = false) const {
    const int c = centered ? 16 : 0;
    const int e = centered ? 0  : 31;
    return { (x1 * 32) + c, (y1 * 32) + c, (x2 * 32) + c + e, (y2 * 32) + c + e };
  }

  constexpr PackedMapRect AsPacked() const { return { uint16(x1), uint16(y1), uint16(x2), uint16(y2) }; }

public:
  int x1;
  int y1;
  int x2;
  int y2;
};
using MapRect = MAP_RECT;


struct PatrolRoute {
  int             field_00;
  const Location* pWaypoints;  ///< Max waypoints = 8, set Location.x = -1 for last waypoint in list if list is short
};
