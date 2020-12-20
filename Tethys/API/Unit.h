
#pragma once

#include "Tethys/Common/Util.h"
#include "Tethys/Common/Memory.h"
#include "Tethys/Game/CommandPacket.h"
#include "Tethys/Game/MapObject.h"
#include "Tethys/Game/Research.h"
#include "Tethys/Game/GameImpl.h"
#include "Tethys/Game/MapImpl.h"
#include "Tethys/API/Location.h"

namespace Tethys::API {

/// Enum specifying unit type classifications.  Used by AI and UnitBlock-related interfaces.
enum class UnitClassification : int {
  Attack           = 0x00,  ///< Lynx, Panther, Tiger, Scorpion (excludes ESG, EMP, Stickyfoam)
  ESG              = 0x01,  ///< Lynx, Panther, Tiger
  EMP              = 0x02,  ///< Lynx, Panther, Tiger
  Stickyfoam       = 0x03,  ///< Lynx, Panther, Tiger
  Spider           = 0x04,
  ConVec           = 0x05,
  RepairVehicle    = 0x06,
  CargoTruck       = 0x07,
  Earthworker      = 0x08,
  Colony           = 0x09,  ///< RoboSurveyor, RoboMiner, GeoCon, Scout, RoboDozer, EvacuationTransport
                            ///  (clsVehicle, not specified elsewhere)
  VehicleFactory   = 0x0A,
  ArachnidFactory  = 0x0B,
  StructureFactory = 0x0C,
  OreMine          = 0x0D,  ///< CommonOreMine, RareOreMine
  GuardPost        = 0x0E,
  Building         = 0x0F,  ///< Includes beacons, disasters, Tube, any non-vehicle;  excludes ArachnidFactory?
                            ///  (More like non-vehicle, and non-other specified class)
  NotSet           = 0x10,
  All              = 0x11,  ///< All vehicles and buildings
};


/// Exported interface wrapping a reference to a MapObject instance.
class Unit : public OP2Class<Unit> {
public:
  Unit() : id_(0) { }
  explicit Unit(int unitID) : id_(unitID) { }
  Unit& operator=(const Unit& unit) = default;

  bool operator==(const Unit& unit) const { return id_ == unit.id_; }

  ///@{ Allow conversion from MapObject pointers/references.
  explicit Unit(MapObject* pMapObject) : id_((pMapObject != nullptr) ? pMapObject->index_         : 0) { }
  explicit Unit(AnyMapObj* pMapObject) : id_((pMapObject != nullptr) ? pMapObject->object_.index_ : 0) { }
  explicit Unit(MapObject&  mapObject) : id_(mapObject.index_) { }
  ///@}

  int  GetID() const        { return id_;      }  ///< Get internal unit ID of this Unit wrapper instance.
  void SetID(int newUnitID) { id_ = newUnitID; }  ///< Change referenced internal unit of this Unit wrapper instance.

  bool IsValid()  const { return (id_ > 0); }  ///< Returns true if this Unit instance is valid (not necessarily live!)
  operator bool() const { return IsValid(); }

  ///@{ Get the underlying MapObject that this Unit is a proxy for.
  template <typename T = MapObject>  T* GetMapObject() { return IsValid() ? T::GetInstance(size_t(id_)) : nullptr; }
  template <typename T = MapObject>
  const T* GetMapObject() const { return IsValid() ? T::GetInstance(size_t(id_)) : nullptr; }
  ///@}

  ///@{ Get the internal MapObjectType that this unit is managed by.  (Returns MaxObjectType if !IsValid())
        MapEntityType* GetMapObjectType()       { return MapEntityType::GetInstance(GetType()); }
  const MapEntityType* GetMapObjectType() const { return MapEntityType::GetInstance(GetType()); }
  ///@}

  int GetOwner()       const { return IsValid() ? GetMapObject()->ownerNum_            : -1; }
  int GetCreator()     const { return IsValid() ? GetMapObject()->creatorNum_          : -1; }
  int GetInstanceNum() const { return IsValid() ? GetMapObject()->unitTypeInstanceNum_ :  0; }

