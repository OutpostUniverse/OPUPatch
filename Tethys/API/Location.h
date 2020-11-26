
#pragma once

#include "Tethys/Common/Memory.h"

struct Waypoint {
  uint32 x : 15;  ///< In pixels (max = 1024 tiles)
  uint32 y : 14;  ///< In pixels (max = 512 tiles)
  uint32   :  3;
};

struct PackedMapRect {
  uint16 x1;  ///< In tiles
  uint16 y1;  ///< In tiles
  uint16 x2;  ///< In tiles
  uint16 y2;  ///< In tiles
};


/// Struct representing tile X/Y coordinates on the map.
struct LOCATION : public OP2Class<LOCATION> {
public:
  LOCATION()                     : x(0),     y(0)     { }
  LOCATION(int tileX, int tileY) : x(tileX), y(tileY) { }

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

  int   GetPixelX(bool centered = false) const { return (x * 32) + (centered ? 16 : 0); }
  int   GetPixelY(bool centered = false) const { return (y * 32) + (centered ? 16 : 0); }
  POINT GetPixel(bool xCentered = false, bool yCentered = false) const
    { return { GetPixelX(xCentered), GetPixelY(yCentered) }; }

  Waypoint AsWaypoint(bool xCentered = false, bool yCentered = false) const
    { return { uint32(GetPixelX(xCentered)), uint32(GetPixelY(yCentered)) }; }

public:
  int x;
  int y;
};
using Location = LOCATION;


/// Struct representing a rectangular tile area on the map.
struct MAP_RECT : public OP2Class<MAP_RECT> {
public:
  MAP_RECT() : x1(0), y1(0), x2(0), y2(0) { }
  MAP_RECT(int leftTile, int topTile, int rightTile, int bottomTile)
    : x1(leftTile), y1(topTile), x2(rightTile), y2(bottomTile) { }
  MAP_RECT(const Location& topLeftTile, const Location& bottomRightTile)
    : x1(topLeftTile.x), y1(topLeftTile.y), x2(bottomRightTile.x), y2(bottomRightTile.y) { }

  int      Width()  const { return Thunk<0x475AA0, &$::Width>();  }
  int      Height() const { return Thunk<0x475AE0, &$::Height>(); }
  Location Size()   const { return Thunk<0x475C70, &$::Size>();   }

  /// Checks if the point is in the rect  [handles x wrap around for rect coordinates]
  int      Check(Location& ptToCheck) { return Thunk<0x475D50, &$::Check>(ptToCheck); }
  Location RandPt() const             { return Thunk<0x475CC0, &$::RandPt>();         }

  MAP_RECT& Clip()                      { Thunk<0x475AF0>();                             return *this; }
  MAP_RECT& Inflate(int wide, int high) { Thunk<0x475A60, void(int, int)>(wide, high);   return *this; }
  MAP_RECT& Offset(int right, int down) { Thunk<0x475BD0, void(int, int)>(right, down);  return *this; }
  MAP_RECT& FromPtSize(const Location& a, const Location& b)
    { Thunk<0x475C10, void(const Location&, const Location&)>(a, b);  return *this; }

  RECT GetPixels(bool xCentered = false, bool yCentered = false) const {
    const int cx = xCentered ? 16 : 0;
    const int cy = yCentered ? 16 : 0;
    return { (x1 * 32) + cx, (y1 * 32) + cy, (x2 * 32) + cx, (y2 * 32) + cy };
  }

  PackedMapRect AsPacked() { return { uint16(x1), uint16(y1), uint16(x2), uint16(y2) }; }

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
