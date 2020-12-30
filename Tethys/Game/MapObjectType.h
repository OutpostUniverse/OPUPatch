
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Location.h"
#include "Tethys/Game/MapObjectManager.h"

namespace Tethys {

class MapObject;
class StreamIO;
enum class SoundID : int;

namespace MapIDImpl {
/// Map object type IDs, including structures, vehicles, disasters, beacons, weapons fire, etc.
/// @note Can be either int, uint8, or uint16 depending on context.
enum MapID : int {
  Any                       = (~0),  ///< Use to specify 'all' or 'any'
  None                      = 0x00,  ///< Use to specify 'none'.  @note Create maps to MaxObjectType.

  CargoTruck                = 0x01,  ///< 'BIG_TRUCK' in sheets.
  ConVec                    = 0x02,  ///< 'CON_TRUCK'
  Spider                    = 0x03,  ///< 'SPIDER'
  Scorpion                  = 0x04,  ///< 'SCORPION'
  Lynx                      = 0x05,  ///< 'LIGHT_TANK'
  Panther                   = 0x06,  ///< 'MED_TANK'
  Tiger                     = 0x07,  ///< 'BIG_TANK'
  RoboSurveyor              = 0x08,  ///< 'SURVEYOR'
  RoboMiner                 = 0x09,  ///< 'MINER'
  GeoCon                    = 0x0A,  ///< 'GEO_METRO'
  Scout                     = 0x0B,  ///< 'SCOUT'
  RoboDozer                 = 0x0C,  ///< 'BULL_DOZER'
  EvacuationTransport       = 0x0D,  ///< 'MOBILE_HOME'
  RepairVehicle             = 0x0E,  ///< 'REPAIR_TRUCK'
  Earthworker               = 0x0F,  ///< 'WALL_TRUCK'
  SmallCapacityAirTransport = 0x10,  ///< 'SCAT';  Mostly unimplemented and tends to crash.

  Tube                      = 0x11,  ///< 'TUBE';  It's not a 'BIG_TRUCK'.
  Wall                      = 0x12,  ///< 'WALL'
  LavaWall                  = 0x13,  ///< 'WALL_LAVA'
  MicrobeWall               = 0x14,  ///< 'WALL_MICROBE'

  CommonOreMine             = 0x15,  ///< 'MINE'
  RareOreMine               = 0x16,  ///< 'MINE_ADV'
  GuardPost                 = 0x17,  ///< 'TOWER_GUARD'
  LightTower                = 0x18,  ///< 'TOWER_LIGHT'
  CommonStorage             = 0x19,  ///< 'STORAGE_ORE'
  RareStorage               = 0x1A,  ///< 'STORAGE_RARE'
  Forum                     = 0x1B,  ///< 'FORUM'
  CommandCenter             = 0x1C,  ///< 'COMMAND'
  MHDGenerator              = 0x1D,  ///< 'ASE'
  Residence                 = 0x1E,  ///< 'RESIDENCE'
  RobotCommand              = 0x1F,  ///< 'ROBOT_COMMAND'
  TradeCenter               = 0x20,  ///< 'TRADE'
  BasicLab                  = 0x21,  ///< 'LAB'
  MedicalCenter             = 0x22,  ///< 'MED_CENTER'
  Nursery                   = 0x23,  ///< 'NURSERY'
  SolarPowerArray           = 0x24,  ///< 'SOLAR'
  RecreationFacility        = 0x25,  ///< 'RECREATION'
  University                = 0x26,  ///< 'UNIVERSITY'
  Agridome                  = 0x27,  ///< 'AGRIDOME'
  DIRT                      = 0x28,  ///< 'DIRT'
  Garage                    = 0x29,  ///< 'GARAGE'
  MagmaWell                 = 0x2A,  ///< 'MAGMA_WELL'
  MeteorDefense             = 0x2B,  ///< 'METEOR_DEF'
  GeothermalPlant           = 0x2C,  ///< 'GEOTHERMAL'
  ArachnidFactory           = 0x2D,  ///< 'FACT_ANDROID'
  ConsumerFactory           = 0x2E,  ///< 'FACT_LUXURY'
  StructureFactory          = 0x2F,  ///< 'FACT_STRUCTURE'
  VehicleFactory            = 0x30,  ///< 'FACT_VEHICLE'
  StandardLab               = 0x31,  ///< 'LAB_STANDARD'
  AdvancedLab               = 0x32,  ///< 'LAB_ADV'
  Observatory               = 0x33,  ///< 'OBSERVATORY'
  ReinforcedResidence       = 0x34,  ///< 'RESIDENCE_P'
  AdvancedResidence         = 0x35,  ///< 'RESIDENCE_E'
  CommonOreSmelter          = 0x36,  ///< 'SMELTER'
  Spaceport                 = 0x37,  ///< 'SPACEPORT'
  RareOreSmelter            = 0x38,  ///< 'SMELTER_ADV'
  GORF                      = 0x39,  ///< 'GORF'
  Tokamak                   = 0x3A,  ///< 'TOKAMAK'

  AcidCloud                 = 0x3B,  ///< 'ACID'
  EMP                       = 0x3C,  ///< 'EMP'
  Laser                     = 0x3D,  ///< 'LASER'
  Microwave                 = 0x3E,  ///< 'MICROWAVE'
  RailGun                   = 0x3F,  ///< 'RAIL_GUN'
  RPG                       = 0x40,  ///< 'CANNON'
  Starflare                 = 0x41,  ///< 'SELF_DESTRUCT';        For vehicles.
  Supernova                 = 0x42,  ///< 'SELF_DESTRUCT_ADV';    For vehicles.  @note Create maps to SupernovaGP?
  StarflareGP               = 0x43,  ///< 'TURRET_DESTRUCT';      For guard posts.
  SupernovaGP               = 0x44,  ///< 'TURRET_DESTRUCT_ADV';  For guard posts.
  SelfDestruct              = 0x45,  ///< 'SCUTTLE_DESTRUCT';     Vehicle self-destruct command explosion.
  ESG                       = 0x46,  ///< 'SPAM'
  Stickyfoam                = 0x47,  ///< 'FOAM'
  ThorsHammer               = 0x48,  ///< 'THORS_HAMMER'
  EnergyCannon              = 0x49,  ///< 'SCORPION_WEAPON'
  EMPBlast                  = 0x4A,  ///< 'EMP_BIG';  EMP missile/power plant explosion.
  BFG                       = 0x4B,  ///< 'BFG';      Unimplemented projectile weapon.