  MapID GetType()     const { return IsValid() ? GetMapObject()->GetTypeID() : MapID::None; }
  bool  IsLive()      const { return IsValid()    && GetMapObject()->IsLive();              }
  bool  IsBuilding()  const { return IsLive()     && HasFlag(MoFlagBuilding);               }
  bool  IsFactory()   const { return IsBuilding() && HasFlag(MoFlagBldFactory);             }
  bool  IsVehicle()   const { return IsLive()     && HasFlag(MoFlagVehicle);                }
  bool  IsOffensive() const { return IsLive()     && HasFlag(MoFlagOffensive);              }
  bool  IsEntity()    const { return HasFlag(MoFlagEntity | MoFlagEntChild);                }

  Location GetLocation() const { return IsValid() ? GetMapObject()->GetTile() : Location(); }
  MapRect  GetRect(bool includeBorder = false) const {
    return IsBuilding() ? GetMapObject<Building>()->GetTileRect(includeBorder) : MapRect(GetLocation(), GetLocation());
  }
  POINT    GetPixel() const { auto*const p = GetMapObject(); return p ? POINT{p->pixelX_, p->pixelY_} : POINT{-1, -1}; }

  int  GetMaxHitpoints() const { return IsLive() ? GetPlayerUnitStats().hp : 0; }
  int  GetDamage()       const { return IsLive() ? GetMapObject()->damage_ : 0; }
  void AddDamage(int damage)   { SetDamage(GetDamage() + damage);               }
  void SetDamage(int damage)
    { if (IsLive() && ((GetMapObject()->damage_ = damage) >= GetMaxHitpoints())) { DoDeath(); } }

  bool IsHostile(Unit   what) const {
    return IsLive() && what.IsLive() && (GameImpl::GetInstance()->player_[GetOwner()].alliedBy_[what.GetOwner()] == 0);
  }
  bool IsHostileTo(Unit what) const {
    return IsLive() && what.IsLive() && GameImpl::GetInstance()->player_[GetOwner()].GetHostileTo()[what.GetOwner()];
  }
  void SetAutoTargeted(bool on) { SetFlag(MoFlagOppFiredUpon, on); }  ///< Set if unit can be auto-targeted by enemies.

  void DoSimpleCommand(CommandType commandType);
  void DoPoof()         { DoSimpleCommand(CommandType::Poof);           }  ///< Immediately delete unit w/o animation
  void DoDeath()        { if (IsValid()) { GetMapObject()->DoDeath(); } }  ///< Order unit to die
  void DoSelfDestruct() { DoSimpleCommand(CommandType::SelfDestruct);   }  ///< Order unit to self-destruct
  void DoStop()         { DoSimpleCommand(CommandType::Stop);           }  ///< Order unit to stop its current command
  void DoTransfer(int toPlayerNum);                                        ///< Transfer unit to another player

	CommandType GetCommand()     const { return CommandType(IsValid() ? GetMapObject()->command_ : 0);        }
  ActionType  GetAction()      const { return IsValid() ?  GetMapObject()->action_      : ActionType::Done; }
  int         GetActionTimer() const { return IsValid() ?  GetMapObject()->actionTimer_ : 0;                }
  bool        IsBusy()         const { return IsValid() && GetMapObject()->isBusy_;                         }
  
  int  GetEMPTimer() const
    { return IsBuilding() ? GetMapObject<Building>()->timerEMP_ : IsVehicle() ? GetMapObject<Vehicle>()->timerEMP_ :0; }
  bool IsEMPed()     const    { return HasFlag(MoFlagEMPed);                          }
  void SetEMPed(int duration) { if (IsLive()) { GetMapObject()->SetEMPed(duration); } }

  int  GetESGTimer() const { return IsVehicle() ? GetMapObject<Vehicle>()->timerESG_ : 0; }
  int  IsESGed()     const { return HasFlag(MoFlagESGed); }
  void SetESGed(int duration)
    { if (IsVehicle()) { GetMapObject<Vehicle>()->timerESG_ = duration; }  SetFlag(MoFlagESGed, true); }

