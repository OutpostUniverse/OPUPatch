
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Unit.h"

namespace Tethys {

class ScBase;
class StreamIO;
class MapObject;

/// Internal class managing the ScStub list.
class ScStubList : public OP2Class<ScStubList> {
public:
  ScStubList() : lastCreatedIndex_(NilIndex), pScStubArray_() { }

  ibool Save(StreamIO* pSavedGame) { return Thunk<0x47AB60, &ScStubList::Save>(pSavedGame); }
  ibool Load(StreamIO* pSavedGame) { return Thunk<0x47ACE0, &ScStubList::Load>(pSavedGame); }
  int   Checksum()                 { return Thunk<0x47B0E0, &ScStubList::Checksum>();       }

  void Init()  { return Thunk<0x47AAB0, &ScStubList::Init>();  }
  void Reset() { return Thunk<0x47AB00, &ScStubList::Reset>(); }

  static ScStubList* GetInstance() { return OP2Mem<ScStubList*&>(0x47B122); }  // 0x56C358

  ScBase*& operator[](size_t index) { return pScStubArray_[index]; }
  ScBase*& At(size_t         index) { return pScStubArray_[index]; }

  // ** TODO 0x47AF20 Something to do with saving
  // ** TODO 0x47AFB0 Something to do with loading
  // ** TODO static CDECL 0x47B150 Compare function for qsort

public:
  static constexpr size_t MaxNumScStubs = 255;
  static constexpr size_t NilIndex      = MaxNumScStubs;

  uint32  lastCreatedIndex_;
  ScBase* pScStubArray_[MaxNumScStubs];
};


/// Internal class for creating ScStubs.
class ScStubFactory {
public:
  ScStubFactory* pParent_;
  const char*    pName_;
  uint32         elementSizeInBytes_;

  ScBase* (FASTCALL* pfnCreateStub_)(void* pMemory);
};


/// Internal base class for mission objects such as triggers, groups, victory/failure conditions, etc.
class ScBase : public OP2Class<ScBase> {
public:
  virtual ScBase*        Destroy(ibool freeMem)     { return Thunk<0x424A60, &ScBase::Destroy>(freeMem);   }
  virtual ScStubFactory* GetScStubFactory()         { return Thunk<0x424A10, &ScBase::GetScStubFactory>(); }
  virtual void           Init()                     { return Thunk<0x424A20, &ScBase::Init>();             }
  virtual void           Enable()                   { return Thunk<0x424A30, &ScBase::Enable>();           }
  virtual void           Disable()                  { return Thunk<0x424A40, &ScBase::Disable>();          }
  virtual void           Save(StreamIO* pSavedGame) { return Thunk<0x47B5B0, &ScBase::Save>(pSavedGame);   }
  virtual ibool          Load(StreamIO* pSavedGame) { return Thunk<0x47B5E0, &ScBase::Load>(pSavedGame);   }
  virtual void           Delete()                   { return Thunk<0x424A50, &ScBase::Delete>();           }
  virtual ibool          IsEnabled()                { return Thunk<0x47B620, &ScBase::IsEnabled>();        }

#define OP2_SCBASE_VTBL($)  \
  $(Destroy)  $(GetScStubFactory)  $(Init)  $(Enable)  $(Disable)  $(Save)  $(Load)  $(Delete)  $(IsEnabled)
  DEFINE_VTBL_TYPE(OP2_SCBASE_VTBL, 0x4CFE98);