  Lightning                 = 0x4C,  ///< 'LIGHTNING'
  Vortex                    = 0x4D,  ///< 'TORNADO'
  Earthquake                = 0x4E,  ///< 'EARTHQUAKE'
  Eruption                  = 0x4F,  ///< 'ERUPTION'
  Meteor                    = 0x50,  ///< 'METEOR'

  MiningBeacon              = 0x51,  ///< 'BEACON'
  MagmaVent                 = 0x52,  ///< 'MAGMA_VENT'
  Fumarole                  = 0x53,  ///< 'FUMAROLE'
  Wreckage                  = 0x54,  ///< 'WRECK_FLAG'

  SmallBuildingExplosion    = 0x55,  ///< 'BLD_EXPL_SML'
  MediumBuildingExplosion   = 0x56,  ///< 'BLD_EXPL_MED'
  LargeBuildingExplosion    = 0x57,  ///< 'BLD_EXPL_BIG'

  // Satellites do not have MapObject instances.
  EDWARDSatellite           = 0x58,  ///< 'EDWARD'
  SolarSatellite            = 0x59,  ///< 'SOLAR_SAT'
  IonDriveModule            = 0x5A,  ///< 'SPACE_1'
  FusionDriveModule         = 0x5B,  ///< 'SPACE_2'
  CommandModule             = 0x5C,  ///< 'SPACE_3'
  FuelingSystems            = 0x5D,  ///< 'SPACE_4'
  HabitatRing               = 0x5E,  ///< 'SPACE_5'
  SensorPackage             = 0x5F,  ///< 'SPACE_6'
  Skydock                   = 0x60,  ///< 'SPACE_7'
  StasisSystems             = 0x61,  ///< 'SPACE_8'
  OrbitalPackage            = 0x62,  ///< 'SPACE_9'
  PhoenixModule             = 0x63,  ///< 'SPACE_10'
  RareMetalsCargo           = 0x64,  ///< 'SPACE_11'
  CommonMetalsCargo         = 0x65,  ///< 'SPACE_12'
  FoodCargo                 = 0x66,  ///< 'SPACE_13'
  EvacuationModule          = 0x67,  ///< 'SPACE_14'
  ChildrenModule            = 0x68,  ///< 'SPACE_15'

  SULV                      = 0x69,  ///< 'SULV'
  RLV                       = 0x6A,  ///< 'RLV'
  EMPMissile                = 0x6B,  ///< 'EMP_MISSILE'

  // Consumer goods do not have MapObject instances.
  ImpulseItems              = 0x6C,  ///< 'LUX_1'
  Wares                     = 0x6D,  ///< 'LUX_2'
  LuxuryWares               = 0x6E,  ///< 'LUX_3'

  InterColonyShuttle        = 0x6F,  ///< 'SHUTTLE';  Unimplemented "projectile", probably meant to go with SCAT?

  // Arachnid 3-packs do not have MapObject instances.
  Spider3Pack               = 0x70,  ///< 'SPIDER_PACK'
  Scorpion3Pack             = 0x71,  ///< 'SCORPION_PACK'

  AmbientAnimation          = 0x72,  ///< 'AMBIENT_ANIM';  Also known as "Pretty art!"

  MaxObject                 = 0x73,  ///< 'MAX_OBJECT_TYPE';  Also happens to be the base type for lab buildings.
};
} // MapIDImpl

                      using MapID = Tethys::MapIDImpl::MapID;
namespace TethysAPI { using MapID = Tethys::MapIDImpl::MapID; }

/// Vehicle track types.
enum class TrackType : uint32 {
  Legged = 0,  ///< 'L' in sheets.
  Wheeled,     ///< 'W'
  Miner,       ///< 'M' (Robo-Miner/GeoCon);  allows movement on magma vents and fumaroles.
  Tracked,     ///< 'T'
  Hover,       ///< 'H' (SCAT?);  only allows movement on bulldozed terrain.
  Count
};

/// Armor class types.
enum class ArmorType : uint32 {
  None        = 256,  ///< Class 0 in sheets.
  VeryLight   = 180,  ///< Class 1
  Light       = 130,  ///< Class 2
  MediumLight =  90,  ///< Class 3
  Medium      =  60,  ///< Class 4
  Heavy       =  40,  ///< Class 5
};

/// Flags to specify what faction owns what MapObjectType.
enum OwnerFlags : uint32 {
  OwnerGaia     = 0,                            ///< 'G' in sheets (beacons, disasters, etc.)
  OwnerPlymouth = (1u << 11),                   ///< 'P'
  OwnerEden     = (1u << 12),                   ///< 'E'
  OwnerBoth     = (OwnerPlymouth | OwnerEden),  ///< 'B'
};

/// Flags that apply to vehicle MapObjectTypes.
enum VehicleFlags : uint16 {
  VecFlagVehicleFactory  = (1u << 0),  ///< May be produced at VF.
  VecFlagArachnidFactory = (1u << 1),  ///< May be produced at AF.
  VecFlagHasWeapon       = (1u << 2),  ///< Can have a weapon turret.
};

/// Flags that apply to building MapObjectTypes.
enum BuildingFlags : uint32 {
  BldFlagHasTubes          = (1u << 0),  ///< "Border" in sheets.
  BldFlagStructureFactory  = (1u << 1),  ///< May be produced at SF.
  BldFlagDockingAll        = (1u << 2),
  BldFlagDockingTruck      = (1u << 3),  ///< Allows Cargo Truck to dock.
  BldFlagDockingConVec     = (1u << 4),  ///< Allows ConVec to dock.
  BldFlagDockingEvac       = (1u << 5),  ///< Allows Evacuation Transport to dock.
  BldFlagCanBeAutoTargeted = (1u << 6),  ///< Can be automatically targeted by combat units.
  BldFlagExplodeEMPBlast   = (1u << 7),  ///< Creates an EMPBlast on death.
  BldFlagExplodeMedium     = (1u << 8),  ///< Creates a MediumBuildingExplosion on death.
  BldFlagExplodeLarge      = (3u << 7),  ///< Creates a LargeBuildingExplosion on death.
};

namespace TethysImpl { template <MapID>  struct MapObjTypeForImpl { using Type = MapObjectType; }; }

/// Template alias to get the MapObjectType subclass associated with the given MapID.
template <MapID ID>  using MapObjTypeFor = typename TethysImpl::MapObjTypeForImpl<ID>::Type;

BEGIN_PACKED

struct PerPlayerUnitStats {
  int       hp;                ///< 'Hit_Points'
  int       repairAmount;
  ArmorType armor;             ///< 'Armor'
  int       commonCost;        ///< 'Common_Required'
  int       rareCost;          ///< 'Rare_Required'
  int       buildTime;         ///< 'Build_Time' or 'Build_Points'
  int       sightRange;        ///< 'Sight_Range'
  uint8     numUnitsOfType;
  uint8     bImproved;         ///< 'Improved'
  uint16    field_1E;
  int       field_20;

