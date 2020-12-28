
#pragma once

#include "Tethys/Game/MapObjectType.h"
#include "Tethys/Game/MapImpl.h"
#include "Tethys/Game/MineManager.h"
#include "Tethys/API/Location.h"
#include <cassert>
#include <utility>

namespace Tethys {

class  MapObject;
union  AnyMapObj;
struct PathContext;
struct Waypoint;
class  Viewport;
class  TubeConnection;

/// Flags that apply to MapObject instances.
/// Some bits have different meanings based on unit type, specified by MoFlagVehicle, MoFlagBuilding, or MoFlagEntity.
enum MapObjectFlags : uint32 {
  // Flags specifying what type of MapObject this is, which determines context for other flags.
  MoFlagVehicle  = (1u << 1),  ///< MapObject is a Vehicle.
  MoFlagBuilding = (1u << 2),  ///< MapObject is a Building.
  MoFlagEntity   = (1u << 6),  ///< MapObject is a MapEntity.

  // Flags that apply to any type.
  MoFlagOffensive         = (1u <<  4),  ///< Unit is a combat unit type.
  MoFlagDoubleFireRate    = (1u <<  5),  ///< Combat unit has 2x weapon fire rate (Tigers).
  MoFlagCanBeDamaged      = (1u << 10),  ///< Unit can be damaged.
  MoFlagEden              = (1u << 12),  ///< Unit was created by an Eden player.
  MoFlagDead              = (1u << 17),  ///< MapObject is deallocated.
  MoFlagEMPed             = (1u << 19),  ///< Unit is EMPed.
  MoFlagESGed             = (1u << 20),  ///< Unit is ESGed.
  MoFlagOppFiredUpon      = (1u << 23),  ///< Unit may be auto-targeted by hostile combat units.  ** TODO Building only?
  MoFlagUndocking         = (1u << 24),  ///< Unit is undocking.  ** TODO What does this apply to, truck and/or factory?
  MoFlagSpecialTarget     = (1u << 25),  ///< Unit has an attached SpecialTarget trigger.
  MoFlagForceFullLighting = (1u << 27),  ///< Always draw MapObject at full light level.
  MoFlagSpecialDraw       = (1u << 28),  ///< Use special drawing method for this MapObject.
  MoFlagMarkedForRedraw   = (1u << 29),  ///< MapObject is marked for redraw.

  // Flags that apply to vehicles.
  MoFlagVecHeadlights   = (1u <<  0),  ///< Vehicle headlights are turned on.
  MoFlagVecArachnid     = (1u << 14),  ///< Vehicle is an arachnid type (cannot be repaired).
  MoFlagVecStickyfoamed = (1u << 18),  ///< Vehicle is stickyfoamed.

  // Flags that apply to buildings.
  MoFlagBldActive             = (1u <<  0),  ///< Building is unidled.
  MoFlagBldFactory            = (1u <<  3),  ///< Building is a FactoryBuilding.
  MoFlagBldCmdCenterConnected = (1u <<  7),  ///< Building has an active Command Center connection.
  MoFlagBldEnabledPower       = (1u << 13),  ///< Building has sufficient power.
  MoFlagBldEnabledWorkers     = (1u << 14),  ///< Building has sufficient workers.
  MoFlagBldEnabledScientists  = (1u << 15),  ///< Building has sufficient scientists.
  MoFlagBldInfected           = (1u << 18),  ///< Building is Blight-infected.
  MoFlagBldNoExplosionOnDeath = (1u << 22),  ///< Skip spawning an explosion on building death.

  // Flags that apply to entities.
  MoFlagEntChild                 = (1u <<  8),  ///< Entity is a MapChildEntity.
  MoFlagEntDisasterStarted       = (1u << 13),  ///< Disaster has started.
  MoFlagEntDisasterDidFirstWarn  = (1u << 14),  ///< Disaster first warning (10 marks out) completed.
  MoFlagEntDisasterDidSecondWarn = (1u << 15),  ///< Disaster second warning (5 marks out) completed.
  MoFlagEntDisasterDoAftershock  = (1u << 16),  ///< Earthquake should do aftershock.

  // ** TODO Figure out what bits 9, 21, 26, and 31 are used for;  11 and 30 are unused?
};

/// Action types, these indicate what a MapObject is currently doing.
enum class ActionType : uint8 {
  Done = 0,         ///< 'moDone'
  Move,             ///< 'moMove'
  Build,            ///< 'moBuild'
  BuildMine,        ///< 'moBuildMine'
  Repair,           ///< 'moRepair'
  Develop,          ///< 'moDevelop'
  ObjWait,          ///< 'moObjWait'
  OperationalWait,  ///< 'moOperationalWait'
  EMPWait,          ///< 'moEMPWait'
  Landing,          ///< 'moLanding'
  ObjFroze,         ///< 'moObjFroze'
  DelSelf,          ///< 'moDelSelf'
  DoResearch,       ///< 'moDoResearch'
  ObjDocking,       ///< 'moObjDocking'
  Scavenge,         ///< 'moScavenge'
  WeaponMove,       ///< 'weaponMove'
  WeatherMove,      ///< 'weatherMove'
  EMPRecover,       ///< 'empRecover'
  WeaponAimCoarse,  ///< 'weaponAimCoarse'
  WeaponAimFine,    ///< 'weaponAimFine'
  SantaWalking,     ///< 'SantaWalking'
  Invalid,          ///< 'moInvalid'

  Count
};

/// Cargo Truck cargo types.
enum class CargoType : int {
  Any   = -1,
  Empty =  0,
  Food,
  CommonOre,
  RareOre,
  CommonMetal,
  RareMetal,
  CommonRubble,
  RareRubble,
  Spacecraft,    ///< Starship module/satellite;  MapID = truckCargoAmount
  Wreckage,      ///< Wreckage;  Tech ID = 8000 + truckCargoAmount
  GeneBank,

  Count
};


namespace TethysImpl { template <MapID>  struct MapObjForImpl { using Type = MapObject; }; }

/// Template alias to get the MapObject subclass associated with the given MapID.
template <MapID ID>  using MapObjFor = typename TethysImpl::MapObjForImpl<ID>::Type;

#define OP2_MO_GET_INSTANCE_IMPL()  \
  static $* GetInstance(int index) { return static_cast<$*>(MapObject::GetInstance(index)); }
#define OP2_MO_CLASS_MAP_ID(id)  \
  static constexpr MapID ClassMapId = MapID::id;  using Type = MapObjTypeFor<MapID::id>;  OP2_MO_GET_INSTANCE_IMPL();

BEGIN_PACKED

//  ====================================================================================================================
/// Base class for all dynamic map entites, e.g. units, buildings, disasters, beacons, weapons fire.  Wrapped by Unit.
/// This class and its subclasses generally should not be instantiated yourself.  Use MapObjectType::Create() instead.
class MapObject : public OP2Class<MapObject> {
public:
  MapObject()                  { InternalCtor<0x439C10>(); }
  MapObject(InternalCtorChain) {                           }

#if defined(_DEBUG)
  /// Placement new override sanity checking derived object sizes are no larger than MapObjectSize.
  void* operator new(size_t size, void* pMemory) noexcept { assert(size <= MapObjectSize);  return pMemory; }
#endif

  MapObject(const MapObject&)            = delete;
  MapObject& operator=(const MapObject&) = delete;

  operator       AnyMapObj&()       { return       reinterpret_cast<AnyMapObj&>(*this); }
  operator const AnyMapObj&() const { return reinterpret_cast<const AnyMapObj&>(*this); }

  template <MapID ID>        auto& As()       { return       static_cast<MapObjFor<ID>&>(*this); }
  template <MapID ID>  const auto& As() const { return static_cast<const MapObjFor<ID>&>(*this); }

  virtual MapObjectType* GetType() const { return Thunk<0x439A00, &$::GetType>(); }

  virtual void  ProcessForGameCycle()       { return Thunk<0x43ADA0, &$::ProcessForGameCycle>();       }
  virtual int   ProcessTimers()             { return Thunk<0x43DF60, &$::ProcessTimers>();             }
  virtual ibool CheckSpontaneouslyExplode() { return Thunk<0x4017E0, &$::CheckSpontaneouslyExplode>(); }

  virtual int  FireWeapon() { return Thunk<0x4017F0, &$::FireWeapon>(); }
  virtual void SelectTurretGraphic(MapObject* pWeapon, int rotation)
    { return Thunk<0x401800, &$::SelectTurretGraphic>(pWeapon, rotation); }

  virtual void  Draw(Viewport*                pViewport) { return Thunk<0x43A2F0, &$::Draw>(pViewport);                }
  virtual void  LightUpVisibleRange(Viewport* pViewport) { return Thunk<0x43A300, &$::LightUpVisibleRange>(pViewport); }
  virtual void  MarkForRedraw(Viewport*       pViewport) { return Thunk<0x401810, &$::MarkForRedraw>(pViewport);       }
  virtual ibool IsVisible(Viewport*           pViewport) { return Thunk<0x43A430, &$::IsVisible>(pViewport);           }
  virtual ibool IsVisibleY(Viewport*          pViewport) { return Thunk<0x43A3D0, &$::IsVisibleY>(pViewport);          }
  virtual int   GetAnimationIndex(Viewport*   pViewport) { return Thunk<0x401820, &$::GetAnimationIndex>(pViewport);   }

  virtual int   GetSelectionBoxSize(int* pWidth,   int* pHeight)
    { return Thunk<0x401830, &$::GetSelectionBoxSize>(pWidth, pHeight); }
  virtual int   GetSelectionBoxPos(int*  pXOffset, int* pYOffset)
    { return Thunk<0x401840, &$::GetSelectionBoxPos>(pXOffset, pYOffset); }
  virtual int   MouseOver(int xOffset, int yOffset)     { return Thunk<0x401850, &$::MouseOver>(xOffset, yOffset);   }
  virtual int   GetMouseOverStr(char* pDst, int bufLen) { return Thunk<0x43A690, &$::GetMouseOverStr>(pDst, bufLen); }
  virtual int   GetSelectionStr(char* pDst, int bufLen) { return Thunk<0x43A6E0, &$::GetSelectionStr>(pDst, bufLen); }
  virtual ibool IsMouseOver(int pixelX, int pixelY)     { return Thunk<0x43A8E0, &$::IsMouseOver>(pixelX, pixelY);   }

  virtual int Func_18() { return Thunk<0x43A980, &$::Func_18>(); }

  virtual void* Destroy(ibool freeMem = 0) { return Thunk<0x439C20, &$::Destroy>(freeMem); }  // virtual ~MapObject()

  virtual void DoEvent() { return Thunk<0x405E10, &$::DoEvent>(); }

  virtual void OnSave() { return Thunk<0x401860, &$::OnSave>(); }
  virtual void OnLoad() { return Thunk<0x401870, &$::OnLoad>(); }

  virtual void SetEMPed(uint16        duration) { return Thunk<0x401880, &$::SetEMPed>(duration);        }
  virtual void SetStickyfoamed(uint16 duration) { return Thunk<0x401890, &$::SetStickyfoamed>(duration); }

  virtual void Move() { return Thunk<0x4018A0, &$::Move>(); }

  virtual void TransferUnit(int playerNum) { return Thunk<0x4399F0, &$::TransferUnit>(playerNum); }

  virtual void GetLocation(int a, int b, int* pTileX, int* pTileY)
    { return Thunk<0x4018B0, &$::GetLocation>(a, b, pTileX, pTileY); }

  virtual int InitPathContext() { return Thunk<0x4018D0, &$::InitPathContext>(); }

  virtual ibool IsSelectable() { return Thunk<0x4018E0, &$::IsSelectable>(); }

#define OP2_MO_MAPOBJECT_VTBL($)                                                                             \
  $(GetType)  $(ProcessForGameCycle)  $(ProcessTimers)  $(CheckSpontaneouslyExplode)  $(FireWeapon)          \
  $(SelectTurretGraphic)  $(Draw)  $(LightUpVisibleRange)  $(MarkForRedraw) $(IsVisible)  $(IsVisibleY)      \
  $(GetAnimationIndex)  $(GetSelectionBoxSize)  $(GetSelectionBoxPos)  $(MouseOver)  $(GetMouseOverStr)      \
  $(GetSelectionStr)  $(IsMouseOver)  $(Func_18)  $(Destroy)  $(DoEvent)  $(OnSave)  $(OnLoad)  $(SetEMPed)  \
  $(SetStickyfoamed)  $(Move)  $(TransferUnit)  $(GetLocation)  $(InitPathContext)  $(IsSelectable)
  DEFINE_VTBL_TYPE(OP2_MO_MAPOBJECT_VTBL, 0x4D0498);

  void SetAnimation(int animIdx, int delay, int startDelay, ibool isSpecialAnim, ibool skipDoDeath)
    { return Thunk<0x405110, &$::SetAnimation>(animIdx, delay, startDelay, isSpecialAnim, skipDoDeath); }

  void CmdMove(int pixelX, int pixelY)   { return Thunk<0x42A120, &$::CmdMove>(pixelX, pixelY); }
  void CmdAttack(int unitIndex)          { return Thunk<0x42A1E0, &$::CmdAttack>(unitIndex);    }
  void CmdReprogram(int unitIndex)       { return Thunk<0x42A270, &$::CmdReprogram>(unitIndex); }
  void CmdStop()                         { return Thunk<0x42A300, &$::CmdStop>();               }
  void CmdDumpCargo()                    { return Thunk<0x42A370, &$::CmdDumpCargo>();          }
  void CmdPoof()                         { return Thunk<0x42A3E0, &$::CmdPoof>();               }
  void CmdSelfDestruct()                 { return Thunk<0x42A430, &$::CmdSelfDestruct>();       }
  void CmdProduce(MapID itemType, MapID weaponType, uint16 scGroupIndex = -1)
    { return Thunk<0x42A4A0, &$::CmdProduce>(itemType, weaponType, scGroupIndex); }
  void CmdDock(int pixelX, int pixelY)   { return Thunk<0x42A5B0, &$::CmdDock>(pixelX, pixelY); }
  void CmdRecycle(int bay)               { return Thunk<0x42A670, &$::CmdRecycle>(bay);         }
  void CmdTransferCargo(int bay)         { return Thunk<0x42A6D0, &$::CmdTransferCargo>(bay);   }
  void CmdPatrol(int a, int b, int c)    { return Thunk<0x42A740, &$::CmdPatrol>(a, b, c);      }
  void CmdBuild(int a, int b)            { return Thunk<0x42A810, &$::CmdBuild>(a, b);          }
  void CmdBuildWall(int a, int b, int c) { return Thunk<0x42A970, &$::CmdBuildWall>(a, b, c);   }
  void CmdRepair(int unitIndex)          { return Thunk<0x42AAF0, &$::CmdRepair>(unitIndex);    }
  void CmdGuard(int a, int b)            { return Thunk<0x42AD60, &$::CmdGuard>(a, b);          }
  // ** TODO Cmd??? 0x42AB80
  // ** TODO Cmd??? 0x42AF50

