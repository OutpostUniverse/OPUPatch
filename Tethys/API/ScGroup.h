
#pragma once

#include "Tethys/Game/MapObjectType.h"
#include "Tethys/API/Location.h"
#include "Tethys/API/ScStub.h"
#include "Tethys/API/Unit.h"

namespace Tethys::TethysAPI {

class UnitBlock;

/// Exported interface for UI unit groups (wraps ScGroupImpl).  @see @ref GroupEnumerator.
class ScGroup : public ScStub {
  using $ = ScGroup;
public:
  using ScStub::ScStub;

        auto* GetImpl()       { return IsValid() ? ScGroupImpl::GetInstance(id_) : nullptr; }
  const auto* GetImpl() const { return IsValid() ? ScGroupImpl::GetInstance(id_) : nullptr; }

  /// Sets this ScGroup to auto-delete when all units are dead.
  void SetDeleteWhenEmpty(ibool state) { return Thunk<0x479B80, &$::SetDeleteWhenEmpty>(state); }

  ///@{ Adds or removes units to/from the ScGroup.
  void TakeUnit(Unit             unitToAdd)    { return Thunk<0x479AF0, &$::TakeUnit>(unitToAdd);        }
  void TakeAllUnits(ScGroup*     pSourceGroup) { return Thunk<0x479BA0, &$::TakeAllUnits>(pSourceGroup); }
  void AddUnits(const UnitBlock& unitsToAdd)   { return Thunk<0x4799D0, &$::AddUnits>(unitsToAdd);       }
  void RemoveUnit(Unit           unitToRemove) { return Thunk<0x479BD0, &$::RemoveUnit>(unitToRemove);   }
  ///@}

  ///@{ Gets the number of units (of the specified classification).
  int TotalUnitCount()                       const { return Thunk<0x479A10, &$::TotalUnitCount>();    }
  int UnitCount(UnitClassification unitType) const { return Thunk<0x4799F0, &$::UnitCount>(unitType); }
  ///@}

  /// Returns true if this group is currently being attacked.
  ibool IsUnderAttack() const { return Thunk<0x479AE0, &$::IsUnderAttack>(); }

  /// Sets headlights on all vehicles in the group.
  void SetLights(ibool on) { return Thunk<0x479B60, &$::SetLights>(on); }

  ///@{ Requests units to be built (by this or other ScGroups) and added to the group.
  void ClearTargCount()                         { return Thunk<0x479CB0, &$::ClearTargCount>();              }
  void SetTargCount(const UnitBlock& unitTypes) { return Thunk<0x479C40, void(const UnitBlock&)>(unitTypes); }
  void SetTargCount(MapID unitType, MapID weaponType, int targetCount)
    { return Thunk<0x479C70, void(MapID, MapID, int)>(unitType, weaponType, targetCount); }
  ///@}

  ///@{ Gets the first Unit of the specified type.
  Unit GetFirstOfType(UnitClassification unitType) const
    { Unit u;  Thunk<0x479A20, int(Unit*, UnitClassification)>(&u, unitType);  return u; }
  Unit GetFirstOfType(MapID unitType, MapID cargoOrWeapon) const
    { Unit u;  Thunk<0x479A60, int(Unit*, MapID, MapID)>(&u, unitType, cargoOrWeapon);  return u; }
  ///@}

  int GetOwner() const { return GetImpl()->ownerPlayerNum_; }  ///< Gets the owner player ID of this ScGroup.
};


/// Exported interface for AI construction unit groups (wraps BuildingGroupImpl).
class BuildingGroup : public ScGroup {
  using $ = BuildingGroup;
public:
  using ScGroup::ScGroup;

        auto* GetImpl()       { return IsValid() ? BuildingGroupImpl::GetInstance(id_) : nullptr; }
  const auto* GetImpl() const { return IsValid() ? BuildingGroupImpl::GetInstance(id_) : nullptr; }

  void SetRect(const MapRect& defaultLocation) { return Thunk<0x47A2E0, &$::SetRect>(defaultLocation); }