  union {
    struct {
      int powerRequired;       ///< 'Power_Required'
      int workersRequired;     ///< 'Workers_Required'
      int scientistsRequired;  ///< 'Production_Rate' or 'Penetration_Damage' (aliased)
      int storageCapacity;     ///< 'Storage_Capacity'
      int productionCapacity;  ///< 'Production_Capacity'
      int storageBays;         ///< 'Storage_Bays'
    } building;

    struct {
      int moveSpeed;           ///< 'Move_Speed'
      int turnRate;            ///< 'Turn_Rate'
      int productionRate;      ///< 'Production_Rate'
      int reloadTime;          ///< 'Rate_Of_Fire'
      int field_38;            
      int cargoCapacity;       ///< 'Storage_Bays' (aliased)
    } vehicle;

    struct {
      int moveSpeed;           ///< 'Move_Speed'
      int concussionDamage;    ///< 'Concussion_Damage'
      int penetrationDamage;   ///< 'Penetration_Damage'
      int reloadTime;          ///< 'Rate_Of_Fire'
      int weaponSightRange;    ///< 'Production_Capacity' (aliased)
      int field_3C;
    } weapon;
  };

  int completedUpgradeTechIDs[2];  // ** TODO int[2] or uint16[4]?
};
static_assert(sizeof(PerPlayerUnitStats) == 68, "Incorrect PerPlayerUnitStats size.");

union GlobalUnitStats {
  struct {
    uint16 width;
    uint16 height;
    uint32 flags;

    int edenAnimationIndex[10];
    int plyAnimationIndex[10];

    uint8 resPriority;
    uint8 rareRubble;
    uint8 unk4;
    uint8 commonRubble;

    uint8 edenDockLocation;
    uint8 plyDockLocation;
  } building;

  struct {
    uint16 flags;
  } vehicle;

  struct {
    uint16 radius;
    uint16 field_246;
    uint32 pixelsSkipped;
  } weapon;
};

#define OP2_MO_TYPE_CLASS_MAP_ID(id)              \
  static constexpr MapID ClassMapId = MapID::id;  \
  static $* GetInstance() { return static_cast<$*>(MapObjectType::GetInstance(MapID::id)); }


//  ====================================================================================================================
/// Base class for all unit and entity types.  This class serves as a factory for MapObjects, and for storing unit stats
/// for each unit type.
class MapObjectType : public OP2Class<MapObjectType> {
public:
  MapObjectType() { InternalCtor<0x444FC0>(); }

  virtual MapObject* Create(int pixelX, int pixelY, int unitIndex) = 0;

  virtual const char* GetCodeName() { return Thunk<0x446E20, &$::GetCodeName>(); }

  virtual ibool Save(StreamIO* pSavedGame) { return Thunk<0x444FE0, &$::Save>(pSavedGame); }
  virtual ibool Load(StreamIO* pSavedGame) { return Thunk<0x445030, &$::Load>(pSavedGame); }

  virtual ibool Init() { return Thunk<0x446E30, &$::Init>(); }

  virtual void       Process()    { return Thunk<0x4A7F30, &$::Process>();    }
  virtual MapObject* FindTarget() { return Thunk<0x4A7F20, &$::FindTarget>(); }

#define OP2_MO_MAPOBJECTTYPE_VTBL($)  $(Create)  $(GetCodeName)  $(Save)  $(Load)  $(Init)  $(Process)  $(FindTarget)
  DEFINE_VTBL_TYPE(OP2_MO_MAPOBJECTTYPE_VTBL, 0x4D46F0);

  static MapObject* STDCALL AllocateMapObj(size_t sizeInBytes, int pixelX, int pixelY, int forceIndex = 0)
    { return OP2Thunk<0x439A10, &$::AllocateMapObj>(sizeInBytes, pixelX, pixelY, forceIndex); }

  /// Helper function to get the MapObjectType instance for the given MapID.
  static MapObjectType* GetInstance(int mapID) {
    return
      (size_t(mapID) <= MapObjectManager::NumMapObjTypes()) ? MapObjectManager::GetMapObjTypeTable()[mapID] : nullptr;
  }