  void DoDeath(int timer = 1) { return Thunk<0x43A990, &$::DoDeath>(timer); }

  void SetCreator(uint8 creatorID) { return Thunk<0x43A090, &$::SetCreator>(creatorID); }

  void AddToOwnerList()      { return Thunk<0x43A0E0, &$::AddToOwnerList>();      }
  void RemoveFromOwnerList() { return Thunk<0x439D00, &$::RemoveFromOwnerList>(); }
  void RemoveFromUnitList()  { return Thunk<0x439DB0, &$::RemoveFromUnitList>();  }
  void RemoveFromMap()       { return Thunk<0x439DE0, &$::RemoveFromMap>();       }

  // ** TODO 0x423D90
  // ** TODO 0x44C520
  // ** TODO 0x408DA0
  // ** TODO 0x439FB0
  // ** TODO 0x43E990
  // ** TODO 0x44D000
  // ** TODO 0x483130

  /// Returns the MapID of this map object's type.
  MapID GetTypeID() const { return GetType()->type_; }

  /// Returns the TruckCargo of this map object's truck cargo type.
  CargoType GetCargoType() const { return CargoType(truckCargoType_); }

  ///@{ Helper functions to get the position of this map object.
  int      GetTileX() const { return pixelX_ / 32;               }
  int      GetTileY() const { return pixelY_ / 32;               }
  Location GetTile()  const { return { GetTileX(), GetTileY() }; }
  POINT    GetPixel() const { return { pixelX_, pixelY_ };       }
  MapRect  GetTileRect(bool includeBorder = false) const {
    return (flags_ & MoFlagBuilding) ? GetType()->GetTileRect(GetTile(), includeBorder) : MapRect(GetTile(), GetTile());
  }
  ///@}

  /// Returns whether this map object is live.
  bool IsLive() const { return (reinterpret_cast<uintptr>(pNext_) != ~0u) && ((flags_ & MoFlagDead) == 0); }

  /// Helper function to access an element of the map object array by index.
  static MapObject* GetInstance(int index);
  
public:
  MapObject* pNext_;        ///< Next MapObject in list (sorted descending by pixelY_), or -1 if this MapObject is dead.
  MapObject* pPrev_;        ///< Previous MapObject in list.
  MapObject* pPlayerNext_;  ///< Next MapObject in player building/vehicle/entity list (sorted by newest to oldest).

  int index_;  ///< Index of this MapObject into @ref g_pMapObjArray.

  int pixelX_;  ///< Pixel X coordinate on the map.
  int pixelY_;  ///< Pixel Y coordinate on the map.

  uint8 rotation_;  ///< [Vehicle] Rotation = 32 * UnitDirection value.  (Sprite indices are in units of 22.5 degrees.)

  union {
    struct {
      uint8 ownerNum_   : 4;      ///< Owner player index (controlling player).
      uint8 creatorNum_ : 4;      ///< Creator player index (source of unit stats).
    };
    uint8   creatorAndOwnerNum_;  ///< Owner and creator player index packed as a single uint8.
  };

  int16 damage_;  ///< Current damage; hitpoints is (max HP - damage).  @note This is signed, can potentially overflow.

  uint8      isBusy_;           ///< Boolean indicating whether this unit is busy.
  uint8      command_;          ///< @see CommandType.
  ActionType action_;           ///< @see ActionType.
  ActionType executingAction_;  ///< @see ActionType.

  union {
    uint16 cargo_;                    ///< [ConVec] MapID structure kit cargo.
    uint16 weapon_;                   ///< [TankVehicle, GuardPost] MapID weapon type.

    struct {
      uint16 truckCargoType_   :  4;  ///< [CargoTruck] Cargo type carried.  @see CargoType.
      uint16 truckCargoAmount_ : 12;  ///< [CargoTruck] Amount of truck cargo.  For wreckage, this is (techID - 8000).
                                      ///  For starship modules, this is its MapID.
    };

    uint16 tubeOrWallType_;           ///< [Earthworker] MapID tube/wall type currently being built.

    uint16 researchTimer_;            ///< [LabBuilding] Research progress.

    uint16 disasterDuration_;         ///< [Vortex, Lightning] Disaster duration in ticks.
    uint16 disasterMagnitude_;        ///< [Meteor, Earthquake] Disaster magnitude.
                                      ///  Meteor sizes are 0 = small, 1 = medium, 2 = large, -1 = random.
    uint16 lavaSpeed_;                ///< [Eruption] Lava spread speed set when the disaster is triggered.
  };

  uint16 attackingUnitIndex_;   ///< [TankVehicle, GuardPost] Index of MapUnit to attack.
  int    field_28;
  uint8  unitTypeInstanceNum_;  ///< Unit type instance number, e.g. Tokamak #5-02's instance number is 2.
  uint8  field_2D;
  uint16 reloadTimer_;          ///< [TankVehicle, GuardPost] Timer before weapon can fire again.
  uint8  scGroupIndex_;         ///< ScGroup this unit is assigned to, or -1 for none.
  uint8  field_31;
  uint8  field_32;
  uint8  field_33;

  union {
    PathContext*    pPathContext_;     ///< [Vehicle, AirUnit] Data for path finder and movement.
    struct {
      uint16        targetTileX_;      ///< [Meteor, ...?] Target map tile X coordinate.
      uint16        targetTileY_;      ///< [Meteor, ...?] Target map tile Y coordinate.
    };
    int             targetPixelX_;     ///< [Rocket, ...?] Target map pixel X coordinate.
  };
  union {
    int             targetPixelY_;     ///< [Rocket, ...?] Target map pixel X coordinate.
    int             unkWaypointData_;  ///< Something associated with pPathContext_ maybe?
    TubeConnection* pTubeConnection_;  ///< [Building] Colony tube connection context.
  };

  int    actionTimer_;     ///< Time left to complete the current action.
  uint16 animationIndex_;  ///< Current animation.
  int16  frameIndex_;      ///< Frame of current animation.

  uint32 flags_;  ///< @see MapObjectFlags.
};
static_assert(sizeof(MapObject) == 0x48, "Incorrect MapObject (base) size.");

//  ====================================================================================================================
/// Base class for Gaia-controlled map objects, such as disasters, beacons, and weapons fire.  These objects are not
/// tracked in TileData::unitIndex and thus can overlap;  index_ is still relevant for indexing into pMapObjArray.
class MapEntity : public MapObject {
  using $ = MapEntity;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  using MapObject::MapObject;

  virtual void DrawSecondary(Viewport* pViewport) { return Thunk<0x4A28D0, &$::DrawSecondary>(pViewport); }

#define OP2_MO_MAPENTITY_VTBL($)  $(DrawSecondary)
  DEFINE_VTBL_TYPE(OP2_MO_MAPENTITY_VTBL);

  // Object size = 0x48?
};

//  ====================================================================================================================
/// Base class for disasters, such as vortexes and meteors.
class Disaster : public MapEntity {
  using $ = Disaster;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  Disaster()                  : MapEntity(UseInternalCtorChain) { InternalCtor<0x4A7F40>(); }
  Disaster(InternalCtorChain) : MapEntity(UseInternalCtorChain) {                           }

  MapEntityType* GetType()            const override { return static_cast<MapEntityType*>(MapEntity::GetType()); }
  void*          Destroy(ibool freeMem = 0) override { return Thunk<0x4A7F70, &$::Destroy>(freeMem);             }
  void           DoEvent()                  override { return Thunk<0x4A8310, &$::DoEvent>();                    }

  virtual void Warn(int warn1StrIdx, SoundID warn1SoundID, int warn2StrIdx, SoundID warn2SoundID)
    { return Thunk<0x4A8040, &$::Warn>(warn1StrIdx, warn1SoundID, warn2StrIdx, warn2SoundID); }
  virtual void PositionAtStart() { return Thunk<0x4A8020, &$::PositionAtStart>(); }

#define OP2_MO_DISASTER_VTBL($)  $(Warn)  $(PositionAtStart)
  DEFINE_VTBL_TYPE(OP2_MO_DISASTER_VTBL, 0x4D73A8);

  void StartDisaster()
    { flags_ |= (MoFlagEntDisasterDidFirstWarn | MoFlagEntDisasterDidSecondWarn);  actionTimer_ = 6; }

  // Object size = between 0x48 and 0x54?
};

//  ====================================================================================================================
/// Base class for Gaia-controlled entities that may be associated with an owner map object, such as weapons fire and
/// mineable resources.
class MapChildEntity : public MapEntity {
  using $ = MapChildEntity;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  MapChildEntity()                  : MapEntity(UseInternalCtorChain) { InternalCtor<0x4A27E0>(); }
  MapChildEntity(InternalCtorChain) : MapEntity(UseInternalCtorChain) {                           }

  MapEntityType* GetType()                const override { return static_cast<MapEntityType*>(MapEntity::GetType()); }
  void           Draw(Viewport*      pViewport) override { return Thunk<0x4A3440, &$::Draw>(pViewport);              }
  ibool          IsVisible(Viewport* pViewport) override { return Thunk<0x4A2960, &$::IsVisible>(pViewport);         }
  void*          Destroy(ibool freeMem = 0)     override { return Thunk<0x4A2810, &$::Destroy>(freeMem);             }

  virtual void Func_31(int a, int b)      { return Thunk<0x4A3EE0, &$::Func_31>(a, b);   }
  virtual void DoExplode(MapObject* pSrc) { return Thunk<0x4A3EB0, &$::DoExplode>(pSrc); }
  virtual void Func_33(int a)             { return Thunk<0x4A3E80, &$::Func_33>(a);      }
  virtual void DoDamage()                 { return Thunk<0x4A3690, &$::DoDamage>();      }

  virtual int GetSecondaryAnimationIndex() { return Thunk<0x4A2950, &$::GetSecondaryAnimationIndex>(); }
  virtual int GetNumAnimationFrames()      { return Thunk<0x4A2890, &$::GetNumAnimationFrames>();      }

  virtual void SetWeaponSource(int unitIndex) { return Thunk<0x4A28B0, &$::SetWeaponSource>(unitIndex); }

#define OP2_MO_MAPCHILDENTITY_VTBL($)                                                                         \
  $(Func_31)  $(DoExplode)  $(Func_33)  $(DoDamage)  $(GetSecondaryAnimationIndex)  $(GetNumAnimationFrames)  \
  $(SetWeaponSource)
  DEFINE_VTBL_TYPE(OP2_MO_MAPCHILDENTITY_VTBL, 0x4D6B58);

public:
  int parentIndex_;  ///< Owner MapObject index.

  int field_4C;
  int field_50;
  int field_54;

  // Object size = 0x58
};

//  ====================================================================================================================
/// Base class for Spaceport rockets.
class Rocket : public MapEntity {
  using $ = Rocket;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  Rocket()                  : MapEntity(UseInternalCtorChain) { InternalCtor<0x4807A0>(); }
  Rocket(InternalCtorChain) : MapEntity(UseInternalCtorChain) {                           }

  void  Draw(Viewport*              pViewport) override { return Thunk<0x4808B0, &$::Draw>(pViewport);              }
  int   GetAnimationIndex(Viewport* pViewport) override { return Thunk<0x4807E0, &$::GetAnimationIndex>(pViewport); }
  void* Destroy(ibool freeMem = 0)             override { return Thunk<0x4807D0, &$::Destroy>(freeMem);             }

  virtual void Launch()  { return Thunk<0x47FC10, &$::Launch>();  }
  virtual void ReEnter() { return Thunk<0x4499D0, &$::ReEnter>(); }
  virtual void Land()    { return Thunk<0x4499E0, &$::Land>();    }

#define OP2_MO_ROCKET_VTBL($)  $(Launch)  $(ReEnter)  $(Land)
  DEFINE_VTBL_TYPE(OP2_MO_ROCKET_VTBL, 0x4D5ED8);

  // NOTE: Uses MapObject::targetPixelX_ and ::targetPixelY_.

  // Object size = 0x54
};

//  ====================================================================================================================
/// Base class for explosive weapons and building explosions.
class Explosive : public MapChildEntity {
  using $ = Explosive;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  using MapChildEntity::MapChildEntity;

  void DoExplode(MapObject* pSrc) override { return Thunk<0x4A4100, &$::DoExplode>(pSrc); }
  void DoDamage()                 override { return Thunk<0x4A4160, &$::DoDamage>();      }

  // Object size = 0x58
};

//  ====================================================================================================================
/// Base class for projectile weapons.
class Projectile : public MapChildEntity {
  using $ = Projectile;
public:
  using MapChildEntity::MapChildEntity;

  void Draw(Viewport* pViewport)    override { return Thunk<0x4A2E20, &$::Draw>(pViewport);              }
  void Func_31(int a, int b)        override { return Thunk<0x4A3B70, &$::Func_31>(a, b);                }
  int  GetSecondaryAnimationIndex() override { return Thunk<0x4A2940, &$::GetSecondaryAnimationIndex>(); }

  // Object size = 0x6C
};

//  ====================================================================================================================
/// Base class for player-controlled map objects.
/// These objects are tracked in TileData::unitIndex, and thus cannot overlap with other MapUnits.
class MapUnit : public MapObject {
  using $ = MapUnit;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  using MapObject::MapObject;

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x44E110, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D4A58);

  ibool IsEnemyUnitSighted()         { return Thunk<0x474AC0, &$::IsEnemyUnitSighted>();         }
  ibool IsNextToSpecialTarget()      { return Thunk<0x44D6C0, &$::IsNextToSpecialTarget>();      }
  ibool CheckSpecialTargetActivate() { return Thunk<0x44D8E0, &$::CheckSpecialTargetActivate>(); }

