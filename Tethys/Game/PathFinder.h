
#pragma once

#include "Tethys/Common/Types.h"
#include "Tethys/Common/Memory.h"
#include "Tethys/Game/CommandPacket.h"

namespace Tethys {

class MapObject;
class StreamIO;

BEGIN_PACKED

// ** TODO What is this used for?
struct LongWaypoint {
  uint32   :  5;
  uint32 x : 15;
  uint32 y : 14;
  uint32   : 30;
};


/// Internal unit pathfinder class.
class PathFinder {
public:
  static PathFinder* GetInstance() { return OP2Mem<0x551340, PathFinder*>(); }

  // ** TODO See separate PathFinder (551340, ctor @ 438200) and PathHelper?[3] (54FC88, size=0x18 x3, ctor @ 438230)

  static void FASTCALL SetStartPos(int tileX, int tileY) { return OP2Thunk<0x438390, &SetStartPos>(tileX, tileY); }
  static void FASTCALL SetEndPos(int tileX, int tileY)   { return OP2Thunk<0x4383A0, &SetEndPos>(tileX, tileY);   }
  static void FASTCALL SetDistance(int distance)         { return OP2Thunk<0x4383B0, &SetDistance>(distance);     }
  static void FASTCALL SelectUnit(MapObject* pUnit)      { return OP2Thunk<0x438360, &SelectUnit>(pUnit);         }

  ///@{ @note SelectUnit() must be called first.
  static ibool FASTCALL IsPassable(int tileX, int tileY) { return OP2Thunk<0x4383C0, &IsPassable>(tileX, tileY); }
  static ibool FASTCALL CanPlaceStructure(int tileX, int tileY)
    { return OP2Thunk<0x438C80, &CanPlaceStructure>(tileX, tileY); }
  static ibool FASTCALL CanPlaceTubeOrWall(int tileX, int tileY)
    { return OP2Thunk<0x438D60, &CanPlaceTubeOrWall>(tileX, tileY); }
  ///@}

  static ibool FASTCALL IsDock(int tileX, int tileY) { return OP2Thunk<0x438B70, &IsDock>(tileX, tileY); }

  // PathHelper? ctor 438260

  // static FASTCALL 438280
  // static FASTCALL 4382D0
  // static FASTCALL 438300
  // static FASTCALL 438490
  // static FASTCALL 4385D0
  // static FASTCALL 438760
  // static FASTCALL 438950
  // static FASTCALL 438AE0
  // static FASTCALL 438E70
  // static FASTCALL 438F50

public:
  ibool (FASTCALL* pfn_00)(int tileX,      int tileY);
  int   (FASTCALL* pfn_04)(int startTileX, int startTileY);

  MapObject* pCurUnit_;

  int startTileX_;
  int startTileY_;
  int dstTileX_;
  int dstTileY_;
  int sqDistance_;    ///< Squared distance (rangeTileX^2 + rangeTileY^2)
  int rangeTileX_;
  int rangeTileY_;
  // ** TODO more fields  0x28 ...

  /// Lookup table of (dx, dy) pairs for PathContext::rotation values.  Corresponds to order of UnitDirection enum.
  static constexpr struct DirOffset {
    int dx;
    int dy;
  } DirOffsetLut[] = {
    {  1,  0 },  ///< East
    {  1,  1 },  ///< SouthEast
    {  0,  1 },  ///< South
    { -1,  1 },  ///< SouthWest
    { -1,  0 },  ///< West
    { -1, -1 },  ///< NorthWest
    {  0, -1 },  ///< North
    {  1, -1 },  ///< NorthEast
  };
};

/// Unit path finding context.
struct PathContext {
  union {
    int          numWaypoints;
    PathContext* pFreeListNext;
  };
  int maxWaypointIndex;
  int currentWaypointIndex;

  int field_0C;
  int field_10;

  Waypoint     waypoints[8];
  uint32       flags;
  PathContext* pNextDestination;

  int    field_3C;  ///< (0 = Move/Dock/DockEG/StandGround/CargoRoute/Patrol/Build, 3 = RemoveWall)
  int    field_40;
  uint16 numPixelsMovedX;
  uint16 numPixelsMovedY;

  int startPixelX;  ///< Of current waypoint
  int startPixelY;  ///< Of current waypoint
  int dstTileX;     ///< Of current waypoint
  int dstTileY;     ///< Of current waypoint

  uint16 field_58;
  uint16 numPixelsToMoveX;
  uint16 field_5C;
  uint16 numPixelsToMoveY;

  int rotationSpeed;
  int field_64;       ///< Timer?
  int field_68;       ///< Timer?
  int field_6C;
  int field_70;

  int numPathfinderPoints;
  int currentPathfinderPoint;
  /// The path information is stored here as the UnitDirection the unit must take along every step of its current path.
  /// @note  If there isn't enough room for the entire path (or due to significant obstructions if RCC is active) then
  ///        the path is fragmented;  when it reaches the end of the current direction list it generates a new list for
  ///        the next path fragment.
  uint8 direction[64];

  int field_BC;
  int field_C0;
  int field_C4;
  int field_C8;
  int field_CC;  ///< Set to arg 3 of Unit.Pathfind (1 if there are player-defined waypoints) enum{0,1,2,3}
  int distance;  ///< Set to arg 4 of Unit.Pathfind (2 if there are player-defined waypoints)

  // ** TODO more fields
  uint8 field_D4[0x200 - 0xD4];
};
static_assert(sizeof(PathContext) == 0x200, "Incorrect PathContext size.");


/// Internal virtual linear allocator class for PathContexts.
class PathContextList : public OP2Class<PathContextList> {
public:
  ibool Init()  { return Thunk<0x446AE0, &$::Init>();  }
  void  Reset() { return Thunk<0x446B30, &$::Reset>(); }

  PathContext* Alloc()                 { return Thunk<0x446B60, &$::Alloc>();    }
  ibool        Free(PathContext* pCtx) { return Thunk<0x446BC0, &$::Free>(pCtx); }

  PathContext* At(size_t index)            { return Thunk<0x446C00, &$::At>(index);      }
  size_t       GetIndex(PathContext* pCtx) { return Thunk<0x446BE0, &$::GetIndex>(pCtx); }

  ibool Save(StreamIO* pSavedGame) { return Thunk<0x446C20, &$::Save>(pSavedGame); }
  ibool Load(StreamIO* pSavedGame) { return Thunk<0x446CF0, &$::Load>(pSavedGame); }

  static PathContextList* GetInstance() { return OP2Mem<0x557220, PathContextList*>(); }

  PathContext& operator[](size_t index) { return *At(index); }

public:
  int          field_00;
  PathContext* pBaseAllocAddr_;
  PathContext* pNextAllocAddr_;
  PathContext* pFreeListHead_;
};

END_PACKED

} // Tethys
