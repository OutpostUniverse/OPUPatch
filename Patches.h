
#pragma once

// NOTE: Pure bugfixes end in -Fix(), other changes end in -Patch().

// Misc
bool SetGameVersion(bool enable);
bool SetNoCdPatch(bool enable);
bool SetDefaultIniSettingsPatch(bool enable);
bool SetForceMoraleFix(bool enable);
bool SetPrintfFloatFix(bool enable);
bool SetGlobalMusicFix(bool enable);

// Stream
bool SetFileSearchPathPatch(bool enable);
bool SetChecksumPatch(bool enable);  // ** WIP
bool SetCodecPatch(bool enable);

// Netplay
bool SetNatFix(bool enable);
bool SetNetGameSpeedPatch(bool enable);
bool SetBindAnyNetAdapterFix(bool enable);
bool SetNoCheatsPatch(bool enable);
bool SetSigsDllCheckPatch(bool enable);
bool SetQuickJoinPatch(bool enable);  // ** WIP

// UI
bool SetUiResourceReplacePatch(bool enable);
bool SetChatLengthPatch(bool enable);
bool SetUiHighlightFix(bool enable);
bool SetIpWindowFocusPatch(bool enable);
bool SetMiniMapFix(bool enable);
bool SetLocalizationPatch(bool enable);
bool SetFontPatch(const char* pNewFont);  // ** WIP
bool SetVehicleCargoDisplayPatch(bool enable);
bool SetMissionListNamePatch(bool enable);
bool SetSavantNotificationPatch(bool enable);
bool SetControlGroupHotkeyPatch(bool enable);

// Graphics
bool SetWindowFix(bool enable);
bool SetDwmFix(bool enable);
bool SetDpiFix(bool enable);
bool SetAlphaBlendPatch(bool enable);

// ScStub
bool SetScStubPatch(bool enable);  // ** WIP

// Map
bool SetLargeMapPatch(bool enable);
bool SetCustomMapFlagsPatch(bool enable);

// Units
bool SetUnitLimitPatch(bool enable);
bool SetUnitTypeLimitPatch(bool enable);  // ** WIP
bool SetDrawLightningFix(bool enable);
bool SetTransferUnitToGaiaFix(bool enable);
bool SetBuildWallFix(bool enable);
bool SetWreckageFix(bool enable);
bool SetMissileFix(bool enable);
bool SetNoAlliedDockDamageFix(bool enable);
bool SetAllyEdwardSurveyMinesPatch(bool enable);
bool SetMultipleRepairPatch(bool enable);
bool SetOreRoutePatch(bool enable);  // ** WIP
bool SetTurretAnimationPatch(bool enable);