  static ScBase* GetInstance(int index)
    { return ((index >= 0) && (index != ScStubList::NilIndex)) ? (*ScStubList::GetInstance())[index] : nullptr; }

public:
  int   index_;
  ibool isEnabled_;
  int   field_0C;
  int   field_10;
};


/// Internal implementation class for entities from the mission DLL.
class ScriptDataBlock : public ScBase {
public:
  static ScriptDataBlock* GetInstance(int index) { return static_cast<ScriptDataBlock*>(ScBase::GetInstance(index)); }

public:
  int   field_14;
  int   field_18;
  ibool useLevelModule_;
  char* pFuncName_;
  void* pFuncAddress_;
};

/// Internal implementation class for mission DLL function references.
class FuncReference : public ScriptDataBlock {
public:
  static FuncReference* GetInstance(int index) { return static_cast<FuncReference*>(ScBase::GetInstance(index)); }

public:
  int field_28;
  int field_2C;
};

/// Internal implementation class for triggers.
class TriggerImpl : public ScBase {
public:
  static TriggerImpl* GetInstance(int index) { return static_cast<TriggerImpl*>(ScBase::GetInstance(index)); }

public:
  int            field_14;
  TriggerImpl*   pNext_;
  ibool          isOneShot_;
  int            playerVectorHasFired_;
  FuncReference* pFuncRef_;
};


class TargetCount {
public:
  // ** TODO member functions

public:
  struct UnitTypeTargetCount {
    MapID unitType;
    MapID weaponType;
    int   targetCount;
    int   field_0C;     ///< currentCount?
  };

  int                  numAllocatedUnitTypeTargetCounts_;
  UnitTypeTargetCount* pUnitTypeTargetCounts_;
  int                  groupScStubIndex_;
  int                  numUnitTypeTargetCounts_;
  int                  field_10;
};
static_assert(sizeof(TargetCount) == 0x14, "Incorrect TargetCount size.");


/// Internal implementation for ScGroups.
class ScGroupImpl : public ScBase {
public:
  static ScGroupImpl* GetInstance(int index) { return static_cast<ScGroupImpl*>(ScBase::GetInstance(index)); }

  virtual void HasFired();                         // 0x24 ** [Name?]
  virtual void AddUnit(MapObject* pMapObject);
  virtual void RemoveUnit(MapObject* pMapObject);
  virtual void RemoveDeadAndCapturedUnits();
  virtual void A2();                               // 0x34 **

public:
  struct UnitNode {
    union {
      int       nextFreeIndex;
      UnitNode* pPrev;
    };
    UnitNode*   pNext;
    MapObject*  pUnit;

    int issueCommandTick;  ///< Set to 0xFFF00000 when adding unit (related to gameTick and deleteWhenEmpty)

    UnitClassification classification;
  };