  /// Global helper function to get the UnitClassification for the given unit type and cargo type combination.
  static UnitClassification FASTCALL GetClassificationFor(MapID unitType, MapID cargoWeaponType)
    { return OP2Thunk<0x49D270, &$::GetClassificationFor>(unitType, cargoWeaponType); }

  /// Gets the UnitClassification for this unit's type.
  UnitClassification GetClassification() const { return GetClassificationFor(GetType(), GetCargo()); }

  void ClearSpecialTarget() { SetFlag(MoFlagSpecialTarget, false); }

  ///@{ [Wreckage]
  bool IsDiscovered() const
    { return IsValid() && GetMapObject<MapObj::Wreckage>()->IsDiscovered(GameImpl::GetInstance()->localPlayer_); }
  void SetDiscovered(int playerNum = AllPlayers) {
    if (IsValid()) {
      GetMapObject<MapObj::Wreckage>()->playerDiscoveredMask_ |= (playerNum == AllPlayers) ? ~0 : (1u << playerNum);
    }
  }
  ///@}

  /// [Mining Beacon]
  void SetSurveyed(int playerNum = AllPlayers) {
    if (IsValid()) {
      GetMapObject<MapObj::MiningBeacon>()->playerSurveyedMask_ |= (playerNum == AllPlayers) ? ~0 : (1u << playerNum);
    }
  }

  void SetAnimation(int animIdx, int delay, int startDelay, bool isSpecialAnim, bool skipDoDeath)
    { if (IsValid()) { GetMapObject()->SetAnimation(animIdx, delay, startDelay, isSpecialAnim, skipDoDeath); } }

        PerPlayerUnitStats& GetPlayerUnitStats()       { return GetMapObjectType()->playerStats_[GetCreator()]; }
  const PerPlayerUnitStats& GetPlayerUnitStats() const { return GetMapObjectType()->playerStats_[GetCreator()]; }
        GlobalUnitStats&    GetGlobalUnitStats()       { return GetMapObjectType()->stats_;                     }
  const GlobalUnitStats&    GetGlobalUnitStats() const { return GetMapObjectType()->stats_;                     }

  ///@{ Get/set internal @ref MapObjectFlags.
  bool HasFlag(uint32  flag)  const   { return IsValid() && TethysUtil::BitFlagTest(GetMapObject()->flags_,  flag);  }
  bool HasFlags(uint32 flags) const   { return IsValid() && TethysUtil::BitFlagsTest(GetMapObject()->flags_, flags); }
  void SetFlag(uint32  flag, bool on) { if (IsValid()) { TethysUtil::SetBitFlag(GetMapObject()->flags_, flag, on); } }
  ///@}

  /// Gets the next Unit on the map (sorted ascending by pixel Y).  Can be iterated while IsValid().
  Unit GetNext() { auto*const p = GetMapObject();  return Unit((p && (p->pNext_ != p)) ? p->pNext_ : nullptr); }
  /// Gets the next Unit in the owner player's building/vehicle/beacon/entity list (sorted by newest to oldest).
  /// Can be iterated while IsValid().
  Unit GetPlayerNext()
    { auto*const p = GetMapObject();  return Unit((p && (p->pPlayerNext_ != p)) ? p->pPlayerNext_ : nullptr); }

  // ---------------------------------------- Specific to weapons/combat units -----------------------------------------

  MapID GetWeapon() const       { return IsLive() ? MapID(GetMapObject()->weapon_) : MapID::None; }
  void  SetWeapon(MapID weapon) {    if (IsLive())      { GetMapObject()->weapon_ = weapon; }     }

  void DoAttack(Unit what) { if (what.IsLive()) { DoAttack({ what.id_, -1 }); } } ///< Orders unit to attack target unit
  void DoAttack(Location where);                                                  ///< Orders unit to attack the ground
  void DoGuard(Unit what);                                                        ///< [TankVehicle]
  void DoStandGround(Location where);                                             ///< [TankVehicle]

  bool HasDoubleFireRate() const  { return IsLive() && HasFlag(MoFlagDoubleFireRate);       }
  void SetDoubleFireRate(bool on) {    if (IsLive()) { SetFlag(MoFlagDoubleFireRate, on); } }