  /// Helper function to get a building type's rect centered at the given location.
  /// @note If (x2,y2) < (x1,y1), that means one of the x-coordinates has wrapped around the map.
  MapRect GetTileRect(Location center, bool includeBorder = false) const {
    const int  border  = includeBorder ? 2 : 0;
    const auto size    = Location(stats_.building.width + border, stats_.building.height + border);
    const auto topLeft = Location(center.x - (size.x / 2), center.y - (size.y / 2)).Clip();
    return MapRect(topLeft, topLeft + size - 1).Clip();
  }

public:
  MapID              type_;
  PerPlayerUnitStats playerStats_[7];
  int                requiredTechID_;
  TrackType          trackType_;
  int                field_1EC;
  SoundID            selectSoundID_;
  uint32             ownerFlags_;
  char               unitName_[40];
  char               produceName_[40];
  GlobalUnitStats    stats_;
};

//  ====================================================================================================================
/// Base class for most Gaia unit types, such as weapons fire, mining beacons, and disasters.
class MapEntityType : public MapObjectType {
  using $ = MapEntityType;
public:
  using MapObjectType::MapObjectType;

  void       Process()    override { return Thunk<0x4A7BC0, &$::Process>();    }
  MapObject* FindTarget() override { return Thunk<0x4A74D0, &$::FindTarget>(); }

  virtual void SelectGuardPostGraphic(MapObject* pChassis, int rotation)
    { return Thunk<0x447180, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  virtual void SelectTigerGraphic(MapObject* pChassis, int rotation)
    { return Thunk<0x447190, &$::SelectTigerGraphic>(pChassis, rotation); }

  virtual void SelectPantherGraphic(MapObject* pChassis, int rotation)
    { return Thunk<0x4471A0, &$::SelectPantherGraphic>(pChassis, rotation); }

  virtual void SelectLynxGraphic(MapObject* pChassis, int rotation)
    { return Thunk<0x4471B0, &$::SelectLynxGraphic>(pChassis, rotation); }

  virtual int  GetMovementAimOffset(int pixelDistance)
    { return Thunk<0x4471C0, &$::GetMovementAimOffset>(pixelDistance); }

  /// Helper function to get the MapEntityType instance for the given MapID.
  static MapEntityType* GetInstance(int mapID){ return static_cast<MapEntityType*>(MapObjectType::GetInstance(mapID)); }

#define OP2_MO_MAPENTITYTYPE_VTBL($)                                                               \
  $(SelectGuardPostGraphic)  $(SelectTigerGraphic)  $(SelectPantherGraphic)  $(SelectLynxGraphic)  \
  $(GetMovementAimOffset)
  DEFINE_VTBL_TYPE(OP2_MO_MAPENTITYTYPE_VTBL);
};


namespace MapObjType {

// =====================================================================================================================
class AdvancedLab : public MapObjectType {
  using $ = AdvancedLab;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(AdvancedLab);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443720, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448570, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448580, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2FB8);
};

// =====================================================================================================================
class AdvancedResidence : public MapObjectType {
  using $ = AdvancedResidence;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(AdvancedResidence);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4438E0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449250, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449260, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3218);
};

// =====================================================================================================================
class Agridome : public MapObjectType {
  using $ = Agridome;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Agridome);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443040, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4475B0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4475C0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D23D0);
};

// =====================================================================================================================
class ArachnidFactory : public MapObjectType {
  using $ = ArachnidFactory;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ArachnidFactory);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4433C0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447040, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447050, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D28A0);
};

// =====================================================================================================================
class BasicLab : public MapObjectType {
  using $ = BasicLab;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(BasicLab);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442C80, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4483C0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4483D0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1D10);
};

// =====================================================================================================================
class CargoTruck : public MapObjectType {
  using $ = CargoTruck;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CargoTruck);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441B40, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447590, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x405E60, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0560);
};

// =====================================================================================================================
class ChildrenModule : public MapObjectType {
  using $ = ChildrenModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ChildrenModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444BF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449960, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D42C0);
};

// =====================================================================================================================
class CommandCenter : public MapObjectType {
  using $ = CommandCenter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CommandCenter);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442960, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448C00, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448C10, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1770);
};

// =====================================================================================================================
class CommandModule : public MapObjectType {
  using $ = CommandModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CommandModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4448F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498A0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4140);
};

// =====================================================================================================================
class CommonMetalsCargo : public MapObjectType {
  using $ = CommonMetalsCargo;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CommonMetalsCargo);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444B30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449930, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4260);
};

// =====================================================================================================================
class CommonOreMine : public MapObjectType {
  using $ = CommonOreMine;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CommonOreMine);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442440, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447E30, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447E40, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1140);
};

// =====================================================================================================================
class CommonOreSmelter : public MapObjectType {
  using $ = CommonOreSmelter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CommonOreSmelter);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443980, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449570, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449580, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3338);
};

// =====================================================================================================================
class CommonStorage : public MapObjectType {
  using $ = CommonStorage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(CommonStorage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4427C0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448260, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448270, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1610);
};

// =====================================================================================================================
class ConsumerFactory : public MapObjectType {
  using $ = ConsumerFactory;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ConsumerFactory);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443470, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4470E0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4470F0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2AF0);
};

// =====================================================================================================================
class ConVec : public MapObjectType {
  using $ = ConVec;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ConVec);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441BA0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447660, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x410C80, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0580);
};

// =====================================================================================================================
class DIRT : public MapObjectType {
  using $ = DIRT;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(DIRT);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4430C0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448190, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4481A0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D23F0);
};

// =====================================================================================================================
class Earthworker : public MapObjectType {
  using $ = Earthworker;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Earthworker);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4421F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448640, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448650, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0DA8);
};

// =====================================================================================================================
class EDWARDSatellite : public MapObjectType {
  using $ = EDWARDSatellite;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EDWARDSatellite);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4447F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449860, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D40C0);
};

