
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Location.h"
#include "Tethys/API/ScGroup.h"
#include "Tethys/Game/GameStartInfo.h"
#include "Tethys/Game/Research.h"
#include "Tethys/Game/GameImpl.h"
#include "Tethys/Game/PlayerImpl.h"

namespace Tethys::TethysAPI {

/// Exported interface for accessing player data (wraps PlayerImpl).
/// @note Some of this class's functions return values that are cached.  You may need to call ResetChecks() first.
class _Player : public OP2Class<_Player> {
public:
  _Player(int playerNum) : id_(playerNum), check_() { }

  bool IsValid()  const { return (id_ >= 0) && (id_ < MaxPlayers); }
  operator bool() const { return IsValid(); }

  bool IsEden()     const { return IsValid() &&  GetImpl()->isEden_;           }  ///< Player is Eden?
  bool IsPlymouth() const { return IsValid() && (GetImpl()->isEden_ == false); }  ///< Player is Plymouth?
  void GoEden()           {    if (IsValid())  { GetImpl()->isEden_ = true;  } }  ///< Set player's faction to Eden.
  void GoPlymouth()       {    if (IsValid())  { GetImpl()->isEden_ = false; } }  ///< Set player's faction to Plymouth.

  bool IsHuman() const { return IsValid() &&  GetImpl()->isHuman_;           }  ///< Player is human?
  bool IsAI()    const { return IsValid() && (GetImpl()->isHuman_ == false); }  ///< Player is AI?
  void GoHuman()       {    if (IsValid())  { GetImpl()->GoHuman(); }        }  ///< Set player to human.
  void GoAI()          {    if (IsValid())  { GetImpl()->GoAI();    }        }  ///< Set player to AI.

  ///@{ Gets the player's resource/difficulty level.
  ResourceLevel   GetResourceLevel() const { return IsValid() ? GetImpl()->resourceLevel_  : ResourceLevel::Count;   }
  DifficultyLevel GetDifficulty()    const { return IsValid() ? GetImpl()->difficulty_     : DifficultyLevel::Count; }
  ///@}

  ///@{ Gets or sets the player's name.
  const char* GetPlayerName() const            { return IsValid() ? GetImpl()->GetPlayerName() : nullptr; }
  void        SetPlayerName(const char* pName) { if (IsValid()) { GetImpl()->SetPlayerName(pName);      } }
  ///@}

  ///@{ Gets or sets the player's color.
  PlayerColor GetColor() const            { return IsValid() ? GetImpl()->colorType_ : PlayerColor::Count; }
  void        SetColor(PlayerColor color) { if (IsValid()) { GetImpl()->colorType_ = color;              } }
  ///@}

  ///@{ Gets or sets the player's population or resources.
  int  GetPopulation()            const { return GetKids() + GetWorkers() + GetScientists();               }
  int  GetKids()                  const { return IsValid() ?  GetImpl()->numKids_       : 0;               }
  int  GetWorkers()               const { return IsValid() ?  GetImpl()->numWorkers_    : 0;               }
  int  GetScientists()            const { return IsValid() ?  GetImpl()->numScientists_ : 0;               }
  int  GetCommonOre()             const { return IsValid() ?  GetImpl()->commonOre_     : 0;               }
  int  GetRareOre()               const { return IsValid() ?  GetImpl()->rareOre_       : 0;               }
  int  GetFoodStored()            const { return IsValid() ?  GetImpl()->foodStored_    : 0;               }
  void SetKids(int       numKids)       {    if (IsValid()) { GetImpl()->numKids_       = numKids;       } }
  void SetWorkers(int    numWorkers)    {    if (IsValid()) { GetImpl()->numWorkers_    = numWorkers;    } }
  void SetScientists(int numScientists) {    if (IsValid()) { GetImpl()->numScientists_ = numScientists; } }
  void SetCommonOre(int  newCommonOre)  {    if (IsValid()) { GetImpl()->commonOre_     = newCommonOre;  } }
  void SetRareOre(int    newRareOre)    {    if (IsValid()) { GetImpl()->rareOre_       = newRareOre;    } }
  void SetFoodStored(int newFoodStored) {    if (IsValid()) { GetImpl()->foodStored_    = newFoodStored; } }
  ///@}

  /// Gets the player's net food production trend.
  FoodStatus  GetFoodSupply()  const { return IsValid() ? GetImpl()->moraleState_.foodSupply  : FoodStatus::Count;  }

  /// Gets the player's morale category.
  MoraleLevel GetMoraleLevel() const { return IsValid() ? GetImpl()->moraleState_.moraleLevel : MoraleLevel::Count; }