  // ---------------------------------------------- Specific to vehicles -----------------------------------------------

  MapID GetCargo()       const { return IsValid()   ? MapID(GetMapObject()->cargo_)                  : MapID::None; }
  MapID GetCargoWeapon() const { return IsVehicle() ? MapID(GetMapObject<Vehicle>()->weaponOfCargo_) : MapID::None; }
  void  SetCargo(MapID cargo, MapID weapon)
    { if (IsVehicle()) { auto* p = GetMapObject<Vehicle>(); p->weapon_ = cargo; p->weaponOfCargo_ = weapon; } }

  ///@{ [Cargo Truck]
  TruckCargo GetTruckCargoType()   { return TruckCargo(IsLive() ? GetMapObject()->truckCargoType_   : 0); }
  int        GetTruckCargoAmount() { return            IsLive() ? GetMapObject()->truckCargoAmount_ : 0;  }
  void SetCargo(TruckCargo cargo, int amount)
    { if (IsVehicle()) { auto*p = GetMapObject(); p->truckCargoType_ = uint16(cargo); p->truckCargoAmount_ = amount; } }
  ///@}

  int  GetStickyfoamTimer() const    { return IsVehicle() ? GetMapObject<Vehicle>()->timerStickyfoam_ : 0; }
  bool IsStickyfoamed()     const    { return IsVehicle() && HasFlag(MoFlagVecStickyfoamed);               }
  void SetStickyfoamed(int duration) { if (IsLive()) { GetMapObject()->SetStickyfoamed(duration); }        }

  bool GetLights() { return IsLive() && HasFlag(MoFlagVecHeadlights); }

  void DoSetLights(bool on);
  void DoMove(Location where) { if (IsLive()) { const auto [x, y] = where.GetPixel(); GetMapObject()->CmdMove(x, y); } }
  void DoDock(Unit at)
    { auto d = at.GetDockLocation();  if (IsLive() && d) { GetMapObject()->CmdDock(d.GetPixelX(), d.GetPixelY()); } }
  void DoDockAtGarage(Unit garage);
  void DoBuild(Location  bottomRight);                                                       ///< [ConVec]
  void DoDeploy(Location center);                                                            ///< [Robo-Miner, GeoCon]
  void DoDismantle(Unit what);                                                               ///< [ConVec]
	void DoRepair(Unit    what) { if (IsLive()) { GetMapObject()->CmdRepair(what.id_); }    }
	void DoReprogram(Unit what) { if (IsLive()) { GetMapObject()->CmdReprogram(what.id_); } }  ///< [Spider]
  void DoBuildWall(MapID tubeWallType, MapRect area);                                        ///< [Earthworker]
  void DoRemoveWall(MapRect area);                                                           ///< [Earthworker]
  void DoDoze(MapRect area);                                                                 ///< [Robo-Dozer]
	void DoSalvage(MapRect area, Unit gorf);                                                   ///< [Cargo Truck]
  void DoDumpCargo()   { if (IsLive()) { GetMapObject()->CmdDumpCargo(); } }
  void DoLoadCargo()   { DoSimpleCommand(CommandType::LoadCargo);          }
  void DoUnloadCargo() { DoSimpleCommand(CommandType::UnloadCargo);        }

  // ---------------------------------------------- Specific to buildings ----------------------------------------------

  ///@{ [Factory]
  void GetFactoryCargo(int bay, MapID* pUnitType, MapID* pCargoOrWeaponType) const {
    auto*const pMo      = (IsFactory() && (bay >= 0) && (bay < 6)) ? GetMapObject<FactoryBuilding>() : nullptr;
    *pUnitType          = (pMo != nullptr) ? MapID(pMo->cargoBayContents_[bay])     : MapID::None;
    *pCargoOrWeaponType = (pMo != nullptr) ? MapID(pMo->cargoBayCargoOrWeapon_[bay]): MapID::None;
  }
  void SetFactoryCargo(int bay, MapID   unitType, MapID   cargoOrWeaponType = MapID::None) {
    if (auto*const pMo = GetMapObject<FactoryBuilding>();  IsFactory() && (bay >= 0) && (bay < 6)) {
      pMo->cargoBayContents_[bay]      = unitType;
      pMo->cargoBayCargoOrWeapon_[bay] = cargoOrWeaponType;
    }
  }