  void FindTarget()        { return Thunk<0x485100, &$::FindTarget>();        }
  int  CalculateStrength() { return Thunk<0x45D2F0, &$::CalculateStrength>(); }

  void DrawUIOverlay() { return Thunk<0x43EA60, &$::DrawUIOverlay>(); }

  // Object size = 0x48?
};

//  ====================================================================================================================
/// Base class for player air units, mostly unimplemented.  The only child class is SmallCapacityAirTransport.
class AirUnit : public MapUnit {
  using $ = AirUnit;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  AirUnit()                  : MapUnit(UseInternalCtorChain) { InternalCtor<0x474240>(); }
  AirUnit(InternalCtorChain) : MapUnit(UseInternalCtorChain) {                           }

  int GetSelectionBoxSize(int* pWidth,   int* pHeight)  override
    { return Thunk<0x4742C0, &$::GetSelectionBoxSize>(pWidth,  pHeight);  }
  int GetSelectionBoxPos(int*  pXOffset, int* pYOffset) override
    { return Thunk<0x474300, &$::GetSelectionBoxPos>(pXOffset, pYOffset); }
  void* Destroy(ibool freeMem = 0)  override { return Thunk<0x474280, &$::Destroy>(freeMem);        }
  void  OnSave()                    override { return Thunk<0x474390, &$::OnSave>();                }
  void  OnLoad()                    override { return Thunk<0x4743C0, &$::OnLoad>();                }
  void  Move()                      override { return Thunk<0x4742B0, &$::Move>();                  }
  void  TransferUnit(int playerNum) override { return Thunk<0x474450, &$::TransferUnit>(playerNum); }
  int   InitPathContext()           override { return Thunk<0x4743F0, &$::InitPathContext>();       }

  virtual int GetSecondaryAnimationIndex() { return Thunk<0x474340, &$::GetSecondaryAnimationIndex>(); }

#define OP2_MO_AIRUNIT_VTBL($)  $(GetSecondaryAnimationIndex)
  DEFINE_VTBL_TYPE(OP2_MO_AIRUNIT_VTBL, 0x4D5D50);

  // Object size = between 0x5A and 0x5C?
};

//  ====================================================================================================================
/// Base class for player structures and vehicles.
class LandUnit : public MapUnit {
  using $ = LandUnit;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  using MapUnit::MapUnit;

  virtual void DrawSecondary(Viewport* pViewport) { return Thunk<0x406DE0, &$::DrawSecondary>(pViewport); }

#define OP2_MO_LANDUNIT_VTBL($)  $(DrawSecondary)
  DEFINE_VTBL_TYPE(OP2_MO_LANDUNIT_VTBL);

  // Object size = 0x48?
};

//  ====================================================================================================================
/// Base class for player structures.
class Building : public LandUnit {
  using $ = Building;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  Building()                  : LandUnit(UseInternalCtorChain) { InternalCtor<0x482EF0>(); }
  Building(InternalCtorChain) : LandUnit(UseInternalCtorChain) {                           }

  int   ProcessTimers()                        override { return Thunk<0x408A00, &$::ProcessTimers>();              }
  ibool CheckSpontaneouslyExplode()            override { return Thunk<0x409400, &$::CheckSpontaneouslyExplode>();  }
  void  Draw(Viewport*              pViewport) override { return Thunk<0x408EA0, &$::Draw>(pViewport);              }
  void  MarkForRedraw(Viewport*     pViewport) override { return Thunk<0x408E20, &$::MarkForRedraw>(pViewport);     }
  ibool IsVisible(Viewport*         pViewport) override { return Thunk<0x408C10, &$::IsVisible>(pViewport);         }
  int   GetAnimationIndex(Viewport* pViewport) override { return Thunk<0x483560, &$::GetAnimationIndex>(pViewport); }
  int   GetSelectionBoxSize(int* pWidth,   int* pHeight)  override
    { return Thunk<0x483A50, &$::GetSelectionBoxSize>(pWidth, pHeight); }
  int   GetSelectionBoxPos(int*  pXOffset, int* pYOffset) override
    { return Thunk<0x483A70, &$::GetSelectionBoxPos>(pXOffset, pYOffset); }
  int   MouseOver(int xOffset, int yOffset)    override { return Thunk<0x4093D0, &$::MouseOver>(xOffset, yOffset); }
  void* Destroy(ibool freeMem = 0)             override { return Thunk<0x482BD0, &$::Destroy>(freeMem);            }
  void  DoEvent()                              override { return Thunk<0x482D90, &$::DoEvent>();                   }
  void  SetEMPed(uint16 duration)              override { return Thunk<0x4836A0, &$::SetEMPed>(duration);          }
  void  TransferUnit(int playerNum)            override { return Thunk<0x482D00, &$::TransferUnit>(playerNum);     }
  void  GetLocation(int a, int b, int* pTileX, int* pTileY) override
    { return Thunk<0x483B50, &$::GetLocation>(a, b, pTileX, pTileY); }

  virtual void DrawShadows(Viewport* pViewport) { return Thunk<0x408CD0, &$::DrawShadows>(pViewport); }

  virtual void Explode(ibool explode) { return Thunk<0x483160, &$::Explode>(explode); }

  virtual void SetDisabled()               { return Thunk<0x483780, &$::SetDisabled>();               }
  virtual void SetEnabled()                { return Thunk<0x4837D0, &$::SetEnabled>();                }
  virtual void SetOwnerRecalculateValues() { return Thunk<0x483810, &$::SetOwnerRecalculateValues>(); }

  virtual int  GetSpecialAnimationIndex(int animationType) const
    { return Thunk<0x4836E0, &$::GetSpecialAnimationIndex>(animationType); }
  virtual void DoAmbientAnimation()       { return Thunk<0x483580, &$::DoAmbientAnimation>(); }
  virtual int  GetAnimationFrame()  const { return Thunk<0x409310, &$::GetAnimationFrame>();  }

  virtual void UpdateTotalCapacities(int a, int b) { return Thunk<0x41D620, &$::UpdateTotalCapacities>(a, b); }

  virtual ibool IsEnabled()              const { return Thunk<0x483710, &$::IsEnabled>();              }
  virtual ibool IsActivePowerGenerator() const { return Thunk<0x483770, &$::IsActivePowerGenerator>(); }
  virtual void  SetDisabledWorkers()           { return Thunk<0x4838D0, &$::SetDisabledWorkers>();     }
  virtual void  SetEnabledWorkers()            { return Thunk<0x483870, &$::SetEnabledWorkers>();      }
  virtual void  SetDisabledScientists()        { return Thunk<0x4838A0, &$::SetDisabledScientists>();  }
  virtual void  SetEnabledScientists()         { return Thunk<0x483840, &$::SetEnabledScientists>();   }
  virtual void  SetEnabledPower()              { return Thunk<0x483900, &$::SetEnabledPower>();        }
  virtual void  SetDisabledPower()             { return Thunk<0x483940, &$::SetDisabledPower>();       }

  virtual void StopProduction() { return Thunk<0x401C20, &$::StopProduction>(); }

  virtual void ProcessDeath() { return Thunk<0x408A20, &$::ProcessDeath>(); }

  virtual int GetNumAnimationFrames() { return Thunk<0x483A10, &$::GetNumAnimationFrames>(); }

  virtual int Func_51()                           { return Thunk<0x483A30, &$::Func_51>();                           }
  virtual int GetPowerProduction()          const { return Thunk<0x401C30, &$::GetPowerProduction>();                }
  virtual int Func_53()                           { return Thunk<0x401C40, &$::Func_53>();                           }
  virtual int GetFoodProduction()           const { return Thunk<0x409AF0, &$::GetFoodProduction>();                 }
  virtual int Func_55()                           { return Thunk<0x401C50, &$::Func_55>();                           }
  virtual int GetNumScientistsRequired()    const { return Thunk<0x401C60, &$::GetNumScientistsRequired>();          }
  virtual int GetNumScientistsResearching() const { return Thunk<0x401C80, &$::GetNumScientistsResearching>();       }
  virtual int GetNumWorkersRequired()       const { return Thunk<0x401C90, &$::GetNumWorkersRequired>();             }
  virtual int Func_59()                           { return Thunk<0x401CB0, &$::Func_59>();                           }
  virtual int GetPowerRequiredAndEnable()         { return Thunk<0x483980, &$::GetPowerRequiredAndEnable>();         }
  virtual int GetNumWorkersRequiredAndEnable()    { return Thunk<0x4839B0, &$::GetNumWorkersRequiredAndEnable>();    }
  virtual int GetNumScientistsRequiredAndEnable() { return Thunk<0x4839E0, &$::GetNumScientistsRequiredAndEnable>(); }

#define OP2_MO_BUILDING_VTBL($)                                                                                        \
  $(DrawShadows)  $(Explode)  $(SetDisabled)  $(SetEnabled)  $(SetOwnerRecalculateValues)  $(GetSpecialAnimationIndex) \
  $(DoAmbientAnimation)  $(GetAnimationFrame)  $(UpdateTotalCapacities)  $(IsEnabled)  $(IsActivePowerGenerator)       \
  $(SetDisabledWorkers)  $(SetEnabledWorkers)  $(SetDisabledScientists)  $(SetEnabledScientists)  $(SetEnabledPower)   \
  $(SetDisabledPower)  $(StopProduction)  $(ProcessDeath)  $(GetNumAnimationFrames)  $(Func_51)  $(GetPowerProduction) \
  $(Func_53)  $(GetFoodProduction)  $(Func_55)  $(GetNumScientistsRequired)  $(GetNumScientistsResearching)            \
  $(GetNumWorkersRequired)  $(Func_59)  $(GetPowerRequiredAndEnable)  $(GetNumWorkersRequiredAndEnable)                \
  $(GetNumScientistsRequiredAndEnable)
  DEFINE_VTBL_TYPE(OP2_MO_BUILDING_VTBL, 0x4D6030);

  void KillOccupants() { return Thunk<0x482BF0, &$::KillOccupants>(); }

  int GetDockLocation(Location* pOut) const { return Thunk<0x482F40, &$::GetDockLocation>(pOut); }

public:
  uint8 cargoBayCargoOrWeapon_[6];  ///< [Factory, Garage] MapID cargo/weapon associated with cargo bay contents.

  uint16 field_4E;
  int    field_50;
  uint16 field_54;

  uint16 timerEMP_;  ///< EMP timer remaining.

  int field_58;
  int field_5C;

  // Object size = 0x60
};

//  ====================================================================================================================
/// Base class for all factory structures.
class FactoryBuilding : public Building {
  using $ = FactoryBuilding;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  FactoryBuilding() : Building() { flags_ |= MoFlagBldFactory; }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x446F60, &$::Destroy>(freeMem); }
  void  StopProduction()           override { return Thunk<0x415AB0, &$::StopProduction>(); }

  virtual int  GetProductionRate()                 { return Thunk<0x4158C0, &$::GetProductionRate>();       }
  virtual int  FinishProduction(MapID type, int a) { return Thunk<0x4157C0, &$::FinishProduction>(type, a); }
  virtual int  GetCargoBay(int bayIndex)           { return Thunk<0x446F50, &$::GetCargoBay>(bayIndex);     }
  virtual int  ActivateDock(uint16 a, int b)       { return Thunk<0x415990, &$::ActivateDock>(a, b);        }
  virtual void DiscardCargoBay(int bayIndex)       { return Thunk<0x4159F0, &$::DiscardCargoBay>(bayIndex); }
  virtual int  GetTotalBuildTime()                 { return Thunk<0x415920, &$::GetTotalBuildTime>();       }
  virtual int  Func_69()                           { return Thunk<0x415AA0, &$::Func_69>();                 }

#define OP2_MO_FACTORYBUILDING_VTBL($)                                                                                 \
  $(GetProductionRate)  $(FinishProduction)  $(GetCargoBay)  $(ActivateDock)  $(DiscardCargoBay)  $(GetTotalBuildTime) \
  $(Func_69)
  DEFINE_VTBL_TYPE(OP2_MO_FACTORYBUILDING_VTBL, 0x4D29D8);

public:
  uint8  itemToProduce_;        ///< MapID item to produce.
  uint8  cargoBayContents_[6];  ///< MapID cargo bay contents' unit type.
  uint8  field_67;
  int    field_68;
  uint16 field_6C;

  // Object size = 0x6E
};

//  ====================================================================================================================
/// Base class for all lab structures.  This is also the class mapped to MapID = 0 and mapMaxObject.
class LabBuilding : public Building {
  using $ = LabBuilding;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  using Building::Building;

  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x431FB0, &$::Destroy>(freeMem);                    }
  void SetDisabledScientists()             override { return Thunk<0x4321B0, &$::SetDisabledScientists>();             }
  void StopProduction()                    override { return Thunk<0x432180, &$::StopProduction>();                    }
  int  GetNumScientistsRequired()    const override { return Thunk<0x432290, &$::GetNumScientistsRequired>();          }
  int  GetNumScientistsResearching() const override { return Thunk<0x4322C0, &$::GetNumScientistsResearching>();       }
  int  GetNumScientistsRequiredAndEnable() override { return Thunk<0x432140, &$::GetNumScientistsRequiredAndEnable>(); }

  DEFINE_VTBL_GETTER(0x4D0160);

  // Object size = 0x60?
};

//  ====================================================================================================================
/// Base class for all ore mine structures.
class MineBuilding : public Building {
  using $ = MineBuilding;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  MineBuilding()                  : Building(UseInternalCtorChain) { InternalCtor<0x44AE30>(); }
  MineBuilding(InternalCtorChain) : Building(UseInternalCtorChain) {                           }

