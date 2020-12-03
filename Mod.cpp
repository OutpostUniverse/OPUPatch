
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Tethys/Game/TApp.h"
#include "Tethys/Game/GameStartInfo.h"

#include "Patches.h"
#include "Util.h"

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
  // Requires official patch 1.2.7 (unpatched English CD version is 1.2.5, localized CDs are {2-5}.2.{7-9})
  const uint32 version = g_tApp.GetVersion();
  bool         result  = (version >= 0x01020007) && (version < 0x02000000);

  result = result && SetGameVersion(true);

  // Misc
  result = result && SetNoCdPatch(true);
  result = result && SetForceMoraleFix(true);
  result = result && SetPrintfFloatFix(true);
  result = result && SetGlobalMusicFix(true);

  // Stream
  result = result && SetFileSearchPathPatch(true);
  //result = result && SetChecksumPatch(true);

  // Netplay
  result = result && SetNatFix(true);
  result = result && SetNetGameSpeedPatch(true);
  result = result && SetBindAnyNetAdapterFix(true);
  result = result && SetNoCheatsPatch(true);
  result = result && SetSigsDllCheckPatch(true);

  // UI
  result = result && SetUiResourceReplacePatch(true);
  result = result && SetChatLengthPatch(true);
  result = result && SetUiHighlightFix(true);
  result = result && SetIpWindowFocusPatch(true);
  result = result && SetMiniMapFix(true);
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
  result = result && SetMultipleRepairPatch(true);
  result = result && SetOreRoutePatch(true);
  result = result && SetTurretAnimationPatch(true);
}
