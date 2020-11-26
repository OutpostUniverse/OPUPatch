
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>

#include "Patcher.h"
#include "Util.h"
#include "Resources.h"

#include "Tethys/API/TethysGame.h"
#include "Tethys/Game/TApp.h"
#include "Tethys/UI/IWnd.h"
#include "Tethys/Resource/ResManager.h"
#include "Tethys/Network/NetGameSession.h"
#include "Tethys/Network/NetGameProtocol.h"
#include "Tethys/Network/NetTransportLayer.h"

#include <string>

using namespace Patcher;
using namespace Patcher::Util;

// =====================================================================================================================
// Fixes replicating players list always failing when the system's LAN is behind a NAT layer.
// While this does fix a hard NAT-related bug, it by itself does not implement any port forwarding or NAT punchthrough.
bool SetNatFix(
  bool enable)
{
  static PatchContext patcher;
  bool success = true;

  if (enable) {
    // In Network::ProcessProtocolControlPacket()
    // ** TODO Come up with a definition for NetTransportLayer::playerNetAddrList_ instead of using [8345]
    patcher.LowLevelHook(0x4972C3, [](Ebp<uint32*> pNetTransportLayer, Esp<void*> pEsp)
      { pNetTransportLayer[8345] = (*static_cast<sockaddr_in**>(PtrInc(pEsp, 20)))->sin_addr.s_addr; });
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
  static PatchContext patcher;
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
  static PatchContext patcher;
  bool success = true;

  if (enable) {
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
// Disable the "Dan's" cheats when the game is in multiplayer mode and cheats are not enabled in game settings.
bool SetNoCheatsPatch(
  bool enable)
{
  static PatchContext patcher;
  bool success = true;

  if (enable) {
    static const auto& flags = g_gameImpl.gameStartInfo_.startupFlags;

    // In CheckChatForCheatCode()
    // NOTE: This hook location was chosen so as to not conflict with the common ChatMessageHook implementation, which
    //       hooks at 0x58642A.
    patcher.LowLevelHook(0x586430, [] { (flags.isMultiplayer && (flags.cheatsEnabled == false)) ? 0x5864A6 : 0; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Reimplement the SIGS DLL check, which is patched out in the GOG distro.
bool SetSigsDllCheckPatch(
  bool enable)
{
  static PatchContext patcher;
  bool success = true;

  if (enable) {
    // Reimplement NetGameProtocol::SIGS::IsEnabled() (replace vtbl entry)
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
// When querying a specific IP address for TCP/IP join, skip the session list and immediately attempt to join instead.
// ** TODO Fix delay in skinning the "contacting host" status dialog, return to select IP dialog on failure
bool SetQuickJoinPatch(
  bool enable)
{
  static PatchContext patcher;
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
        *static_cast<NetGameSession**>(PtrInc(pFindSessionDlg, 40)) = pSearchProtocol->pSession_ = pSession;
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
