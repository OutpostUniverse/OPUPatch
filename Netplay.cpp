
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>

#include "Patcher.h"
#include "Util.h"
#include "Resources.h"

#include "Tethys/API/TethysGame.h"
#include "Tethys/Game/TApp.h"
#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/MultiplayerDialogs.h"
#include "Tethys/Resource/ResManager.h"
#include "Tethys/Network/NetGameSession.h"
#include "Tethys/Network/NetGameProtocol.h"
#include "Tethys/Network/NetTransportLayer.h"
#include "Tethys/Common/Library.h"

#include <string>

using namespace Tethys;
using namespace TethysAPI;
using namespace Patcher::Util;
using namespace Patcher::Registers;

// =====================================================================================================================
// Fixes replicating players list always failing when the system's LAN is behind a NAT layer.
// While this does fix a hard NAT-related bug, it by itself does not implement any port forwarding or NAT punchthrough.
bool SetNatFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In Network::ProcessProtocolControlPacket()
    // ** TODO Come up with a definition for NetTransportLayer::playerNetAddrList_ instead of using [8345]
    patcher.LowLevelHook(0x4972C3, [](Ebp<uint32*> pNetTransportLayer, Esp<void*> pEsp)
      { pNetTransportLayer[8345] = (*PtrInc<sockaddr_in**>(pEsp, 20))->sin_addr.s_addr; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Bypasses the behavior where game speed is halved in netplay, so multiplayer games run as fast as single player.
bool SetNetGameSpeedPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In GameFrame::OnIdle()
    success = (patcher.WriteNop(0x49C3EC) == PatcherStatus::Ok);  // ++log2CommandPacketProcessingInterval => nop
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes network code to attach to all network adapters instead of only the highest-priority one.
bool SetBindAnyNetAdapterFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Patch all calls to bind().
    for (uintptr loc : { 0x48C0FE, 0x48C12B, 0x48C700, 0x49165C, 0x495F69, 0x4960F5, 0x4964DA }) {
      patcher.HookCall(loc, StdcallLambdaPtr([](SOCKET s, sockaddr_in* pName, int nameLen) {
        pName->sin_addr.s_addr = INADDR_ANY;
        return OP2Thunk<0x4C0E40, int __stdcall(SOCKET, void*, int)>(s, pName, nameLen);
      }));
    }

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Bypass the IPX emulator in the GOG distro of the game to work around its potential crash bugs.
bool SetBypassIpxEmulatorPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  // Load the real wsock32.dll.
  HMODULE hWinSock = NULL;
  if (static bool inited = false;  inited == false) {
    wchar_t dllPath[MAX_PATH] = L"";
    GetSystemDirectoryW(&dllPath[0], MAX_PATH);
    wcsncat_s(&dllPath[0], MAX_PATH, L"\\wsock32.dll", _TRUNCATE);

    hWinSock = LoadLibraryW(&dllPath[0]);
    inited   = true;
  }

  static TethysUtil::Library winSockLib(hWinSock, true);
  enable &= winSockLib.IsLoaded();

  if (enable) {
    patcher.Hook(0x4C0E22, winSockLib.Get("WSACleanup"));
    patcher.Hook(0x4C0E28, winSockLib.Get("WSAStartup"));
    patcher.Hook(0x4C0E2E, winSockLib.Get("gethostbyname"));
    patcher.Hook(0x4C0E34, winSockLib.Get("inet_addr"));
    patcher.Hook(0x4C0E3A, winSockLib.Get("closesocket"));
    patcher.Hook(0x4C0E40, winSockLib.Get("bind"));
    patcher.Hook(0x4C0E46, winSockLib.Get("htons"));
    patcher.Hook(0x4C0E4C, winSockLib.Get("socket"));
    patcher.Hook(0x4C0E52, winSockLib.Get("sendto"));
    patcher.Hook(0x4C0E58, winSockLib.Get("recvfrom"));
    patcher.Hook(0x4C0E5E, winSockLib.Get("select"));
    patcher.Hook(0x4C0E64, winSockLib.Get("setsockopt"));
    patcher.Hook(0x4C0E6A, winSockLib.Get("ntohs"));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Disable the "Dan's" cheats when the game is in multiplayer mode and cheats are not enabled in game settings.
bool SetNoCheatsPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static const auto& flags = g_gameImpl.gameStartInfo_.startupFlags;

    // In CheckChatForCheatCode()
    patcher.LowLevelHook(0x586430, [] { (flags.isMultiplayer && (flags.cheatsEnabled == false)) ? 0x5864A6 : 0; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Guts DirectPlay netplay protocol checks; reimplements the SIGS DLL check, which is patched out in the GOG distro.
bool SetNetProtocolEnabledPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Remove DirectPlay protocol enumeration
    // Reimplement {IPXGameProtocol, ModemGameProtocol, SerialGameProtocol}::IsEnabled() (replace vtbl entry)
    for (uintptr loc : { 0x4CF8E8, 0x4CF948, 0x4CF958 }) {
      patcher.Write(loc, ThiscallLambdaPtr([](NetGameProtocol* pProtocol) -> ibool { return false; }));
    }

    // Reimplement SIGSGameProtocol::IsEnabled() (replace vtbl entry)
    patcher.Write(0x4CF968, ThiscallLambdaPtr([](void* pThis) {
      static const bool result =
        [] { return g_resManager.FileExists("SNWValid.dll") && g_resManager.FileExists("SierraNW.dll"); }();
      return result;
    }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Changes some multiplayer game lobby default settings.
bool SetGameLobbyDefaultsPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Change default resources setting to high instead of medium.
    // In MultiplayerLobbyDialog::AddNewPlayer()
    patcher.WriteNop(0x45F17C);

    // In MultiplayerLobbyDialog::ShowHostGame()
    patcher.WriteNop(0x45F81E);
    // Change morale steady to default on.
    patcher.LowLevelHook(0x45F13E, [](Eax<StartupFlags>& flags) { flags->moraleEnabled = 1; });
    // Change disasters to default off for Last One Standing and Land Rush, keep at default on otherwise.
    patcher.LowLevelHook(0x45F28B, [](Ebx<MultiplayerLobbyDialog*> pThis, Eax<MissionType> missionType) {
      if ((missionType == MissionType::LastOneStanding) || (missionType == MissionType::LandRush)) {
        pThis->gameStartInfo_.startupFlags.disastersEnabled = 0;
      }
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// When querying a specific IP address for TCP/IP join, skip the session list and immediately attempt to join instead.
// ** TODO Fix delay in skinning the "contacting host" status dialog, return to select IP dialog on failure
bool SetQuickJoinPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static IDlgWnd* pFindSessionDlg = nullptr;

    // In TCPGameSearchProtocol::SetupHostedGame()
    patcher.HookCall(0x491D4B, StdcallLambdaPtr([](GUID* pGuid) { *pGuid = *TApp::GetGameIdentifier(); return S_OK; }));

    // In FindSessionDlg::FindNetGameSession()
    patcher.LowLevelHook(0x414E50, [](Ecx<IDlgWnd*> pThis) { pFindSessionDlg = pThis;   });
    patcher.LowLevelHook(0x414EA5, []                      { pFindSessionDlg = nullptr; });

    // In TCPSearchProtocol::SearchForGames()
    patcher.LowLevelHook(0x491717, [](Ebx<TCPGameSearchProtocol*> pSearchProtocol) {
      if (pFindSessionDlg != nullptr) {
        CloseHandle(pSearchProtocol->hBroadcastThread_);
        pSearchProtocol->hBroadcastThread_ = NULL;

        auto*const pSession = new(OP2Heap) TCPGameSession;
        *PtrInc<NetGameSession**>(pFindSessionDlg, 40) = pSearchProtocol->pSession_ = pSession;
        if (pSession != nullptr) {
          pSession->hostAddress_       = pSearchProtocol->specifiedIP_;
          pSession->sessionIdentifier_ = *TApp::GetGameIdentifier();
        }

        EndDialog(pFindSessionDlg->hWnd_, 1);
      }

      return (pFindSessionDlg != nullptr) ? 0x491773 : 0;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
