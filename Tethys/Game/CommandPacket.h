
#pragma once

#include "Tethys/Common/Util.h"
#include "Tethys/API/Location.h"

namespace Tethys {

// ** TODO This interface will change substantially when a CmdPacketBuilder/Parser is made.
// SingleUnitSimpleCommand, SimpleCommand, and MoveCommand will be changed to be empty types, so instantiating packet
// structs manually will be broken.

namespace MapIDImpl { enum MapID : int; }
using MapID = MapIDImpl::MapID;

constexpr int AllPlayers = -1;

constexpr size_t CommandPacketSize     = 113;
constexpr size_t CommandPacketDataSize = CommandPacketSize - 0x0E;

BEGIN_PACKED

/// Command packet types.  @note int in some contexts (CommandPacket), uint8 in others (MapObject)
enum class CommandType : int {
  Nop = 0x00,       ///< 'ctNop'
  Doze,             ///< 'ctMoDoze'
  Move,             ///< 'ctMoMove'
  Dock,             ///< 'ctMoDock'
  DockEG,           ///< 'ctMoDockEG';  Garage-specific dock command.
  Stop,             ///< 'ctMoStop'
  Build,            ///< 'ctMoBuild'
  BuildWall,        ///< 'ctMoBuildWall'
  RemoveWall,       ///< 'ctMoRemoveWall'
  Produce,          ///< 'ctMoProduce'
  TransferCargo,    ///< 'ctMoTransferCargo'
  LoadCargo,        ///< 'ctMoLoadCargo'
  UnloadCargo,      ///< 'ctMoUnloadCargo'
  Recycle,          ///< 'ctMoRecycle'
  DumpCargo,        ///< 'ctMoDumpCargo'
  Scavenge,         ///< 'ctMoScavenge'
  SpecialWait,      ///< 'ctMoSpecialWait'
  Survey,           ///< 'ctMoSurvey'
  Idle,             ///< 'ctMoIdle'
  Unidle,           ///< 'ctMoUnidle'
  SelfDestruct,     ///< 'ctMoSelfDestruct'
  Scatter,          ///< 'ctMoScatter'
  Research,         ///< 'ctMoResearch'
  TrainScientists,  ///< 'ctMoTrainScientists'
  Transfer,         ///< 'ctMoTransfer'
  Launch,           ///< 'ctMoLaunch'
  FlyInSpace,       ///< 'ctMoFlyInSpace'
  Repair,           ///< 'ctMoRepair'
  RepairObj,        ///< 'ctMoRepairObj'
  Reprogram,        ///< 'ctMoReprogram'
  Dismantle,        ///< 'ctMoDismantle';  ConVec performing dismantle
  Salvage,          ///< 'ctMoSalvage'
  Create,           ///< 'ctMoCreate'
  Develop,          ///< 'ctMoDevelop';    Building under construction
  UnDevelop,        ///< 'ctMoUnDevelop';  Building being dismantled
  LightToggle,      ///< 'ctMoLightToggle'
  Attack,           ///< 'ctMoAttackObj'
  Guard,            ///< 'ctMoGuard'
  StandGround,      ///< 'ctMoStandGround'
  CargoRoute,       ///< 'ctMoCargoRoute'
  Patrol,           ///< 'ctMoPatrol'
  MapChange,        ///< 'ctMapChange';  Unused.
  Poof,             ///< 'ctMoPoof'
  GameOpt,          ///< 'ctGameOpt'
  GodWeapon,        ///< 'ctGodWeapon';  Unused.
  ChatText,         ///< 'ctChatText';   Unused.
  ChatSFX,          ///< 'ctChatSFX';    Unused.
  Death,            ///< 'ctMoDeath'
  Chat,             ///< 'ctChat'
  Quit,             ///< 'ctQuit'
  Ally,             ///< 'ctAlly'
  GoAI,             ///< 'ctGoAI'
  MachineSettings,  ///< 'ctMachineSettings'
  InvalidCommand,   ///< 'ctInvalidCommand'

