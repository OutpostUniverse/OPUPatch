
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Location.h"
#include "Tethys/Game/Sheet.h"

class MapObject;
class StreamIO;
enum class SoundID : int;

/// Map object type IDs, including structures, vehicles, disasters, beacons, weapons fire, etc.
/// @note Can be either int, uint8, or uint16 depending on context.
enum map_id : int {
  mapAny                       = (~0),  ///< Use to specify 'all' or 'any'
  mapNone                      = 0x00,  ///< Use to specify 'none'.  @note Create maps to MaxObjectType.

  mapCargoTruck                = 0x01,  ///< 'BIG_TRUCK' in sheets.
  mapConVec                    = 0x02,  ///< 'CON_TRUCK'
  mapSpider                    = 0x03,  ///< 'SPIDER'
  mapScorpion                  = 0x04,  ///< 'SCORPION'
  mapLynx                      = 0x05,  ///< 'LIGHT_TANK'
  mapPanther                   = 0x06,  ///< 'MED_TANK'
  mapTiger                     = 0x07,  ///< 'BIG_TANK'
  mapRoboSurveyor              = 0x08,  ///< 'SURVEYOR'
  mapRoboMiner                 = 0x09,  ///< 'MINER'
  mapGeoCon                    = 0x0A,  ///< 'GEO_METRO'
  mapScout                     = 0x0B,  ///< 'SCOUT'
  mapRoboDozer                 = 0x0C,  ///< 'BULL_DOZER'
  mapEvacuationTransport       = 0x0D,  ///< 'MOBILE_HOME'
  mapRepairVehicle             = 0x0E,  ///< 'REPAIR_TRUCK'
  mapEarthworker               = 0x0F,  ///< 'WALL_TRUCK'
  mapSmallCapacityAirTransport = 0x10,  ///< 'SCAT';  Mostly unimplemented and tends to crash.

  mapTube                      = 0x11,  ///< 'TUBE';  It's not a 'BIG_TRUCK'.
  mapWall                      = 0x12,  ///< 'WALL'
  mapLavaWall                  = 0x13,  ///< 'WALL_LAVA'
  mapMicrobeWall               = 0x14,  ///< 'WALL_MICROBE'

  mapCommonOreMine             = 0x15,  ///< 'MINE'
  mapRareOreMine               = 0x16,  ///< 'MINE_ADV'
  mapGuardPost                 = 0x17,  ///< 'TOWER_GUARD'
  mapLightTower                = 0x18,  ///< 'TOWER_LIGHT'
  mapCommonStorage             = 0x19,  ///< 'STORAGE_ORE'
  mapRareStorage               = 0x1A,  ///< 'STORAGE_RARE'
  mapForum                     = 0x1B,  ///< 'FORUM'
  mapCommandCenter             = 0x1C,  ///< 'COMMAND'
  mapMHDGenerator              = 0x1D,  ///< 'ASE'
  mapResidence                 = 0x1E,  ///< 'RESIDENCE'
  mapRobotCommand              = 0x1F,  ///< 'ROBOT_COMMAND'
  mapTradeCenter               = 0x20,  ///< 'TRADE'
  mapBasicLab                  = 0x21,  ///< 'LAB'
  mapMedicalCenter             = 0x22,  ///< 'MED_CENTER'
  mapNursery                   = 0x23,  ///< 'NURSERY'
  mapSolarPowerArray           = 0x24,  ///< 'SOLAR'
  mapRecreationFacility        = 0x25,  ///< 'RECREATION'
  mapUniversity                = 0x26,  ///< 'UNIVERSITY'
  mapAgridome                  = 0x27,  ///< 'AGRIDOME'
  mapDIRT                      = 0x28,  ///< 'DIRT'
  mapGarage                    = 0x29,  ///< 'GARAGE'
  mapMagmaWell                 = 0x2A,  ///< 'MAGMA_WELL'
  mapMeteorDefense             = 0x2B,  ///< 'METEOR_DEF'
  mapGeothermalPlant           = 0x2C,  ///< 'GEOTHERMAL'
  mapArachnidFactory           = 0x2D,  ///< 'FACT_ANDROID'
  mapConsumerFactory           = 0x2E,  ///< 'FACT_LUXURY'
  mapStructureFactory          = 0x2F,  ///< 'FACT_STRUCTURE'
  mapVehicleFactory            = 0x30,  ///< 'FACT_VEHICLE'
  mapStandardLab               = 0x31,  ///< 'LAB_STANDARD'
  mapAdvancedLab               = 0x32,  ///< 'LAB_ADV'
  mapObservatory               = 0x33,  ///< 'OBSERVATORY'
  mapReinforcedResidence       = 0x34,  ///< 'RESIDENCE_P'
  mapAdvancedResidence         = 0x35,  ///< 'RESIDENCE_E'
  mapCommonOreSmelter          = 0x36,  ///< 'SMELTER'
  mapSpaceport                 = 0x37,  ///< 'SPACEPORT'
  mapRareOreSmelter            = 0x38,  ///< 'SMELTER_ADV'
  mapGORF                      = 0x39,  ///< 'GORF'
  mapTokamak                   = 0x3A,  ///< 'TOKAMAK'

