
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Game/CommandPacket.h"
#include "Tethys/Game/GameStartInfo.h"
#include "Tethys/Game/UnitGroup.h"

namespace Tethys {

class MapObject;
class Building;
class Vehicle;
class MapEntity;
class MapChildEntity;

constexpr uint32 MaxPlayers = 7;

/// Possible values for PlayerImpl::difficulty_.
enum class DifficultyLevel : int {
  Easy = 0,
  Normal,
  Hard,
  Count
};

/// Possible values for PlayerImpl::resourceLevel_.
enum class ResourceLevel : int {
  High = 0,
  Medium,
  Low,
  Count
};

/// Possible values for PlayerImpl::colorType_.
enum class PlayerColor : int {
  Blue = 0,
  Red,
  Green,
  Yellow,
  Cyan,
  Magenta,
  _6,
  _7,
  Count
};

/// Internal player object (wrapped by _Player).
class PlayerImpl : public OP2Class<PlayerImpl> {
public:
   PlayerImpl() { }
  ~PlayerImpl() { }

  void Init(int playerNum) { return Thunk<0x490270, &$::Init>(playerNum); }

  /// Returns pointer to CommandPacket buffer that will get sent over the network at the present tick.
  CommandPacket* GetNextCommandPacket()                   { return Thunk<0x490810, &$::GetNextCommandPacket>();       }
  ibool ProcessCommandPacket(const CommandPacket& packet) { return Thunk<0x40E300, &$::ProcessCommandPacket>(packet); }
  size_t  GetPacketUnitCount(const void* pCPData) const   { return Thunk<0x490920, &$::GetPacketUnitCount>(pCPData);  }
  uint16* GetPacketUnitIDs(const void*   pCPData) const   { return Thunk<0x490960, &$::GetPacketUnitIDs>(pCPData);    }

  const char* GetPlayerName() const            { return Thunk<0x490C80, &$::GetPlayerName>();      }
  void        SetPlayerName(const char* pName) { return Thunk<0x490CB0, &$::SetPlayerName>(pName); }

  int GetSatelliteCount(MapID objectType) const { return Thunk<0x4908E0, &$::GetSatelliteCount>(objectType); }

  ibool         IsAlly(int playerNum) const { return Thunk<0x490D30, &$::IsAlly>(playerNum); } ///< Both allied together
  PlayerBitmask GetAlliedTo()         const { return Thunk<0x490D60, &$::GetAlliedTo>();     } ///< Allied by us
  PlayerBitmask GetHostileTo()        const { return Thunk<0x490DB0, &$::GetHostileTo>();    } ///< Not allied by us

  void Starve(int numToStarve, ibool skipMoraleUpdate)
    { return Thunk<0x471C70, &$::Starve>(numToStarve, skipMoraleUpdate); }

  void GoAI()    { return Thunk<0x490680, &$::GoAI>();    }
  void GoHuman() { return Thunk<0x4906C0, &$::GoHuman>(); }

  /// Self destruct without quitting the game.
  void DoSelfDestruct()     { return Thunk<0x4909B0, &$::DoSelfDestruct>();     }
  /// Transfer to allies without quitting the game.
  void DoTransferToAllies() { return Thunk<0x4909F0, &$::DoTransferToAllies>(); }

  void UpdateNumColonists()          { return Thunk<0x471D60, &$::UpdateNumColonists>();          }
  void UpdateColonyStatusAndReport() { return Thunk<0x471FD0, &$::UpdateColonyStatusAndReport>(); }

  uint16 GetMiniMapColor()  const { return Thunk<0x490CE0, &$::GetMiniMapColor>();  }
  uint16 GetChatTextColor() const { return Thunk<0x490BE0, &$::GetChatTextColor>(); }

  auto* GetBuildings() const { return reinterpret_cast<Building*>(pBuildingList_);      }
  auto* GetVehicles()  const { return reinterpret_cast<Vehicle*>(pVehicleList_);        }
  auto* GetBeacons()   const { return reinterpret_cast<MapChildEntity*>(pVehicleList_); }
  auto* GetEntities()  const { return reinterpret_cast<MapEntity*>(pEntityList_);       }

public:
  struct BuildingStats {
    int numBuildings;
    int numActive;
    int numDisabled;
    int numIdle;
  };

  int playerNum_;

  PlayerBitmask alliedBy_;   ///< Players which have set themselves as allies to this player.

  SatelliteCounts satellites_;

  union {
    DifficultyLevel difficulty_;
    ResourceLevel   resourceLevel_;
  };

  int   foodStored_;
  int   maxFood_;
  int   maxCommonOre_;
  int   maxRareOre_;
  int   commonOre_;
  int   rareOre_;
  ibool isHuman_;
  ibool isEden_;

  PlayerColor colorType_;

  MoraleState moraleState_;

  int scientistGrowthRemainder_;
  int workerGrowthRemainder_;
  int kidGrowthRemainder_;
  int kidDeathRemainder_;
  int scientistDeathRemainder_;
  int workerDeathRemainder_;
  int numWorkers_;
  int numScientists_;
  int numKids_;

  ibool recalculateValues_;

  int numAvailableWorkers_;
  int numAvailableScientists_;
  int amountPowerGenerated_;
  int inactivePowerCapacity_;
  int amountPowerConsumed_;
  int amountPowerAvailable_;
  int numIdleBuildings_;
  int numActiveBuildings_;
  int numBuildings_;
  int numUnpoweredStructures_;
  int numWorkersRequired_;
  int numScientistsRequired_;
  int numScientistsAsWorkers_;
  int numScientistsResearching_;
  int totalFoodProduction_;
  int totalFoodConsumption_;
  int foodLacking_;
  int netFoodProduction_;
  int numSolarSatellites_;
  int totalRecFacilityCapacity_;
  int totalForumCapacity_;
  int totalMedCenterCapacity_;
  int totalResidenceCapacity_;
  int numActiveCommandCenters_;
  int numActiveNurseries_;
  int numActiveUniversities_;
  int numActiveObservatories_;
  int numActiveMeteorDefenses_;
  int numActiveTradeCenters_;

  BuildingStats powerPlants_;
  BuildingStats agridomes_;
  BuildingStats commonOreSmelters_;
  BuildingStats commonOreStorage_;
  BuildingStats rareOreStorage_;
  BuildingStats rareOreSmelters_;
  BuildingStats gorfs_;
  BuildingStats commonOreMines_;
  BuildingStats rareOreMines_;
  BuildingStats robotCommandCenters_;
  ibool         rccOperational_;

  PlayerEndInfo endInfo_;
  int           field_1D8[5];
  int           starvationCase_;

  PackedUnitGroup unitGroup_[11];
  uint8           field_4BB;

  int             playerNetID_;
  MachineSettings machineSettings_;
  CommandPacket   commandPacketBuffer_[16];

  int        unitIndex_[16];
  MapObject* pBuildingList_;
  MapObject* pVehicleList_;   ///< @note Also includes mining beacons, magma vents, fumaroles, and wreckage
  MapObject* pEntityList_;
};
static_assert(sizeof(PlayerImpl) == 3108, "Incorrect PlayerImpl size.");

} // Tethys