  Count
};

/// Used by ctGameOpt
enum class GameOpt : uint16 {
  UnlimitedResources = 0,  ///< Unlimited storage/power cheat.
  ProduceAll,              ///< All unit types unlocked/all beacons surveyed/research disabled cheat.
  LogMoraleOutput,         ///< Debug log to morale.log.
  QuadDamage,              ///< Quad damage cheat.
  FastUnits,               ///< Fast units cheat.
  InstantKitBuild,         ///< Instant build cheat.
  ShowUnitPaths,           ///< Shows debug unit pathfinding routes.
  AllUnitsVisible,         ///< All units visible cheat (always visible on minimap regardless of headlights).
  // ** TODO Unclear if the ones above can be set?

  CommonOre,
  RareOre,
  Scientists,
  Workers,
  FoodStored,
  Kids,

  ForceDisableRCCEffect,
  ForceEnableRCCEffect,

  // 0x10  GameImpl::dataChecking_?
  // 0x11  GameImpl::strictMode_?

  DaylightMoves = 0x12,
  DaylightEverywhere,          ///< @note Does not trigger "cheated game!"

  GameSpeed,                   ///< Value = UI game speed * 4.  @note Does not trigger "cheated game!"

  ForceMoraleExcellent,
  ForceMoraleGood,
  ForceMoraleFair,
  ForceMoralePoor,
  ForceMoraleTerrible,
  
  IncreasePlayerResources,     ///< Give workers/scientists/kids/common/rare/food to one player.
  IncreaseAllPlayerResources,  ///< Give workers/scientists/kids/common/rare/food to all players.

  CreateVortex,
  CreateLightning,
  CreateEarthquake,
  CreateMeteor,

  FreeMoraleLevel,             ///< @note Does not trigger "cheated game!"

  // The ones below here are not "intended" game opts but happen to work since the ctGameOpt handler allows writing to
  // arbitrary memory in GameImpl
  Tick,
  TickOfLastSetGameOpt,        ///< @note Immediately gets overwritten if used as a GameOpt.
  // 0x23  GameImpl::field_8C
  LocalPlayer = 0x24,
  ChatDstMask,
  StartFadeOutTick,
  GameTermReasons,
  SkipProgressSave,
  // 0x29...
};

enum class QuitMethod : uint8 {
  SelfDestruct = 0,
  Transfer,
  GoAI
};


namespace TethysImpl {
/// Bitmask of player IDs.
template <typename BitmaskType>
union PlayerBitmaskImpl {
  operator       BitmaskType&()       { return mask; }  ///< Allows bitwise, assignment, etc. operators.
  operator const BitmaskType&() const { return mask; }  ///< Allows bitwise, assignment, etc. operators.

  bool Get(int player) const       { return (mask & (1u << player)) != 0;                        }
  void Set(int player, bool state) { return TethysUtil::SetBitFlag(mask, (1u << player), state); }

  const bool operator[](int player) const { return Get(player); }  ///< Read-only index operator.

  struct {
    BitmaskType player0 : 1;
    BitmaskType player1 : 1;
    BitmaskType player2 : 1;
    BitmaskType player3 : 1;
    BitmaskType player4 : 1;
    BitmaskType player5 : 1;
    BitmaskType player6 : 1;
    BitmaskType         : ((sizeof(BitmaskType) * 8) - 7);
  };
  BitmaskType mask;
};
} // TethysImpl

using PlayerBitmask       = TethysImpl::PlayerBitmaskImpl<uint32>;  ///< 32-bit mask of player IDs.
using PackedPlayerBitmask = TethysImpl::PlayerBitmaskImpl<uint8>;   ///<  8-bit mask of player IDs.


/// Used by some commands to indicate either a tile or unit target.
struct CommandTarget {
  union {
    uint16 tileX;
    uint16 unitID;
  };
  uint16 tileY;  ///< Set to -1 if target is unit
};


/// Has a single unit ID.  May be used as a header for other command packet data types.
/// Used with single unit commands (ctMoLoadCargo, ctMoUnloadCargo, ctMoDumpCargo, ctMoIdle, ctMoUnidle, ctMoPoof, ...)
struct SingleUnitSimpleCommand {
  uint16 unitID;
};

/// Has an array of unit IDs.  May be used as a header for other command packet data types.
/// Used with DoSimpleCommand (ctMoStop, ctMoSelfDestruct, ctMoScatter, ...)
struct SimpleCommand {
  /// Gets the size in bytes of this struct.
  /// To get the address of a subclass's data, this value can be used as the offset into CommandPacketData.
  size_t GetSimpleCommandSize() const { return OP2Thunk<0x4102C0, uint32 FASTCALL(const void*)>(this); }