// =====================================================================================================================
class EMPMissile : public MapObjectType {
  using $ = EMPMissile;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EMPMissile);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444D30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449990, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4430);
};

// =====================================================================================================================
class EvacuationModule : public MapObjectType {
  using $ = EvacuationModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EvacuationModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444BB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449950, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D42A0);
};

// =====================================================================================================================
class EvacuationTransport : public MapObjectType {
  using $ = EvacuationTransport;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EvacuationTransport);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4420F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448A70, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448A80, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0C38);
};

// =====================================================================================================================
class FoodCargo : public MapObjectType {
  using $ = FoodCargo;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(FoodCargo);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444B70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449940, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4280);
};

// =====================================================================================================================
class Forum : public MapObjectType {
  using $ = Forum;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Forum);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4428C0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448F60, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448F70, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1650);
};

// =====================================================================================================================
class FuelingSystems : public MapObjectType {
  using $ = FuelingSystems;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(FuelingSystems);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444930, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498B0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4160);
};

// =====================================================================================================================
class FusionDriveModule : public MapObjectType {
  using $ = FusionDriveModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(FusionDriveModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4448B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449890, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4120);
};

// =====================================================================================================================
class Garage : public MapObjectType {
  using $ = Garage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Garage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443160, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x44A110, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x44A120, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2510);
};

// =====================================================================================================================
class GeoCon : public MapObjectType {
  using $ = GeoCon;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(GeoCon);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441FA0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447C60, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447C70, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0AA8);
};

// =====================================================================================================================
class GeothermalPlant : public MapObjectType {
  using $ = GeothermalPlant;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(GeothermalPlant);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443320, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449D00, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449D10, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2780);
};

// =====================================================================================================================
class GORF : public MapObjectType {
  using $ = GORF;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(GORF);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443B70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449E70, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449E80, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D36B0);
};

// =====================================================================================================================
class GuardPost : public MapObjectType {
  using $ = GuardPost;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(GuardPost);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4426A0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448110, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448120, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D14D0);
};

// =====================================================================================================================
class HabitatRing : public MapObjectType {
  using $ = HabitatRing;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(HabitatRing);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444970, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498C0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4180);
};

// =====================================================================================================================
class ImpulseItems : public MapObjectType {
  using $ = ImpulseItems;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ImpulseItems);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444DB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4499A0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D44D8);
};

// =====================================================================================================================
class IonDriveModule : public MapObjectType {
  using $ = IonDriveModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(IonDriveModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444870, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449880, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4100);
};

// =====================================================================================================================
class LavaWall : public MapObjectType {
  using $ = LavaWall;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(LavaWall);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4424C0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448020, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x408960, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1160);
};

// =====================================================================================================================
class LightTower : public MapObjectType {
  using $ = LightTower;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(LightTower);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442720, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448060, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448070, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D14F0);
};

// =====================================================================================================================
class LuxuryWares : public MapObjectType {
  using $ = LuxuryWares;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(LuxuryWares);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444E30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4499C0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4518);
};

// =====================================================================================================================
class Lynx : public MapObjectType {
  using $ = Lynx;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Lynx);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441DA0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4476D0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x485630, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0860);
};

// =====================================================================================================================
class MagmaWell : public MapObjectType {
  using $ = MagmaWell;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MagmaWell);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4431E0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448DA0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448DB0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2530);
};

// =====================================================================================================================
class MaxObjectType : public MapObjectType {
  using $ = MaxObjectType;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(None);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441AE0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x446E40, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D0540);
};

// =====================================================================================================================
class MedicalCenter : public MapObjectType {
  using $ = MedicalCenter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MedicalCenter);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442D20, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449AF0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449B00, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1E30);
};

// =====================================================================================================================
class MeteorDefense : public MapObjectType {
  using $ = MeteorDefense;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MeteorDefense);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443280, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x44A190, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x44A1A0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2660);
};

// =====================================================================================================================
class MHDGenerator : public MapObjectType {
  using $ = MHDGenerator;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MHDGenerator);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442A00, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x44A070, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x44A080, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1890);
};

// =====================================================================================================================
class MicrobeWall : public MapObjectType {
  using $ = MicrobeWall;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MicrobeWall);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442560, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448040, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x408960, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1280);
};

// =====================================================================================================================
class Nursery : public MapObjectType {
  using $ = Nursery;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Nursery);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442DC0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449C00, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449C10, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1F50);
};

// =====================================================================================================================
class Observatory : public MapObjectType {
  using $ = Observatory;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Observatory);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4437A0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449A20, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449A30, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2FD8);
};

// =====================================================================================================================
class OrbitalPackage : public MapObjectType {
  using $ = OrbitalPackage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(OrbitalPackage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444A70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449900, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4200);
};

// =====================================================================================================================
class Panther : public MapObjectType {
  using $ = Panther;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Panther);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441E30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4476B0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x485810, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0918);
};

// =====================================================================================================================
class PhoenixModule : public MapObjectType {
  using $ = PhoenixModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(PhoenixModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444AB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449910, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4220);
};

// =====================================================================================================================
class RareMetalsCargo : public MapObjectType {
  using $ = RareMetalsCargo;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RareMetalsCargo);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444AF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449920, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4240);
};

// =====================================================================================================================
class RareOreMine : public MapObjectType {
  using $ = RareOreMine;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RareOreMine);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442600, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447EE0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447EF0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D13A0);
};

// =====================================================================================================================
class RareOreSmelter : public MapObjectType {
  using $ = RareOreSmelter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RareOreSmelter);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443AD0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449680, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449690, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3590);
};

// =====================================================================================================================
class RareStorage : public MapObjectType {
  using $ = RareStorage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RareStorage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442840, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448310, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448320, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1630);
};

// =====================================================================================================================
class RecreationFacility : public MapObjectType {
  using $ = RecreationFacility;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RecreationFacility);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442F00, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449340, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449350, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2190);
};

