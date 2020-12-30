
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Patcher.h"
#include "Util.h"

#include "Tethys/API/Unit.h"
#include "Tethys/API/Trigger.h"
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
      PfnOnLoad        pfnOnLoad        = nullptr;
      PfnOnUnload      pfnOnUnload      = nullptr;
      PfnOnEnd         pfnOnEnd         = nullptr;
      PfnOnChat        pfnOnChat        = nullptr;
      PfnOnCreateUnit  pfnOnCreateUnit  = nullptr;
      PfnOnDestroyUnit pfnOnDestroyUnit = nullptr;
    } callbacks{};

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
    patcher.LowLevelHook(0x49CB3E, [](Eax <MissionResults*> pMissionResults)
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
      std::deque<std::pair<TriggerImpl*, PfnOnTrigger>> pFiredTriggers;

      for (TriggerImpl* pTrig = TriggerImpl::GetTriggerList(); pTrig != nullptr; pTrig = pTrig->pNext_) {
        if (pTrig->isEnabled_ && pTrig->HasFired()) {
          if (auto* pfn = pTrig->GetCallbackFunction();  pfn != nullptr) {
            pFiredTriggers.emplace_back(pTrig, pfn);
          }
        }
      }

      // Defer calling user callbacks in case they have side effects that would cause other triggers to fire.
      for (auto [pTrig, pfn] : pFiredTriggers) {
        pfn({ sizeof(OnTriggerArgs), Trigger(pTrig) });
      }

      return 0x40329C;
    });
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
