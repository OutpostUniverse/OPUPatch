
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Tethys/Game/TApp.h"
#include "Tethys/Game/GameStartInfo.h"

#include "Patches.h"
#include "Util.h"

#include <list>

using namespace Tethys;

// =====================================================================================================================
BOOL APIENTRY DllMain(
  HINSTANCE  hInstDll,
  DWORD      fdwReason,
  LPVOID     lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH) {
    g_hInst = hInstDll;
  }
  return TRUE;
}

// =====================================================================================================================
DLLAPI void InitMod(
  const char* pIniSectionName)
{
  using SetPatchFunc = bool(bool);
  static std::list<SetPatchFunc*> pfnPatches;

  auto RegisterPatch = [](SetPatchFunc* pfnSetPatch) {
    static bool result = true;
    result = result && pfnSetPatch(true);
    if (result) {
      pfnPatches.push_front(pfnSetPatch);
    }
  };

  // Requires official patch 1.2.7 (unpatched English CD version is 1.2.5, localized CDs are {2-5}.2.{7-9})
  const uint32 version = g_tApp.GetVersion();
  if ((version >= 0x01020007) && (version < 0x02000000)) {
    RegisterPatch(&SetGameVersion);

    // Stream
    RegisterPatch(&SetFileSearchPathPatch);
    //RegisterPatch(&SetChecksumPatch);
    RegisterPatch(&SetCodecPatch);

    // Misc
    RegisterPatch(&SetNoCdPatch);
    RegisterPatch(&SetDefaultIniSettingsPatch);
    RegisterPatch(&SetForceMoraleFix);
    RegisterPatch(&SetPrintfFloatFix);
    RegisterPatch(&SetGlobalMusicFix);

    // Netplay
    RegisterPatch(&SetNatFix);
    RegisterPatch(&SetNetGameSpeedPatch);
    RegisterPatch(&SetBindAnyNetAdapterFix);
    RegisterPatch(&SetNoCheatsPatch);
    RegisterPatch(&SetSigsDllCheckPatch);

    // UI
    RegisterPatch(&SetUiResourceReplacePatch);
    RegisterPatch(&SetChatLengthPatch);
    RegisterPatch(&SetUiHighlightFix);
    RegisterPatch(&SetIpWindowFocusPatch);
    RegisterPatch(&SetMiniMapFix);
    RegisterPatch(&SetVehicleCargoDisplayPatch);
    RegisterPatch(&SetMissionListNamePatch);
    RegisterPatch(&SetSavantNotificationPatch);
    RegisterPatch(&SetControlGroupHotkeyPatch);

    // Graphics
    RegisterPatch(&SetWindowFix);
    RegisterPatch(&SetDwmFix);
    RegisterPatch(&SetDpiFix);
    RegisterPatch(&SetAlphaBlendPatch);

    // ScStub
    RegisterPatch(&SetScStubPatch);

    // Map
    RegisterPatch(&SetLargeMapPatch);
    RegisterPatch(&SetCustomMapFlagsPatch);

    // Units
    RegisterPatch(&SetUnitLimitPatch);
    RegisterPatch(&SetDrawLightningFix);
    RegisterPatch(&SetTransferUnitToGaiaFix);
    RegisterPatch(&SetBuildWallFix);
    RegisterPatch(&SetWreckageFix);
    RegisterPatch(&SetMissileFix);
    RegisterPatch(&SetNoAlliedDockDamageFix);
    RegisterPatch(&SetAllyEdwardSurveyMinesPatch);
    RegisterPatch(&SetMultipleRepairPatch);
    RegisterPatch(&SetOreRoutePatch);
    RegisterPatch(&SetTurretAnimationPatch);
  }

  atexit([] { for (SetPatchFunc* pfn : pfnPatches) { pfn(false); }  pfnPatches.clear(); });
}
