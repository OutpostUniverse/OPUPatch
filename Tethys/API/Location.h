
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

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


/// Exported struct typically representing tile X/Y coordinates on the map.
struct Location : public OP2Class<Location> {
public:
  constexpr Location()                     : x(-1),    y(-1)    { }
  constexpr Location(int tileX, int tileY) : x(tileX), y(tileY) { }

  constexpr bool operator==(const Location& other) const { return (x == other.x) && (y == other.y); }
  constexpr operator bool()                        const { return (x != -1)      && (y != -1);      }

  ///@{ Adds or subtracts Locations and wraps around the map.
  Location& Add(const Location& vector) { Thunk<0x475A30, void(const Location&)>(vector);  return *this; }
  static Location FASTCALL Difference(const Location& a, const Location& b)
    { return OP2Thunk<0x4759D0, &$::Difference>(a, b); }

  Location& operator+=(const Location& vector)       { return Add(vector);                      }
  Location   operator+(const Location& vector) const { return Location(*this).Add(vector);      }
  Location   operator-(const Location& vector) const { return Difference(vector, *this);        }
  Location   operator+(int scalar)             const { return *this + Location(scalar, scalar); }
  Location   operator-(int scalar)             const { return *this - Location(scalar, scalar); }
  ///@}

  Location& Clip() { Thunk<0x475960>();  return *this; }  ///< Wraps X coordinate around the map, clips Y to edge

  int Norm() { return Thunk<0x401E50, &$::Norm>(); }  ///< Returns euclidean distance: ftol(sqrt(x*x + y*y) + 0.5)

  ///@{ Converts map tile coordinates to map pixel coordinates.
  constexpr int   GetPixelX(bool centered = true) const { return (x * 32) + (centered ? 16 : 0); }
  constexpr int   GetPixelY(bool centered = true) const { return (y * 32) + (centered ? 16 : 0); }
  constexpr POINT GetPixel(bool xCentered = true, bool yCentered = true) const
    { return { GetPixelX(xCentered), GetPixelY(yCentered) }; }
  ///@}

  constexpr Waypoint AsWaypoint(bool xCentered = true, bool yCentered = true) const
    { return { uint32(GetPixelX(xCentered)), uint32(GetPixelY(yCentered)) }; }

public:
  int x;
  int y;
};


/// Exported struct typically representing a rectangular tile area on the map.
struct MapRect : public OP2Class<MapRect> {
public:
  constexpr MapRect() : x1(-1), y1(-1), x2(-1), y2(-1) { }
  constexpr MapRect(int leftTile, int topTile, int rightTile, int bottomTile)
    : x1(leftTile), y1(topTile), x2(rightTile), y2(bottomTile) { }
  constexpr MapRect(const Location& topLeftTile, const Location& bottomRightTile)
    : x1(topLeftTile.x), y1(topLeftTile.y), x2(bottomRightTile.x), y2(bottomRightTile.y) { }

  constexpr bool operator==(const MapRect& other) const
    { return (x1 == other.x1) && (y1 == other.y1) && (x2 == other.x2) && (y2 == other.y2); }
  constexpr operator bool() const { return (x1 != -1) && (y1 != -1) && (x2 != -1) && (y2 != -1); }

  int      Width()  const { return Thunk<0x475AA0, &$::Width>();  }
  int      Height() const { return Thunk<0x475AE0, &$::Height>(); }
  Location Size()   const { return Thunk<0x475C70, &$::Size>();   }

  /// Checks if the point is in the rect  [handles x wrap around for rect coordinates]
  ibool    Contains(const Location& ptToCheck) const { return Thunk<0x475D50, &$::Contains>(ptToCheck); }
  Location RandomPoint()                       const { return Thunk<0x475CC0, &$::RandomPoint>();       }

  MapRect& Clip()                      { Thunk<0x475AF0>();                             return *this; }
  MapRect& Inflate(int wide, int high) { Thunk<0x475A60, void(int, int)>(wide, high);   return *this; }
  MapRect& Offset(int right, int down) { Thunk<0x475BD0, void(int, int)>(right, down);  return *this; }
  MapRect& FromPtSize(const Location& a, const Location& b)
    { Thunk<0x475C10, void(const Location&, const Location&)>(a, b);  return *this; }

  Location MidPoint() const { return { (x1 + x2) / 2, (y1 + y2) / 2 }; }  // ** TODO This needs to handle wraparound!

  /// Converts map tile coordinates to pixel coordinates.
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


struct PatrolRoute {
  int             field_00;
  const Location* pWaypoints;  ///< Max waypoints = 8, set Location.x = -1 for last waypoint in list if list is short
};


namespace TethysAPI {
  using Location    = Tethys::Location;
  using MapRect     = Tethys::MapRect;
  using PatrolRoute = Tethys::PatrolRoute;
} // TethysAPI

} // Tethys