// =====================================================================================================================
class ReinforcedResidence : public MapObjectType {
  using $ = ReinforcedResidence;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ReinforcedResidence);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443840, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449160, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449170, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D30F8);
};

// =====================================================================================================================
class RepairVehicle : public MapObjectType {
  using $ = RepairVehicle;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RepairVehicle);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442170, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448870, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448880, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0CF0);
};

// =====================================================================================================================
class Residence : public MapObjectType {
  using $ = Residence;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Residence);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442AA0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449040, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449050, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D19B0);
};

// =====================================================================================================================
class RLV : public MapObjectType {
  using $ = RLV;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RLV);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444CB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449980, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4388);
};

// =====================================================================================================================
class RoboDozer : public MapObjectType {
  using $ = RoboDozer;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RoboDozer);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442090, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4475A0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x409BA0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0C18);
};

// =====================================================================================================================
class RoboMiner : public MapObjectType {
  using $ = RoboMiner;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RoboMiner);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441F20, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447AD0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447AE0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D09F0);
};

// =====================================================================================================================
class RoboSurveyor : public MapObjectType {
  using $ = RoboSurveyor;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RoboSurveyor);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441EC0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447AC0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4841D0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D09D0);
};

// =====================================================================================================================
class RobotCommand : public MapObjectType {
  using $ = RobotCommand;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RobotCommand);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442B40, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448CC0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448CD0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1AD0);
};

// =====================================================================================================================
class Scorpion : public MapObjectType {
  using $ = Scorpion;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Scorpion);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441D10, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447910, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447920, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D07A8);
};

// =====================================================================================================================
class Scorpion3Pack : public MapObjectType {
  using $ = Scorpion3Pack;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Scorpion3Pack);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444EB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447AB0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x481780, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D4558);
};

// =====================================================================================================================
class Scout : public MapObjectType {
  using $ = Scout;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Scout);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442020, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447DD0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x474950, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0B60);
};

// =====================================================================================================================
class SensorPackage : public MapObjectType {
  using $ = SensorPackage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SensorPackage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4449B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498D0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D41A0);
};

// =====================================================================================================================
class Skydock : public MapObjectType {
  using $ = Skydock;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Skydock);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4449F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498E0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D41C0);
};

// =====================================================================================================================
class SmallCapacityAirTransport : public MapObjectType {
  using $ = SmallCapacityAirTransport;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SmallCapacityAirTransport);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442270, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447E00, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x474770, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0E60);
};

// =====================================================================================================================
class SolarPowerArray : public MapObjectType {
  using $ = SolarPowerArray;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SolarPowerArray);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442E60, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449FA0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449FB0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2070);
};

// =====================================================================================================================
class SolarSatellite : public MapObjectType {
  using $ = SolarSatellite;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SolarSatellite);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444830, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449870, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D40E0);
};

// =====================================================================================================================
class Spaceport : public MapObjectType {
  using $ = Spaceport;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Spaceport);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443A20, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449780, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449790, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3458);
};

// =====================================================================================================================
class Spider : public MapObjectType {
  using $ = Spider;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Spider);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441C90, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4476F0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x447700, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D06F0);
};

// =====================================================================================================================
class Spider3Pack : public MapObjectType {
  using $ = Spider3Pack;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Spider3Pack);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444E70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447AA0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x481710, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D4538);
};

// =====================================================================================================================
class StandardLab : public MapObjectType {
  using $ = StandardLab;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(StandardLab);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443680, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448480, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448490, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2E98);
};

// =====================================================================================================================
class StasisSystems : public MapObjectType {
  using $ = StasisSystems;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(StasisSystems);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444A30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498F0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D41E0);
};

// =====================================================================================================================
class StructureFactory : public MapObjectType {
  using $ = StructureFactory;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(StructureFactory);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443520, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x446EA0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x446EB0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2C28);
};

// =====================================================================================================================
class SULV : public MapObjectType {
  using $ = SULV;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SULV);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444C30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449970, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D42E0);
};

// =====================================================================================================================
class Tiger : public MapObjectType {
  using $ = Tiger;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Tiger);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441C00, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447680, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4859C0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D05A0);
};

// =====================================================================================================================
class Tokamak : public MapObjectType {
  using $ = Tokamak;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Tokamak);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443C10, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449DA0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449DB0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D37D0);
};

// =====================================================================================================================
class TradeCenter : public MapObjectType {
  using $ = TradeCenter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(TradeCenter);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442BE0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449480, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x449490, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1BF0);
};

// =====================================================================================================================
class Tube : public MapObjectType {
  using $ = Tube;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Tube);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442300, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447FE0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x408960, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D0F00);
};

// =====================================================================================================================
class University : public MapObjectType {
  using $ = University;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(University);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x442FA0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448E40, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x448E50, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D22B0);
};

// =====================================================================================================================
class VehicleFactory : public MapObjectType {
  using $ = VehicleFactory;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(VehicleFactory);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4435D0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x446F80, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x446F90, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D2D60);
};

// =====================================================================================================================
class Wall : public MapObjectType {
  using $ = Wall;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Wall);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4423A0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x448000, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x408960, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D1020);
};

// =====================================================================================================================
class Wares : public MapObjectType {
  using $ = Wares;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Wares);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444DF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4499B0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D44F8);
};

// =====================================================================================================================
class AcidCloud : public MapEntityType {
  using $ = AcidCloud;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(AcidCloud);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443C90, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4471D0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5550, &$::Init>();        }
  void        Process()     override { return Thunk<0x4A7810, &$::Process>();     }
  MapObject*  FindTarget()  override { return Thunk<0x4A6B70, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A55A0, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A55E0, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5630, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5670, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x4471E0, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D38F0);
};

// =====================================================================================================================
class AmbientAnimation : public MapEntityType {
  using $ = AmbientAnimation;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(AmbientAnimation);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
  { return Thunk<0x444F70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4473E0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5700, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D4640);
};