  uint8  numUnits;
  uint16 unitID[1];
};

/// Has an array of unit IDs and an array of Waypoints.  May be used as a header for other command packet data types.
/// Used with ctMoMove, ctMoDock, ctMoDockEG, ctMoStandGround, ctMoCargoRoute, ctMoPatrol
struct MoveCommand : public SimpleCommand {
  uint16   numWaypoints;
  Waypoint waypoint[1];  ///< In pixels
};

/// Used with ctMoDoze
struct DozeCommand : public SimpleCommand {
  PackedMapRect rect;  ///< In tiles (inclusive)
};

/// Used with ctMoBuild
struct BuildCommand : public MoveCommand {
  PackedMapRect rect;     ///< In tiles (inclusive)
  uint16        unknown;  ///< Stored to MapObj+0x6A;  typically set to -1
};

/// Used with ctMoBuildWall
struct BuildWallCommand : public SimpleCommand {
  PackedMapRect rect;          ///< In tiles (top-left inclusive, but bottom-right not inclusive!)
  uint16        tubeWallType;  ///< MapID
  uint16        unknown;       ///< Set to 0
};

/// Used with ctMoRemoveWall
struct RemoveWallCommand : public MoveCommand {
  PackedMapRect rect;  ///< In tiles (inclusive)
};

/// Used with ctMoProduce
struct ProduceCommand : public SingleUnitSimpleCommand {
  uint16 itemToProduce;  ///< MapID
  uint16 weaponType;     ///< MapID
  uint16 scGroupIndex;   ///< -1 = none
};

/// Used with ctMoTransfer
struct TransferCommand : public SimpleCommand {
  uint16 toPlayerNum;
};

/// Used with ctMoTransferCargo
struct TransferCargoCommand : public SingleUnitSimpleCommand {
  uint16 bay;
  uint16 unknown;  ///< 0
};

/// Used with ctMoRecycle
struct RecycleCommand : public SingleUnitSimpleCommand {
  uint16 unknown;
};

/// Used with ctMoResearch
struct ResearchCommand : public SingleUnitSimpleCommand {
  uint16 techNum;
  uint16 numScientists;
};

/// Used with ctMoTrainScientists
struct TrainScientistsCommand : public SingleUnitSimpleCommand {
  uint16 numScientists;
};

/// Used with ctMoLaunch
struct LaunchCommand : public SingleUnitSimpleCommand {
  uint16 targetPixelX;
  uint16 targetPixelY;
};

/// Used with ctMoRepairObj, ctMoReprogram, ctMoDismantle
struct RepairCommand : public SimpleCommand {
  uint16        unknown1;      ///< 0
  CommandTarget target;
};

/// Used with ctMoSalvage
struct SalvageCommand : public SingleUnitSimpleCommand {
  PackedMapRect rect;        ///< In tiles (inclusive)
  uint16        unitIDGorf;
};

/// Used with ctMoCreate
struct CreateCommand {
  uint16 numUnits;
  struct {
    MapID  unitType;
    uint16 tileX;
    uint16 tileY;
    MapID  weaponOrCargo;
  } unit[8];
};

/// Used with ctMoLightToggle
struct LightToggleCommand : public SimpleCommand {
  uint16 headlightState;
};

/// Used with ctMoAttackObj, ctMoGuard
struct AttackCommand : public SimpleCommand {
  uint16        unknown;  ///< 0
  CommandTarget target;
};

/// Used by ctGameOpt
struct GameOptCommand {
  uint16    unknown;         ///< Uninitialized, unused
  union {
    GameOpt variableID;
    uint16  offsetInDwords;  ///< Dword offset into GameImpl instance
  };
  uint16    value;           ///< Param 1
  uint16    playerNum;       ///< Param 2
};

/// Used by ctChat
struct ChatCommand {
  uint8               playerID;
  PackedPlayerBitmask dstPlayerMask;
  char                message[CommandPacketDataSize - (sizeof(uint8) * 2)];
};

/// Used by ctQuit
struct QuitCommand {
  QuitMethod quitMethod;
  uint8      delay;       ///< Typically 2
};

/// Used by ctAlly
struct AllyCommand {
  uint16 fromPlayerID;
  uint16 toPlayerID;
};

/// Used by ctMachineSettings
struct MachineSettingsCommand {
  uint16 cpuSpeed;
  uint16 memoryMB;      ///< Rounded to multiple of 4
  uint16 windowWidth;
  uint16 windowHeight;
};
using MachineSettings = MachineSettingsCommand;

/// ctGoAI, ctInvalidCommand do not use a message

union CommandPacketData {
  SimpleCommand           simple;
  SingleUnitSimpleCommand singleUnitSimple;
  DozeCommand             doze;
  MoveCommand             move;
  BuildCommand            build;
  BuildWallCommand        buildWall;
  RemoveWallCommand       removeWall;
  ProduceCommand          produce;
  TransferCommand         transfer;
  TransferCargoCommand    transferCargo;
  RecycleCommand          recycle;
  ResearchCommand         research;
  TrainScientistsCommand  trainScientists;
  LaunchCommand           launch;
  RepairCommand           repair;
  SalvageCommand          salvage;
  CreateCommand           create;
  LightToggleCommand      lightToggle;
  AttackCommand           attack;
  GameOptCommand          gameOpt;
  ChatCommand             chat;
  QuitCommand             quit;
  AllyCommand             ally;
  MachineSettingsCommand  machineSettings;