  /// Gets the amount of a satelllite type the player has in orbit.
  int GetSatelliteCount(MapID objectType) const { return IsValid() ? GetImpl()->GetSatelliteCount(objectType) : 0; }
  /// Steals an RLV from the source Player, provided they have one.
  void CaptureRLV(int srcPlayerNum) { return Thunk<0x477130, &$::CaptureRLV>(srcPlayerNum); }
  /// Sets the player's number of Solar Satellites in orbit.
  void SetSolarSat(int numSolarSatellites)
    { if (IsValid()) { GetImpl()->satellites_.numSolarSatellites = numSolarSatellites; } }

  ibool HasTechnology(int        techID) const { return Thunk<0x477570, &$::HasTechnology>(techID);            }
  /// Gives the tech with the given tech ID, and all free subsequent techs.
  void  MarkResearchComplete(int techID)       { return Thunk<0x477510, &$::MarkResearchComplete>(techID);     }
  /// Gives all techs with techID <= (techLevel * 1000), and all free subsequent techs.
  void  SetTechLevel(int techLevel)            { Research::GetInstance()->SetTechLevel(id_, techLevel * 1000); }

  ///@{ Gets or sets player's ally status.
  bool IsAlliedTo(int       playerNum) const { return IsValid() && (GetImpl()->GetAlliedTo()[playerNum]);   }
  bool IsAlliedBy(int       playerNum) const { return IsValid() && (GetImpl()->alliedBy_[playerNum]);       }
  bool IsAlly(int           playerNum) const { return IsValid() &&  GetImpl()->IsAlly(playerNum);           }
  void AllyWith(int         playerNum)       { return Thunk<0x4774C0, &$::AllyWith>(playerNum);             }
  void MutuallyAllyWith(int playerNum)       { AllyWith(playerNum);  GetInstance(playerNum)->AllyWith(id_); }
  ///@}

  ///@{ Sets the view for this Player (does nothing if player is not the local player).
  void CenterViewOn(Location location) const { return Thunk<0x477490, void(Location)>(location); }
  void CenterViewOn(Unit     unit)     const { CenterViewOn(unit.GetLocation());                 }
  ///@}

  /// Returns (strength / 8), where strength is the sum of all units owned by the player in the given map rectangle
  /// 
  /// @note  Unit Strengths are as follows:
  ///  Spider/Scorpion : 4
  ///  Lynx            : Laser/Microwave: 5 Other: 6 ThorsHammer: 7
  ///  Panther         : Laser/Microwave: 7 Other: 8 ThorsHammer: 9
  ///  Tiger           : Laser/Microwave: 8 Other: 9 ThorsHammer: 10
  ///  Guard Post      : Laser/Microwave: 4 Other: 5 ThorsHammer: 7
  ///  Other           : 0  [Including Units in a Garage]
  int GetPlayerStrength(const MapRect& area) const { return Thunk<0x477CD0, &$::GetPlayerStrength>(area); }

  // Returns (strength / 8), where strength is the sum of all units owned by the player.
  // @see Comments for @ref GetPlayerStrength() for notes about strength calculation.
  int GetTotalPlayerStrength() { return Thunk<0x477DA0, &$::GetTotalPlayerStrength>(); }

  /// Checks for (CommonOreMine, or (hasVehicle(mapRoboMiner, mapAny), or
  /// canBuildVehicle(true))) + (hasVehicle(mapCargoTruck, mapAny),    or
  /// canBuildVehicle(true)) + (CommonOreSmelter, or canBuildBuilding)
  ibool CanAccumulateOre()        { return Thunk<0x477910, &$::CanAccumulateOre>();     }
  /// Checks for (RareOreMine, or (hasVehicle(mapRoboMiner, mapAny), or
  /// canBuildVehicle(true))) + (hasVehicle(mapCargoTruck, mapAny),  or
  /// canBuildVehicle(true)) + (RareOreSmelter, or canBuildBuilding)
  ibool CanAccumulateRareOre()    { return Thunk<0x477A80, &$::CanAccumulateRareOre>(); }
  /// Checks for Spaceport, or hasVehicle(mapConvec, mapSpaceport), or canBuildBuilding
  ibool CanBuildSpace()           { return Thunk<0x477890, &$::CanBuildSpace>();        }
  /// Checks for StructureFactory + (Convec, or (VehicleFactory, or (redundant)
  /// hasVehicle(mapConvec, mapVehicleFactory))), or hasVehicle(mapConvec, mapStructureFactory)
  ibool CanBuildBuilding()        { return Thunk<0x477700, &$::CanBuildBuilding>();     }
  /// Checks for VehicleFactory, or hasVehicle(mapConvec, mapVehicleFactory)
  ibool CanBuildVehicle()         { return Thunk<0x4777F0, ibool(ibool)>(0);            }
  /// Checks for <Tech.labType>Lab, or hasVehicle(mapConvec, map<Tech.labType>Lab), or canBuildBuilding
  ibool CanDoResearch(int techID) { return Thunk<0x477BF0, &$::CanDoResearch>(techID);  }
  /// [cargoOrWeaponType: -1 = mapAny]  Checks for free units, or units in Garages
  ibool HasVehicle(MapID vehicleType, MapID cargoOrWeaponType = MapID::Any)
    { return Thunk<0x4775E0, &$::HasVehicle>(vehicleType, cargoOrWeaponType); }
  /// Returns (numActiveCommandCenters > 0)
  ibool HasActiveCommand()        { return Thunk<0x477E10, &$::HasActiveCommand>();     }
  /// Reset cached check values (clears checkValue array to -1)
  void  ResetChecks()             { return Thunk<0x4776E0, &$::ResetChecks>();          }

