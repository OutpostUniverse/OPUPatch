
/// Mission DLLs should include this header.
/// Mission DLLs are required to export the following functions and global data, at minimum:
///
/// EXPORT_OP2_MULTIPLAYER_SCRIPT("Mission Name", missionType, numHumans, "map_file.map", "tech_file.txt"(, numAIs))
///   ** OR **
/// EXPORT_OP2_MISSION_SCRIPT("Mission Name", missionType, numPlayers, "map_file.map", "tech_file.txt", maxTechLevel,
///                           isUnitMission(, numMultiplayerAIs))
///
/// MISSION_API int  InitProc()   { return 1; }        // Set up bases, triggers, etc. here.  Return 1 = OK, 0 = failure
/// MISSION_API void AIProc()     {           }        // Gets called every 4 ticks during gameplay
/// MISSION_API int  StatusProc() { return 0; }        // Must return 0
/// MISSION_API void GetSaveRegions(SaveRegion* pSave) // Single-player maps must set pSave to point at a buffer to save
///   { pSave->pData = nullptr;  pSave->size = 0; }

#pragma once

#include "Tethys/Common/Types.h"

namespace Tethys {

#ifndef MISSION_API
# define MISSION_API DLLAPI
#endif  // MISSION_API

/// Macro that defines and exports all global data required for Outpost 2 to interface with a mission DLL.
/// numMultiplayerAIs may be provided as an optional argument.
/// Most multiplayer missions can use the simplified @ref EXPORT_OP2_MULTIPLAYER_SCRIPT macro instead.
#define EXPORT_OP2_MISSION_SCRIPT(                                                                   \
    nameDesc, missionType, numPlayers, mapFilename, techFilename, maxTechLevel, isUnitMission, ...)  \
  MISSION_API char      LevelDesc[]    = nameDesc;                                                   \
  MISSION_API char      MapName[]      = mapFilename;                                                \
  MISSION_API char      TechtreeName[] = techFilename;                                               \
  MISSION_API ModDesc   DescBlock      = { missionType, numPlayers, maxTechLevel, isUnitMission };   \
  MISSION_API ModDescEx DescBlockEx    = { __VA_ARGS__ };

/// Macro that defines a typical Outpost 2 multiplayer mission DLL, with max tech level = 12 and isUnitMission = false.
/// numMultiplayerAIs may be provided as an optional argument.
#define EXPORT_OP2_MULTIPLAYER_SCRIPT(nameDesc, missionType, numHumans, mapFilename, techFilename, ...)  \
  EXPORT_OP2_MISSION_SCRIPT(nameDesc, missionType, numHumans, mapFilename, techFilename, 12, false, __VA_ARGS__)

/// Enum defining mission types.
enum class MissionType : int {
  // Single-player mission types
  Campaign1       =   1,
  Campaign2       =   2,
  Campaign3       =   3,
  Campaign4       =   4,
  Campaign5       =   5,
  Campaign6       =   6,
  Campaign7       =   7,
  Campaign8       =   8,
  Campaign9       =   9,
  Campaign10      =  10,
  Campaign11      =  11,
  Campaign12      =  12,

  Colony          =  -1,
  AutoDemo        =  -2,
  Tutorial        =  -3,

  // Multiplayer mission types
  LandRush        =  -4,
  SpaceRace       =  -5,
  ResourceRace    =  -6,
  Midas           =  -7,
  LastOneStanding =  -8
};

struct ModDesc {
  constexpr ModDesc() : missionType(), numPlayers(), maxTechLevel(), unitMission() { }
  constexpr ModDesc(MissionType missionType, int numPlayers, int maxTechLevel = 12, ibool unitMission = false)
    : missionType(missionType), numPlayers(numPlayers), maxTechLevel(maxTechLevel), unitMission(unitMission) { }
  constexpr ModDesc(int missionType, int numPlayers, int maxTechLevel = 12, ibool unitMission = false)
    : missionType{missionType}, numPlayers(numPlayers), maxTechLevel(maxTechLevel), unitMission(unitMission) { }

  MissionType missionType;      ///< Campaign mission (positive values) or other mission type (negative values).
  int         numPlayers;       ///< Number of players (1-6), including AIs on single-player maps, but excluding
                                ///  them on multiplayer maps;  @see AIModDescEx::numMultiplayerAISlots.
  int         maxTechLevel;     ///< Maximum tech level.  Set to 12 to enable all techs for standard tech trees.
  ibool       unitMission;      ///< Set to 1 to disable most reports.  Suitable for unit-only missions.
};

/// Used for multiplayer missions only.
struct ModDescEx {
  /// The number of extra player slots to initialize for multiplayer AI (besides gaia).
  /// Not setting this correctly can cause weird bugs with things like alliances, player color/colony, etc.
  int numMultiplayerAIs;

  int field_04;
  int field_08;
  int field_0C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
};

/// Used by GetSaveRegions() to determine the regions that need to be read/written.
struct SaveRegion {
  void*  pData;
  size_t size;
};

} // Tethys