  mapAcidCloud                 = 0x3B,  ///< 'ACID'
  mapEMP                       = 0x3C,  ///< 'EMP'
  mapLaser                     = 0x3D,  ///< 'LASER'
  mapMicrowave                 = 0x3E,  ///< 'MICROWAVE'
  mapRailGun                   = 0x3F,  ///< 'RAIL_GUN'
  mapRPG                       = 0x40,  ///< 'CANNON'
  mapStarflare                 = 0x41,  ///< 'SELF_DESTRUCT';        For vehicles.
  mapSupernova                 = 0x42,  ///< 'SELF_DESTRUCT_ADV';    For vehicles.  @note Create maps to SupernovaGP?
  mapStarflareGP               = 0x43,  ///< 'TURRET_DESTRUCT';      For guard posts.
  mapSupernovaGP               = 0x44,  ///< 'TURRET_DESTRUCT_ADV';  For guard posts.
  mapSelfDestruct              = 0x45,  ///< 'SCUTTLE_DESTRUCT';     Vehicle self-destruct command explosion.
  mapESG                       = 0x46,  ///< 'SPAM'
  mapStickyfoam                = 0x47,  ///< 'FOAM'
  mapThorsHammer               = 0x48,  ///< 'THORS_HAMMER'
  mapEnergyCannon              = 0x49,  ///< 'SCORPION_WEAPON'
  mapEMPBlast                  = 0x4A,  ///< 'EMP_BIG';  EMP missile/power plant explosion.
  mapBFG                       = 0x4B,  ///< 'BFG';      Unimplemented projectile weapon.

  mapLightning                 = 0x4C,  ///< 'LIGHTNING'
  mapVortex                    = 0x4D,  ///< 'TORNADO'
  mapEarthquake                = 0x4E,  ///< 'EARTHQUAKE'
  mapEruption                  = 0x4F,  ///< 'ERUPTION'
  mapMeteor                    = 0x50,  ///< 'METEOR'

  mapMiningBeacon              = 0x51,  ///< 'BEACON'
  mapMagmaVent                 = 0x52,  ///< 'MAGMA_VENT'
  mapFumarole                  = 0x53,  ///< 'FUMAROLE'
  mapWreckage                  = 0x54,  ///< 'WRECK_FLAG'

  mapSmallBuildingExplosion    = 0x55,  ///< 'BLD_EXPL_SML'
  mapMediumBuildingExplosion   = 0x56,  ///< 'BLD_EXPL_MED'
  mapLargeBuildingExplosion    = 0x57,  ///< 'BLD_EXPL_BIG'

  // Satellites do not have MapObject instances.
  mapEDWARDSatellite           = 0x58,  ///< 'EDWARD'
  mapSolarSatellite            = 0x59,  ///< 'SOLAR_SAT'
  mapIonDriveModule            = 0x5A,  ///< 'SPACE_1'
  mapFusionDriveModule         = 0x5B,  ///< 'SPACE_2'
  mapCommandModule             = 0x5C,  ///< 'SPACE_3'
  mapFuelingSystems            = 0x5D,  ///< 'SPACE_4'
  mapHabitatRing               = 0x5E,  ///< 'SPACE_5'
  mapSensorPackage             = 0x5F,  ///< 'SPACE_6'
  mapSkydock                   = 0x60,  ///< 'SPACE_7'
  mapStasisSystems             = 0x61,  ///< 'SPACE_8'
  mapOrbitalPackage            = 0x62,  ///< 'SPACE_9'
  mapPhoenixModule             = 0x63,  ///< 'SPACE_10'
  mapRareMetalsCargo           = 0x64,  ///< 'SPACE_11'
  mapCommonMetalsCargo         = 0x65,  ///< 'SPACE_12'
  mapFoodCargo                 = 0x66,  ///< 'SPACE_13'
  mapEvacuationModule          = 0x67,  ///< 'SPACE_14'
  mapChildrenModule            = 0x68,  ///< 'SPACE_15'