  Location FindFactoryOutputLocation(MapID itemToProduce = MapID::CargoTruck) const
    { Location loc; MapImpl::GetInstance()->FindUnitPlacementLocation(itemToProduce, GetLocation(), &loc); return loc; }
  ///@}

  /// [Garage, StructureFactory, Spaceport]
  bool HasOccupiedBay() const {
    bool result = (GetType() == MapID::Garage) && (GetMapObject<MapObj::Garage>()->GetNumOccupiedBays() != 0);
    if ((result == false) && IsFactory()) {
      auto*const pMo = GetMapObject<FactoryBuilding>();
      for (int i = 0; ((result == false) && (i < 6)); result = (pMo->cargoBayCargoOrWeapon_[i++] != MapID::None));
    }
    return result;
  }

  ///@{ [Garage]
  Unit GetUnitInGarage(int bay) const
    { return Unit((IsLive() && (bay >= 0) && (bay < 6)) ? GetMapObject<MapObj::Garage>()->pUnitInBay_[bay] : nullptr); }
  void PutInGarage(Unit garage, int bay)
    { Location dock = garage.GetDockLocation();  Thunk<0x476160, void(int, int, int)>(bay, dock.x, dock.y); }
  ///@}

  ///@{ [Spaceport]
  MapID GetRocketOnPad() const { return IsBuilding() ? GetMapObject<MapObj::Spaceport>()->objectOnPad_ : MapID::None; }
  MapID GetRocketCargo() const { return IsBuilding() ? GetMapObject<MapObj::Spaceport>()->launchCargo_ : MapID::None; }
  void  SetRocketOnPad(MapID rocket, MapID cargo = MapID::None) {
    if (auto*const pMo = GetMapObject<MapObj::Spaceport>();  pMo != nullptr) {
      pMo->objectOnPad_ = rocket;
      pMo->launchCargo_ = cargo;
    }
  }
  ///@}

  Location GetDockLocation() const
    { Location dock;  if (IsBuilding()) { GetMapObject<Building>()->GetDockLocation(&dock); }  return dock; }
  Unit     GetUnitOnDock()   const {
    const auto dock = GetDockLocation();
    Unit       unit = Unit(dock ? g_mapImpl.Tile(dock).unitIndex : 0);
    return unit.IsVehicle() ? unit : Unit();
  }

  bool IsUnderConstruction() const
    { const auto cmd = GetCommand();  return (cmd == CommandType::Develop) || (cmd == CommandType::UnDevelop); }
  bool IsDisabled() const
    { return IsBuilding() && (IsUnderConstruction() == false) && (GetMapObject<Building>()->IsEnabled() == false); }
  bool IsIdled()    const { return IsBuilding() && (HasFlag(MoFlagBldActive)    == false); }
  bool IsEnabled()  const { return IsBuilding() && ((IsIdled() || IsDisabled()) == false); }
  bool IsInfected() const { return IsBuilding() &&  HasFlag(MoFlagBldInfected);            }

