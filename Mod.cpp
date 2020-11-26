
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Tethys/Game/TApp.h"

#include "Patches.h"
#include "Util.h"

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
  const char*  pIniSectionName)
{
  bool result = (g_tApp.Version() >= 0x01020007);  // Requires official patch 1.2.0.7 (unpatched CD version is 1.2.0.5)
  result = result && SetGameVersion(true);

  // Misc
  result = result && SetNoCdPatch(true);
  result = result && SetForceMoraleFix(true);
  result = result && SetPrintfFloatFix(true);
  result = result && SetGlobalMusicFix(true);

  // Stream
  result = result && SetFileSearchPathPatch(true);

  // Netplay
  result = result && SetNatFix(true);
  result = result && SetNetGameSpeedPatch(true);
  result = result && SetBindAnyNetAdapterFix(true);
  result = result && SetNoCheatsPatch(true);
  result = result && SetSigsDllCheckPatch(true);
  //result = result && SetQuickJoinPatch(true);  // ** TODO

  // UI
  result = result && SetUiResourceReplacePatch(true);
  result = result && SetChatLengthPatch(true);
  result = result && SetUiHighlightFix(true);
  result = result && SetIpWindowFocusPatch(true);
  result = result && SetMiniMapFix(true);
  //result = result && SetFontPatch("Arial");  // ** TODO
  result = result && SetVehicleCargoDisplayPatch(true);
  result = result && SetMissionListNamePatch(true);
  result = result && SetSavantNotificationPatch(true);
  result = result && SetControlGroupHotkeyPatch(true);

  // Graphics
  result = result && SetWindowFix(true);
  result = result && SetDwmFix(true);
  result = result && SetDpiFix(true);
  result = result && SetAlphaBlendPatch(true);

  // ScStub
  result = result && SetScStubPatch(true);

  // Map
  result = result && SetLargeMapPatch(true);
  result = result && SetCustomMapFlagsPatch(true);

  // Units
  result = result && SetUnitLimitPatch(true);
  result = result && SetDrawLightningFix(true);
  result = result && SetTransferUnitToGaiaFix(true);
  result = result && SetBuildWallFix(true);
  result = result && SetWreckageFix(true);
  result = result && SetMissileFix(true);
  result = result && SetNoAlliedDockDamageFix(true);
  result = result && SetAllyEdwardSurveyMinesPatch(true);
  result = result && SetTurretAnimationPatch(true);
}

// =====================================================================================================================
DLLAPI bool DestroyMod() {
  return true;
}
