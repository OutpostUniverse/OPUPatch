
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Game/PlayerImpl.h"
#include "Tethys/Game/CommandPacket.h"
#include "Tethys/Game/GameStartInfo.h"

namespace Tethys {

class TFileDialog;
class GameNetLayer;
class StreamIO;

/// Internal game manager class (wrapped by TethysGame).
class GameImpl : public OP2Class<GameImpl> {
public:
   GameImpl() { InternalCtor<0x489130>(); }
  ~GameImpl() { Deinit();                 }

  ibool PrepareGame(GameStartInfo* pGameStartInfo) { return Thunk<0x4893B0, &$::PrepareGame>(pGameStartInfo); }

  ibool           LoadCampaignState(GameStartInfo* pGameStartInfo)
    { return Thunk<0x4899C0, &$::LoadCampaignState>(pGameStartInfo); }
  MissionResults* GetMissionResults() { return Thunk<0x489B00, &$::GetMissionResults>(); }
  void            InitPlayers()       { return Thunk<0x489E50, &$::InitPlayers>();       }

  void ProcessGameCycle() { return Thunk<0x489EA0, &$::ProcessGameCycle>(); }

  void Deinit() { return Thunk<0x48A080, &$::Deinit>(); }

  ibool SaveSelf(StreamIO* pSavedGame) { return Thunk<0x48A150, &$::SaveSelf>(pSavedGame); }
  ibool LoadSelf(StreamIO* pSavedGame) { return Thunk<0x48A170, &$::LoadSelf>(pSavedGame); }

  ibool VerifySavedGameFileTag(StreamIO* pSavedGame) { return Thunk<0x48A190, &$::VerifySavedGameFileTag>(pSavedGame); }

  ibool SaveGame(StreamIO* pSavedGame, TFileDialog* pFileDialog = nullptr)
    { return Thunk<0x48A1F0, &$::SaveGame>(pSavedGame, pFileDialog); }
  ibool LoadGame(StreamIO* pSavedGame) { return Thunk<0x48A3E0, &$::LoadGame>(pSavedGame); }

  void LocalPlayerQuit(QuitMethod method, uint8 delay) { return Thunk<0x48A6A0, &$::LocalPlayerQuit>(method, delay); }
  void PostQuitMessage(QuitMethod method, uint8 delay) { return Thunk<0x48A6A0, &$::PostQuitMessage>(method, delay); }

  void ProcessGameOptPacket(const CommandPacket& packet) { return Thunk<0x422B00, &$::ProcessGameOptPacket>(packet); }

  ibool LoadDebugMap()   { return Thunk<0x489170, &$::LoadDebugMap>();   }
  void  InitDebugFlags() { return Thunk<0x422AE0, &$::InitDebugFlags>(); }

  PlayerImpl* GetPlayerArray() { return (this == GetInstance()) ? OP2Mem<PlayerImpl*&>(0x4890C3) : &player_[0]; }

  PlayerImpl* GetPlayer(int playerNum)
    { return ((playerNum >= 0) && (playerNum < MaxPlayers)) ? &GetPlayerArray()[playerNum] : nullptr; }

  /// Gets the global GameImpl object instance.
  static GameImpl* GetInstance() { return OP2Mem<0x56EA98, GameImpl*>(); }

public:
  ibool unlimitedResources_;
  ibool produceAll_;
  ibool logMorale_;
  ibool quadDamage_;
  ibool fastUnits_;
  ibool fastProduction_;
  ibool showUnitPaths_;
  ibool allUnitsVisible_;
  int   field_20;
  int   field_24;
  int   field_28;
  int   field_2C;
  int   field_30;
  int   field_34;
  ibool forceDisableRCC_;
  ibool forceEnableRCC_;

  ibool dataChecking_;  ///< Presumed from the demo menus?
  ibool strictMode_;    ///< Presumed from the demo menus?

  ibool daylightMoves_;
  ibool daylightEverywhere_;

  int gameSpeed_;  ///< Game speed setting in UI * 4

  PlayerBitmask forceMorale_[size_t(MoraleLevel::Count)];

  int numPlayers_;
  int numHumanPlayers_;

  int commandPacketProcessingInterval_;
  int log2CommandPacketProcessingInterval_;
  int networkCommandPacketArraySpace_;
  int commandPacketProcessingTick_;
  int commandPacketProcessingDelay_;

  int tick_;
  int tickOfLastSetGameOpt_;  ///< If nonzero, "CHEATED GAME!" is displayed in the top left corner of the detail pane
  int field_8C;

  int           localPlayer_;
  PlayerBitmask chatDstMask_;

  int             startFadeOutTick_;
  GameTermReasons gameTermReasons_;
  ibool           skipProgressSave_;
  GameStartInfo   gameStartInfo_;

  int field_160[201];  // ** TODO

  PlayerImpl player_[MaxPlayers];

  GameNetLayer* pGameNetLayer_;
};

inline auto& g_gameImpl = *GameImpl::GetInstance();

} // Tethys