  int          field_14;
  int          field_18;
  int          lastFreeUnitNodeIndex_;                           ///< (+= 1)
  int          field_20;                                         ///< (+= 1)
  UnitNode     unitNode_[32];                                    ///< Linked list storage for pointers to up to 32 units
  UnitNode*    pUnitByType_[size_t(UnitClassification::NotSet)]; ///< Indexed by UnitClassification
  TargetCount* pTargetCount_;                                    ///< Object to keep track of target counts
  UnitNode*    pUnitListHead_;                                   ///< Ordered by UnitClassification
  UnitNode*    pUnitListTail_;                                   ///< Ordered by UnitClassification
  int          numUnits_;                                        ///< Returned by TotalUnitCount()
  int          ownerPlayerNum_;                                  ///< All units in group should belong to this player
  int          deleteWhenEmptyTick_;                             ///< Inited to -1
                                                                 ///  Set to tick + 10000 by SetDeleteWhenEmpty(true)
  ibool        setLights_;                                       ///< Inited to  1 (on)
  int          field_300;                                        ///< (Part of this class, or derived classes?)
  int          field_304;                                        ///< (Part of this class, or derived classes?)
};


struct RecordedBuilding {
  MapRect buildingTileRect;
  MapID    buildingType;
  MapID    weaponType;
  int      groupScStubIndex;
};

struct RecordedMine {
  MapRect mineRectInTiles;
  MapID    mineType;
  int      buildGroupScStubIndex;
  int      minerUnitIndex;
};

struct RecordedTubeWall {
  uint16 tileX;
  uint16 tileY;
  int    cellType;  ///< 0x17 = Wall, 0x18 = MicrobeWall, 0x19 = LavaWall, 0x1A = Tube
  int    field_08;
};

struct RecordedVehGroup {
  int targetGroupScStubIndex;
  int priority;
  int unitIndex;
};

struct RecordInfo {
  int  numRecordedBuildings;
  int  numRecordedMines;
  int  numRecordedTubesWalls;
  int  numRecordedVehGroup;
  RECT defaultRectInPixels;
  int  field_348;               ///< (=0) Index into function pointer table
  int  convecUnitIndex;         ///< TransferCargo related
  int  factoryUnitIndex;        ///< TransferCargo related
  int  recordedBuildingIndex;   ///< TransferCargo related
  int  factoryBayIndex;         ///< TransferCargo related
};

/// Internal implementation for BuildingGroups.
class BuildingGroupImpl : public ScGroupImpl {
public:
  static BuildingGroupImpl* GetInstance(int index)
    { return static_cast<BuildingGroupImpl*>(ScBase::GetInstance(index)); }

public:
  int               numAllocatedRecordedBuildings_;   ///< Allocated in blocks of 8 entries
  RecordedBuilding* pRecordedBuildings_;              ///< Allocated in blocks of 8 entries
  int               numAllocatedRecordedMines_;       ///< Allocated in blocks of 4 entries
  RecordedMine*     pRecordedMines_;                  ///< Allocated in blocks of 4 entries
  int               numAllocatedRecordedTubesWalls_;  ///< Allocated in blocks of 8 entries
  RecordedTubeWall* pRecordedTubesWalls_;             ///< Allocated in blocks of 8 entries
  int               numAllocatedRecordedVehGroups_;   ///< Allocated in blocks of 4 entries
  RecordedVehGroup* pRecordedVehGroups_;              ///< Allocated in blocks of 4 entries
  RecordInfo        recordInfo_;
};


enum class CombatGroupObjective : int {
  GuardRect = 0,
  Patrol,
  GuardGroup,
  GuardUnit,
  AttackUnit,
  AttackEnemy,
  ExitMap
};

/// Internal implementation for CombatGroups.
class FightGroupImpl : public ScGroupImpl {
public:
  static FightGroupImpl* GetInstance(int index) { return static_cast<FightGroupImpl*>(ScBase::GetInstance(index)); }

public:
  ibool    combineFire_;
  int      combineFireUnitIndex_;
  int      field_310;              ///< Set to 0xFFF00000, related to game tick, possibly spider related?
  int      numWaypoints_;
  int      field_318;              ///< Set in SetWaypoints to unknown parameter, used in IssuePatrol?
  Waypoint waypointList_[8];
  int      patrolMode_;
  int      followMode_;
  RECT     pixelRect_;             ///< SetRect converted to pixels (rect.p1.y initialized to -1)
  int      targetGroupIndex_;      ///< Initialized to -1
  MapID    attackType_;            ///< Initialized to mapNone
  int      targetUnitIndex_;       ///< Initialized to -1
  int      numGuardedRects_;
  RECT     guardedRect_[8];
  int      field_3E4;              ///< Index into a list of objects with 5 function pointers each
};
static_assert(sizeof(FightGroupImpl) == 0x3E8, "Incorrect CombatBase size.");


/// Internal implementation for MineGroups.
class MineGroupImpl : public ScGroupImpl {
public:
  static MineGroupImpl* GetInstance(int index) { return static_cast<MineGroupImpl*>(ScBase::GetInstance(index)); }

public:
  int   mineUnitIndex_;
  int   smelterUnitIndex_;
  int   minePixelX_;
  int   minePixelY_;
  int   smelterPixelX_;
  int   smelterPixelY_;
  MapID mineType_;
  MapID smelterType_;
  RECT  mineGroupPixelRect_;
};

} // Tethys
