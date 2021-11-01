
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Patcher.h"
#include "Util.h"

#include "Tethys/API/Unit.h"
#include "Tethys/API/TethysGame.h"
#include "Tethys/API/Trigger.h"
#include "Tethys/Game/MissionManager.h"
#include "Tethys/Game/GameStartInfo.h"
#include "Tethys/Game/ScBase.h"
#include "Tethys/Resource/StreamIO.h"

#include <deque>
#include <algorithm>

using namespace Tethys;
using namespace TethysAPI;
using namespace Patcher::Util;
using namespace Patcher::Registers;

// ** TODO: * Finish extended ScStub array.  Needs to work with saved games (it might work as is?)
//          * Expose ScBase::FunctionReference more directly
#define SCSTUB_USE_EXTENDED_ARRAY 0

#if SCSTUB_USE_EXTENDED_ARRAY
class ScStubListEx : public ScStubList {
public:
  ScStubListEx() : ScStubList(), pScStubArrayExt_(), lastCreatedGroupIndex_(NilIndex) { }
  static constexpr size_t MaxNumScStubs = 4096;

public:
  ScBase* pScStubArrayExt_[MaxNumScStubs - ScStubList::MaxNumScStubs]; // Note: access using pScStubArray_ or operator[]
  uint32  lastCreatedGroupIndex_;
};

static ScStubListEx g_scStubListEx;
#endif

