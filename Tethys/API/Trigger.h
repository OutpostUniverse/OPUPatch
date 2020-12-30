
#pragma once

#include "Tethys/Game/MapObjectType.h"
#include "Tethys/API/ScGroup.h"
#include "Tethys/API/Unit.h"

namespace Tethys::TethysAPI {

// ** TODO The Trigger API will be changed substantially.  The current interface should be considered a placeholder.

/// Comparison modes used by various trigger creation functions.
enum class CompareMode : int {
  Equal = 0,
  LowerEqual,
  GreaterEqual,
  Lower,
  Greater,
};

/// Trigger resource types used by CreateResourceTrigger.
enum class TriggerResource : int {
  Food = 0,
  CommonOre,
  RareOre,
  Kids,
  Workers,
  Scientists,
  Colonists,
};

/// Damage types used by CreateDamagedTrigger.  Refers to % of FightGroup that has been destroyed.
enum class TriggerDamage : int {
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


/// Info passed to trigger callbacks.  (1.4.0)
struct OnTriggerArgs {
  size_t  structSize;  ///< Size of this structure.
  Trigger trigger;     ///< Trigger that was fired.
};


/// Creates a victory condition (wraps another Trigger).
inline Trigger CreateVictoryCondition(Trigger condition, const char* pText, bool oneShot = false, bool enabled = true) {
  return OP2Thunk<0x479930, Trigger FASTCALL(ibool, ibool, const Trigger&, const char*)>(
    enabled, oneShot, condition, pText);
}

/// Creates a failure condition (wraps another Trigger).
inline Trigger CreateFailureCondition(Trigger condition, bool enabled = true)
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

/// Creates a trigger used for victory condition in Last One Standing and later part of Land Rush.
inline Trigger CreateLastOneStandingTrigger(
  const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
    { return OP2Thunk<0x478F30, Trigger FASTCALL(ibool, ibool, const char*)>(enabled, oneShot, pTriggerFunction); }

/// Creates a trigger used for victory condition in Space Race.
inline Trigger CreateSpaceRaceTrigger(
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479260, Trigger FASTCALL(ibool, ibool, int, const char*)>(
    enabled, oneShot, playerNum, pTriggerFunction);
}

/// Creates a trigger used for victory condition in Midas.
inline Trigger CreateMidasTrigger(
  int time, const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479300, Trigger FASTCALL(ibool, ibool, int, const char*)>(enabled, oneShot, time, pTriggerFunction);
}

// -------------------- Research and resource count triggers  (typically used in campaign missions) --------------------

/// Resource Race and campaign objectives
inline Trigger CreateResourceTrigger(
  TriggerResource resourceType, CompareMode compare, int refAmount,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x478DE0, Trigger FASTCALL(ibool, ibool, TriggerResource, int, int, CompareMode, const char*)>(
    enabled, oneShot, resourceType, refAmount, playerNum, compare, pTriggerFunction);
}

/// Creates a tech research trigger.
inline Trigger CreateResearchTrigger(
  int techID,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x478E90, Trigger FASTCALL(ibool, ibool, int, int, const char*)>(
    enabled, oneShot, techID, playerNum, pTriggerFunction);
}

/// Creates a structure kit count trigger.
inline Trigger CreateKitTrigger(
  MapID structureKitType, int refCount,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x4791C0, Trigger FASTCALL(ibool, ibool, int, MapID, int, const char*)>(
    enabled, oneShot, playerNum, structureKitType, refCount, pTriggerFunction);
}

// ------------------------------------------------ Unit count triggers ------------------------------------------------

/// Creates a unit/cargo type count trigger.
inline Trigger CreateCountTrigger(
  MapID unitType, MapID cargoOrWeapon, CompareMode compare, int refCount,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x479110, Trigger FASTCALL(ibool, ibool, int, MapID, MapID, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, unitType, cargoOrWeapon, refCount, compare, pTriggerFunction);
}

/// Used for Last One Standing failure condition and converting Land Rush to Last One Standing (when CC becomes active).
/// @warning Do not use AllPlayers.
inline Trigger CreateOperationalTrigger(
  MapID structureType, CompareMode compare, int refCount, int playerNum,
  const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479880, Trigger FASTCALL(ibool, ibool, int, MapID, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, structureType, refCount, compare, pTriggerFunction);
}

/// Creates a trigger that fires based on the player(s)' total number of vehicles.
inline Trigger CreateVehicleCountTrigger(
  CompareMode compare, int refCount,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x479440, Trigger FASTCALL(ibool, ibool, int, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, refCount, compare, pTriggerFunction);
}

/// Creates a trigger that fires based on the player(s)' total number of structures.
inline Trigger CreateBuildingCountTrigger(
  CompareMode compare, int refCount,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x4793A0, Trigger FASTCALL(ibool, ibool, int, int, CompareMode, const char*)>(
    enabled, oneShot, playerNum, refCount, compare, pTriggerFunction);
}

// --------------------------------------------------- Time triggers ---------------------------------------------------

/// Creates a trigger that fires on the specified time interval in ticks.
/// If @ref oneShot = false, fires again on every interval.
inline Trigger CreateTimeTrigger(
  int time, const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x478D00, Trigger FASTCALL(ibool, ibool, int, const char*)>(enabled, oneShot, time, pTriggerFunction);
}

/// Creates a trigger that fires randomly between the specified time interval in ticks.
/// If @ref oneShot = false, fires again on every interval, chosen randomly each time.
inline Trigger CreateTimeTrigger(
  int timeMin, int timeMax, const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x478DA0, Trigger FASTCALL(ibool, ibool, int, int, const char*)>(
    enabled, oneShot, timeMin, timeMax, pTriggerFunction);
}

// -------------------------------------------------- Special target ---------------------------------------------------

/// Creates a special target that triggers when any sourceUnitType instance moves next to targetUnit for a few seconds.
inline Trigger CreateSpecialTarget(
  const Unit& targetUnit, MapID sourceUnitType,
  const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x4797A0, Trigger FASTCALL(ibool, ibool, const Unit&, MapID, const char*)>(
    enabled, oneShot, targetUnit, sourceUnitType, pTriggerFunction);
}

/// Gets the unit that triggered the special target.
inline Unit GetSpecialTargetData(const Trigger& specialTargetTrigger)
  { Unit u;  OP2Thunk<0x479860, void FASTCALL(const Trigger&, Unit*)>(specialTargetTrigger, &u);  return u; }

// ---------------------------------------------- Attack/Damage triggers -----------------------------------------------

/// Creates a trigger that fires when the given ScGroup is under attack.
inline Trigger CreateAttackedTrigger(
  const ScGroup& group, const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x4795A0, Trigger FASTCALL(ibool, ibool, const ScGroup&, const char*)>(
    enabled, oneShot, group, pTriggerFunction);
}

/// Creates a trigger that fires when a percentage of the given ScGroup has been destroyed.
inline Trigger CreateDamagedTrigger(
  const ScGroup& group, TriggerDamage damage,
  const char* pTriggerFunction = nullptr, bool oneShot = true, bool enabled = true)
{
  return OP2Thunk<0x479640, Trigger FASTCALL(ibool, ibool, const ScGroup&, TriggerDamage, const char*)>(
    enabled, oneShot, group, damage, pTriggerFunction);
}

// ------------------------------------------------ Positional triggers ------------------------------------------------

/// Creates a trigger that fires when any unit is at the location.
inline Trigger CreatePointTrigger(
  Location where,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x479070, Trigger FASTCALL(ibool, ibool, int, int, int, const char*)>(
    enabled, oneShot, playerNum, where.x, where.y, pTriggerFunction);
}

/// Creates a trigger that fires when any unit enters the area rect.
inline Trigger CreateRectTrigger(
  MapRect area,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x478FC0, Trigger FASTCALL(ibool, ibool, int, int, int, int, int, const char*)>(
    enabled, oneShot, playerNum, area.x1, area.x2, area.Width(), area.Height(), pTriggerFunction);
}

/// Creates a rect trigger that filters based on the specified unit type (and cargo type/amount).
inline Trigger CreateEscapeTrigger(
  MapRect area, MapID unitType, int refCount, int cargoType, int cargoAmount,
  const char* pTriggerFunction = nullptr, int playerNum = AllPlayers, bool oneShot = false, bool enabled = true)
{
  return OP2Thunk<0x4796E0, Trigger FASTCALL(ibool, ibool, int, int, int, int, int, int, MapID, int, int, const char*)>(
    enabled, oneShot, playerNum, area.x1, area.y1, area.Width(), area.Height(), refCount, unitType, cargoType,
    cargoAmount, pTriggerFunction);
}

} // Tethys::TethysAPI
