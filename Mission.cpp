
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Patcher.h"
#include "Util.h"

#include "Tethys/API/Unit.h"
#include "Tethys/API/TethysGame.h"
#include "Tethys/API/Trigger.h"
#include "Tethys/API/Enumerators.h"
#include "Tethys/Game/MissionManager.h"
#include "Tethys/Game/GameStartInfo.h"

#include <deque>

using namespace Tethys;
using namespace TethysAPI;
using namespace Patcher::Util;
using namespace Patcher::Registers;

// =====================================================================================================================
// Adds new callback entry points for mission DLLs.
// ** TODO This should be integrated with op2ext's event handlers
bool SetMissionCallbackPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static struct {
      ibool (__cdecl*  pfnOnLoad)(OnLoadArgs&&);
      ibool (__cdecl*  pfnOnUnload)(OnUnloadArgs&&);
      void  (__cdecl*  pfnOnEnd)(OnEndArgs&&);
      void  (__cdecl*  pfnOnChat)(OnChatArgs&&);
      void  (__cdecl*  pfnOnCreateUnit)(OnCreateUnitArgs&&);
      void  (__cdecl*  pfnOnDestroyUnit)(OnDestroyUnitArgs&&);
    } callbacks = { };

    // In MissionManager::LoadScript()
    patcher.LowLevelHook(0x402C0B, [](Esi<MissionManager*> pThis) {
      (FARPROC&)(callbacks.pfnOnLoad)        = GetProcAddress(pThis->hModule_, "OnLoad");
      (FARPROC&)(callbacks.pfnOnUnload)      = GetProcAddress(pThis->hModule_, "OnUnload");
      (FARPROC&)(callbacks.pfnOnEnd)         = GetProcAddress(pThis->hModule_, "OnEnd");
      (FARPROC&)(callbacks.pfnOnChat)        = GetProcAddress(pThis->hModule_, "OnChat");
      (FARPROC&)(callbacks.pfnOnCreateUnit)  = GetProcAddress(pThis->hModule_, "OnCreateUnit");
      (FARPROC&)(callbacks.pfnOnDestroyUnit) = GetProcAddress(pThis->hModule_, "OnDestroyUnit");

      return ((callbacks.pfnOnLoad == nullptr) || callbacks.pfnOnLoad({ sizeof(OnLoadArgs) })) ? 0 : 0x402B65;
    });

    // In MissionManager::Deinit()
    // ** TODO Handle error returned from pfnOnUnload(), would need to trigger OP2 restart
    patcher.LowLevelHook(0x402C29, []
      { if (callbacks.pfnOnUnload != nullptr) { callbacks.pfnOnUnload({ sizeof(OnUnloadArgs) }); }  callbacks = { }; });

    // In GameFrame::EndMission()
    patcher.LowLevelHook(0x49CB3E, [](Eax<MissionResults*> pMissionResults)
      { if (callbacks.pfnOnEnd != nullptr) { callbacks.pfnOnEnd({ sizeof(OnEndArgs), pMissionResults }); } });

    // Hook call to CheckChatForCheatCode() in PlayerImpl::ProcessCommandPacket()
    patcher.HookCall(0x40FD85, SetCapturedTrampoline, [F = (void(*)(char*, int))0](char* pText, int playerNum) {
      if (callbacks.pfnOnChat != nullptr) {
        callbacks.pfnOnChat({ sizeof(OnChatArgs), pText, sizeof(ChatCommand::message), playerNum });
      }
      F(pText, playerNum);
    });

    // In MapObjectTypeManager::CreateMapUnit()
    patcher.LowLevelHook(0x446A9A, [](Ebx<MapObject*> pMo) {
      if (callbacks.pfnOnCreateUnit != nullptr) {
        callbacks.pfnOnCreateUnit({ sizeof(OnCreateUnitArgs), Unit(pMo) });
      }
    });

    // In MapObject::DoDeath()
    patcher.LowLevelHook(0x43AA06, [](Esi<MapObject*> pThis) {
      if (callbacks.pfnOnDestroyUnit != nullptr) {
        callbacks.pfnOnDestroyUnit({ sizeof(OnDestroyUnitArgs), Unit(pThis) });
      }
    });
    
    // Pass extended params to triggers, i.e. function signature is now of the form void __cdecl(OnTriggerArgs*).
    // Because trigger callbacks always used cdecl, this is ABI-compatible with existing callbacks (caller cleanup).
    // In MissionManager::ProcessScStubs()
    patcher.LowLevelHook(0x403251, [] {
      using PfnTrigger = void(__cdecl*)(OnTriggerArgs&&);
      std::deque<std::pair<TriggerImpl*, PfnTrigger>> firedTriggers;

      for (TriggerImpl* pTrigger = TriggerImpl::GetTriggerList(); pTrigger != nullptr; pTrigger = pTrigger->pNext_) {
        if (pTrigger->isEnabled_ && pTrigger->HasFired()) {
          if (auto* pfn = pTrigger->GetCallbackFunction();  pfn != nullptr) {
            firedTriggers.emplace_back(pTrigger, reinterpret_cast<PfnTrigger>(pfn));
          }
        }
      }

      // Defer calling user callbacks in case they have side effects that would cause other triggers to fire.
      for (auto [pTrigger, pfnOnTrigger] : firedTriggers) {
        pfnOnTrigger({ sizeof(OnTriggerArgs), Trigger(pTrigger) });
      }

      return 0x40329C;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// When running multiplayer missions from the debug "run script" dialog, don't automatically trigger victory when the
// mission loads.
bool SetMissionDebugNoInstantWin(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  static const auto& flags = g_gameImpl.gameStartInfo_.startupFlags;

  if (enable) {
    // In MissionManager::ProcessScStubs()
    patcher.LowLevelHook(0x4033AA, [] { return flags.isMultiplayer ? 0 : 0x4033B9; });

    // In LastOneStandingTriggerImpl::HasFired()
    patcher.LowLevelHook(0x492A61, [](Ebp<ibool*> pIsHuman) {
      Unit u;
      auto*const p = PtrDec<PlayerImpl*>(pIsHuman, offsetof(PlayerImpl, isHuman_));
      return flags.isMultiplayer ? 0 : PlayerBuildingEnum(p->playerNum_, MapID::CommandCenter).GetNext(u) ? 0x492A72: 0;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}