// =====================================================================================================================
class BFG : public MapEntityType {
  using $ = BFG;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(BFG);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4442B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447370, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A63F0, &$::Init>();        }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6440, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6480, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A64D0, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6510, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447380, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3C88);
};

// =====================================================================================================================
class EMPBlast : public MapEntityType {
  using $ = EMPBlast;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EMPBlast);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444230, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4473C0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A56B0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3BC0);
};

// =====================================================================================================================
class Earthquake : public MapEntityType {
  using $ = Earthquake;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Earthquake);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4443D0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4474C0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3D18);
};

// =====================================================================================================================
class EMP : public MapEntityType {
  using $ = EMP;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EMP);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443CF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447200, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5740, &$::Init>();        }
  void        Process()     override { return Thunk<0x4A7650, &$::Process>();     }
  MapObject*  FindTarget()  override { return Thunk<0x4A7130, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5790, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5810, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A58A0, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5920, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447210, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3920);
};

// =====================================================================================================================
class EnergyCannon : public MapEntityType {
  using $ = EnergyCannon;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(EnergyCannon);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4441D0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4473A0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A6550, &$::Init>();        }
  MapObject*  FindTarget()  override { return Thunk<0x4A70B0, &$::FindTarget>();  }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x4473B0, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3B90);
};

// =====================================================================================================================
class Eruption : public MapEntityType {
  using $ = Eruption;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Eruption);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444490, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4474E0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3D78);
};

// =====================================================================================================================
class ESG : public MapEntityType {
  using $ = ESG;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ESG);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4440B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447560, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x480A70, &$::Init>();        }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x480AB0, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x480AF0, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x480B40, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x480B80, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447570, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3B00);
};

// =====================================================================================================================
class Fumarole : public MapEntityType {
  using $ = Fumarole;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Fumarole);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4445B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447520, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3E08);
};

// =====================================================================================================================
class InterColonyShuttle : public MapEntityType {
  using $ = InterColonyShuttle;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(InterColonyShuttle);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444EF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447490, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x47C9B0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D4578);
};

// =====================================================================================================================
class LargeBuildingExplosion : public MapEntityType {
  using $ = LargeBuildingExplosion;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(LargeBuildingExplosion);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444770, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447460, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A66D0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3FF8);
};

// =====================================================================================================================
class Laser : public MapEntityType {
  using $ = Laser;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Laser);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443D50, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447230, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A59A0, &$::Init>();        }
  MapObject*  FindTarget()  override { return Thunk<0x4A70F0, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A59F0, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5A30, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5A80, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5AC0, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447240, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3950);
};

// =====================================================================================================================
class Lightning : public MapEntityType {
  using $ = Lightning;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Lightning);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444310, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4474B0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3CB8);
};

// =====================================================================================================================
class MagmaVent : public MapEntityType {
  using $ = MagmaVent;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MagmaVent);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444550, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447510, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3DD8);
};

// =====================================================================================================================
class MediumBuildingExplosion : public MapEntityType {
  using $ = MediumBuildingExplosion;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MediumBuildingExplosion);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4446F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447430, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A6680, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3F30);
};

// =====================================================================================================================
class Meteor : public MapEntityType {
  using $ = Meteor;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Meteor);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444430, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4474D0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3D48);
};

// =====================================================================================================================
class Microwave : public MapEntityType {
  using $ = Microwave;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Microwave);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443DB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447250, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5B00, &$::Init>();        }
  void        Process()     override { return Thunk<0x4A7810, &$::Process>();     }
  MapObject*  FindTarget()  override { return Thunk<0x4A7110, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5B50, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5B90, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5BE0, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5C20, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447260, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3980);
};

// =====================================================================================================================
class MiningBeacon : public MapEntityType {
  using $ = MiningBeacon;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(MiningBeacon);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4444F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447500, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3DA8);
};

// =====================================================================================================================
class SelfDestruct : public MapEntityType {
  using $ = SelfDestruct;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SelfDestruct);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444050, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447330, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A6240, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3AD0);
};

// =====================================================================================================================
class RailGun : public MapEntityType {
  using $ = RailGun;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RailGun);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443E10, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447270, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5C60, &$::Init>();        }
  void        Process()     override { return Thunk<0x4A7810, &$::Process>();     }
  MapObject*  FindTarget()  override { return Thunk<0x4A70D0, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5CB0, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5CF0, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5D40, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5D80, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447280, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D39B0);
};

// =====================================================================================================================
class RPG : public MapEntityType {
  using $ = RPG;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(RPG);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443E70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4472A0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5DC0, &$::Init>();        }
  void        Process()     override { return Thunk<0x4A7810, &$::Process>();     }
  MapObject*  FindTarget()  override { return Thunk<0x4A6B70, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5E10, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5E50, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5EA0, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A5EE0, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x4472B0, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D39E0);
};

// =====================================================================================================================
class SmallBuildingExplosion : public MapEntityType {
  using $ = SmallBuildingExplosion;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SmallBuildingExplosion);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444670, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447400, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A6630, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3E68);
};

// =====================================================================================================================
class Starflare : public MapEntityType {
  using $ = Starflare;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Starflare);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443ED0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4472E0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5FC0, &$::Init>();        }
  MapObject*  FindTarget()  override { return Thunk<0x4472F0, &$::FindTarget>();  }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6060, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A60B0, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6100, &$::SelectLynxGraphic>(pChassis, rotation); }

  DEFINE_VTBL_GETTER(0x4D3A10);
};

// =====================================================================================================================
class StarflareGP : public MapEntityType {
  using $ = StarflareGP;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(StarflareGP);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443F90, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4472D0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5F20, &$::Init>();        }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6010, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  DEFINE_VTBL_GETTER(0x4D3A70);
};