  mapSULV                      = 0x69,  ///< 'SULV'
  mapRLV                       = 0x6A,  ///< 'RLV'
  mapEMPMissile                = 0x6B,  ///< 'EMP_MISSILE'

  // Consumer goods do not have MapObject instances.
  mapImpulseItems              = 0x6C,  ///< 'LUX_1'
  mapWares                     = 0x6D,  ///< 'LUX_2'
  mapLuxuryWares               = 0x6E,  ///< 'LUX_3'

  mapInterColonyShuttle        = 0x6F,  ///< 'SHUTTLE';  Unimplemented "projectile", probably meant to go with SCAT?

  // Arachnid 3-packs do not have MapObject instances.
  mapSpider3Pack               = 0x70,  ///< 'SPIDER_PACK'
  mapScorpion3Pack             = 0x71,  ///< 'SCORPION_PACK'

  mapAmbientAnimation          = 0x72,  ///< 'AMBIENT_ANIM';  Also known as "Pretty art!"

  mapMaxObject                 = 0x73,  ///< 'MAX_OBJECT_TYPE';  Also happens to be the base type for lab buildings.
};
using MapID = map_id;

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

BEGIN_PACKED

struct PerPlayerUnitStats {
  int       hp;
  int       repairAmount;
  ArmorType armor;
  int       commonCost;
  int       rareCost;
  int       buildTime;
  int       sightRange;
  uint8     numUnitsOfType;
  uint8     bImproved;
  uint16    field_1E;
  int       playerPropOre;

  union {
    struct {
      int powerRequired;
      int workersRequired;
      int scientistsRequired;
      int storageCapacity;
      int productionCapacity;
      int storageBays;
    } building;

    struct {
      int   moveSpeed;
      int   turnRate;
      int   productionRate;
      int   reloadTime;
      uint8 field_38;
      uint8 field_39;
      uint8 field_3A;
    } vehicle;

    struct {
      int moveSpeed;
      int concussionDamage;
      int penetrationDamage;
      int reloadTime;
      int weaponSightRange;
      int field_3C;
    } weapon;
  };

  int completedUpgradeTechIDs[2];
};

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

#define OP2_MO_TYPE_CLASS_MAP_ID(id) static constexpr MapID ClassMapId = id;  \
                                     static $* GetInstance() { return static_cast<$*>(MapObjectType::GetInstance(id)); }


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
  static MapObjectType* GetInstance(int mapID)
    { return (size_t(mapID) <= Sheet::NumMapObjTypes()) ? Sheet::GetMapObjTypeTable()[mapID] : nullptr; }

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapAdvancedLab);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapAdvancedResidence);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapAgridome);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapArachnidFactory);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapBasicLab);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapCargoTruck);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapChildrenModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444BF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449960, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D42C0);
};

// =====================================================================================================================
class CommandCenter : public MapObjectType {
  using $ = CommandCenter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapCommandCenter);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapCommandModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4448F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498A0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4140);
};

// =====================================================================================================================
class CommonMetalsCargo : public MapObjectType {
  using $ = CommonMetalsCargo;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapCommonMetalsCargo);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444B30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449930, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4260);
};

// =====================================================================================================================
class CommonOreMine : public MapObjectType {
  using $ = CommonOreMine;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapCommonOreMine);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapCommonOreSmelter);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapCommonStorage);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapConsumerFactory);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapConVec);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapDIRT);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEarthworker);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEDWARDSatellite);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4447F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449860, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D40C0);
};

// =====================================================================================================================
class EMPMissile : public MapObjectType {
  using $ = EMPMissile;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapEMPMissile);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444D30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449990, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4430);
};