  /// Gets the default ScGroup units will be added to (for AI players).
  ScGroup GetDefaultGroup() const { return Thunk<0x4775C0, &$::GetDefaultGroup>(); }
  /// Changes the default ScGroup units will be added to (for AI players).
  void SetDefaultGroup(const ScGroup& newDefaultGroup) { return Thunk<0x4775A0, &$::SetDefaultGroup>(newDefaultGroup); }

  ///@{ Player-owned Unit lists by type, sorted by newest to oldest.
  ///   Call unit.GetPlayerNext() to traverse the list while unit.IsValid().
  Unit GetBuildings() const { return Unit(IsValid() ? GetImpl()->GetBuildings() : nullptr); }
  Unit GetVehicles()  const { return Unit(IsValid() ? GetImpl()->GetVehicles()  : nullptr); }
  Unit GetBeacons()   const { return Unit(IsValid() ? GetImpl()->GetBeacons()   : nullptr); }
  Unit GetEntities()  const { return Unit(IsValid() ? GetImpl()->GetEntities()  : nullptr); }
  ///@}

  /// Kills the specified number of player's colonists.
  void Starve(int numToStarve, bool skipMoraleUpdate)
    { if (IsValid()) { GetImpl()->Starve(numToStarve, skipMoraleUpdate); } }

  /// Self destruct without quitting the game.
  void DoSelfDestruct()     { if (IsValid()) { GetImpl()->DoSelfDestruct();     } }
  /// Transfer to allies without quitting the game.
  void DoTransferToAllies() { if (IsValid()) { GetImpl()->DoTransferToAllies(); } }

  /// Immediately process the passed in CommandPacket locally.
  /// @note This does not buffer packets for network transport, so the timestamp and net ID need not be filled in.
  bool ProcessCommandPacket(const CommandPacket& packet){ return IsValid() && GetImpl()->ProcessCommandPacket(packet); }

  ///@{ Returns a pointer to the internal Player implementation.
        PlayerImpl* GetImpl()       { return IsValid() ? GameImpl::GetInstance()->GetPlayer(id_) : nullptr; }
  const PlayerImpl* GetImpl() const { return IsValid() ? GameImpl::GetInstance()->GetPlayer(id_) : nullptr; }
  ///@}

  /// Gets the global _Player array instance.
  static auto& GetInstance() { return OP2Mem<0x56C280, _Player(&)[MaxPlayers]>(); }

  /// Gets the global _Player instance for the given player ID.
  static _Player* GetInstance(int playerNum)
    { return ((playerNum >= 0) && (playerNum < MaxPlayers)) ? &GetInstance()[playerNum] : nullptr; }

public:
  int id_;

  struct {
    uint8 canBuildBuilding_;
    uint8 canBuildVehicle_;
    uint8 canBuildSpace_;
    uint8 canAccumulateOre_;
    uint8 canAccumulateRareOre_;
    uint8 canResearchBasic_;
    uint8 canResearchStandard_;
    uint8 canResearchAdvanced_;
  } check_;
};

inline auto& Player = _Player::GetInstance();

// ** TODO These don't really belong here, but it helps break the cyclic dependency between Player and ScGroup
inline MiningGroup   FASTCALL CreateMiningGroup(_Player     p) { return OP2Thunk<0x47A0D0, &CreateMiningGroup>(p);   }
inline BuildingGroup FASTCALL CreateBuildingGroup(_Player   p) { return OP2Thunk<0x47A2B0, &CreateBuildingGroup>(p); }
inline FightGroup    FASTCALL CreateFightGroup(_Player      p) { return OP2Thunk<0x47A0A0, &CreateFightGroup>(p);    }
inline Pinwheel      FASTCALL CreatePinwheel(const _Player& p) { return OP2Thunk<0x47A880, &CreatePinwheel>(p);      }

} // Tethys::TethysAPI
