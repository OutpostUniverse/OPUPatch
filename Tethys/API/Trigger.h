
#pragma once

#include "Tethys/Game/MapObjectType.h"
#include "Tethys/API/ScGroup.h"
#include "Tethys/API/Unit.h"

/// Used by various trigger creation functions.
enum class compare_mode : int {
  Equal = 0,
  LowerEqual,
  GreaterEqual,
  Lower,
  Greater,
};
using CompareMode = compare_mode;

/// Used by CreateResourceTrigger.
enum class trig_res : int {
  Food = 0,
  CommonOre,
  RareOre,
  Kids,
  Workers,
  Scientists,
  Colonists,
};
using TriggerResource = trig_res;

/// Damage types used by CreateDamagedTrigger.  Refers to % of FightGroup that has been destroyed.
enum class DamageType : int {
  _100 = 1,  ///< 100% damaged
  _75,       ///<  75% damaged
  _50,       ///<  50% damaged
};


/// Exported class wrapping TriggerImpl.
class Trigger : public ScStub {
  using $ = Trigger;
public:
  using ScStub::ScStub;

        TriggerImpl* GetImpl()       { return IsValid() ? TriggerImpl::GetInstance(id_) : nullptr; }
  const TriggerImpl* GetImpl() const { return IsValid() ? TriggerImpl::GetInstance(id_) : nullptr; }

  ibool HasFired(int playerNum) { return Thunk<0x478CC0, &$::HasFired>(playerNum); }  ///< @note Do not use AllPlayers.
};


/// Creates a victory condition (wraps another Trigger).
Trigger CreateVictoryCondition(Trigger condition, const char* pText, bool oneShot = false, bool enabled = true) {
  return OP2Thunk<0x479930, Trigger FASTCALL(ibool, ibool, const Trigger&, const char*)>(
    enabled, oneShot, condition, pText);
}

/// Creates a failure condition (wraps another Trigger).
Trigger CreateFailureCondition(Trigger condition, bool enabled = true)
  { return OP2Thunk<0x479980, Trigger FASTCALL(ibool, ibool, const Trigger&, const char*)>(enabled, 0, condition, ""); }


/// Set trigger.  Used to collect a number of other triggers into a single trigger output.  Can be used for e.g. any 3
/// in a set of 5 objectives.
template <typename... Ts>
Trigger CreateSetTrigger(
  const char* pTriggerFunction, int needed = sizeof...(Ts), bool oneShot = false, bool enabled = true, Ts... triggers)
{
  return OP2Thunk<0x4794E0, Trigger CDECL(ibool, ibool, int, int, const char*, ...)>(
    enabled, oneShot, sizeof...(Ts), needed, pTriggerFunction, triggers...);
}


// --------------------------------------------- Typical victory triggers ----------------------------------------------

/// Last One Standing and later part of Land Rush
Trigger CreateLastOneStandingTrigger(const char* pTriggerFunction, bool oneShot = true, bool enabled = true)
  { return OP2Thunk<0x478F30, Trigger FASTCALL(ibool, ibool, const char*)>(enabled, oneShot, pTriggerFunction); }

/// Space Race
Trigger CreateSpaceRaceTrigger(
  const char* pTriggerFunction, int playerNum = AllPlayers, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479260, Trigger FASTCALL(ibool, ibool, int, const char*)>(
    enabled, oneShot, playerNum, pTriggerFunction);
}

/// Midas
Trigger CreateMidasTrigger(int time, const char* pTriggerFunction, bool oneShot = true, bool enabled = true) {
  return OP2Thunk<0x479300, Trigger FASTCALL(ibool, ibool, int, const char*)>(enabled, oneShot, time, pTriggerFunction);
}

// -------------------- Research and resource count triggers  (typically used in campaign missions) --------------------

/// Resource Race and campaign objectives
Trigger CreateResourceTrigger(
  TriggerResource resourceType, CompareMode compare, int refAmount, const char* pTriggerFunction,
  int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x478DE0, Trigger FASTCALL(ibool, ibool, TriggerResource, int, int, CompareMode, const char*)>(
    enabled, oneShot, resourceType, refAmount, playerNum, compare, pTriggerFunction);
}

Trigger CreateResearchTrigger(
  int techID, const char* pTriggerFunction, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x478E90, Trigger FASTCALL(ibool, ibool, int, int, const char*)>(
    enabled, oneShot, techID, playerNum, pTriggerFunction);
}

Trigger CreateKitTrigger(
  MapID structureKitType, int refCount, const char* pTriggerFunction,
  int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x4791C0, Trigger FASTCALL(ibool, ibool, int, MapID, int, const char*)>(
    enabled, oneShot, playerNum, structureKitType, refCount, pTriggerFunction);
}