// =====================================================================================================================
class EvacuationModule : public MapObjectType {
  using $ = EvacuationModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapEvacuationModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444BB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449950, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D42A0);
};

// =====================================================================================================================
class EvacuationTransport : public MapObjectType {
  using $ = EvacuationTransport;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapEvacuationTransport);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapFoodCargo);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444B70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449940, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4280);
};

// =====================================================================================================================
class Forum : public MapObjectType {
  using $ = Forum;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapForum);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapFuelingSystems);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444930, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498B0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4160);
};

// =====================================================================================================================
class FusionDriveModule : public MapObjectType {
  using $ = FusionDriveModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapFusionDriveModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4448B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449890, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4120);
};

// =====================================================================================================================
class Garage : public MapObjectType {
  using $ = Garage;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapGarage);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapGeoCon);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapGeothermalPlant);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapGORF);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapGuardPost);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapHabitatRing);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444970, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498C0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4180);
};

// =====================================================================================================================
class ImpulseItems : public MapObjectType {
  using $ = ImpulseItems;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapImpulseItems);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444DB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4499A0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D44D8);
};

// =====================================================================================================================
class IonDriveModule : public MapObjectType {
  using $ = IonDriveModule;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapIonDriveModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444870, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449880, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4100);
};

// =====================================================================================================================
class LavaWall : public MapObjectType {
  using $ = LavaWall;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapLavaWall);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapLightTower);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapLuxuryWares);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444E30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4499C0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4518);
};

// =====================================================================================================================
class Lynx : public MapObjectType {
  using $ = Lynx;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapLynx);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMagmaWell);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapNone);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x441AE0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x446E40, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D0540);
};

// =====================================================================================================================
class MedicalCenter : public MapObjectType {
  using $ = MedicalCenter;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapMedicalCenter);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMeteorDefense);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMHDGenerator);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMicrobeWall);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapNursery);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapObservatory);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapOrbitalPackage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444A70, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449900, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4200);
};

// =====================================================================================================================
class Panther : public MapObjectType {
  using $ = Panther;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapPanther);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapPhoenixModule);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444AB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449910, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4220);
};

// =====================================================================================================================
class RareMetalsCargo : public MapObjectType {
  using $ = RareMetalsCargo;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapRareMetalsCargo);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444AF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449920, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4240);
};

// =====================================================================================================================
class RareOreMine : public MapObjectType {
  using $ = RareOreMine;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapRareOreMine);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRareOreSmelter);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRareStorage);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRecreationFacility);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapReinforcedResidence);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRepairVehicle);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapResidence);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRLV);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444CB0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449980, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D4388);
};

// =====================================================================================================================
class RoboDozer : public MapObjectType {
  using $ = RoboDozer;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapRoboDozer);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRoboMiner);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRoboSurveyor);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRobotCommand);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapScorpion);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapScorpion3Pack);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapScout);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSensorPackage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4449B0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498D0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D41A0);
};

// =====================================================================================================================
class Skydock : public MapObjectType {
  using $ = Skydock;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapSkydock);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x4449F0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498E0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D41C0);
};

// =====================================================================================================================
class SmallCapacityAirTransport : public MapObjectType {
  using $ = SmallCapacityAirTransport;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapSmallCapacityAirTransport);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSolarPowerArray);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSolarSatellite);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444830, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449870, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D40E0);
};

// =====================================================================================================================
class Spaceport : public MapObjectType {
  using $ = Spaceport;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapSpaceport);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSpider);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSpider3Pack);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapStandardLab);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapStasisSystems);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444A30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4498F0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D41E0);
};

// =====================================================================================================================
class StructureFactory : public MapObjectType {
  using $ = StructureFactory;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapStructureFactory);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSULV);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444C30, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x449970, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D42E0);
};

// =====================================================================================================================
class Tiger : public MapObjectType {
  using $ = Tiger;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapTiger);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapTokamak);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapTradeCenter);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapTube);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapUniversity);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapVehicleFactory);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapWall);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapWares);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444DF0, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x4499B0, &$::GetCodeName>(); }

  DEFINE_VTBL_GETTER(0x4D44F8);
};