// =====================================================================================================================
class Stickyfoam : public MapEntityType {
  using $ = Stickyfoam;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Stickyfoam);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444110, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447340, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A6290, &$::Init>();        }
  void        Process()     override { return Thunk<0x4A7A20, &$::Process>();     }
  MapObject*  FindTarget()  override { return Thunk<0x4A7340, &$::FindTarget>();  }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A62E0, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6320, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6370, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A63B0, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447350, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3B30);
};

// =====================================================================================================================
class Supernova : public MapEntityType {
  using $ = Supernova;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Supernova);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443F30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447300, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A6150, &$::Init>();        }
  MapObject*  FindTarget()  override { return Thunk<0x447310, &$::FindTarget>();  }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A61C0, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A61F0, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A6210, &$::SelectLynxGraphic>(pChassis, rotation); }

  DEFINE_VTBL_GETTER(0x4D3A40);
};

// =====================================================================================================================
class SupernovaGP : public MapEntityType {
  using $ = SupernovaGP;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(SupernovaGP);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x443FF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447320, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A5F70, &$::Init>();        }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x4A61A0, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  DEFINE_VTBL_GETTER(0x4D3AA0);
};

// =====================================================================================================================
class ThorsHammer : public MapEntityType {
  using $ = ThorsHammer;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(ThorsHammer);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444170, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447540, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x48A7C0, &$::Init>();        }

  void SelectGuardPostGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x48A800, &$::SelectGuardPostGraphic>(pChassis, rotation); }

  void SelectTigerGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x48A840, &$::SelectTigerGraphic>(pChassis, rotation); }

  void SelectPantherGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x48A890, &$::SelectPantherGraphic>(pChassis, rotation); }

  void SelectLynxGraphic(MapObject* pChassis, int rotation) override
    { return Thunk<0x48A8D0, &$::SelectLynxGraphic>(pChassis, rotation); }

  int GetMovementAimOffset(int pixelDistance) override
    { return Thunk<0x447550, &$::GetMovementAimOffset>(pixelDistance); }

  DEFINE_VTBL_GETTER(0x4D3B60);
};

// =====================================================================================================================
class Vortex : public MapEntityType {
  using $ = Vortex;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Vortex);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444370, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4474F0, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3CE8);
};

// =====================================================================================================================
class Wreckage : public MapEntityType {
  using $ = Wreckage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(Wreckage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444610, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447530, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3E38);
};

END_PACKED

} // MapObjType

#define OP2_EMIT_MO_MAPPINGS($)                                                                                        \
  $(CargoTruck)  $(ConVec)  $(Spider)  $(Scorpion)  $(Lynx)  $(Panther)  $(Tiger)  $(RoboSurveyor)  $(RoboMiner)       \
  $(GeoCon)  $(Scout)  $(RoboDozer)  $(EvacuationTransport)  $(RepairVehicle)  $(Earthworker)                          \
  $(SmallCapacityAirTransport)  $(Tube)  $(Wall)  $(LavaWall)  $(MicrobeWall)  $(CommonOreMine)  $(RareOreMine)        \
  $(GuardPost)  $(LightTower)  $(CommonStorage)  $(RareStorage)  $(Forum)  $(CommandCenter)  $(MHDGenerator)           \
  $(Residence)  $(RobotCommand)  $(TradeCenter)  $(BasicLab)  $(MedicalCenter)  $(Nursery)  $(SolarPowerArray)         \
  $(RecreationFacility)  $(University)  $(Agridome)  $(DIRT)  $(Garage)  $(MagmaWell)  $(MeteorDefense)                \
  $(GeothermalPlant)  $(ArachnidFactory)  $(ConsumerFactory)  $(StructureFactory)  $(VehicleFactory)  $(StandardLab)   \
  $(AdvancedLab)  $(Observatory)  $(ReinforcedResidence)  $(AdvancedResidence)  $(CommonOreSmelter)  $(Spaceport)      \
  $(RareOreSmelter)  $(GORF)  $(Tokamak)  $(AcidCloud)  $(EMP)  $(Laser)  $(Microwave)  $(RailGun)  $(RPG)             \
  $(Starflare)  $(Supernova)  $(StarflareGP)  $(SupernovaGP)  $(SelfDestruct)  $(ESG)  $(Stickyfoam)  $(ThorsHammer)   \
  $(EnergyCannon)  $(EMPBlast)  $(BFG)  $(Lightning)  $(Vortex)  $(Earthquake)  $(Eruption)  $(Meteor)  $(MiningBeacon)\
  $(MagmaVent)  $(Fumarole)  $(Wreckage)  $(SmallBuildingExplosion)  $(MediumBuildingExplosion)                        \
  $(LargeBuildingExplosion)  $(SULV)  $(RLV)  $(EMPMissile)  $(InterColonyShuttle)  $(AmbientAnimation)
#define OP2_EMIT_MO_TYPE_MAPPINGS($)                                                                                   \
  OP2_EMIT_MO_MAPPINGS($);  $(EDWARDSatellite)  $(SolarSatellite)  $(IonDriveModule)  $(FusionDriveModule)             \
  $(CommandModule)  $(FuelingSystems)  $(HabitatRing)  $(SensorPackage)  $(Skydock)  $(StasisSystems)                  \
  $(OrbitalPackage)  $(PhoenixModule)  $(RareMetalsCargo)  $(CommonMetalsCargo)  $(FoodCargo)  $(EvacuationModule)     \
  $(ChildrenModule)  $(ImpulseItems)  $(Wares)  $(LuxuryWares)  $(Spider3Pack)  $(Scorpion3Pack)

namespace TethysImpl {
#define OP2_MO_TYPE_FOR_DEF(name) template <>  struct MapObjTypeForImpl<MapID::name> { using Type = MapObjType::name; };
OP2_EMIT_MO_TYPE_MAPPINGS(OP2_MO_TYPE_FOR_DEF);
} // TethysImpl

} // Tethys