  void  MarkForRedraw(Viewport* pViewport)  override { return Thunk<0x44B3A0, &$::MarkForRedraw>(pViewport);    }
  int   MouseOver(int xOffset, int yOffset) override { return Thunk<0x44B430, &$::MouseOver>(xOffset, yOffset); }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x44AE60, &$::Destroy>(freeMem);            }
  void  DrawSecondary(Viewport* pViewport)  override { return Thunk<0x44B280, &$::DrawSecondary>(pViewport);    }
  void  DrawShadows(Viewport*   pViewport)  override { return Thunk<0x44B380, &$::DrawShadows>(pViewport);      }
  void  Explode(ibool explode)              override { return Thunk<0x44AFB0, &$::Explode>(explode);            }
  ibool IsEnabled()                   const override { return Thunk<0x44AE70, &$::IsEnabled>();                 }

  virtual void     BuildMine(int barYield, int variant, int numTruckLoads, int beaconUnitIdx)
    { return Thunk<0x44AED0, &$::BuildMine>(barYield, variant, numTruckLoads, beaconUnitIdx); }
  virtual int      NextTruckLoad()     { return Thunk<0x44AF00, &$::NextTruckLoad>();      }
  virtual int      CalculateOreYield() { return Thunk<0x44AF10, &$::CalculateOreYield>();  }
  virtual OreYield GetBarYield()       { return Thunk<0x44AFF0, &$::GetBarYield>();        }

#define OP2_MO_MINEBUILDING_VTBL($)  $(BuildMine)  $(NextTruckLoad)  $(CalculateOreYield)  $(GetBarYield)
  DEFINE_VTBL_TYPE(OP2_MO_MINEBUILDING_VTBL, 0x4D4798);

public:
  // ** TODO this doesn't look correct
  //int      numTruckLoads_;
  //int      variant_;
  //OreYield barYield_;

  // Object size = 0x74
};

//  ====================================================================================================================
/// Base class for all power generator structures - Tokamak, Solar Power Array, MHD Generator, Geothermal Plant.
class PowerBuilding : public Building {
  using $ = PowerBuilding;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  using Building::Building;

  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x446E50, &$::UpdateTotalCapacities>(a, b); }
  ibool IsEnabled()                   const override { return Thunk<0x46E7E0, &$::IsEnabled>();                 }
  ibool IsActivePowerGenerator()      const override { return Thunk<0x46E830, &$::IsActivePowerGenerator>();    }
  int   GetPowerProduction()          const override { return Thunk<0x483A90, &$::GetPowerProduction>();        }

  // Object size = 0x60
};

//  ====================================================================================================================
/// Base class for player vehicles.
class Vehicle : public LandUnit {
  using $ = Vehicle;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  Vehicle()                  : LandUnit(UseInternalCtorChain) { InternalCtor<0x44B550>(); }
  Vehicle(InternalCtorChain) : LandUnit(UseInternalCtorChain) {                           }

  void  Draw(Viewport*          pViewport) override { return Thunk<0x44CA20, &$::Draw>(pViewport);          }
  void  MarkForRedraw(Viewport* pViewport) override { return Thunk<0x44C8C0, &$::MarkForRedraw>(pViewport); }
  ibool IsVisible(Viewport*     pViewport) override { return Thunk<0x44CE20, &$::IsVisible>(pViewport);     }
  int   GetSelectionBoxSize(int* pWidth,   int* pHeight)  override
    { return Thunk<0x44CDA0, &$::GetSelectionBoxSize>(pWidth, pHeight); }
  int   GetSelectionBoxPos(int*  pXOffset, int* pYOffset) override
    { return Thunk<0x44CDE0, &$::GetSelectionBoxPos>(pXOffset, pYOffset); }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x44B590, &$::Destroy>(freeMem);          }
  void  OnSave()                           override { return Thunk<0x44B5E0, &$::OnSave>();                  }
  void  OnLoad()                           override { return Thunk<0x44B610, &$::OnLoad>();                  }
  void  SetEMPed(uint16        duration)   override { return Thunk<0x44B700, &$::SetEMPed>(duration);        }
  void  SetStickyfoamed(uint16 duration)   override { return Thunk<0x44B6D0, &$::SetStickyfoamed>(duration); }
  void  Move()                             override { return Thunk<0x44C1C0, &$::Move>();                    }
  void  TransferUnit(int playerNum)        override { return Thunk<0x44CC70, &$::TransferUnit>(playerNum);   }
  ibool IsSelectable()                     override { return Thunk<0x44B730, &$::IsSelectable>();            }

  virtual void PathFind(int dstTileX, int dstTileY, int a, int b)
    { return Thunk<0x44C460, &$::PathFind>(dstTileX, dstTileY, a, b); }

  virtual int GetChassisAnimationIndex(int rotation) { return Thunk<0x44C600, &$::GetChassisAnimationIndex>(rotation); }

  virtual void  DoBuild(int numWaypoints, const Waypoint* pWaypoints, int a)
    { return Thunk<0x406DF0, &$::DoBuild>(numWaypoints, pWaypoints, a); }
  virtual void  DoDock()                           { return Thunk<0x40A590, &$::DoDock>();                   }
  virtual void  UnblockBuildArea()                 { return Thunk<0x406E00, &$::UnblockBuildArea>();         }
  virtual ibool CanProduceAt(int tileX, int tileY) { return Thunk<0x411230, &$::CanProduceAt>(tileX, tileY); }

#define OP2_MO_VEHICLE_VTBL($)  \
  $(PathFind)  $(GetChassisAnimationIndex)  $(DoBuild)  $(DoDock)  $(UnblockBuildArea)  $(CanProduceAt)
  DEFINE_VTBL_TYPE(OP2_MO_VEHICLE_VTBL, 0x4D49C0);

  void  SetDestinationWaypoints(int numWaypoints, const Waypoint* pWaypoints, int a, int b)
    { return Thunk<0x423210, &$::SetDestinationWaypoints>(numWaypoints, pWaypoints, a, b); }
  void  StartNextWaypoint()                          { return Thunk<0x44B640, &$::StartNextWaypoint>();         }
  void  PathFindToNextWaypoint()                     { return Thunk<0x44C400, &$::PathFindToNextWaypoint>();    }
  ibool IsTileUnoccupied()                           { return Thunk<0x44D640, &$::IsTileUnoccupied>();          }
  ibool CanBeBumpedBy(const MapObject* pBumpingUnit) { return Thunk<0x44BE70, &$::CanBeBumpedBy>(pBumpingUnit); }
  ibool ShouldAIUseToDestroyWalls()                  { return Thunk<0x44D5F0, &$::ShouldAIUseToDestroyWalls>(); }

  ibool ScavengeRubble() { return Thunk<0x406D10, &$::ScavengeRubble>(); }
  ibool SurveyBeacon()   { return Thunk<0x484420, &$::SurveyBeacon>();   }

public:
  union {
    int        field_48;
    MapObject* pTargetUnit_;
  };

  uint16 field_4C;
  uint8  field_4E;

  uint8 weaponOfCargo_;  ///< [ConVec] MapID weapon of structure kit cargo.

  int field_50;

  uint16 timerStickyfoam_;  ///< Stickyfoam timer remaining.
  uint16 timerEMP_;         ///< EMP timer remaining.
  uint16 timerESG_;         ///< ESG timer remaining.

  uint16 field_5A;
  int    cargoToLoad_;  ///< [CargoTruck, ...?] Cargo to load when animation has sufficiently progressed.
  uint16 field_60;

  // ** TODO these should be moved to child classes
  uint16 field_62;
  int    field_64;
  int    field_68;
  uint16 field_6C;
  int    field_6E;
  int    field_72;

  // Object size = 0x62
};

//  ====================================================================================================================
/// Base class for all vehicles with weapon turrets.
class TankVehicle : public Vehicle {
  using $ = TankVehicle;
public:
  OP2_MO_GET_INSTANCE_IMPL();

  TankVehicle() : Vehicle() { flags_ |= MoFlagOppFiredUpon | MoFlagOffensive; }

  int   FireWeapon()                       override { return Thunk<0x484F40, &$::FireWeapon>();             }
  void  Draw(Viewport*          pViewport) override { return Thunk<0x484CB0, &$::Draw>(pViewport);          }
  void  MarkForRedraw(Viewport* pViewport) override { return Thunk<0x484A50, &$::MarkForRedraw>(pViewport); }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x447670, &$::Destroy>(freeMem);         }
  void  DoEvent()                          override { return Thunk<0x485300, &$::DoEvent>();                }
  void  SetEMPed(uint16 duration)          override { return Thunk<0x484C10, &$::SetEMPed>(duration);       }

  DEFINE_VTBL_GETTER(0x4D0658);

  // Object size = 0x62
};


namespace MapObj {

// =====================================================================================================================
class AmbientAnimation : public MapEntity {
  using $ = AmbientAnimation;
public:
  OP2_MO_CLASS_MAP_ID(AmbientAnimation);

  using MapEntity::MapEntity;

  Type* GetType()                    const override { return Thunk<0x4473F0, &$::GetType>();                }
  void  Draw(Viewport*          pViewport) override { return Thunk<0x405270, &$::Draw>(pViewport);          }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x444FB0, &$::Destroy>(freeMem);         }
  void  DrawSecondary(Viewport* pViewport) override { return Thunk<0x405190, &$::DrawSecondary>(pViewport); }

  DEFINE_VTBL_GETTER(0x4D4670);

  // Object size = 0x68
};

// =====================================================================================================================
class Earthquake : public Disaster {
  using $ = Earthquake;
public:
  OP2_MO_CLASS_MAP_ID(Earthquake);

  Earthquake() : Disaster(UseInternalCtorChain) { InternalCtor<0x413610>(); }

  Type* GetType()                const override { return Thunk<0x413CC0, &$::GetType>();            }
  void  Draw(Viewport*      pViewport) override { return Thunk<0x413660, &$::Draw>(pViewport);      }
  ibool IsVisible(Viewport* pViewport) override { return Thunk<0x4137C0, &$::IsVisible>(pViewport); }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x413630, &$::Destroy>(freeMem);     }
  void  DoEvent()                      override { return Thunk<0x413890, &$::DoEvent>();            }

  DEFINE_VTBL_GETTER(0x4CFB10);

  // Object size = 0x5C
};

// =====================================================================================================================
class Eruption : public Disaster {
  using $ = Eruption;
public:
  OP2_MO_CLASS_MAP_ID(Eruption);

  Eruption() : Disaster(UseInternalCtorChain) { InternalCtor<0x4A81F0>(); }

  Type* GetType()                const override { return Thunk<0x4A8320, &$::GetType>();            }
  ibool IsVisible(Viewport* pViewport) override { return Thunk<0x4A8260, &$::IsVisible>(pViewport); }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x4A8230, &$::Destroy>(freeMem);     }
  void  DoEvent()                      override { return Thunk<0x4A8270, &$::DoEvent>();            }

  DEFINE_VTBL_GETTER(0x4D7430);

  // Object size = 0x54
};

// =====================================================================================================================
class Lightning : public Disaster {
  using $ = Lightning;
public:
  OP2_MO_CLASS_MAP_ID(Lightning);

  Lightning() : Disaster(UseInternalCtorChain) { InternalCtor<0x432F10>(); }

  Type* GetType()            const override { return Thunk<0x433E50, &$::GetType>();         }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x433780, &$::Draw>(pViewport);   }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x432F30, &$::Destroy>(freeMem);  }
  void  DoEvent()                  override { return Thunk<0x433030, &$::DoEvent>();         }
  void  PositionAtStart()          override { return Thunk<0x432F60, &$::PositionAtStart>(); }

  DEFINE_VTBL_GETTER(0x4D0388);

public:
  int field_48;
  int field_4C;
  int field_50;
  int field_54;
  int field_58;
  int field_5C;
  int field_60;

  uint16 endTileX_;  ///< End map tile X (may not reach before timeout).  If -1, then gets initialized when triggered.
  uint16 endTileY_;  ///< End map tile Y (may not reach before timeout).  If -1, then gets initialized when triggered.

  // Object size = 0x68
};

// =====================================================================================================================
class Meteor : public Disaster {
  using $ = Meteor;
public:
  OP2_MO_CLASS_MAP_ID(Meteor);

  Meteor() : Disaster(UseInternalCtorChain) { InternalCtor<0x44A300>(); }

  Type* GetType()                const override { return Thunk<0x44ADE0, &$::GetType>();            }
  void  Draw(Viewport*      pViewport) override { return Thunk<0x44AAB0, &$::Draw>(pViewport);      }
  ibool IsVisible(Viewport* pViewport) override { return Thunk<0x44AD00, &$::IsVisible>(pViewport); }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x44A2E0, &$::Destroy>(freeMem);     }
  void  DoEvent()                      override { return Thunk<0x44A320, &$::DoEvent>();            }
  void  PositionAtStart()              override { return Thunk<0x44ADC0, &$::PositionAtStart>();    }

  DEFINE_VTBL_GETTER(0x4D4710);

  // Note: Uses MapObject::targetTileX_ and targetTileY_.

  // Object size = 0x5C
};

// =====================================================================================================================
class Vortex : public Disaster {
  using $ = Vortex;
public:
  OP2_MO_CLASS_MAP_ID(Vortex);

  Vortex() : Disaster(UseInternalCtorChain) { InternalCtor<0x48F4A0>(); }

  Type* GetType()                    const override { return Thunk<0x490220, &$::GetType>();                }
  void  Draw(Viewport*          pViewport) override { return Thunk<0x48FEF0, &$::Draw>(pViewport);          }
  ibool IsVisible(Viewport*     pViewport) override { return Thunk<0x490160, &$::IsVisible>(pViewport);     }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x48F4D0, &$::Destroy>(freeMem);         }
  void  DoEvent()                          override { return Thunk<0x48F500, &$::DoEvent>();                }
  void  DrawSecondary(Viewport* pViewport) override { return Thunk<0x48FC80, &$::DrawSecondary>(pViewport); }
  void  PositionAtStart()                  override { return Thunk<0x490170, &$::PositionAtStart>();        }

  DEFINE_VTBL_GETTER(0x4D6460);

public:
  int field_48;

  int   directionChangeStartTick_;  ///< Makes smooth transitions for the cloud column position
  uint8 direction_;                 ///< Value from 0x00-0xFF;  0x00 = right, 0x40 = down, 0x80 = left, 0xC0 = up

  uint8  field_51;
  uint8  field_52;
  uint8  field_53;
  int    field_54;
  uint8  field_58;
  uint8  field_59;
  uint8  field_5A;
  uint8  field_5B;
  uint8  field_5C;
  uint16 field_5D;
  uint16 field_5F;
  uint16 field_61;
  uint16 field_63;

  uint16 endTileX_;  ///< End map tile X (may not reach before timeout).  If -1, then gets initialized when triggered.
  uint16 endTileY_;  ///< End map tile Y (may not reach before timeout).  If -1, then gets initialized when triggered.

  // Object size = 0x69
};