// =====================================================================================================================
// Adds new callback entry points for mission DLLs.
// ** TODO fix OnDestroyUnit to handle weapons fire/Pretty art (moDelSelf action)
bool SetMissionCallbackPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static struct {
      ibool (__cdecl*  pfnOnLoad)(OnLoadMissionArgs&&);
      ibool (__cdecl*  pfnOnUnload)(OnUnloadMissionArgs&&);
      void  (__cdecl*  pfnOnEnd)(OnEndMissionArgs&&);
      ibool (__cdecl*  pfnOnSave)(OnSaveGameArgs&&);
      ibool (__cdecl*  pfnOnLoadSave)(OnLoadSavedGameArgs&&);
      void  (__cdecl*  pfnOnChat)(OnChatArgs&&);
      void  (__cdecl*  pfnOnCreateUnit)(OnCreateUnitArgs&&);
      void  (__cdecl*  pfnOnDestroyUnit)(OnDestroyUnitArgs&&);
      void  (__cdecl*  pfnOnDamageUnit)(OnDamageUnitArgs&&);
    } callbacks = { };

    // Insert OnLoadMission() hook and initialize callbacks In MissionManager::LoadScript()
    patcher.LowLevelHook(0x402C0B, [](Esi<MissionManager*> pThis) {
      (FARPROC&)(callbacks.pfnOnLoad)        = GetProcAddress(pThis->hModule_, "OnLoadMission");
      (FARPROC&)(callbacks.pfnOnUnload)      = GetProcAddress(pThis->hModule_, "OnUnloadMission");
      (FARPROC&)(callbacks.pfnOnEnd)         = GetProcAddress(pThis->hModule_, "OnEndMission");
      (FARPROC&)(callbacks.pfnOnSave)        = GetProcAddress(pThis->hModule_, "OnSaveGame");
      (FARPROC&)(callbacks.pfnOnLoadSave)    = GetProcAddress(pThis->hModule_, "OnLoadSavedGame");
      (FARPROC&)(callbacks.pfnOnChat)        = GetProcAddress(pThis->hModule_, "OnChat");
      (FARPROC&)(callbacks.pfnOnCreateUnit)  = GetProcAddress(pThis->hModule_, "OnCreateUnit");
      (FARPROC&)(callbacks.pfnOnDestroyUnit) = GetProcAddress(pThis->hModule_, "OnDestroyUnit");
      (FARPROC&)(callbacks.pfnOnDamageUnit)  = GetProcAddress(pThis->hModule_, "OnDamageUnit");

      return ((callbacks.pfnOnLoad == nullptr) || callbacks.pfnOnLoad({ sizeof(OnLoadMissionArgs) })) ? 0 : 0x402B65;
    });

    // Insert OnUnloadMission() hook in MissionManager::Deinit()
    // ** TODO Handle error returned from pfnOnUnload(), would need to trigger OP2 restart
    patcher.LowLevelHook(0x402C25, [](Eax<HMODULE> hMission) {
      if ((hMission != NULL) && (callbacks.pfnOnUnload != nullptr)) {
        callbacks.pfnOnUnload({ sizeof(OnUnloadMissionArgs) });
      }
      callbacks = { };
    });

    // Insert OnEndMission() hook in GameFrame::EndMission()
    patcher.LowLevelHook(0x49CB3E, [](Eax<MissionResults*> pMissionResults)
      { if (callbacks.pfnOnEnd != nullptr) { callbacks.pfnOnEnd({ sizeof(OnEndMissionArgs), pMissionResults }); } });

    // Insert OnChat() hook in PlayerImpl::ProcessCommandPacket() (hook call to CheckChatForCheatCode())
    patcher.HookCall(0x40FD85, SetCapturedTrampoline, [F = (void(*)(char*, int))0](char* pText, int playerNum) {
      if (callbacks.pfnOnChat != nullptr) {
        callbacks.pfnOnChat({ sizeof(OnChatArgs), pText, sizeof(ChatCommand::message), playerNum });
      }
      F(pText, playerNum);
    });
    
    // Insert OnSaveGame() hook in GameImpl::SaveGame()
    patcher.LowLevelHook(0x48A394, [](Edi<StreamIO*> pSavedGame) { 
      return ((callbacks.pfnOnSave == nullptr) || callbacks.pfnOnSave({ sizeof(OnSaveGameArgs), pSavedGame })) ?
             0 : 0x48A39C;
    });

    // Insert OnLoadSavedGame() hook in GameImpl::LoadGame()
    patcher.LowLevelHook(0x48A61E, [](Edi<StreamIO*> pSavedGame) {
      return
        ((callbacks.pfnOnLoadSave == nullptr) || callbacks.pfnOnLoadSave({ sizeof(OnLoadSavedGameArgs), pSavedGame })) ?
        0 : 0x48A687;
    });

    // Insert OnCreateUnit() hook in MapObjectTypeManager::CreateMapUnit()
    patcher.LowLevelHook(0x446A9A, [](Ebx<MapObject*> pMo)
      { if (callbacks.pfnOnCreateUnit) { callbacks.pfnOnCreateUnit({ sizeof(OnCreateUnitArgs), Unit(pMo) }); } });

    // Insert OnDestroyUnit() hook in ProcessUnits() (moDelSelf handler)
    patcher.LowLevelHook(0x43AD82, [](Ebp<MapObject*> pMo)
      { if (callbacks.pfnOnDestroyUnit) { callbacks.pfnOnDestroyUnit({ sizeof(OnDestroyUnitArgs), Unit(pMo) }); } });

    static const auto OnDamageUnit = [](MapObject* pSrc, MapObject* pTarget, int damage) {
      if (callbacks.pfnOnDamageUnit != nullptr) {
        callbacks.pfnOnDamageUnit({ sizeof(OnDamageUnitArgs), Unit(pSrc), Unit(pTarget), damage });
      }
    };

    // Insert OnDamageUnit() hooks
    // In Weapon::DoDamage()
    patcher.LowLevelHook(0x4A3A28, [](Esi<MapChildEntity*> pThis, Edi<MapObject*> pTarget, Eax<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In Explosive::DoDamage()
    patcher.LowLevelHook(0x4A4479, [](Ebp<Explosive*> pThis, Ebx<MapObject*> pTarget, Eax<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In Stickyfoam::DoDamage()
    patcher.LowLevelHook(0x4A5221, [](Edi<MapObj::Stickyfoam*> pThis, Esi<MapObject*> pTarget, Eax<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In EMP::DoDamage()
    patcher.LowLevelHook(0x4A4861, [](Edi<MapObj::EMP*> pThis, Ecx<MapObject*> pTarget, Eax<int> empDuration)
      { OnDamageUnit(pThis, pTarget, empDuration); });
    // In EMPBlast::DoEvent()
    patcher.LowLevelHook(0x4141C2, [](Esi<MapObj::EMPBlast*> pThis, Ecx<MapObject*> pTarget, Eax<int> empDuration)
      { OnDamageUnit(pThis, pTarget, empDuration + 32); });
    // In AcidCloud::DoEvent()
    patcher.LowLevelHook(0x4011E9, [](Edi<MapObj::AcidCloud*> pThis, Esi<MapObject*> pTarget, Ebx<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In ESG::DoEvent()
    patcher.LowLevelHook(0x48134A, [](Edi<MapObj::ESG*> pThis, Esi<MapObject*> pTarget, Ebx<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In Building::DoDockDamage()
    patcher.LowLevelHook(0x482E7C, [](Edi<Building*> pThis, Esi<Vehicle*> pTarget, Ecx<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In Tokamak::DoEvent()
    patcher.LowLevelHook(0x46E89B, [](Esi<MapObj::Tokamak*> pThis) { OnDamageUnit(pThis, pThis, 1); });
    // In Earthquake::DoEvent()
    patcher.LowLevelHook(0x413BC7, [](Esi<MapObj::Earthquake*> pThis, Edi<MapObject*> pTarget, Ebx<int16> damage) {
      pTarget->damage_ += damage;
      OnDamageUnit(pThis, pTarget, damage);
      return 0x413BD1;
    });
    // In Lightning::DoEvent()
    patcher.LowLevelHook(0x4336B1, [](Esp<void*> pEsp, Ebx<MapObject*> pTarget, Eax<int16> damage)
      { OnDamageUnit(PtrInc<MapObj::Lightning*>(pEsp, 16), pTarget, damage); });
    // In Meteor::DoEvent()
    patcher.LowLevelHook(0x44A8DC, [](Ebp<MapObj::Meteor*> pThis, Esi<MapObject*> pTarget, Ebx<int16> damage)
      { OnDamageUnit(pThis, pTarget, damage); });
    // In Vortex::DoEvent()
    patcher.LowLevelHook(0x48F9C7, [](Esp<void*> pEsp, Ebx<MapObject*> pTarget, Esi<int16> damage)
      { OnDamageUnit(PtrInc<MapObj::Vortex*>(pEsp, 16), pTarget, damage); });

    // Pass extended params to triggers, i.e. function signature is now of the form void __cdecl(OnTriggerArgs*).
    // Because trigger callbacks always used cdecl (caller cleanup), this is ABI-compatible with existing callbacks.
    // In MissionManager::ProcessScStubs()
    patcher.LowLevelHook(0x403251, [] {
      using PfnTrigger = void(__cdecl*)(OnTriggerArgs&&);
      std::deque<std::tuple<TriggerImpl*, PfnTrigger, PlayerBitmask>> firedTriggers;

      for (TriggerImpl* pTrigger = TriggerImpl::GetTriggerList(); pTrigger != nullptr; pTrigger = pTrigger->pNext_) {
        const PlayerBitmask prevTriggeredBy = pTrigger->playerVectorHasFired_;
        if (pTrigger->isEnabled_ && pTrigger->HasFired()) {
          if (auto* pfn = pTrigger->GetCallbackFunction();  pfn != nullptr) {
            firedTriggers.emplace_back(pTrigger, reinterpret_cast<PfnTrigger>(pfn), prevTriggeredBy);
          }
        }
      }

      // Defer calling user callbacks in case they have side effects that would cause other triggers to fire.
      for (auto [pTrigger, pfnOnTrigger, prevTriggeredBy] : firedTriggers) {
        pfnOnTrigger({ sizeof(OnTriggerArgs), Trigger(pTrigger), pTrigger->playerVectorHasFired_, prevTriggeredBy });
      }

      return 0x40329C;
    });

    // Hook FuncReference::SetData() to allow trigger callback function of nullptr.
    patcher.Hook(0x4757D0, SetCapturedTrampoline, ThiscallFunctor(
      [F = (ibool(__thiscall*)(ScBase*, char*, ibool))0](ScBase* pThis, char* pFuncName, ibool useLevelDLL) -> ibool {
        return ((pFuncName == nullptr) || F(pThis, pFuncName, useLevelDLL));
      }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Changes victory conditions to make a copy of their objective text string and store it inside save files.
bool SetVictoryConditionTextPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Replace VictoryConditionImpl::Init()
    patcher.Hook(
      0x495870,
      ThiscallLambdaPtr([](VictoryConditionImpl* pThis, ScStub* pVictoryTrigger, const char* pObjectiveText) {
        pThis->pObjectiveText_      = OP2Strdup(pObjectiveText);
        pThis->victoryTriggerIndex_ = pVictoryTrigger->id_;
      }));

    // Hook ~VictoryConditionImpl()
    patcher.Hook(0x495800, SetCapturedTrampoline, ThiscallFunctor(
      [F = decltype(ScBase::VtblType::pfnDestroy){}](VictoryConditionImpl* pThis, ibool freeMem) {
        OP2Free(const_cast<char*>(pThis->pObjectiveText_));
        return F(pThis, freeMem);
      }));

    // Hook VictoryConditionImpl::Save()
    patcher.LowLevelHook(0x49594A, [](Edi<VictoryConditionImpl*> pThis, Esi<StreamIO*> pStream) {
      pStream->WriteString(pThis->pObjectiveText_);
      pStream->WriteValue(pThis->victoryTriggerIndex_);
      return 0x495957;
    });

    // Hook VictoryConditionImpl::Load()
    patcher.LowLevelHook(0x4959CF, [](Edi<VictoryConditionImpl*> pThis, Esi<StreamIO*> pStream) {
      if (GetSavedGameVersion(pStream) >= 142) {
        pThis->pObjectiveText_ = pStream->ReadString();
      }
      else {
        const char* pObjectiveText = nullptr;
        pStream->Read(sizeof(pObjectiveText), &pObjectiveText);
        pThis->pObjectiveText_ = OP2Strdup(pObjectiveText);
      }
      pStream->ReadValue(&pThis->victoryTriggerIndex_);
      return 0x4959DC;
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
bool SetMissionDebugNoInstantWinPatch(
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
      auto*const pPlayer = PtrDec<PlayerImpl*>(pIsHuman, offsetof(PlayerImpl, isHuman_));
      const bool hasCC   = PlayerBuildingEnum(pPlayer->playerNum_, MapID::CommandCenter).begin();
      return (flags.isMultiplayer || (hasCC == false)) ? 0 : 0x492A72;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Replacement member function for ScStubFactory::Create.
static ScBase* __fastcall ScStubFactory_Create(
  ScStubFactory*  pThis)
{
  auto*const pAllocation = OP2Alloc(pThis->elementSizeInBytes_);
  ScBase*    pScStub     = (pAllocation != nullptr) ? pThis->pfnCreateStub_(pAllocation) : nullptr;

  if (pScStub != nullptr) {
    pScStub->Init();

#if SCSTUB_USE_EXTENDED_ARRAY
    // Test if this is a Group, i.e. used by units.
    bool isGroup = true;
    for (auto* pCurType = pThis; pCurType != nullptr; pCurType = pCurType->pParent_) {
      if ((strcmp(pCurType->pName_, "Group") == 0) || (strcmp(pCurType->pName_, "Strategy") == 0)) {
        isGroup = true;
        break;
      }
    }

    auto& stubList = g_scStubListEx;

    // Units store ScStub ID in a uint8, and therefore can only use IDs 0-254 (255 = sentinel value for no group).
    auto*const   pLastCreated = isGroup ? &stubList.lastCreatedGroupIndex_ : &stubList.lastCreatedIndex_;
    const uint32 end          = isGroup ?  stubList.NilIndex               :  stubList.MaxNumScStubs;
    const uint32 start        = isGroup ?  0                               :  stubList.NilIndex + 1;
#else
    auto& stubList = *ScStubList::GetInstance();

    auto*const       pLastCreated = &stubList.lastCreatedIndex_;
    constexpr uint32 end          =  stubList.MaxNumScStubs;
    constexpr uint32 start        =  0;
#endif

    const auto Inc = [start, end](uint32& index) -> uint32& { return (index = (std::max)((index + 1) % end, start)); };

    uint32 index = *pLastCreated;
    for (const uint32 lastCreated = index; stubList[Inc(index)] != nullptr;) {
      if (index == lastCreated) {
        // ScStub array is full.
        pScStub->index_ = stubList.NilIndex;
        pScStub->Destroy(true);
        pScStub = nullptr;
        break;
      }
    }

    if (pScStub != nullptr) {
      pScStub->index_ = *pLastCreated = index;
      stubList[index] = pScStub;
    }
  }

  return pScStub;
}

// =====================================================================================================================
// Fixes a bug where destroyed ScStubs don't get recycled, and expands the number of non-ScGroup ScStubs that can exist.
bool SetScStubPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  static auto*const pOldScStubArray = OP2Mem<ScStubList*>(0x56C358);

  if (enable && (patcher.NumPatches() == 0)) {
#if SCSTUB_USE_EXTENDED_ARRAY
    // Replace the ScStub array with a larger one.
    memcpy(&g_scStubListEx, pOldScStubArray, sizeof(*pOldScStubArray));
    patcher.ReplaceReferencesToGlobal(pOldScStubArray, &g_scStubListEx);

    // In ScStubList::Init()
    patcher.LowLevelHook(0x47AAC3, [](Ecx<ScStubListEx*> pThis) {
      const size_t count = (pThis == &g_scStubListEx) ? pThis->MaxNumScStubs : ScStubList::MaxNumScStubs;
      memset(&(*pThis)[0], 0, count * sizeof((*pThis)[0]));
      return 0x47AACA;
    });

    // In ScStubList::Load()
    patcher.LowLevelHook(0x47ADF6, [](Edi<ScStubListEx*> pThis) {
      const size_t count = (pThis == &g_scStubListEx) ? pThis->MaxNumScStubs : ScStubList::MaxNumScStubs;
      memset(&(*pThis)[0], 0, count * sizeof((*pThis)[0]));
      return 0x47AE00;
    });
    
    // In ScStubList::Reset()
    patcher.LowLevelHook(0x47AB11, [](Ebx<ScStubListEx*> pThis, Esi<uint32>& counter) {
      counter = (pThis == &g_scStubListEx) ? ScStubListEx::MaxNumScStubs : ScStubList::MaxNumScStubs;  return 0x47AB16;
    });
    patcher.LowLevelHook(0x47AB2C, [](Ebx<ScStubListEx*> pThis)
      { if (pThis == &g_scStubListEx) pThis->lastCreatedGroupIndex_ = pThis->NilIndex; });
#endif

    // Reimplement ScStubFactory::Create to make ScStub recycling work properly (and use extended array).
    patcher.Hook(0x47B410, &ScStubFactory_Create);

    success = (patcher.GetStatus() == PatcherStatus::Ok);

    if (success) {
      static const auto cleanup = atexit([] { SetScStubPatch(false); });
    }
  }

#if SCSTUB_USE_EXTENDED_ARRAY
  if ((enable == false) && (patcher.NumPatches() != 0)) {
    memcpy(pOldScStubArray, &g_scStubListEx, sizeof(*pOldScStubArray));
    memset(&g_scStubListEx[0], 0, sizeof(ScStubList::pScStubArray_));  // Skip destroying non-extended-space ScStubs.
    g_scStubListEx.Reset();  // Destroy all the ScStubs in extended space and null out their pointers.
  }
#endif

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
