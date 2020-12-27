
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Game/MapObjectType.h"
#include "Tethys/API/Location.h"

namespace Tethys {

class StreamIO;
class GFXBitmap;
class GFXTilesetBitmap;
class MapObject;
union AnyMapObj;

/// All MapObject instances are this size (in bytes), regardless of type.
constexpr size_t MapObjectSize = 120;

/// Enum specifying cell types (affects passability, vehicle speed, tube connections, direct weapon firing, etc.)
enum class CellType : int {
  FastPassible1 = 0,  ///< Rock vegetation
  Impassible2,        ///< Meteor craters, cracks/crevases
  SlowPassible1,      ///< Lava rock (dark)
  SlowPassible2,      ///< Rippled dirt/Lava rock bumps
  MediumPassible1,    ///< Dirt
  MediumPassible2,    ///< Lava rock
  Impassible1,        ///< Dirt/Rock/Lava rock mound/ice/volcano
  FastPassible2,      ///< Rock
  NorthCliffs,
  CliffsHighSide,
  CliffsLowSide,
  VentsAndFumaroles,  ///< Fumaroles (passable by GeoCons)
  ZPad12,
  ZPad13,
  ZPad14,
  ZPad15,
  ZPad16,
  ZPad17,
  ZPad18,
  ZPad19,
  ZPad20,
  DozedArea,
  Rubble,
  NormalWall,
  MicrobeWall,
  LavaWall,
  Tube0,              ///< Used for tubes and areas under buildings
  Tube1,              ///< Unused
  Tube2,              ///< Unused
  Tube3,              ///< Unused
  Tube4,              ///< Unused
  Tube5,              ///< Unused
  Count
};

inline bool operator==(CellType first, int      second) { return int(first) == second; }
inline bool operator==(int      first, CellType second) { return first == int(second); }

/// Data describing each map tile.
union TileData {
  struct {
    uint32 cellType        :  5;  ///< Cell type of this tile.
    uint32 tileIndex       : 11;  ///< Mapping index (tile graphics to use).
    uint32 unitIndex       : 11;  ///< Index of MapUnit on this tile.
    uint32 lava            :  1;  ///< True if lava is on this tile.
    uint32 lavaPossible    :  1;  ///< True if lava can flow on this tile.
    uint32 expand          :  1;  ///< True if lava / microbe is expanding to this tile.
    uint32 microbe         :  1;  ///< True if microbe is on this tile.
    uint32 wallOrBuilding  :  1;  ///< True if a wall or building is on this tile.
  };
  uint32 u32All;  ///< All TileData bits packed as a single unsigned integer.
};

/// Defines information about each cell type.  @see CellType.
struct CellTypeInfo {
  char* pName;
  int   trackSpeed[size_t(TrackType::Count)];
  int   field_18;                              // ** TODO lava related?
  int   blightSpeed;
  int   field_20;
};

/// Defines information about each tile type index.
struct TilesetMapping {
  uint16 tilesetIndex;    ///< Index of the tileset that contains this tile.
  uint16 tileIndex;       ///< Index into the tileset of this tile.
  uint16 numAnimation;    ///< Number of alternate tiles that can be displayed in place of this one (all consecutive).
  uint16 animationDelay;  ///< Number of game cycles elapsed before the tile graphic is changed.
};

/// Defines tile index mappings for connectable tiles (walls, tubes).  Fields are named based on surrounding tiles.
struct ConnectedTileMapping {
  uint16 leftRight;
  uint16 topBottom;
  uint16 leftBottom;
  uint16 rightBottom;
  uint16 leftTop;
  uint16 rightTop;
  uint16 leftRightTop;
  uint16 leftRightBottom;
  uint16 leftRightTopBottom;
  uint16 leftTopBottom;
  uint16 rightTopBottom;
  uint16 bottom;
  uint16 top;
  uint16 right;
  uint16 left;
  uint16 none;
};

/// Defines information about terrain types.
struct TerrainType {
  uint16 firstTile;     ///< First tile index in this terrain type.
  uint16 lastTile;      ///< Last tile index in this terrain type.

  uint16 bulldozed;     ///< Index of bulldozed tile for this terrain type.
  uint16 rubbleStart;   ///< Start tile index of rubble.  4 common rubble tiles, followed by 4 rare rubble tiles.

  uint16 playerTube[6]; ///< (Unused?) Index of legacy per-player tube tiles for this terrain type.

  ConnectedTileMapping lavaWall;        ///< Lava wall tile indices.
  ConnectedTileMapping microbeWall;     ///< Microbe wall tile indices.
  ConnectedTileMapping wall;            ///< Normal wall tile indices.
  ConnectedTileMapping wallLightDamage; ///< Lightly-damaged normal wall tile indices.
  ConnectedTileMapping wallHeavyDamage; ///< Heavily-damaged normal wall tile indices.