// =====================================================================================================================
class EMPBlast : public MapChildEntity {
  using $ = EMPBlast;
public:
  OP2_MO_CLASS_MAP_ID(EMPBlast);

  using MapChildEntity::MapChildEntity;

  Type* GetType()                const override { return Thunk<0x4473D0, &$::GetType>();            }
  void  Draw(Viewport*      pViewport) override { return Thunk<0x413CD0, &$::Draw>(pViewport);      }
  ibool IsVisible(Viewport* pViewport) override { return Thunk<0x413E60, &$::IsVisible>(pViewport); }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x444270, &$::Destroy>(freeMem);     }
  void  DoEvent()                      override { return Thunk<0x413F00, &$::DoEvent>();            }
  void  DoDamage()                     override { return Thunk<0x413E50, &$::DoDamage>();           }

  DEFINE_VTBL_GETTER(0x4D3BF0);

  // Object size = 0x58
};

// =====================================================================================================================
class EnergyCannon : public MapChildEntity {
  using $ = EnergyCannon;
public:
  OP2_MO_CLASS_MAP_ID(EnergyCannon);

  EnergyCannon() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x4A54A0>(); }

  Type* GetType()            const override { return Thunk<0x4A6A00, &$::GetType>();        }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x4A32A0, &$::Draw>(pViewport);  }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4A54D0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D7310);

  // Object size = 0x58
};

// =====================================================================================================================
class Fumarole : public MapChildEntity {
  using $ = Fumarole;
public:
  OP2_MO_CLASS_MAP_ID(Fumarole);

  Fumarole() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x405900>(); }

  Type* GetType()            const override { return Thunk<0x405E40, &$::GetType>();        }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x405950, &$::Draw>(pViewport);  }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x405920, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4CF368);

  // Object size = 0x58
};

// =====================================================================================================================
class Laser : public MapChildEntity {
  using $ = Laser;
public:
  OP2_MO_CLASS_MAP_ID(Laser);

  Laser() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x4A49B0>(); }

  Type* GetType()            const override { return Thunk<0x4A68F0, &$::GetType>();        }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x4A34F0, &$::Draw>(pViewport);  }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4A49E0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D6C88);

  // Object size = 0x58
};

// =====================================================================================================================
class MagmaVent : public MapChildEntity {
  using $ = MagmaVent;
public:
  OP2_MO_CLASS_MAP_ID(MagmaVent);

  MagmaVent() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x405780>(); }

  Type* GetType()                    const override { return Thunk<0x405E30, &$::GetType>();                }
  void  Draw(Viewport*          pViewport) override { return Thunk<0x4057E0, &$::Draw>(pViewport);          }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x4057B0, &$::Destroy>(freeMem);         }
  void  DrawSecondary(Viewport* pViewport) override { return Thunk<0x4057F0, &$::DrawSecondary>(pViewport); }

  DEFINE_VTBL_GETTER(0x4CF2D0);

  // Object size = 0x58
};

// =====================================================================================================================
class Microwave : public MapChildEntity {
  using $ = Microwave;
public:
  OP2_MO_CLASS_MAP_ID(Microwave);

  Microwave() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x4A4A60>(); }

  Type* GetType()            const override { return Thunk<0x4A6900, &$::GetType>();        }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x4A30D0, &$::Draw>(pViewport);  }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4A4A90, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D6D20);

  // Object size = 0x58
};

// =====================================================================================================================
class MiningBeacon : public MapChildEntity {
  using $ = MiningBeacon;
public:
  OP2_MO_CLASS_MAP_ID(MiningBeacon);

  MiningBeacon() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x405350>(); }

  Type* GetType()                const override { return Thunk<0x405E20, &$::GetType>();            }
  void  Draw(Viewport*      pViewport) override { return Thunk<0x405420, &$::Draw>(pViewport);      }
  ibool IsVisible(Viewport* pViewport) override { return Thunk<0x4053C0, &$::IsVisible>(pViewport); }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x405390, &$::Destroy>(freeMem);     }

  virtual OreType  GetOreType()                        { return Thunk<0x405520, &$::GetOreType>();                    }
  virtual void     Survey(int playerNum)               { return Thunk<0x405530, &$::Survey>(playerNum);               }
  virtual ibool    IsSurveyed(int playerNum)           { return Thunk<0x4055F0, &$::IsSurveyed>(playerNum);           }
  virtual void     CreateBeacon(int oreType, int barYield, int variant)
    { return Thunk<0x405650, &$::CreateBeacon>(oreType, barYield, variant); }
  virtual OreYield GetBarYield()                       { return Thunk<0x405720, &$::GetBarYield>();                   }
  virtual int      BuildMine(MapObject* pMineUnit)     { return Thunk<0x405730, &$::BuildMine>(pMineUnit);            }
  virtual void     SetNumTruckLoads(int numTruckLoads) { return Thunk<0x405770, &$::SetNumTruckLoads>(numTruckLoads); }

#define OP2_MO_MININGBEACON_VTBL($)  \
  $(GetOreType)  $(Survey)  $(IsSurveyed)  $(CreateBeacon)  $(GetBarYield)  $(BuildMine)  $(SetNumTruckLoads)
  DEFINE_VTBL_TYPE(OP2_MO_MININGBEACON_VTBL, 0x4CF218);

public:
  int numTruckLoads_;  ///< Number of truck loads so far for ore yield peaking calculation.
  
  OreYield mineYield_;    ///< Bar yield type (major impact on output).
  int      mineVariant_;  ///< Yield variant  (minor impact on output).
  OreType  mineType_;     ///< Common or rare ore.

  uint16 field_65;

  PackedPlayerBitmask playerSurveyedMask_;  ///< Which players have surveyed this beacon

  // Object size = 0x6B
};

// =====================================================================================================================
class ThorsHammer : public MapChildEntity {
  using $ = ThorsHammer;
public:
  OP2_MO_CLASS_MAP_ID(ThorsHammer);

  ThorsHammer() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x48A770>(); }

  Type* GetType()                    const override { return Thunk<0x48B290, &$::GetType>();                }
  void  Draw(Viewport* pViewport)          override { return Thunk<0x48AE40, &$::Draw>(pViewport);          }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x48A790, &$::Destroy>(freeMem);         }
  void  DrawSecondary(Viewport* pViewport) override { return Thunk<0x48AD40, &$::DrawSecondary>(pViewport); }
  void  Func_31(int a, int b)              override { return Thunk<0x48B030, &$::Func_31>(a, b);            }

  DEFINE_VTBL_GETTER(0x4D63A8);

public:
  int field_58;

  // Object size = 0x64
};

// =====================================================================================================================
class Wreckage : public MapChildEntity {
  using $ = Wreckage;
public:
  OP2_MO_CLASS_MAP_ID(Wreckage);

  Wreckage() : MapChildEntity(UseInternalCtorChain) { InternalCtor<0x405AF0>(); }

  Type* GetType()            const override { return Thunk<0x405E50, &$::GetType>();        }
  int   ProcessTimers()            override { return Thunk<0x405CF0, &$::ProcessTimers>();  }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x405B50, &$::Draw>(pViewport);  }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x405B20, &$::Destroy>(freeMem); }

  virtual ibool DiscoverWreckage(int playerNum)       { return Thunk<0x405C10, &$::DiscoverWreckage>(playerNum); }
  virtual ibool IsDiscovered(int     playerNum) const { return Thunk<0x405C80, &$::IsDiscovered>(playerNum);     }
  virtual void  SetWreckageTech(int  techId)          { return Thunk<0x405CE0, &$::SetWreckageTech>(techId);     }

#define OP2_MO_WRECKAGE_VTBL($)  $(DiscoverWreckage)  $(IsDiscovered)  $(SetWreckageTech)
  DEFINE_VTBL_TYPE(OP2_MO_WRECKAGE_VTBL, 0x4CF400);

public:
  PlayerBitmask playerDiscoveredMask_;
  int           techID_;                ///< Tech ID of technology granted when wreckage is turned in at a Spaceport.

  // Object size = 0x60
};

// =====================================================================================================================
class LargeBuildingExplosion : public Explosive {
  using $ = LargeBuildingExplosion;
public:
  OP2_MO_CLASS_MAP_ID(LargeBuildingExplosion);

  using Explosive::Explosive;

  Type* GetType()              const override { return Thunk<0x447470, &$::GetType>();                    }
  void  Draw(Viewport* pViewport)    override { return Thunk<0x4A6840, &$::Draw>(pViewport);              }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4447B0, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x447480, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D4028);

  // Object size = 0x58
};

// =====================================================================================================================
class MediumBuildingExplosion : public Explosive {
  using $ = MediumBuildingExplosion;
public:
  OP2_MO_CLASS_MAP_ID(MediumBuildingExplosion);

  using Explosive::Explosive;

  Type* GetType()              const override { return Thunk<0x447440, &$::GetType>();                    }
  void  Draw(Viewport* pViewport)    override { return Thunk<0x4A67B0, &$::Draw>(pViewport);              }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x444730, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x447450, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D3F60);

  // Object size = 0x58
};

// =====================================================================================================================
class SmallBuildingExplosion : public Explosive {
  using $ = SmallBuildingExplosion;
public:
  OP2_MO_CLASS_MAP_ID(SmallBuildingExplosion);

  using Explosive::Explosive;

  Type* GetType()              const override { return Thunk<0x447410, &$::GetType>();                    }
  void  Draw(Viewport* pViewport)    override { return Thunk<0x4A6720, &$::Draw>(pViewport);              }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4446B0, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x447420, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D3E98);

  // Object size = 0x58
};

// =====================================================================================================================
class SelfDestruct : public Explosive {
  using $ = SelfDestruct;
public:
  OP2_MO_CLASS_MAP_ID(SelfDestruct);

  SelfDestruct() : Explosive(UseInternalCtorChain) { InternalCtor<0x4A4DD0>(); }

  Type* GetType()              const override { return Thunk<0x4A69B0, &$::GetType>();                    }
  void  Draw(Viewport* pViewport)    override { return Thunk<0x4A65A0, &$::Draw>(pViewport);              }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4A4E00, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x4A69C0, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D7018);

  // Object size = 0x58
};

// =====================================================================================================================
class Starflare : public Explosive {
  using $ = Starflare;
public:
  OP2_MO_CLASS_MAP_ID(Starflare);

  Starflare() : Explosive(UseInternalCtorChain) { InternalCtor<0x4A4C70>(); }

  Type* GetType()              const override { return Thunk<0x4A6950, &$::GetType>();                    }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4A4CA0, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x4A6960, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D6EE8);

  // Object size = 0x58
};

// =====================================================================================================================
class StarflareGP : public Explosive {
  using $ = StarflareGP;
public:
  OP2_MO_CLASS_MAP_ID(StarflareGP);

  StarflareGP() : Explosive(UseInternalCtorChain) { InternalCtor<0x4A4E80>(); }

  Type* GetType()              const override { return Thunk<0x4A6930, &$::GetType>();                    }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4A4EB0, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x4A6940, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D70B0);

  // Object size = 0x58
};

//  ====================================================================================================================
/// @note This is an orphaned type - creating Supernova maps to SupernovaGP.
class Supernova : public Explosive {
  using $ = Supernova;
public:
  OP2_MO_CLASS_MAP_ID(Supernova);

  Supernova() : Explosive(UseInternalCtorChain) { InternalCtor<0x4A4D20>(); }

  Type* GetType()              const override { return Thunk<0x4A6970, &$::GetType>();                    }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x4A6980, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D6F80);

  // Object size = 0x58
};

// =====================================================================================================================
class SupernovaGP : public Explosive {
  using $ = SupernovaGP;
public:
  OP2_MO_CLASS_MAP_ID(SupernovaGP);

  SupernovaGP() : Explosive(UseInternalCtorChain) { InternalCtor<0x4A4F30>(); }

  Type* GetType()              const override { return Thunk<0x4A6990, &$::GetType>();                    }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4A4F60, &$::Destroy>(freeMem);             }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x4A69A0, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D7148);

  // Object size = 0x58
};

// =====================================================================================================================
class AcidCloud : public Projectile {
  using $ = AcidCloud;
public:
  OP2_MO_CLASS_MAP_ID(AcidCloud);

  AcidCloud() : Projectile(UseInternalCtorChain) { InternalCtor<0x401000>(); }

  Type* GetType()                const override { return Thunk<0x401900, &$::GetType>();                    }
  void  Draw(Viewport*      pViewport) override { return Thunk<0x401560, &$::Draw>(pViewport);              }
  ibool IsVisible(Viewport* pViewport) override { return Thunk<0x401410, &$::IsVisible>(pViewport);         }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x401020, &$::Destroy>(freeMem);             }
  void  DoEvent()                      override { return Thunk<0x401050, &$::DoEvent>();                    }
  void  DoDamage()                     override { return Thunk<0x4013C0, &$::DoDamage>();                   }
  int   GetSecondaryAnimationIndex()   override { return Thunk<0x401910, &$::GetSecondaryAnimationIndex>(); }
  int   GetNumAnimationFrames()        override { return Thunk<0x4013D0, &$::GetNumAnimationFrames>();      }

  DEFINE_VTBL_GETTER(0x4CF058);

  // Object size = 0x6C
};

// =====================================================================================================================
class BFG : public Projectile {
  using $ = BFG;
public:
  OP2_MO_CLASS_MAP_ID(BFG);

  BFG() : Projectile(UseInternalCtorChain) { InternalCtor<0x4A53F0>(); }

  Type* GetType()            const override { return Thunk<0x4A69F0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4A5420, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D7278);

  // Object size = 0x6C
};

// =====================================================================================================================
class EMP : public Projectile {
  using $ = EMP;
public:
  OP2_MO_CLASS_MAP_ID(EMP);

  EMP() : Projectile(UseInternalCtorChain) { InternalCtor<0x4A4900>(); }