  void DoIdle()   { DoSimpleCommand(CommandType::Idle);   }
  void DoUnidle() { DoSimpleCommand(CommandType::Unidle); }
  void DoInfect() { Thunk<0x476B90, &$::DoInfect>();      }
  void DoProduce(MapID itemType, MapID weaponType = MapID::None, uint16 scGroupIndex = -1)    ///< [Factory]
    { if (IsLive()) { GetMapObject()->CmdProduce(itemType, weaponType, scGroupIndex); } }
  void DoLaunch(Location target = { }, bool forceEnable = false);                             ///< [Spaceport]
  void DoTransferCargo(int bay) { if (IsLive()) { GetMapObject()->CmdTransferCargo(bay); } }  ///< [Factory, Garage]
	void DoResearch(int techID, int numScientists);                                             ///< [Lab]
	void DoTrainScientists(int numToTrain);                                                     ///< [University]

protected:
  bool ProcessCommandPacket(const CommandPacket& packet) const
    { auto*const p = GameImpl::GetInstance()->GetPlayer(GetOwner());  return p && p->ProcessCommandPacket(packet); }

public:
  int id_;
};


// =====================================================================================================================
inline void Unit::DoSimpleCommand(
  CommandType commandPacketType)
{
  if (IsValid()) {
    CommandPacket packet;

    switch (commandPacketType) {
    case CommandType::LoadCargo:  case CommandType::UnloadCargo:  case CommandType::DumpCargo:  case CommandType::Idle:
    case CommandType::Unidle:     case CommandType::Poof:
      packet = { commandPacketType, sizeof(SingleUnitSimpleCommand) };
      packet.data.singleUnitSimple.unitID = id_;
      break;

    default:
      packet = { commandPacketType, sizeof(SimpleCommand) };
      packet.data.simple.numUnits  = 1;
      packet.data.simple.unitID[0] = id_;
      break;
    }

    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoSetLights(
  bool on)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::LightToggle, sizeof(LightToggleCommand) };
    packet.data.lightToggle.numUnits       = 1;
    packet.data.lightToggle.unitID[0]      = id_;
    packet.data.lightToggle.headlightState = on;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoBuild(
  Location bottomRight)
{
  if (IsLive() && (GetCargoWeapon() != MapID::None)) {
    const auto&   s      = MapObjectType::GetInstance(GetCargoWeapon())->stats_.building;
    CommandPacket packet = { CommandType::Build, sizeof(BuildCommand) };
    packet.data.build.numUnits     = 1;
    packet.data.build.unitID[0]    = id_;
    packet.data.build.numWaypoints = 1;
    packet.data.build.waypoint[0]  = bottomRight.AsWaypoint(true, false);
    packet.data.build.rect         = MapRect(bottomRight - Location(s.width + 1, s.height + 1), bottomRight).AsPacked();
    packet.data.build.unknown      = -1;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoDeploy(
  Location center)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::Build, sizeof(BuildCommand) };
    packet.data.build.numUnits     = 1;
    packet.data.build.unitID[0]    = id_;
    packet.data.build.numWaypoints = 1;
    packet.data.build.waypoint[0]  = center.AsWaypoint(false, false);
    packet.data.build.rect         = MapRect(center - Location(1, 0), center).AsPacked();
    packet.data.build.unknown      = -1;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoTransfer(
  int toPlayerNum)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::Transfer, sizeof(TransferCommand) };
    packet.data.transfer.numUnits    = 1;
    packet.data.transfer.unitID[0]   = id_;
    packet.data.transfer.toPlayerNum = toPlayerNum;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoAttack(
  Location where)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::Attack, sizeof(AttackCommand) };
    packet.data.attack.numUnits  = 1;
    packet.data.attack.unitID[0] = id_;
    packet.data.attack.unknown   = 0;
    packet.data.attack.target    = { uint16(where.x), uint16(where.y) };
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoGuard(
  Unit what)
{
  if (IsLive() && what.IsLive()) {
    CommandPacket packet{ CommandType::Guard, sizeof(AttackCommand) };
    packet.data.attack.numUnits  = 1;
    packet.data.attack.unitID[0] = id_;
    packet.data.attack.unknown   = 0;
    packet.data.attack.target    = { uint16(what.id_), UINT16_MAX };
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoDoze(
  MapRect area)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::Doze, sizeof(DozeCommand) };
    packet.data.doze.numUnits  = 1;
    packet.data.doze.unitID[0] = id_;
    packet.data.doze.rect      = area.AsPacked();
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoDockAtGarage(
  Unit garage)
{
  if (IsLive() && garage.IsLive()) {
    CommandPacket packet = { CommandType::DockEG, sizeof(MoveCommand) };
    packet.data.move.numUnits     = 1;
    packet.data.move.unitID[0]    = id_;
    packet.data.move.numWaypoints = 1;
    packet.data.move.waypoint[0]  = garage.GetDockLocation().AsWaypoint(true, true);
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoStandGround(
  Location where)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::StandGround, sizeof(MoveCommand) };
    packet.data.move.numUnits     = 1;
    packet.data.move.unitID[0]    = id_;
    packet.data.move.numWaypoints = 1;
    packet.data.move.waypoint[0]  = where.AsWaypoint();
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoBuildWall(
  MapID   tubeWallType,
  MapRect area)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::BuildWall, sizeof(BuildWallCommand) };
    packet.data.buildWall.numUnits     = 1;
    packet.data.buildWall.unitID[0]    = id_;
    packet.data.buildWall.rect.x1      = area.x1;
    packet.data.buildWall.rect.y1      = area.y1;
    packet.data.buildWall.rect.x2      = area.x2 + 1;
    packet.data.buildWall.rect.y2      = area.y2 + 1;
    packet.data.buildWall.tubeWallType = tubeWallType;
    packet.data.buildWall.unknown      = 0;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoRemoveWall(
  MapRect area)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::RemoveWall, sizeof(RemoveWallCommand) };
    packet.data.removeWall.numUnits     = 1;
    packet.data.removeWall.unitID[0]    = id_;
    packet.data.removeWall.numWaypoints = 0;
    packet.data.removeWall.rect         = area.AsPacked();
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoDismantle(
  Unit what)
{
  if (IsLive() && what.IsLive()) {
    CommandPacket packet = { CommandType::Dismantle, sizeof(RepairCommand) };
    packet.data.repair.numUnits  = 1;
    packet.data.repair.unitID[0] = id_;
    packet.data.repair.unknown1  = 0;
    packet.data.repair.target    = { uint16(what.id_), UINT16_MAX };
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoSalvage(
  MapRect area,
  Unit    gorf)
{
  if (IsLive() && gorf.IsLive()) {
    CommandPacket packet = { CommandType::Salvage, sizeof(SalvageCommand) };
    packet.data.salvage.unitID     = id_;
    packet.data.salvage.rect       = area.AsPacked();
    packet.data.salvage.unitIDGorf = gorf.id_;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoLaunch(
  Location target,
  bool     forceEnable)
{
  if (IsLive()) {
    constexpr uint32 SetMask   = (MoFlagBldActive | MoFlagBldCmdCenterConnected | MoFlagBldEnabledPower |
                                  MoFlagBldEnabledWorkers | MoFlagBldEnabledScientists);
    constexpr uint32 UnsetMask = MoFlagEMPed | MoFlagBldInfected;

    auto*const   pMo       = GetMapObject();
    const uint32 oldFlags  = pMo->flags_ & (SetMask | UnsetMask);
    const int16  oldDamage = pMo->damage_;
    if (forceEnable) {
      pMo->flags_ |=  SetMask;
      pMo->flags_ &= ~UnsetMask;
      pMo->damage_ = 0;
    }

    CommandPacket packet = { CommandType::Launch, sizeof(LaunchCommand) };
    packet.data.launch.unitID       = id_;
    packet.data.launch.targetPixelX = target.GetPixelX();
    packet.data.launch.targetPixelY = target.GetPixelY();
    ProcessCommandPacket(packet);

    if (forceEnable) {
      pMo->flags_ &= ~SetMask;
      pMo->flags_ |=  oldFlags;
      pMo->damage_ =  oldDamage;
    }
  }
}

// =====================================================================================================================
inline void Unit::DoResearch(
  int techID,
  int numScientists)
{
  if (int techNum = Research::GetInstance()->GetTechNum(techID);  IsLive() && (techNum >= 0)) {
    CommandPacket packet = { CommandType::Research, sizeof(ResearchCommand) };
    packet.data.research.unitID        = id_;
    packet.data.research.techNum       = techNum;
    packet.data.research.numScientists = numScientists;
    ProcessCommandPacket(packet);
  }
}

// =====================================================================================================================
inline void Unit::DoTrainScientists(
  int numToTrain)
{
  if (IsLive()) {
    CommandPacket packet = { CommandType::TrainScientists, sizeof(TrainScientistsCommand) };
    packet.data.trainScientists.unitID        = id_;
    packet.data.trainScientists.numScientists = numToTrain;
    ProcessCommandPacket(packet);
  }
}

} // Tethys::API