  uint16 lavaStart; ///< First lava tile index.

  uint16 field_B6;
  uint16 field_B8;
  uint16 field_BA;

  ConnectedTileMapping tube; ///< Tube tile indices.

  struct {
    uint16 tile;    ///< Index of scorch mark tile.
    struct {
      uint16 start; ///< First tile index this scorch tile can be applied to.
      uint16 end;   ///< Last tile index this scorch tile can be applied to.
    }  range[3];
  }  scorched[1]; ///< Scorch tile indices and tiles they can be applied to.

  int field_EA[7];  // ** TODO more scorched tiles?
};
static_assert(sizeof(TerrainType) == 0x108, "Incorrect TerrainType size.");


/// Internal terrain type manager.
class TerrainManager : public OP2Class<TerrainManager> {
public:
  // ** TODO member functions

public:
  int             numTilesetMappings_;
  TilesetMapping* pTilesetMappings_;
  int             field_08;
  int             numTerrainTypes_;     ///< Max = 12
  TerrainType*    pTerrainTypes_;
};


/// Internal day/night cycle manager.
class DayNightManager : public OP2Class<DayNightManager> {
public:
   DayNightManager() { InternalCtor<0x41F9A0>(); }
  ~DayNightManager() { Thunk<0x41F9E0>();        }

  ibool EnableDayNight(ibool on) { return Thunk<0x41FA30, &$::EnableDayNight>(on); }

  ibool Save(StreamIO* pSavedGame) { return Thunk<0x4206A0, &$::Save>(pSavedGame); }
  ibool Load(StreamIO* pSavedGame) { return Thunk<0x420760, &$::Save>(pSavedGame); }

  void CheckSetViewToMiniMapPosition() { return Thunk<0x41FB00, &$::CheckSetViewToMiniMapPosition>(); }

  // ** TODO
  ibool F1(int pixelX, int pixelY, Location* pMapPixelLocation)
    { return Thunk<0x420870, &$::F1>(pixelX, pixelY, pMapPixelLocation); }

  void F2(GFXBitmap* pBitmap, int a, int b, int c) { return Thunk<0x41FC90, &$::F2>(pBitmap, a, b, c); }

  void SetDaylightPosition(int a, int position, int b)
    { return Thunk<0x41FAC0, &$::SetDaylightPosition>(a, position, b); }

  // ** TODO more functions?

  /// Gets the global day/night manager instance.
  static DayNightManager* GetInstance() { return OP2Mem<0x547298, DayNightManager*>(); }

public:
  int   field_00;
  ibool dayNight_;
  int   field_08[5];
  void* field_1C[6];
  int   field_34[4];
  int   actualPosition_;
  int   position_;
  int   field_4C;
  // ** TODO more fields?
};


/// Internal map object (wrapped by GameMap).
class MapImpl : public OP2Class<MapImpl> {
public:
   MapImpl() { InternalCtor<0x435320>(); }
  ~MapImpl() { Thunk<0x435350>();        }

  /// Unloads the current map and cleans up memory allocations.
  void Deinit() { return Thunk<0x435350, &$::Deinit>(); }

  /// Loads a new map file.  Deinit() should be called before calling this.
  ibool LoadFromFile(const char* pFilename) { return Thunk<0x437310, &$::LoadFromFile>(pFilename); }

  ibool Load(StreamIO* pMapOrSavedGame, ibool isSavedGame)
    { return Thunk<0x435DC0, &$::Load>(pMapOrSavedGame, isSavedGame); }
  ibool Save(StreamIO* pMapOrSavedGame, ibool isSavedGame)
    { return Thunk<0x4357F0, &$::Save>(pMapOrSavedGame, isSavedGame); }

  ibool SaveUnits(StreamIO* pSavedGame) { return Thunk<0x435B00, &$::SaveUnits>(pSavedGame); }
  void  FixupSavedUnitData()            { return Thunk<0x435CD0, &$::FixupSavedUnitData>();  }

  ibool AllocateMap(int log2TileWidth, int tileHeight, int numTilesetMappings, int numTilesets)
    { return Thunk<0x4354F0, &$::AllocateMap>(log2TileWidth, tileHeight, numTilesetMappings, numTilesets); }

  void FindUnitPlacementLocation(MapID unitType, Location where, Location* pPlacementLocation) const
    { return Thunk<0x4367D0, &$::FindUnitPlacementLocation>(unitType, where, pPlacementLocation); }