  Type* GetType()                const override { return Thunk<0x4A68D0, &$::GetType>();                    }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x4A4930, &$::Destroy>(freeMem);             }
  void  DoDamage()                     override { return Thunk<0x4A4560, &$::DoDamage>();                   }
  int   GetSecondaryAnimationIndex()   override { return Thunk<0x4A68E0, &$::GetSecondaryAnimationIndex>(); }
  void  SetWeaponSource(int unitIndex) override { return Thunk<0x4A48C0, &$::SetWeaponSource>(unitIndex);   }

  DEFINE_VTBL_GETTER(0x4D6BF0);

  // Object size = 0x6C
};

// =====================================================================================================================
class ESG : public Projectile {
  using $ = ESG;
public:
  OP2_MO_CLASS_MAP_ID(ESG);

  ESG() : Projectile(UseInternalCtorChain) { InternalCtor<0x4809D0>(); }

  Type* GetType()                    const override { return Thunk<0x4815B0, &$::GetType>();                  }
  void  Draw(Viewport* pViewport)          override { return Thunk<0x480D60, &$::Draw>(pViewport);            }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x4809F0, &$::Destroy>(freeMem);           }
  void  DoEvent()                          override { return Thunk<0x481120, &$::DoEvent>();                  }
  void  DrawSecondary(Viewport* pViewport) override { return Thunk<0x480BD0, &$::DrawSecondary>(pViewport);   }
  void  DoDamage()                         override { return Thunk<0x480A20, &$::DoDamage>();                 }
  int   GetNumAnimationFrames()            override { return Thunk<0x480BC0, &$::GetNumAnimationFrames>();    }
  void  SetWeaponSource(int unitIndex)     override { return Thunk<0x480A30, &$::SetWeaponSource>(unitIndex); }

  DEFINE_VTBL_GETTER(0x4D5F60);

  // Object size = 0x70
};

// =====================================================================================================================
class InterColonyShuttle : public Projectile {
  using $ = InterColonyShuttle;
public:
  OP2_MO_CLASS_MAP_ID(InterColonyShuttle);

  using Projectile::Projectile;

  Type* GetType()            const override { return Thunk<0x4474A0, &$::GetType>();               }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x47CB50, &$::Draw>(pViewport);         }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x444F30, &$::Destroy>(freeMem);        }
  void  DoDamage()                 override { return Thunk<0x47CC70, &$::DoDamage>();              }
  int   GetNumAnimationFrames()    override { return Thunk<0x47CE30, &$::GetNumAnimationFrames>(); }

  DEFINE_VTBL_GETTER(0x4D45A8);

  // Object size = 0x6C
};

// =====================================================================================================================
class RailGun : public Projectile {
  using $ = RailGun;
public:
  OP2_MO_CLASS_MAP_ID(RailGun);

  RailGun() : Projectile(UseInternalCtorChain) { InternalCtor<0x4A4B10>(); }

  Type* GetType()            const override { return Thunk<0x4A6910, &$::GetType>();               }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x4A2AD0, &$::Draw>(pViewport);         }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4A4B40, &$::Destroy>(freeMem);        }
  int   GetNumAnimationFrames()    override { return Thunk<0x4A2A90, &$::GetNumAnimationFrames>(); }

  DEFINE_VTBL_GETTER(0x4D6DB8);

  // Object size = 0x6C
};

// =====================================================================================================================
class RPG : public Projectile {
  using $ = RPG;
public:
  OP2_MO_CLASS_MAP_ID(RPG);

  RPG() : Projectile(UseInternalCtorChain) { InternalCtor<0x4A4BC0>(); }

  Type* GetType()            const override { return Thunk<0x4A6920, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4A4BF0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D6E50);

  // Object size = 0x6C
};

// =====================================================================================================================
class Stickyfoam : public Projectile {
  using $ = Stickyfoam;
public:
  OP2_MO_CLASS_MAP_ID(Stickyfoam);

  Stickyfoam() : Projectile(UseInternalCtorChain) { InternalCtor<0x4A5340>(); }

  Type* GetType()              const override { return Thunk<0x4A69D0, &$::GetType>();                    }
  void* Destroy(ibool freeMem = 0)   override { return Thunk<0x4A5370, &$::Destroy>(freeMem);             }
  void  DoDamage()                   override { return Thunk<0x4A4FE0, &$::DoDamage>();                   }
  int   GetSecondaryAnimationIndex() override { return Thunk<0x4A69E0, &$::GetSecondaryAnimationIndex>(); }

  DEFINE_VTBL_GETTER(0x4D71E0);

  // Object size = 0x6C
};

// =====================================================================================================================
class EMPMissile : public Rocket {
  using $ = EMPMissile;
public:
  OP2_MO_CLASS_MAP_ID(EMPMissile);

  using Rocket::Rocket;

  Type* GetType()            const override { return Thunk<0x449A10, &$::GetType>();        }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x4805A0, &$::Draw>(pViewport);  }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x444D70, &$::Destroy>(freeMem); }
  void  DoEvent()                  override { return Thunk<0x480250, &$::DoEvent>();        }
  void  ReEnter()                  override { return Thunk<0x480230, &$::ReEnter>();        }
  void  Land()                     override { return Thunk<0x480540, &$::Land>();           }

  DEFINE_VTBL_GETTER(0x4D4450);

  // Object size = 0x58
};

// =====================================================================================================================
class RLV : public Rocket {
  using $ = RLV;
public:
  OP2_MO_CLASS_MAP_ID(RLV);

  using Rocket::Rocket;

  Type* GetType()            const override { return Thunk<0x4499F0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x444CF0, &$::Destroy>(freeMem); }
  void  ReEnter()                  override { return Thunk<0x480100, &$::ReEnter>();        }
  void  Land()                     override { return Thunk<0x480190, &$::Land>();           }

  DEFINE_VTBL_GETTER(0x4D43A8);

  // Object size = 0x54
};

// =====================================================================================================================
class SULV : public Rocket {
  using $ = SULV;
public:
  OP2_MO_CLASS_MAP_ID(SULV);

  using Rocket::Rocket;

  Type* GetType()            const override { return Thunk<0x449A00, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x444C70, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D4300);

  // Object size = 0x54
};

// =====================================================================================================================
class SmallCapacityAirTransport : public AirUnit {
  using $ = SmallCapacityAirTransport;
public:
  OP2_MO_CLASS_MAP_ID(SmallCapacityAirTransport);

  SmallCapacityAirTransport() : AirUnit() { flags_ |= MoFlagOffensive; }

  Type* GetType()            const override { return Thunk<0x447E10, &$::GetType>();       }
  int   FireWeapon()               override { return Thunk<0x4744E0, &$::FireWeapon>();    }
  void  Draw(Viewport* pViewport)  override { return Thunk<0x474620, &$::Draw>(pViewport); }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x447E20, &$::Destroy>(freeMem);}

  DEFINE_VTBL_GETTER(0x4D0E80);

  // Object size = 0x5C
};

// =====================================================================================================================
class AdvancedResidence : public Building {
  using $ = AdvancedResidence;
public:
  OP2_MO_CLASS_MAP_ID(AdvancedResidence);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x4492F0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x443920, &$::Destroy>(freeMem);            }
  void  DoEvent()                           override { return Thunk<0x483C80, &$::DoEvent>();                   }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449300, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D3238);

  // Object size = 0x60
};

// =====================================================================================================================
class Agridome : public Building {
  using $ = Agridome;
public:
  OP2_MO_CLASS_MAP_ID(Agridome);

  Agridome() : Building(UseInternalCtorChain) { InternalCtor<0x401920>(); }

  Type* GetType()                     const override { return Thunk<0x401CC0, &$::GetType>();                   }
  void  MarkForRedraw(Viewport* pViewport)  override { return Thunk<0x401AC0, &$::MarkForRedraw>(pViewport);    }
  int   MouseOver(int xOffset, int yOffset) override { return Thunk<0x401B60, &$::MouseOver>(xOffset, yOffset); }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x401940, &$::Destroy>(freeMem);            }
  void  DrawSecondary(Viewport* pViewport)  override { return Thunk<0x401970, &$::DrawSecondary>(pViewport);    }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x401CD0, &$::UpdateTotalCapacities>(a, b); }
  int   GetFoodProduction()           const override { return Thunk<0x483AF0, &$::GetFoodProduction>();         }

  DEFINE_VTBL_GETTER(0x4CF0F0);

  // Object size = 0x60
};

// =====================================================================================================================
class CommandCenter : public Building {
  using $ = CommandCenter;
public:
  OP2_MO_CLASS_MAP_ID(CommandCenter);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x448CB0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x4429A0, &$::Destroy>(freeMem);            }
  void  SetOwnerRecalculateValues()         override { return Thunk<0x40C350, &$::SetOwnerRecalculateValues>(); }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x40C230, &$::UpdateTotalCapacities>(a, b); }
  ibool IsEnabled()                   const override { return Thunk<0x40C2A0, &$::IsEnabled>();                 }
  int   GetPowerProduction()          const override { return Thunk<0x40C2F0, &$::GetPowerProduction>();        }

  DEFINE_VTBL_GETTER(0x4D1790);

  // Object size = 0x60
};

// =====================================================================================================================
class CommonOreSmelter : public Building {
  using $ = CommonOreSmelter;
public:
  OP2_MO_CLASS_MAP_ID(CommonOreSmelter);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449630, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x4439C0, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449640, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D3358);

  // Object size = 0x60
};

// =====================================================================================================================
class CommonStorage : public Building {
  using $ = CommonStorage;
public:
  OP2_MO_CLASS_MAP_ID(CommonStorage);

  CommonStorage() : Building(UseInternalCtorChain) { InternalCtor<0x409530>(); }

  Type* GetType()                     const override { return Thunk<0x409B00, &$::GetType>();                   }
  void  MarkForRedraw(Viewport* pViewport)  override { return Thunk<0x4096E0, &$::MarkForRedraw>(pViewport);    }
  int   MouseOver(int xOffset, int yOffset) override { return Thunk<0x409780, &$::MouseOver>(xOffset, yOffset); }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x409550, &$::Destroy>(freeMem);            }
  void  DrawSecondary(Viewport* pViewport)  override { return Thunk<0x409590, &$::DrawSecondary>(pViewport);    }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x409B10, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4CF5F8);

  // Object size = 0x60
};

// =====================================================================================================================
class DIRT : public Building {
  using $ = DIRT;
public:
  OP2_MO_CLASS_MAP_ID(DIRT);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x448250, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x443100, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x412040, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D2410);

  // Object size = 0x60
};

// =====================================================================================================================
class Forum : public Building {
  using $ = Forum;
public:
  OP2_MO_CLASS_MAP_ID(Forum);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x448FF0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442900, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449000, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D1670);

  // Object size = 0x60
};

// =====================================================================================================================
class Garage : public Building {
  using $ = Garage;
public:
  OP2_MO_CLASS_MAP_ID(Garage);

  Garage() : Building(UseInternalCtorChain) { InternalCtor<0x41C650>(); }

  Type* GetType()                     const override { return Thunk<0x41D630, &$::GetType>();                   }
  void  MarkForRedraw(Viewport* pViewport)  override { return Thunk<0x41D280, &$::MarkForRedraw>(pViewport);    }
  int   MouseOver(int xOffset, int yOffset) override { return Thunk<0x41D540, &$::MouseOver>(xOffset, yOffset); }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x41C670, &$::Destroy>(freeMem);            }
  void  OnSave()                            override { return Thunk<0x41CEE0, &$::OnSave>();                    }
  void  OnLoad()                            override { return Thunk<0x41CF40, &$::OnLoad>();                    }
  void  DrawSecondary(Viewport* pViewport)  override { return Thunk<0x41CFB0, &$::DrawSecondary>(pViewport);    }
  void  Explode(ibool explode)              override { return Thunk<0x41C8F0, &$::Explode>(explode);           }
  void  SetEnabled()                        override { return Thunk<0x41CA30, &$::SetEnabled>();                }
  void  ProcessDeath()                      override { return Thunk<0x41D320, &$::ProcessDeath>();              }

  virtual int  GetNumOccupiedBays() const  { return Thunk<0x41CAE0, &$::GetNumOccupiedBays>();    }
  virtual int  DoRepairs()                 { return Thunk<0x41CB50, &$::DoRepairs>();             }
  virtual void LoadUnloadBay(int bayIndex) { return Thunk<0x41C6F0, &$::LoadUnloadBay>(bayIndex); }
  
#define OP2_MO_GARAGE_VTBL($)  $(GetNumOccupiedBays)  $(DoRepairs)  $(LoadUnloadBay)
  DEFINE_VTBL_TYPE(OP2_MO_GARAGE_VTBL, 0x4CFCA0);

public:
  MapObject* pUnitInBay_[6];

  // Object size = 0x78
};

// =====================================================================================================================
class GORF : public Building {
  using $ = GORF;
public:
  OP2_MO_CLASS_MAP_ID(GORF);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449F40, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x443BB0, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449F50, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D36D0);

  // Object size = 0x60
};

// =====================================================================================================================
class GuardPost : public Building {
  using $ = GuardPost;
public:
  OP2_MO_CLASS_MAP_ID(GuardPost);

  GuardPost() : Building(UseInternalCtorChain) { InternalCtor<0x42CEE0>(); }

  Type* GetType()                    const override { return Thunk<0x42D700, &$::GetType>();                }
  int   FireWeapon()                       override { return Thunk<0x42D120, &$::FireWeapon>();             }
  void  SelectTurretGraphic(MapObject* pWeapon, int rotation) override
    { return Thunk<0x42D100, &$::SelectTurretGraphic>(pWeapon, rotation); }
  void  Draw(Viewport*          pViewport) override { return Thunk<0x42D450, &$::Draw>(pViewport);          }
  void  MarkForRedraw(Viewport* pViewport) override { return Thunk<0x42D3A0, &$::MarkForRedraw>(pViewport); }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x42CF10, &$::Destroy>(freeMem);         }
  void  DoEvent()                          override { return Thunk<0x42CF20, &$::DoEvent>();                }
  void  StopProduction()                   override { return Thunk<0x42D320, &$::StopProduction>();         }

  virtual void  Func_63(uint8 a, void* pB)        { return Thunk<0x423F70, &$::Func_63>(a, pB);            }
  virtual ibool IsInRange(int pixelX, int pixelY) { return Thunk<0x42D260, &$::IsInRange>(pixelX, pixelY); }
  
