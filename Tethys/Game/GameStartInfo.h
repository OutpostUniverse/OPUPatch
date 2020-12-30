
#pragma once

#include "Tethys/Game/CommandPacket.h"

namespace Tethys {

namespace TethysAPI { enum class MissionType : int; }

BEGIN_PACKED
enum class GameTermReasons : int {
  Running = 0,          ///< Game still running
  U1,
  U2,
  MissionAccomplished,  ///< Single player win
  MissionFailed,        ///< Single player loss
  U5,
  MissionAborted,       ///< Single player abort
  Victory,              ///< Multiplayer win
  Defeat,               ///< Multiplayer loss
  ControlTerminated,    ///< Multiplayer abort
  AutoDemo,             ///< Auto demo
  AutoDemo2,            ///< Auto demo
  GameOver
};

struct StartupFlags {
  uint32 disastersEnabled   : 1;
  uint32 dayNightEnabled    : 1;
  uint32 moraleEnabled      : 1;
  uint32 isCampaign         : 1;
  uint32 isMultiplayer      : 1;
  uint32 cheatsEnabled      : 1;
  uint32 maxPlayers         : 3;
  uint32 missionType        : 5;
  uint32 b1                 : 3;
  uint32 numInitialVehicles : 4;
};

struct GameFlags {
  uint32 numPlayers : 3;
  uint32 isReplay   : 1;
};

struct PlayerFlags {
  uint32 b1        : 1;
  uint32 isHost    : 1;
  uint32 isEden    : 1;
  uint32 resources : 2;
  uint32 color     : 3;
};

struct GameStartInfo {
  StartupFlags startupFlags;
  int          version;
  uint32       field_08;
  GameFlags    gameFlags;
  char         scriptName[32];
  PlayerFlags  playerFlags[6];
  int          playerNetID[6];
  struct {
    char       str[13];
  }            playerName[6];
  int          gameSpeed;         ///< UI value * 4
  uint32       randomNumberSeed;
  uint32       checksum;
};
static_assert(sizeof(GameStartInfo) == 0xBA, "Incorrect GameStartInfo size.");

struct PlayerEndInfo {
  int structuresBuilt;
  int vehiclesBuilt;
  int structuresLost;
  int vehiclesLost;
  int enemyStructuresDestroyed;
  int enemyVehiclesDestroyed;
  int oreHarvested;
};

struct SatelliteCounts {
  uint32 numRLV             : 4;
  uint32 numSolarSatellites : 4;
  uint32 b1                 : 1;
  uint32 numEDWARDSatllites : 4;
};

struct PlayerStartInfo {
  int             food;
  int             commonOre;
  int             rareOre;
  int             workers;
  int             scientists;
  int             kids;
  SatelliteCounts satelliteCounts;
};

enum class MoraleLevels : int {
  Excellent = 0,  ///< 'Great'
  Good,           ///< 'Good'
  Fair,           ///< 'OK'
  Poor,           ///< 'Poor'
  Terrible,       ///< 'Rotten'
  Count
};
using MoraleLevel = MoraleLevels;

enum class FoodStatus : int {
  Rising = 0,
  NoChange,    ///< Pretty hard, if not impossible to achieve with food
  Falling,
  Shortage,
  Count
};

struct MoraleState {
  MoraleLevel moraleLevel;
  int         eventMoraleModifier;
  int         field_08;
  int         morale;
  int         field_10;
  int         residenceDemandPercent;
  FoodStatus  foodSupply;
  int         disabledBuildingRatio;
  int         recForumDemandPercent;
  int         field_24;
  int         medCenterDemandPercent;
  ibool       isNurseryOperational;
  ibool       isUniversityOperational;
  int         numDIRTs;
  int         dirtAvgDamagePrevention;
  int         unoccupiedColonistsPercent;
  int         scientistsAsWorkersPercent;
  ibool       isGORFOperational;
};

struct ResearchState {
  int                 numTechs;
  uint16              techLevel;
  uint8               techNum[256];
  PackedPlayerBitmask playerBitVector[256];
};

struct MissionResults : public GameStartInfo {
  GameTermReasons        gameTermReason;
  uint32                 field_BE;
  PlayerEndInfo          playerEndInfo[6];
  TethysAPI::MissionType missionType;       ///< Mission type (negative values) or campaign mission number (positive values)
  PlayerStartInfo        playerStartInfo;   ///< Start info for local player
  uint8                  localPlayerID;
  int                    tick;
  MoraleState            moraleState;
  ResearchState          researchState;
};
static_assert(sizeof(MissionResults) == 0x3DD, "Incorrect MissionResults size.");

END_PACKED

} // Tethys