// =====================================================================================================================
class AcidCloud : public MapEntityType {
  using $ = AcidCloud;
public:
  OP2_MO_TYPE_CLASS_MAP_ID(mapAcidCloud);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapAmbientAnimation);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapBFG);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEMPBlast);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEarthquake);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEMP);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEnergyCannon);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapEruption);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapESG);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapFumarole);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapInterColonyShuttle);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapLargeBuildingExplosion);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapLaser);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapLightning);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMagmaVent);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMediumBuildingExplosion);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMeteor);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMicrowave);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapMiningBeacon);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSelfDestruct);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRailGun);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapRPG);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSmallBuildingExplosion);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapStarflare);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapStarflareGP);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapStickyfoam);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSupernova);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapSupernovaGP);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapThorsHammer);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapVortex);

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
  OP2_MO_TYPE_CLASS_MAP_ID(mapWreckage);

  MapObject*  Create(int pixelX, int pixelY, int unitIndex) override
    { return Thunk<0x444610, &$::Create>(pixelX, pixelY, unitIndex); }
  const char* GetCodeName() override { return Thunk<0x447530, &$::GetCodeName>(); }
  ibool       Init()        override { return Thunk<0x4A28E0, &$::Init>();        }

  DEFINE_VTBL_GETTER(0x4D3E38);
};

END_PACKED

} // MapObjType