#define OP2_MO_GUARDPOST_VTBL($)  $(Func_63)  $(IsInRange)
  DEFINE_VTBL_TYPE(OP2_MO_GUARDPOST_VTBL, 0x4CFFB0);

  // Object size = 0x60
};

// =====================================================================================================================
class LavaWall : public Building {
  using $ = LavaWall;
public:
  OP2_MO_CLASS_MAP_ID(LavaWall);

  using Building::Building;

  Type* GetType()            const override { return Thunk<0x448030, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x442500, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D1180);

  // Object size = 0x60
};

// =====================================================================================================================
class LightTower : public Building {
  using $ = LightTower;
public:
  OP2_MO_CLASS_MAP_ID(LightTower);

  using Building::Building;

  Type* GetType()            const override { return Thunk<0x448100, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x442760, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D1510);

  // Object size = 0x60
};

// =====================================================================================================================
class MedicalCenter : public Building {
  using $ = MedicalCenter;
public:
  OP2_MO_CLASS_MAP_ID(MedicalCenter);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449BB0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442D60, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449BC0, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D1E50);

  // Object size = 0x60
};

// =====================================================================================================================
class MeteorDefense : public Building {
  using $ = MeteorDefense;
public:
  OP2_MO_CLASS_MAP_ID(MeteorDefense);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x44A230, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x4432C0, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x44A240, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D2680);

  // Object size = 0x60
};

// =====================================================================================================================
class MicrobeWall : public Building {
  using $ = MicrobeWall;
public:
  OP2_MO_CLASS_MAP_ID(MicrobeWall);

  using Building::Building;

  Type* GetType()            const override { return Thunk<0x448050, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4425A0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D12A0);

  // Object size = 0x60
};

// =====================================================================================================================
class Nursery : public Building {
  using $ = Nursery;
public:
  OP2_MO_CLASS_MAP_ID(Nursery);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449CD0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442E00, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449CE0, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D1F70);

  // Object size = 0x60
};

// =====================================================================================================================
class Observatory : public Building {
  using $ = Observatory;
public:
  OP2_MO_CLASS_MAP_ID(Observatory);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449AC0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x4437E0, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449AD0, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D2FF8);

  // Object size = 0x60
};

// =====================================================================================================================
class RareOreSmelter : public Building {
  using $ = RareOreSmelter;
public:
  OP2_MO_CLASS_MAP_ID(RareOreSmelter);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449730, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x443B10, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449740, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D35B0);

  // Object size = 0x60
};

// =====================================================================================================================
class RareStorage : public Building {
  using $ = RareStorage;
public:
  OP2_MO_CLASS_MAP_ID(RareStorage);

  RareStorage() : Building(UseInternalCtorChain) { InternalCtor<0x409560>(); }

  Type* GetType()                     const override { return Thunk<0x409B50, &$::GetType>();                   }
  void  MarkForRedraw(Viewport* pViewport)  override { return Thunk<0x409990, &$::MarkForRedraw>(pViewport);    }
  int   MouseOver(int xOffset, int yOffset) override { return Thunk<0x409A30, &$::MouseOver>(xOffset, yOffset); }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x409580, &$::Destroy>(freeMem);            }
  void  DrawSecondary(Viewport* pViewport)  override { return Thunk<0x409840, &$::DrawSecondary>(pViewport);    }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x409B60, &$::UpdateTotalCapacities>(a, b); }
  
  DEFINE_VTBL_GETTER(0x4CF6F8);

  // Object size = 0x60
};

// =====================================================================================================================
class RecreationFacility : public Building {
  using $ = RecreationFacility;
public:
  OP2_MO_CLASS_MAP_ID(RecreationFacility);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449430, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442F40, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449440, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D21B0);

  // Object size = 0x60
};

// =====================================================================================================================
class ReinforcedResidence : public Building {
  using $ = ReinforcedResidence;
public:
  OP2_MO_CLASS_MAP_ID(ReinforcedResidence);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449200, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x443880, &$::Destroy>(freeMem);            }
  void  DoEvent()                           override { return Thunk<0x483C90, &$::DoEvent>();                   }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449210, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D3118);

  // Object size = 0x60
};

// =====================================================================================================================
class Residence : public Building {
  using $ = Residence;
public:
  OP2_MO_CLASS_MAP_ID(Residence);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449110, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442AE0, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449120, &$::UpdateTotalCapacities>(a, b); }
  
  DEFINE_VTBL_GETTER(0x4D19D0);

  // Object size = 0x60
};

// =====================================================================================================================
class RobotCommand : public Building {
  using $ = RobotCommand;
public:
  OP2_MO_CLASS_MAP_ID(RobotCommand);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x448D90, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442B80, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x40C360, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D1AF0);

  // Object size = 0x60
};

// =====================================================================================================================
class TradeCenter : public Building {
  using $ = TradeCenter;
public:
  OP2_MO_CLASS_MAP_ID(TradeCenter);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x449540, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442C20, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x449550, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D1C10);

  // Object size = 0x60
};

// =====================================================================================================================
class Tube : public Building {
  using $ = Tube;
public:
  OP2_MO_CLASS_MAP_ID(Tube);

  using Building::Building;

  Type* GetType()            const override { return Thunk<0x447FF0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x442340, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D0F20);

  // Object size = 0x60
};

// =====================================================================================================================
class University : public Building {
  using $ = University;
public:
  OP2_MO_CLASS_MAP_ID(University);

  using Building::Building;

  Type* GetType()                     const override { return Thunk<0x448F00, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442FE0, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x448F40, &$::UpdateTotalCapacities>(a, b); }
  void  SetDisabledWorkers()                override { return Thunk<0x49D710, &$::SetDisabledWorkers>();        }
  void  StopProduction()                    override { return Thunk<0x49D6E0, &$::StopProduction>();            }
  int   GetNumWorkersRequired()       const override { return Thunk<0x448F10, &$::GetNumWorkersRequired>();     }
  int   GetNumWorkersRequiredAndEnable()    override
    { return Thunk<0x49D6A0, &$::GetNumWorkersRequiredAndEnable>(); }

  DEFINE_VTBL_GETTER(0x4D22D0);

  // Object size = 0x68
};

// =====================================================================================================================
class Wall : public Building {
  using $ = Wall;
public:
  OP2_MO_CLASS_MAP_ID(Wall);

  using Building::Building;

  Type* GetType()            const override { return Thunk<0x448010, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4423E0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D1040);

  // Object size = 0x60
};

// =====================================================================================================================
class ArachnidFactory : public FactoryBuilding {
  using $ = ArachnidFactory;
public:
  OP2_MO_CLASS_MAP_ID(ArachnidFactory);

  using FactoryBuilding::FactoryBuilding;

  Type* GetType()            const override { return Thunk<0x4470D0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x443410, &$::Destroy>(freeMem); }
  int   Func_69()                  override { return Thunk<0x415C40, &$::Func_69>();        }

  DEFINE_VTBL_GETTER(0x4D28C0);

  // Object size = 0x6E
};

// =====================================================================================================================
class ConsumerFactory : public FactoryBuilding {
  using $ = ConsumerFactory;
public:
  OP2_MO_CLASS_MAP_ID(ConsumerFactory);

  using FactoryBuilding::FactoryBuilding;

  Type* GetType()            const override { return Thunk<0x447170, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4434C0, &$::Destroy>(freeMem); }
  int   Func_69()                  override { return Thunk<0x415C20, &$::Func_69>();        }

  DEFINE_VTBL_GETTER(0x4D2B10);

  // Object size = 0x6E
};

// =====================================================================================================================
class Spaceport : public FactoryBuilding {
  using $ = Spaceport;
public:
  OP2_MO_CLASS_MAP_ID(Spaceport);

  using FactoryBuilding::FactoryBuilding;

  Type* GetType()                     const override { return Thunk<0x449850, &$::GetType>();                 }
  void  Draw(Viewport* pViewport)           override { return Thunk<0x47F830, &$::Draw>(pViewport);           }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x443A70, &$::Destroy>(freeMem);          }
  int   FinishProduction(MapID type, int a) override { return Thunk<0x47F5B0, &$::FinishProduction>(type, a); }
  int   ActivateDock(uint16 a, int b)       override { return Thunk<0x47F420, &$::ActivateDock>(a, b);        }
  int   Func_69()                           override { return Thunk<0x415C60, &$::Func_69>();                 }

  int     GetRocketAnimationIndex(int objectOnPad) { return Thunk<0x47F670, &$::GetRocketAnimationIndex>(objectOnPad); }
  Rocket* Launch(int dstPixelX, int dstPixelY)     { return Thunk<0x424260, &$::Launch>(dstPixelX, dstPixelY);         }

  DEFINE_VTBL_GETTER(0x4D3478);

public:
  MapID objectOnPad_;  ///< Rocket type on launch pad.
  MapID launchCargo_;  ///< Cargo type carried in rocket.

  // Object size = 0x76
};

// =====================================================================================================================
class StructureFactory : public FactoryBuilding {
  using $ = StructureFactory;
public:
  OP2_MO_CLASS_MAP_ID(StructureFactory);

  using FactoryBuilding::FactoryBuilding;

  Type* GetType()            const override { return Thunk<0x446F70, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x443570, &$::Destroy>(freeMem); }
  int   Func_69()                  override { return Thunk<0x415BC0, &$::Func_69>();        }

  DEFINE_VTBL_GETTER(0x4D2C48);

  // Object size = 0x6E
};

// =====================================================================================================================
class VehicleFactory : public FactoryBuilding {
  using $ = VehicleFactory;
public:
  OP2_MO_CLASS_MAP_ID(VehicleFactory);

  using FactoryBuilding::FactoryBuilding;

  Type* GetType()            const override { return Thunk<0x447030, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x443620, &$::Destroy>(freeMem); }
  int   Func_69()                  override { return Thunk<0x415C00, &$::Func_69>();        }

  DEFINE_VTBL_GETTER(0x4D2D80);

  // Object size = 0x6E
};

// =====================================================================================================================
class AdvancedLab : public LabBuilding {
  using $ = AdvancedLab;
public:
  OP2_MO_CLASS_MAP_ID(AdvancedLab);

  AdvancedLab() : LabBuilding(UseInternalCtorChain) { InternalCtor<0x4320E0>(); }

  Type* GetType()            const override { return Thunk<0x4322D0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4320D0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D0260);

  // Object size = 0x65
};

// =====================================================================================================================
class BasicLab : public LabBuilding {
  using $ = BasicLab;
public:
  OP2_MO_CLASS_MAP_ID(BasicLab);

  using LabBuilding::LabBuilding;

  Type* GetType()            const override { return Thunk<0x448470, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x442CC0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D1D30);

  // Object size = 0x65
};

// =====================================================================================================================
class StandardLab : public LabBuilding {
  using $ = StandardLab;
public:
  OP2_MO_CLASS_MAP_ID(StandardLab);

  using LabBuilding::LabBuilding;

  Type* GetType()            const override { return Thunk<0x448560, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4436C0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D2EB8);

  // Object size = 0x65
};

// =====================================================================================================================
class CommonOreMine : public MineBuilding {
  using $ = CommonOreMine;
public:
  OP2_MO_CLASS_MAP_ID(CommonOreMine);

  CommonOreMine() : MineBuilding(UseInternalCtorChain) { InternalCtor<0x44ADF0>(); }

  Type* GetType()                     const override { return Thunk<0x44B4B0, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x44AE20, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x44B4C0, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D48A8);

  // Object size = 0x74
};

// =====================================================================================================================
class MagmaWell : public MineBuilding {
  using $ = MagmaWell;
public:
  OP2_MO_CLASS_MAP_ID(MagmaWell);

  using MineBuilding::MineBuilding;

  Type* GetType()            const override { return Thunk<0x448E30, &$::GetType>();           }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x443220, &$::Destroy>(freeMem);    }
  void  Explode(ibool explode)     override { return Thunk<0x44B270, &$::Explode>(explode);    }
  int   NextTruckLoad()            override { return Thunk<0x44B230, &$::NextTruckLoad>();     }
  int   CalculateOreYield()        override { return Thunk<0x44B250, &$::CalculateOreYield>(); }

  DEFINE_VTBL_GETTER(0x4D2550);

  // Object size = 0x74
};

// =====================================================================================================================
class RareOreMine : public MineBuilding {
  using $ = RareOreMine;
public:
  OP2_MO_CLASS_MAP_ID(RareOreMine);

  using MineBuilding::MineBuilding;

  Type* GetType()                     const override { return Thunk<0x447F90, &$::GetType>();                   }
  void* Destroy(ibool freeMem = 0)          override { return Thunk<0x442640, &$::Destroy>(freeMem);            }
  void  UpdateTotalCapacities(int a, int b) override { return Thunk<0x447FA0, &$::UpdateTotalCapacities>(a, b); }

  DEFINE_VTBL_GETTER(0x4D13C0);

  // Object size = 0x74
};

// =====================================================================================================================
class GeothermalPlant : public PowerBuilding {
  using $ = GeothermalPlant;
public:
  OP2_MO_CLASS_MAP_ID(GeothermalPlant);

  using PowerBuilding::PowerBuilding;

  Type* GetType()            const override { return Thunk<0x449D90, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x443360, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D27A0);

  // Object size = 0x60
};

// =====================================================================================================================
class MHDGenerator : public PowerBuilding {
  using $ = MHDGenerator;
public:
  OP2_MO_CLASS_MAP_ID(MHDGenerator);

  using PowerBuilding::PowerBuilding;

  Type* GetType()            const override { return Thunk<0x44A100, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x442A40, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D18B0);

  // Object size = 0x60
};

// =====================================================================================================================
class SolarPowerArray : public PowerBuilding {
  using $ = SolarPowerArray;
public:
  OP2_MO_CLASS_MAP_ID(SolarPowerArray);

  using PowerBuilding::PowerBuilding;

  Type* GetType()                const override { return Thunk<0x44A060, &$::GetType>();                }
  void* Destroy(ibool freeMem = 0)     override { return Thunk<0x442EA0, &$::Destroy>(freeMem);         }
  ibool IsEnabled()              const override { return Thunk<0x46E990, &$::IsEnabled>();              }
  ibool IsActivePowerGenerator() const override { return Thunk<0x46E900, &$::IsActivePowerGenerator>(); }

