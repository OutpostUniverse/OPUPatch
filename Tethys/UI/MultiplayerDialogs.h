
#pragma once

#include "Tethys/API/Mission.h"
#include "Tethys/Game/GameStartInfo.h"
#include "Tethys/UI/IWnd.h"

namespace Tethys {

struct HostGameParameters {
  StartupFlags startupFlags;
  int          unused[2];
  char         gameCreatorName[13];
};

struct PlayerControls {
  HWND hPlayerNameTextBoxWnd;
  HWND hColorComboBoxWnd;
  HWND hColonyCheckBoxWnd;     ///< 0 = Plymouth, 1 = Eden
  HWND hResourceComboBoxWnd;
  HWND hReadyCheckBoxWnd;
  HWND hEjectButtonWnd;
};

class MultiplayerLobbyDialog : public IDlgWnd {
  using $ = MultiplayerLobbyDialog;
public:
  MultiplayerLobbyDialog() : IDlgWnd(UseInternalCtorChain) { InternalCtor<0x45F050>(); }

  void* Destroy(ibool freeMem = 0)                       override { return Thunk<0x45F080, &$::Destroy>(freeMem); }
  int   DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    { return Thunk<0x460BC0, &$::DlgProc>(uMsg, wParam, lParam); }

  ibool ShowHostGame(const HostGameParameters& hostGameParameters)
    { return Thunk<0x45F0D0, &$::ShowHostGame>(hostGameParameters); }
  ibool ShowJoinGame(const char* pPlayerName, int hostPlayerNetID, ibool purgeDroppedPlayers)
    { return Thunk<0x45F2F0, &$::ShowJoinGame>(pPlayerName, hostPlayerNetID, purgeDroppedPlayers); }

  // ** TODO 0x45F420

  static int FASTCALL PopulateMissionDropdown(
    HWND hCBWnd, int maxPlayers, API::MissionType maxMissionType, API::MissionType minMissionType)
      { return OP2Thunk<0x497780, &$::PopulateMissionDropdown>(hCBWnd, maxPlayers, maxMissionType, minMissionType); }

public:
  UINT           timerID_;
  HWND           hEnterChatTextBoxWnd_;
  WNDPROC        lpPrevWndFunc_;
  HWND           hMessageTextBoxWnd_;
  HWND           hLevelComboBoxWnd_;
  HWND           hStartGameButtonWnd_;
  PlayerControls playerControls_[6];

  GameStartInfo gameStartInfo_;

  uint16 field_182;
  int    field_184[3];

  ibool purgeDroppedPlayers_;
  ibool needRedraw_;
  ibool gameStarting_;
  int   time_;
  int   hostPlayerNetID_;
  int   missionType1_;
  int   missionType2_;
};

} // Tethys