  uint8 buffer[CommandPacketDataSize];  ///< +0x0E Dependent on message type
};

struct CommandPacket {
  CommandType       type;        ///< Type of command
  uint16            dataLength;  ///< Length of dataBuff
  int               timeStamp;   ///< Game tick (only used for network traffic)
  int               netID;       ///< Player net ID (only used for network traffic)
  CommandPacketData data;        ///< Message data, dependent on type
};
static_assert(sizeof(CommandPacket) == CommandPacketSize, "Incorrect CommandPacket size.");

END_PACKED


namespace TethysImpl {
template <CommandType>                                      struct CmdPacketDataForImpl      { using Type = void; };
#define OP2_CMD_PACKET_DATA_FOR_DEF(cmd, type)  template <> struct CmdPacketDataForImpl<cmd> { using Type = type; };
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Nop,             void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Doze,            DozeCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Move,            MoveCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Dock,            MoveCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::DockEG,          MoveCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Stop,            SimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Build,           BuildCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::BuildWall,       BuildWallCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::RemoveWall,      RemoveWallCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Produce,         ProduceCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::TransferCargo,   TransferCargoCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::LoadCargo,       SingleUnitSimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::UnloadCargo,     SingleUnitSimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Recycle,         RecycleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::DumpCargo,       SingleUnitSimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Scavenge,        void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::SpecialWait,     void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Survey,          void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Idle,            SingleUnitSimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Unidle,          SingleUnitSimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::SelfDestruct,    SimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Scatter,         SimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Research,        ResearchCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::TrainScientists, TrainScientistsCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Transfer,        TransferCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Launch,          LaunchCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::FlyInSpace,      void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Repair,          void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::RepairObj,       RepairCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Reprogram,       RepairCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Dismantle,       RepairCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Salvage,         SalvageCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Create,          CreateCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Develop,         void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::UnDevelop,       void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::LightToggle,     LightToggleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Attack,          AttackCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Guard,           AttackCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::StandGround,     MoveCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::CargoRoute,      MoveCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Patrol,          MoveCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Poof,            SingleUnitSimpleCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::GameOpt,         GameOptCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::GodWeapon,       void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::ChatText,        void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::ChatSFX,         void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Death,           void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Chat,            ChatCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Quit,            QuitCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::Ally,            AllyCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::GoAI,            void);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::MachineSettings, MachineSettingsCommand);
OP2_CMD_PACKET_DATA_FOR_DEF(CommandType::InvalidCommand,  void);
}  // Impl

/// Template alias to get the command packet data class associated with the given CommandType.
template <CommandType Command>  using CmdPacketDataFor = typename TethysImpl::CmdPacketDataForImpl<Command>::Type;


class CmdPacketWriter {
public:
  // ** TODO

public:
  CommandPacket packet_;
  uint8*        pDataWriter_;
  uint16        units_[32];
  size_t        numUnits_;
  Waypoint      waypoints_[8];
  size_t        numWaypoints_;
};

} // Tethys