  DEFINE_VTBL_GETTER(0x4D2090);

  // Object size = 0x60
};

// =====================================================================================================================
class Tokamak : public PowerBuilding {
  using $ = Tokamak;
public:
  OP2_MO_CLASS_MAP_ID(Tokamak);

  using PowerBuilding::PowerBuilding;

  Type* GetType()            const override { return Thunk<0x449E60, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x443C50, &$::Destroy>(freeMem); }
  void  DoEvent()                  override { return Thunk<0x46E880, &$::DoEvent>();        }

  DEFINE_VTBL_GETTER(0x4D37F0);

  // Object size = 0x60
};

// =====================================================================================================================
class CargoTruck : public Vehicle {
  using $ = CargoTruck;
public:
  OP2_MO_CLASS_MAP_ID(CargoTruck);

  CargoTruck() : Vehicle(UseInternalCtorChain) { InternalCtor<0x405FD0>(); }

  Type* GetType()                         const override { return Thunk<0x406E10, &$::GetType>();                     }
  int   GetMouseOverStr(char* pDst, int bufLen) override { return Thunk<0x43A690, &$::GetMouseOverStr>(pDst, bufLen); }
  void* Destroy(ibool freeMem = 0)              override { return Thunk<0x405FF0, &$::Destroy>(freeMem);              }
  int   GetChassisAnimationIndex(int rotation)  override
    { return Thunk<0x4063B0, &$::GetChassisAnimationIndex>(rotation); }
  void  DoDock()                                override { return Thunk<0x406170, &$::DoDock>();                      }
  ibool CanProduceAt(int tileX, int tileY)      override { return Thunk<0x40A4B0, &$::CanProduceAt>(tileX, tileY);    }

  virtual void SetCargoToLoad(CargoType cargoType, int amount, ibool a)
    { return Thunk<0x406630, &$::SetCargoToLoad>(cargoType, amount, a); }
  virtual void TransferCargo() { return Thunk<0x406710, &$::TransferCargo>(); }
  
#define OP2_MO_CARGOTRUCK_VTBL($)  $(SetCargoToLoad)  $(TransferCargo)
  DEFINE_VTBL_TYPE(OP2_MO_CARGOTRUCK_VTBL, 0x4CF4A8);

  // Object size = 0x6E
};

// =====================================================================================================================
class ConVec : public Vehicle {
  using $ = ConVec;
public:
  OP2_MO_CLASS_MAP_ID(ConVec);

  ConVec() : Vehicle(UseInternalCtorChain) { InternalCtor<0x410EB0>(); }

  Type* GetType()                        const override { return Thunk<0x411240, &$::GetType>();                }
  void  MarkForRedraw(Viewport* pViewport)     override { return Thunk<0x410F40, &$::MarkForRedraw>(pViewport); }
  void* Destroy(ibool freeMem = 0)             override { return Thunk<0x410ED0, &$::Destroy>(freeMem);         }
  int   GetChassisAnimationIndex(int rotation) override
    { return Thunk<0x410EF0, &$::GetChassisAnimationIndex>(rotation); }
  void  DoBuild(int numWaypoints, const Waypoint* pWaypoints, int a) override
    { return Thunk<0x423430, &$::DoBuild>(numWaypoints, pWaypoints, a); }

  DEFINE_VTBL_GETTER(0x4CF990);

  // Object size = 0x6C
};

// =====================================================================================================================
class Earthworker : public Vehicle {
  using $ = Earthworker;
public:
  OP2_MO_CLASS_MAP_ID(Earthworker);

  using Vehicle::Vehicle;

  Type* GetType()                    const override { return Thunk<0x448860, &$::GetType>();                  }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x442230, &$::Destroy>(freeMem);           }
  void  DoBuild(int numWaypoints, const Waypoint* pWaypoints, int a) override
    { return Thunk<0x446E90, &$::DoBuild>(numWaypoints, pWaypoints, a); }
  void  UnblockBuildArea()                 override { return Thunk<0x4236B0, &$::UnblockBuildArea>();         }
  ibool CanProduceAt(int tileX, int tileY) override { return Thunk<0x40A560, &$::CanProduceAt>(tileX, tileY); }

  virtual void DoBuildWall(int numWaypoints, const Waypoint* pWaypoints, int a, uint8 command)
    { return Thunk<0x423620, &$::DoBuildWall>(numWaypoints, pWaypoints, a, command); }

#define OP2_MO_EARTHWORKER_VTBL($)  $(DoBuildWall)
  DEFINE_VTBL_TYPE(OP2_MO_EARTHWORKER_VTBL, 0x4D0DC8);

  void FinishBuildingWall() { return Thunk<0x423700, &$::FinishBuildingWall>(); }

  // Object size = 0x71
};

// =====================================================================================================================
class EvacuationTransport : public Vehicle {
  using $ = EvacuationTransport;
public:
  OP2_MO_CLASS_MAP_ID(EvacuationTransport);

  using Vehicle::Vehicle;

  Type* GetType()            const override { return Thunk<0x448BF0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x442130, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D0C58);

  // Object size = 0x62
};

// =====================================================================================================================
class GeoCon : public Vehicle {
  using $ = GeoCon;
public:
  OP2_MO_CLASS_MAP_ID(GeoCon);

  using Vehicle::Vehicle;

  Type* GetType()            const override { return Thunk<0x447DC0, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x441FE0, &$::Destroy>(freeMem); }
  void  DoBuild(int numWaypoints, const Waypoint* pWaypoints, int a) override
    { return Thunk<0x423550, &$::DoBuild>(numWaypoints, pWaypoints, a); }

  DEFINE_VTBL_GETTER(0x4D0AC8);

  // Object size = 0x6C
};

// =====================================================================================================================
class RepairVehicle : public Vehicle {
  using $ = RepairVehicle;
public:
  OP2_MO_CLASS_MAP_ID(RepairVehicle);

  using Vehicle::Vehicle;

  Type* GetType()            const override { return Thunk<0x448A60, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4421B0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D0D10);

  // Object size = 0x62
};

// =====================================================================================================================
class RoboDozer : public Vehicle {
  using $ = RoboDozer;
public:
  OP2_MO_CLASS_MAP_ID(RoboDozer);

  RoboDozer() : Vehicle(UseInternalCtorChain) { InternalCtor<0x409DF0>(); }

  Type* GetType()                    const override { return Thunk<0x40A5A0, &$::GetType>();                  }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x409E10, &$::Destroy>(freeMem);           }
  ibool CanProduceAt(int tileX, int tileY) override { return Thunk<0x40A3F0, &$::CanProduceAt>(tileX, tileY); }

  DEFINE_VTBL_GETTER(0x4CF7F8);

  // Object size = 0x6B
};

// =====================================================================================================================
class RoboMiner : public Vehicle {
  using $ = RoboMiner;
public:
  OP2_MO_CLASS_MAP_ID(RoboMiner);

  using Vehicle::Vehicle;

  Type* GetType()            const override { return Thunk<0x447C50, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x441F60, &$::Destroy>(freeMem); }
  void  DoBuild(int numWaypoints, const Waypoint* pWaypoints, int a) override
    { return Thunk<0x4234C0, &$::DoBuild>(numWaypoints, pWaypoints, a); }

  DEFINE_VTBL_GETTER(0x4D0A10);

  // Object size = 0x62
};

// =====================================================================================================================
class RoboSurveyor : public Vehicle {
  using $ = RoboSurveyor;
public:
  OP2_MO_CLASS_MAP_ID(RoboSurveyor);

  RoboSurveyor() : Vehicle(UseInternalCtorChain) { InternalCtor<0x4843D0>(); }

  Type* GetType()            const override { return Thunk<0x484490, &$::GetType>();        }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4843F0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D61A0);

  // Object size = 0x62
};

// =====================================================================================================================
class Scout : public Vehicle {
  using $ = Scout;
public:
  OP2_MO_CLASS_MAP_ID(Scout);

  using Vehicle::Vehicle;

  Type* GetType()                    const override { return Thunk<0x447DE0, &$::GetType>();                }
  void  MarkForRedraw(Viewport* pViewport) override { return Thunk<0x474BC0, &$::MarkForRedraw>(pViewport); }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x447DF0, &$::Destroy>(freeMem);         }
  void  DoEvent()                          override { return Thunk<0x474DB0, &$::DoEvent>();                }

  DEFINE_VTBL_GETTER(0x4D0B80);

  // Object size = 0x64
};

// =====================================================================================================================
class Spider : public Vehicle {
  using $ = Spider;
public:
  OP2_MO_CLASS_MAP_ID(Spider);

  Spider() : Vehicle() { flags_ |= MoFlagVecArachnid; }

  Type* GetType()                    const override { return Thunk<0x4478F0, &$::GetType>();                }
  void  MarkForRedraw(Viewport* pViewport) override { return Thunk<0x4817F0, &$::MarkForRedraw>(pViewport); }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x447900, &$::Destroy>(freeMem);         }
  void  DoEvent()                          override { return Thunk<0x4816D0, &$::DoEvent>();                }

  DEFINE_VTBL_GETTER(0x4D0710);

  // Object size = 0x62
};

// =====================================================================================================================
class Scorpion : public TankVehicle {
  using $ = Scorpion;
public:
  OP2_MO_CLASS_MAP_ID(Scorpion);

  Scorpion() : TankVehicle() { flags_ |= MoFlagVecArachnid; }

  Type* GetType()                    const override { return Thunk<0x447A80, &$::GetType>();                }
  void  SelectTurretGraphic(MapObject* pWeapon, int rotation) override
    { return Thunk<0x481800, &$::SelectTurretGraphic>(pWeapon, rotation); }
  void  Draw(Viewport*          pViewport) override { return Thunk<0x481820, &$::Draw>(pViewport);          }
  void  MarkForRedraw(Viewport* pViewport) override { return Thunk<0x481830, &$::MarkForRedraw>(pViewport); }
  void* Destroy(ibool freeMem = 0)         override { return Thunk<0x447A90, &$::Destroy>(freeMem);         }

  DEFINE_VTBL_GETTER(0x4D07C8);

  // Object size = 0x62
};

// =====================================================================================================================
class Lynx : public TankVehicle {
  using $ = Lynx;
public:
  OP2_MO_CLASS_MAP_ID(Lynx);

  using TankVehicle::TankVehicle;

  Type* GetType()            const override { return Thunk<0x4476E0, &$::GetType>(); }
  void  SelectTurretGraphic(MapObject* pWeapon, int rotation) override
    { return Thunk<0x4855C0, &$::SelectTurretGraphic>(pWeapon, rotation); }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x441DF0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D0880);

  // Object size = 0x62
};

// =====================================================================================================================
class Panther : public TankVehicle {
  using $ = Panther;
public:
  OP2_MO_CLASS_MAP_ID(Panther);

  using TankVehicle::TankVehicle;

  Type* GetType()            const override { return Thunk<0x4476C0, &$::GetType>(); }
  void  SelectTurretGraphic(MapObject* pWeapon, int rotation) override
    { return Thunk<0x4857A0, &$::SelectTurretGraphic>(pWeapon, rotation); }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x441E80, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D0938);

  // Object size = 0x62
};

// =====================================================================================================================
class Tiger : public TankVehicle {
  using $ = Tiger;
public:
  OP2_MO_CLASS_MAP_ID(Tiger);

  Tiger() : TankVehicle() { flags_ |= MoFlagDoubleFireRate; }

  Type* GetType()            const override { return Thunk<0x447690, &$::GetType>(); }
  void  SelectTurretGraphic(MapObject* pWeapon, int rotation) override
    { return Thunk<0x485950, &$::SelectTurretGraphic>(pWeapon, rotation); }
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4476A0, &$::Destroy>(freeMem); }

  DEFINE_VTBL_GETTER(0x4D05C0);

  // Object size = 0x62
};

END_PACKED

} // MapObj

namespace TethysImpl {
#define OP2_MO_FOR_DEF(name)  template <> struct MapObjForImpl<MapID::name> { using Type = MapObj::name; };
OP2_EMIT_MO_MAPPINGS(OP2_MO_FOR_DEF);
} // Impl

//  ====================================================================================================================
/// Union of all the MapObject subclasses together.
union AnyMapObj {
  ///@{ Overload conversion and reference operators to make this class act as if it's MapObject.
  template <typename T, typename = std::enable_if_t<std::is_base_of_v<MapObject, T>>>
  explicit operator T&() { return static_cast<T&>(object_); }
  operator  MapObject&() { return  object_; }
  MapObject* operator&() { return &object_; }
  ///@}

  static AnyMapObj* GetInstance(int index) { return reinterpret_cast<AnyMapObj*>(MapObject::GetInstance(index)); }

  template <MapID ID>        auto& As()       { return       static_cast<MapObjFor<ID>&>(object_); }
  template <MapID ID>  const auto& As() const { return static_cast<const MapObjFor<ID>&>(object_); }

  MapID GetTypeID() const { return object_.GetType()->type_; }

  MapObject       object_;

  MapEntity       entity_;
  Disaster        disaster_;
  MapChildEntity  childEntity_;
  Rocket          rocket_;
  Explosive       explosive_;
  Projectile      projectile_;

  MapUnit         unit_;
  AirUnit         airUnit_;
  LandUnit        landUnit_;

  Building        building_;
  FactoryBuilding factoryBuilding_;
  LabBuilding     labBuilding_;
  MineBuilding    mineBuilding_;
  PowerBuilding   powerBuilding_;

  Vehicle         vehicle_;
  TankVehicle     tankVehicle_;

#define OP2_MO_ANY_MAP_OBJ_DEF(name)  MapObj::name  map##name##_;
  OP2_EMIT_MO_MAPPINGS(OP2_MO_ANY_MAP_OBJ_DEF);

  uint8           raw_[MapObjectSize];  // Forces size to MapObjectSize
};
static_assert(sizeof(AnyMapObj) == MapObjectSize, "Incorrect MapObject size.");


// =====================================================================================================================
inline MapObject* MapObject::GetInstance(
  int index)
{
  auto*const pMap = MapImpl::GetInstance();
  return ((index >= 0) && (static_cast<size_t>(index) < pMap->MaxNumUnits())) ? &pMap->pMapObjArray_[index] : nullptr;
}

} // Tethys
