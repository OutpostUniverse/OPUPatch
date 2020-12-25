
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Patches.h"
#include "Util.h"
#include "Library.h"

#include <list>

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
  bool success = SetGameVersion(true);

  // Misc
  success &= SetNoCdPatch(true);
  success &= SetForceMoraleFix(true);
  success &= SetPrintfFloatFix(true);
  success &= SetGlobalMusicFix(true);

  // Graphics
  success &= SetWindowFix(true);
  success &= SetDwmFix(true);
  success &= SetDpiFix(true);
  success &= SetAlphaBlendPatch(true);

  // Stream
  success &= SetFileSearchPathPatch(true);  // NOTE: This must handle loading patched DLLs like OP2Shell, odasl, etc.
  success &= SetChecksumPatch(true);
  success &= SetCodecFix(true);

  // Netplay
  success &= SetNatFix(true);
  success &= SetNetGameSpeedPatch(true);
  success &= SetBindAnyNetAdapterFix(true);
  success &= SetNoCheatsPatch(true);
  success &= SetSigsDllCheckPatch(true);

  // UI
  success &= SetUiResourceReplacePatch(true);
  success &= SetChatLengthPatch(true);
  success &= SetUiHighlightFix(true);
  success &= SetIpWindowFocusPatch(true);
  success &= SetMiniMapFix(true);
  success &= SetLocalizationPatch(true);
  success &= SetVehicleCargoDisplayPatch(true);
  success &= SetMissionListNamePatch(true);
  success &= SetSavantNotificationPatch(true);
  success &= SetControlGroupHotkeyPatch(true);
  success &= SetDefaultIniSettingsPatch(true);
  success &= SetDebugRestartGamePatch(true);

  // ScStub
  success &= SetScStubPatch(true);

  // Map
  success &= SetLargeMapPatch(true);
  success &= SetCustomMapFlagsPatch(true);

  // Units
  success &= SetUnitLimitPatch(true);
  success &= SetDrawLightningFix(true);
  success &= SetTransferUnitToGaiaFix(true);
  success &= SetDoInfectFix(true);
  success &= SetBuildWallFix(true);
  success &= SetWreckageFix(true);
  success &= SetMissileFix(true);
  success &= SetPathContextLeakFix(true);
  success &= SetNoAlliedDockDamageFix(true);
  success &= SetAllyEdwardSurveyMinesPatch(true);
  success &= SetMultipleRepairPatch(true);
  success &= SetOreRoutePatch(true);
  success &= SetTurretAnimationPatch(true);
  success &= SetTruckLoadPartialCargoPatch(true);
}