  void        SetTile(Location where, int tileIndex) { return Thunk<0x435430, &$::SetTile>(where, tileIndex); }
  void InitialSetTile(Location where, int tileIndex) { return Thunk<0x4354A0, &$::SetTile>(where, tileIndex); }

  // ** TODO more member functions
  /* GetTileCoordinates?   0x437FB0
     GetNextClosest        0x437D10
     DrawMiniMapBackground 0x437350
     ???5                  0x437FE0
     ???4                  0x437940
     ???3                  0x436FA0
     ???2                  0x436AE0
     ???(???)              0x437990
     ???                   0x436920
     ?                     0x4363D0 */

  /// Helper function for indexing into pTileArray_.
  size_t GetTileArrayOffset(int x, int y) const
    { return ((tileXMask_ & x) & 31) + (((((tileXMask_ & x) >> 5) << log2TileHeight_) + y) << 5); }

  ///@{ Gets the tile data at the given location for read/write access.
  TileData& Tile(Location where) { return Tile(where.x, where.y);                }
  TileData& Tile(int x, int y)   { return pTileArray_[GetTileArrayOffset(x, y)]; }
  ///@}

  ///@{ Helper functions used in getting a location's daylight level.
  int    GetRelDaylightPos(int tileX) { return (tileX - DayNightManager::GetInstance()->actualPosition_) & tileXMask_; }
  uint8* GetLightLevelAdjustTable()
    { return (this == GetInstance()) ? OP2Mem<uint8*&>(0x49DEED) : &lightLevelAdjustTable_[0]; }  // 0x54F854
  uint8* GetSpriteLightLevelAdjustTable()
    { auto*const p = GetLightLevelAdjustTable();  return p + ((p == &lightLevelAdjustTable_[0]) ? 512 : tileWidth_); }
  ///@}

  /// Gets the local daylight level at the specified map tile coordinates.
  uint8 GetLightLevel(Location where, bool forSprite = false) {
    const int pos = GetRelDaylightPos(where.x);
    return (forSprite ? GetSpriteLightLevelAdjustTable() : GetLightLevelAdjustTable())[pos];
  }

  /// Gets the max capacity of the map object array.
  size_t MaxNumUnits() const { return (uintptr(pMapObjListEnd_) - uintptr(pMapObjArray_)) / MapObjectSize; }

  static void FASTCALL CreateWallOrTube(int x, int y, MapID type)
    { return OP2Thunk<0x4A1D80, &$::CreateWallOrTube>(x, y, type); }

  /// Damages a wall.  Damage state change is based on RNG.
  static void FASTCALL DamageWall(int x, int y, int damage) { return OP2Thunk<0x4A24C0, &$::DamageWall>(x, y, damage); }

  /// Gets the internal map instance.
  static MapImpl* GetInstance() { return OP2Mem<0x54F7F8, MapImpl*>(); }

  /// Gets internal cell type info.
  static CellTypeInfo* GetCellTypeInfo(CellType cellType) {
    const auto index = size_t(cellType);
    return ((index >= 0) && (cellType < CellType::Count)) ? &OP2Mem<0x4DEBA8, CellTypeInfo*>()[index] : nullptr;
  }

public:
  int     field_00;
  uint32  pixelWidthMask_;
  int     pixelWidth_;
  uint32  tileXMask_;
  int     tileWidth_;
  uint8   log2TileWidth_;
  int     tileHeight_;
  MapRect clipRect_;
  int     numTiles_;
  int     numTilesets_;
  uint8   log2TileHeight_;
  int     paddingOffsetTileX_;  ///< Padding tiles to add to the left side of the map.  0 for world maps, +32 otherwise.

  int         numUnits_;
  int         lastUsedUnitIndex_;
  int         nextFreeUnitIndex_;
  int         firstFreeUnitIndex_;
  MapObject** ppMapObjFreeList_;    ///< Linked list of recycled MapObjects.
  AnyMapObj*  pMapObjArray_;        ///< This map's MapObject data array.
  MapObject*  pMapObjListBegin_;    ///< Top-most map object linked list (sorted ascending by pixelY).  Head == [0].
  MapObject*  pMapObjListEnd_;      ///< One past the end of pMapObjArray_.

  uint8              lightLevelAdjustTable_[1024];
  uint32             field_45C;
  char**             pTilesetNames_;
  TileData*          pTileArray_;
  GFXTilesetBitmap** ppTilesetBitmaps_;
  TerrainManager*    pTerrainManager_;
};
static_assert(sizeof(MapImpl) == 1136, "Incorrect MapImpl size.");

inline       auto& g_mapImpl      = *MapImpl::GetInstance();
inline const auto& g_pMapObjArray =  MapImpl::GetInstance()->pMapObjArray_;

} // Tethys