  void RecordBuilding(Location where, MapID unitType, MapID cargoOrWeapon = MapID::None)
    { return Thunk<0x47A3E0, void(Location&, MapID, MapID)>(where, unitType, cargoOrWeapon); }
  void RecordBuilding(Location where, MapID unitType, MapID cargoOrWeapon, ScGroup produceGroup) {
    return Thunk<0x47A410, void(Location&, MapID, MapID, ScGroup&)>(where, unitType, cargoOrWeapon, produceGroup);
  }

  void RecordTube(Location         where)                   { return Thunk<0x47A390, &$::RecordTube>(where);           }
  void RecordConnectTubes(Location toWhere)                 { return Thunk<0x47A370, &$::RecordConnectTubes>(toWhere); }
  void RecordWall(Location where, MapID type = MapID::Wall) { return Thunk<0x47A3B0, &$::RecordWall>(where, type);     }

  void RecordUnitBlock(const UnitBlock& units) { return Thunk<0x47A330, void(const UnitBlock&)>(units); }
  void RecordUnitBlock(const UnitBlock& units, ScGroup produceGroup)
    { return Thunk<0x47A350, void(const UnitBlock&, ScGroup&)>(units, produceGroup); }

  void RecordVehReinforceGroup(ScGroup targetGroup, int priority)  ///< 0 = lowest priority, 0xFFFF = highest
    { return Thunk<0x47A440, &$::RecordVehReinforceGroup>(targetGroup, priority); }
  void UnRecordVehGroup(ScGroup        targetGroup) { return Thunk<0x47A460, &$::UnRecordVehGroup>(targetGroup); }
};


/// Exported interface for AI mining unit groups (wraps MiningGroupImpl).
class MiningGroup : public ScGroup {
public:
  using ScGroup::ScGroup;

  void Setup(Location mine, Location smelter, const MapRect& smelterArea)
    { return Thunk<0x47A160, void(Location&, Location&, const MapRect&)>(mine, smelter, smelterArea); }
  void Setup(Location mine, Location smelter, MapID mineType, MapID smelterType, const MapRect& smelterArea) {
    return Thunk<0x47A240, void(Location&, Location&, MapID, MapID, const MapRect&)>(
      mine, smelter, mineType, smelterType, smelterArea);
  }
  void Setup(Unit mine, Unit smelter, const MapRect& smelterArea)
    { return Thunk<0x47A100, void(Unit, Unit, const MapRect&)>(mine, smelter, smelterArea); }
};


/// Exported interface for AI combat unit groups (wraps FightGroupImpl).
class FightGroup : public ScGroup {
  using $ = FightGroup;
public:
  using ScGroup::ScGroup;

        auto* GetImpl()       { return IsValid() ? FightGroupImpl::GetInstance(id_) : nullptr; }
  const auto* GetImpl() const { return IsValid() ? FightGroupImpl::GetInstance(id_) : nullptr; }

  void SetRect(const MapRect& idleRect)           { return Thunk<0x479DA0, &$::SetRect>(idleRect);           }
  void AddGuardedRect(const MapRect& guardedRect) { return Thunk<0x479E60, &$::AddGuardedRect>(guardedRect); }
  void ClearGuardedRects()                        { return Thunk<0x479EE0, &$::ClearGuardedRects>();         }

  void SetFollowMode(int mode)                  { return Thunk<0x479F30, &$::SetFollowMode>(mode);   }
  void SetPatrolMode(const PatrolRoute& waypts) { return Thunk<0x479D20, &$::SetPatrolMode>(waypts); }
  void ClearPatrolMode()                        { return Thunk<0x479D80, &$::ClearPatrolMode>();     }

  void SetCombineFire()   { return Thunk<0x479CC0, &$::SetCombineFire>();   }
  void ClearCombineFire() { return Thunk<0x479CF0, &$::ClearCombineFire>(); }