namespace TethysImpl {
template <MapID>                                   struct MapObjTypeForImpl     { using Type = MapObjectType;    };
#define OP2_MO_TYPE_FOR_DEF(id, name)  template <> struct MapObjTypeForImpl<id> { using Type = MapObjType::name; };
OP2_MO_TYPE_FOR_DEF(mapAdvancedLab,                AdvancedLab);
OP2_MO_TYPE_FOR_DEF(mapAdvancedResidence,          AdvancedResidence);
OP2_MO_TYPE_FOR_DEF(mapAgridome,                   Agridome);
OP2_MO_TYPE_FOR_DEF(mapArachnidFactory,            ArachnidFactory);
OP2_MO_TYPE_FOR_DEF(mapBasicLab,                   BasicLab);
OP2_MO_TYPE_FOR_DEF(mapCargoTruck,                 CargoTruck);
OP2_MO_TYPE_FOR_DEF(mapChildrenModule,             ChildrenModule);
OP2_MO_TYPE_FOR_DEF(mapCommandCenter,              CommandCenter);
OP2_MO_TYPE_FOR_DEF(mapCommandModule,              CommandModule);
OP2_MO_TYPE_FOR_DEF(mapCommonMetalsCargo,          CommonMetalsCargo);
OP2_MO_TYPE_FOR_DEF(mapCommonOreMine,              CommonOreMine);
OP2_MO_TYPE_FOR_DEF(mapCommonOreSmelter,           CommonOreSmelter);
OP2_MO_TYPE_FOR_DEF(mapCommonStorage,              CommonStorage);
OP2_MO_TYPE_FOR_DEF(mapConsumerFactory,            ConsumerFactory);
OP2_MO_TYPE_FOR_DEF(mapConVec,                     ConVec);
OP2_MO_TYPE_FOR_DEF(mapDIRT,                       DIRT);
OP2_MO_TYPE_FOR_DEF(mapEarthworker,                Earthworker);
OP2_MO_TYPE_FOR_DEF(mapEDWARDSatellite,            EDWARDSatellite);
OP2_MO_TYPE_FOR_DEF(mapEMPMissile,                 EMPMissile);
OP2_MO_TYPE_FOR_DEF(mapEvacuationModule,           EvacuationModule);
OP2_MO_TYPE_FOR_DEF(mapEvacuationTransport,        EvacuationTransport);
OP2_MO_TYPE_FOR_DEF(mapFoodCargo,                  FoodCargo);
OP2_MO_TYPE_FOR_DEF(mapForum,                      Forum);
OP2_MO_TYPE_FOR_DEF(mapFuelingSystems,             FuelingSystems);
OP2_MO_TYPE_FOR_DEF(mapFusionDriveModule,          FusionDriveModule);
OP2_MO_TYPE_FOR_DEF(mapGarage,                     Garage);
OP2_MO_TYPE_FOR_DEF(mapGeoCon,                     GeoCon);
OP2_MO_TYPE_FOR_DEF(mapGeothermalPlant,            GeothermalPlant);
OP2_MO_TYPE_FOR_DEF(mapGORF,                       GORF);
OP2_MO_TYPE_FOR_DEF(mapGuardPost,                  GuardPost);
OP2_MO_TYPE_FOR_DEF(mapHabitatRing,                HabitatRing);
OP2_MO_TYPE_FOR_DEF(mapImpulseItems,               ImpulseItems);
OP2_MO_TYPE_FOR_DEF(mapIonDriveModule,             IonDriveModule);
OP2_MO_TYPE_FOR_DEF(mapLavaWall,                   LavaWall);
OP2_MO_TYPE_FOR_DEF(mapLightTower,                 LightTower);
OP2_MO_TYPE_FOR_DEF(mapLuxuryWares,                LuxuryWares);
OP2_MO_TYPE_FOR_DEF(mapLynx,                       Lynx);
OP2_MO_TYPE_FOR_DEF(mapMagmaWell,                  MagmaWell);
OP2_MO_TYPE_FOR_DEF(mapMedicalCenter,              MedicalCenter);
OP2_MO_TYPE_FOR_DEF(mapMeteorDefense,              MeteorDefense);
OP2_MO_TYPE_FOR_DEF(mapMHDGenerator,               MHDGenerator);
OP2_MO_TYPE_FOR_DEF(mapMicrobeWall,                MicrobeWall);
OP2_MO_TYPE_FOR_DEF(mapNursery,                    Nursery);
OP2_MO_TYPE_FOR_DEF(mapObservatory,                Observatory);
OP2_MO_TYPE_FOR_DEF(mapOrbitalPackage,             OrbitalPackage);
OP2_MO_TYPE_FOR_DEF(mapPanther,                    Panther);
OP2_MO_TYPE_FOR_DEF(mapPhoenixModule,              PhoenixModule);
OP2_MO_TYPE_FOR_DEF(mapRareMetalsCargo,            RareMetalsCargo);
OP2_MO_TYPE_FOR_DEF(mapRareOreMine,                RareOreMine);
OP2_MO_TYPE_FOR_DEF(mapRareOreSmelter,             RareOreSmelter);
OP2_MO_TYPE_FOR_DEF(mapRareStorage,                RareStorage);
OP2_MO_TYPE_FOR_DEF(mapRecreationFacility,         RecreationFacility);
OP2_MO_TYPE_FOR_DEF(mapReinforcedResidence,        ReinforcedResidence);
OP2_MO_TYPE_FOR_DEF(mapRepairVehicle,              RepairVehicle);
OP2_MO_TYPE_FOR_DEF(mapResidence,                  Residence);
OP2_MO_TYPE_FOR_DEF(mapRLV,                        RLV);
OP2_MO_TYPE_FOR_DEF(mapRoboDozer,                  RoboDozer);
OP2_MO_TYPE_FOR_DEF(mapRoboMiner,                  RoboMiner);
OP2_MO_TYPE_FOR_DEF(mapRoboSurveyor,               RoboSurveyor);
OP2_MO_TYPE_FOR_DEF(mapRobotCommand,               RobotCommand);
OP2_MO_TYPE_FOR_DEF(mapScorpion,                   Scorpion);
OP2_MO_TYPE_FOR_DEF(mapScorpion3Pack,              Scorpion3Pack);
OP2_MO_TYPE_FOR_DEF(mapScout,                      Scout);
OP2_MO_TYPE_FOR_DEF(mapSensorPackage,              SensorPackage);
OP2_MO_TYPE_FOR_DEF(mapSkydock,                    Skydock);
OP2_MO_TYPE_FOR_DEF(mapSmallCapacityAirTransport,  SmallCapacityAirTransport);
OP2_MO_TYPE_FOR_DEF(mapSolarPowerArray,            SolarPowerArray);
OP2_MO_TYPE_FOR_DEF(mapSolarSatellite,             SolarSatellite);
OP2_MO_TYPE_FOR_DEF(mapSpaceport,                  Spaceport);
OP2_MO_TYPE_FOR_DEF(mapSpider,                     Spider);
OP2_MO_TYPE_FOR_DEF(mapSpider3Pack,                Spider3Pack);
OP2_MO_TYPE_FOR_DEF(mapStandardLab,                StandardLab);
OP2_MO_TYPE_FOR_DEF(mapStasisSystems,              StasisSystems);
OP2_MO_TYPE_FOR_DEF(mapStructureFactory,           StructureFactory);
OP2_MO_TYPE_FOR_DEF(mapSULV,                       SULV);
OP2_MO_TYPE_FOR_DEF(mapTiger,                      Tiger);
OP2_MO_TYPE_FOR_DEF(mapTokamak,                    Tokamak);
OP2_MO_TYPE_FOR_DEF(mapTradeCenter,                TradeCenter);
OP2_MO_TYPE_FOR_DEF(mapTube,                       Tube);
OP2_MO_TYPE_FOR_DEF(mapUniversity,                 University);
OP2_MO_TYPE_FOR_DEF(mapVehicleFactory,             VehicleFactory);
OP2_MO_TYPE_FOR_DEF(mapWall,                       Wall);
OP2_MO_TYPE_FOR_DEF(mapWares,                      Wares);
OP2_MO_TYPE_FOR_DEF(mapAcidCloud,                  AcidCloud);
OP2_MO_TYPE_FOR_DEF(mapAmbientAnimation,           AmbientAnimation);
OP2_MO_TYPE_FOR_DEF(mapBFG,                        BFG);
OP2_MO_TYPE_FOR_DEF(mapEMPBlast,                   EMPBlast);
OP2_MO_TYPE_FOR_DEF(mapEarthquake,                 Earthquake);
OP2_MO_TYPE_FOR_DEF(mapEMP,                        EMP);
OP2_MO_TYPE_FOR_DEF(mapEnergyCannon,               EnergyCannon);
OP2_MO_TYPE_FOR_DEF(mapEruption,                   Eruption);
OP2_MO_TYPE_FOR_DEF(mapESG,                        ESG);
OP2_MO_TYPE_FOR_DEF(mapFumarole,                   Fumarole);
OP2_MO_TYPE_FOR_DEF(mapInterColonyShuttle,         InterColonyShuttle);
OP2_MO_TYPE_FOR_DEF(mapLargeBuildingExplosion,     LargeBuildingExplosion);
OP2_MO_TYPE_FOR_DEF(mapLaser,                      Laser);
OP2_MO_TYPE_FOR_DEF(mapLightning,                  Lightning);
OP2_MO_TYPE_FOR_DEF(mapMagmaVent,                  MagmaVent);
OP2_MO_TYPE_FOR_DEF(mapMediumBuildingExplosion,    MediumBuildingExplosion);
OP2_MO_TYPE_FOR_DEF(mapMeteor,                     Meteor);
OP2_MO_TYPE_FOR_DEF(mapMicrowave,                  Microwave);
OP2_MO_TYPE_FOR_DEF(mapMiningBeacon,               MiningBeacon);
OP2_MO_TYPE_FOR_DEF(mapSelfDestruct,               SelfDestruct);
OP2_MO_TYPE_FOR_DEF(mapRailGun,                    RailGun);
OP2_MO_TYPE_FOR_DEF(mapRPG,                        RPG);
OP2_MO_TYPE_FOR_DEF(mapSmallBuildingExplosion,     SmallBuildingExplosion);
OP2_MO_TYPE_FOR_DEF(mapStarflare,                  Starflare);
OP2_MO_TYPE_FOR_DEF(mapStarflareGP,                StarflareGP);
OP2_MO_TYPE_FOR_DEF(mapStickyfoam,                 Stickyfoam);
OP2_MO_TYPE_FOR_DEF(mapSupernova,                  Supernova);
OP2_MO_TYPE_FOR_DEF(mapSupernovaGP,                SupernovaGP);
OP2_MO_TYPE_FOR_DEF(mapThorsHammer,                ThorsHammer);
OP2_MO_TYPE_FOR_DEF(mapVortex,                     Vortex);
OP2_MO_TYPE_FOR_DEF(mapWreckage,                   Wreckage);
} // Impl

/// Template alias to get the MapObjectType subclass associated with the given MapID.
template <MapID ID>  using MapObjTypeFor = typename TethysImpl::MapObjTypeForImpl<ID>::Type;
