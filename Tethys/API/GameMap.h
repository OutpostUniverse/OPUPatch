
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Game/MapImpl.h"
#include "Tethys/API/Location.h"
#include "Tethys/API/Unit.h"

namespace Tethys::TethysAPI {

/// Public game map interface (wraps MapImpl).  Used to access map tile data, set daylight level, and create Blight.
class GameMap : public OP2Class<GameMap> {
public:
  /// Gets the internal map instance.
  static MapImpl* GetImpl() { return MapImpl::GetInstance(); }

  ///@{ Helpers to access and set tile data.
  static int FASTCALL GetTile(Location           where) { return OP2Thunk<0x476D00, &$::GetTile>(where);    }
  static CellType     GetCellType(Location       where) { return CellType(GetImpl()->Tile(where).cellType); }
  static Unit         GetUnitOnTile(Location     where) { return Unit(GetImpl()->Tile(where).unitIndex);    }
  static bool         GetLavaPresent(Location    where) { return GetImpl()->Tile(where).lava;               }
  static bool         GetLavaPossible(Location   where) { return GetImpl()->Tile(where).lavaPossible;       }
  static bool         GetExpand(Location         where) { return GetImpl()->Tile(where).expand;             }
  static bool         GetMicrobe(Location        where) { return GetImpl()->Tile(where).microbe;            }
  static bool         GetWallOrBuilding(Location where) { return GetImpl()->Tile(where).wallOrBuilding;     }

  static void InitialSetTile(Location   where, int tileIndex) { return GetImpl()->InitialSetTile(where, tileIndex); }
  static void FASTCALL SetTile(Location where, int tileIndex)
    { return OP2Thunk<0x476D80, &$::SetTile>(where, tileIndex); }
  static void FASTCALL SetLavaPossible(Location where, ibool lavaPossible)
    { return OP2Thunk<0x476F20, &$::SetLavaPossible>(where, lavaPossible); }

  static void SetCellType(Location       where, CellType type)    { GetImpl()->Tile(where).cellType  = uint32(type); }
  static void SetUnitOnTile(Location     where, Unit     unit)    { GetImpl()->Tile(where).unitIndex = unit.id_;     }
  static void SetLavaPresent(Location    where, bool     lava)    { GetImpl()->Tile(where).lava      = lava;         }
  static void SetExpand(Location         where, bool     expand)  { GetImpl()->Tile(where).expand    = expand;       }
  static void SetMicrobe(Location        where, bool     microbe) { GetImpl()->Tile(where).microbe   = microbe;      }
  static void SetWallOrBuilding(Location where, bool     wallOrBuilding)
    { GetImpl()->Tile(where).wallOrBuilding = wallOrBuilding; }
  ///@}

  /// Sets the daylight position on the map.
  static void FASTCALL SetInitialLightLevel(int lightPosition)
    { return OP2Thunk<0x476F90, &$::SetInitialLightLevel>(lightPosition); }

  /// Gets the daylight level at the specified map tile coordinates.
  static int GetLightLevel(Location where, bool forSprite = false){ return GetImpl()->GetLightLevel(where, forSprite); }

  /// Damages a wall.  Damage state change is based on RNG.
  static void DamageWall(Location where, int damage) { GetImpl()->DamageWall(where.x, where.y, damage); }

  /// Translates in-game map (x, y) coordinates as displayed on the status bar, to real map coordinates.
  /// Useful with e.g. GetTile(), TethysGame::CreateUnit(), and other functions that take a Location.
  static Location At(int x, int y)
    { return Location(((x - 1 + GetPaddingWidth()) & GetImpl()->tileXMask_), ((y - 1) & (GetHeight() - 1))); }

  ///@{ Returns the map dimensions.
  static int GetWidth()  { return GetImpl()->tileWidth_;  }
  static int GetHeight() { return GetImpl()->tileHeight_; }

  static int GetPaddingWidth()  { return GetImpl()->paddingOffsetTileX_; }  ///< 0 for world maps, +32 otherwise.
  static int GetUnpaddedWidth() { return GetWidth() / ((GetPaddingWidth() != 0) ? 2 : 1); }

  static MapRect GetClipRect() { return GetImpl()->clipRect_; }
  ///@}

  /// Finds a clear area to place a unit of the given type nearest to the requested location.
  static Location FindUnitPlacementLocation(Location where, MapID unitType = MapID::CargoTruck)
    { Location loc;  GetImpl()->FindUnitPlacementLocation(unitType, where, &loc);  return loc; }

  /// Loads a new map from the given file.  @warning This may crash if there are units on the map.
  static bool Load(const char* pFilename) { GetImpl()->Deinit();  return GetImpl()->LoadFromFile(pFilename); }

public:
  uint8 field_00;
};

} // Tethys::TethysAPI