  void SetAttackType(MapID type)           { return Thunk<0x479F60, &$::SetAttackType>(type);         }
  void SetTargetGroup(ScGroup targetGroup) { return Thunk<0x479F90, &$::SetTargetGroup>(targetGroup); }
  void SetTargetUnit(Unit targetUnit)      { return Thunk<0x479F00, &$::SetTargetUnit>(targetUnit);   }

  void DoAttackEnemy() { return Thunk<0x47A080, &$::DoAttackEnemy>(); }  ///< Use in combination with SetAttackType()
  void DoAttackUnit()  { return Thunk<0x47A060, &$::DoAttackUnit>();  }
  void DoGuardGroup()  { return Thunk<0x47A020, &$::DoGuardGroup>();  }  ///< Use in combination with SetTargetGroup()
  void DoGuardRect()   { return Thunk<0x479FC0, &$::DoGuardRect>();   }
  void DoGuardUnit()   { return Thunk<0x47A040, &$::DoGuardUnit>();   }
  void DoPatrolOnly()  { return Thunk<0x47A000, &$::DoPatrolOnly>();  }
  void DoExitMap()     { return Thunk<0x479FE0, &$::DoExitMap>();     }
};


struct MrRec {
  MrRec(MapID unit, MapID weapon = MapID::None, int u1 = 0, int u2 = 0)
    : unitType(unit), weaponType(weapon), unknown1(u1), unknown2(u2) {}
  MrRec()
    : unitType(MapID::None), weaponType(MapID::None), unknown1(-1), unknown2(-1) {}

  MapID unitType;
  MapID weaponType;
  int   unknown1;    ///< -1 to terminate list
  int   unknown2;    ///< -1 to terminate list
};

struct PWDef {
  PWDef(int x1_, int y1_, int x2_, int y2_, int x3_, int y3_, int x4_, int y4_, int t1, int t2, int t3)
    : x1(x1_), y1(y1_), x2(x2_), y2(y2_), x3(x3_), x4(x4_), y4(y4_), time1(t1), time2(t2), time3(t3) { }
  PWDef()
    : x1(-1), y1(0), x2(0), y2(0), x3(0), x4(0), y4(0), time1(-1), time2(-1), time3(-1) { }

  int x1;     ///< -1 to terminate list
  int y1;
  int x2;
  int y2;
  int x3;
  int y3;
  int x4;
  int y4;
  int time1;
  int time2;
  int time3;
};


/// Exported interface for AI strategy manager.  Can spawn ScGroups.
class Pinwheel : public ScStub {
  using $ = Pinwheel;
public:
  using ScStub::ScStub;

  void SendWaveNow(int a)                      { return Thunk<0x47AA40, &$::SendWaveNow>(a);                  }
  void SetWavePeriod(int minTime, int maxTime) { return Thunk<0x47A970, &$::SetWavePeriod>(minTime, maxTime); }

  void SetGuardComp(int  minUnits, int maxUnits, const MrRec* pMrRecList)
    { return Thunk<0x47A9E0, &$::SetGuardComp>(minUnits,  maxUnits, pMrRecList); }
  void SetAttackComp(int minUnits, int maxUnits, const MrRec* pMrRecList)
    { return Thunk<0x47A9B0, &$::SetAttackComp>(minUnits, maxUnits, pMrRecList); }
  void SetSapperComp(int minUnits, int maxUnits, const MrRec* pMrRecList)
    { return Thunk<0x47AA10, &$::SetSapperComp>(minUnits, maxUnits, pMrRecList); }

  void SetAttackFraction(int attackFraction) { return Thunk<0x47AA60, &$::SetAttackFraction>(attackFraction); }
  void SetContactDelay(int delay)            { return Thunk<0x47A990, &$::SetContactDelay>(delay);            }
  void SetNoRange(int a, int b)              { return Thunk<0x47A950, &$::SetNoRange>(a, b);                  }
  void SetPoints(const PWDef* pPwDefList)    { return Thunk<0x47A8B0, &$::SetPoints>(pPwDefList);             }
};

} // Tethys::TethysAPI