// ------------------------------------------------ Unit count triggers ------------------------------------------------

Trigger CreateCountTrigger(
  MapID unitType, MapID cargoOrWeapon, CompareMode compare, int refCount, const char* pTriggerFunction,
  int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x479110, Trigger FASTCALL(ibool, ibool, int, MapID, MapID, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, unitType, cargoOrWeapon, refCount, compare, pTriggerFunction);
}

/// Used for Last One Standing failure condition and converting Land Rush to Last One Standing (when CC becomes active).
/// @warning Do not use AllPlayers.
Trigger CreateOperationalTrigger(
  MapID structureType, CompareMode compare, int refCount, int playerNum, const char* pTriggerFunction,
  bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479880, Trigger FASTCALL(ibool, ibool, int, MapID, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, structureType, refCount, compare, pTriggerFunction);
}

Trigger CreateEscapeTrigger(
  MapRect area, MapID unitType, int refCount, int cargoType, int cargoAmount, const char* pTriggerFunction,
  int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x4796E0, Trigger FASTCALL(ibool, ibool, int, int, int, int, int, int, MapID, int, int, const char*)>(
    enabled, oneShot, playerNum, area.x1, area.y1, area.Width(), area.Height(), refCount, unitType, cargoType,
    cargoAmount, pTriggerFunction);
}

Trigger CreateVehicleCountTrigger(
  CompareMode compare, int refCount, const char* pTriggerFunction,
  int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x479440, Trigger FASTCALL(ibool, ibool, int, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, refCount, compare, pTriggerFunction);
}

Trigger CreateBuildingCountTrigger(
  CompareMode compare, int refCount, const char* pTriggerFunction,
  int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x4793A0, Trigger FASTCALL(ibool, ibool, int, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, refCount, compare, pTriggerFunction);
}

// --------------------------------------------------- Time triggers ---------------------------------------------------

Trigger CreateTimeTrigger(int time, const char* pTriggerFunction, bool oneShot = true, bool enabled = true) {
  return OP2Thunk<0x478D00, Trigger FASTCALL(ibool, ibool, int, const char*)>(enabled, oneShot, time, pTriggerFunction);
}

Trigger CreateTimeTrigger(
  int timeMin, int timeMax, const char* pTriggerFunction, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x478DA0, Trigger FASTCALL(ibool, ibool, int, int, const char*)>(
    enabled, oneShot, timeMin, timeMax, pTriggerFunction);
}

// -------------------------------------------------- Special target ---------------------------------------------------

/// Creates a special target that triggers when any sourceUnitType instance moves next to targetUnit for a few seconds.
Trigger CreateSpecialTarget(
  const Unit& targetUnit, MapID sourceUnitType, const char* pTriggerFunction, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x4797A0, Trigger FASTCALL(ibool, ibool, const Unit&, MapID, const char*)>(
    enabled, oneShot, targetUnit, sourceUnitType, pTriggerFunction);
}

/// Gets the unit that triggered the special target.
Unit GetSpecialTargetData(const Trigger& specialTargetTrigger)
  { Unit u;  OP2Thunk<0x479860, void FASTCALL(const Trigger&, Unit*)>(specialTargetTrigger, &u);  return u; }

// ---------------------------------------------- Attack/Damage triggers -----------------------------------------------

Trigger CreateAttackedTrigger(
  const ScGroup& group, const char* pTriggerFunction, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x4795A0, Trigger FASTCALL(ibool, ibool, const ScGroup&, const char*)>(
    enabled, oneShot, group, pTriggerFunction);
}

Trigger CreateDamagedTrigger(
  const ScGroup& group, DamageType damageType, const char* pTriggerFunction, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479640, Trigger FASTCALL(ibool, ibool, const ScGroup&, DamageType, const char*)>(
    enabled, oneShot, group, damageType, pTriggerFunction);
}

// ------------------------------------------------ Positional triggers ------------------------------------------------

Trigger CreatePointTrigger(
  Location where, const char* pTriggerFunction, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x479070, Trigger FASTCALL(ibool, ibool, int, int, int, const char*)>(
    enabled, oneShot, playerNum, where.x, where.y, pTriggerFunction);
}

Trigger CreateRectTrigger(
  MapRect area, const char* pTriggerFunction, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x478FC0, Trigger FASTCALL(ibool, ibool, int, int, int, int, int, const char*)>(
    enabled, oneShot, playerNum, area.x1, area.x2, area.Width(), area.Height(), pTriggerFunction);
}
