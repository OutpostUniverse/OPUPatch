
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <timeapi.h>

#include "Patcher.h"
#include "Util.h"
#include "Resources.h"
#include "Library.h"

#include "Tethys/API/Mission.h"
#include "Tethys/API/TethysGame.h"
#include "Tethys/API/GameMap.h"
#include "Tethys/API/Enumerators.h"

#include "Tethys/Game/TApp.h"
#include "Tethys/Game/MapObject.h"

#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/GameFrame.h"

#include "Tethys/Resource/StreamIO.h"
#include "Tethys/Resource/Font.h"
#include "Tethys/Resource/SoundManager.h"
#include "Tethys/Resource/LocalizedStrings.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <filesystem>
#include <map>
#include <set>

using namespace Tethys;
using namespace Patcher::Util;
using namespace Patcher::Registers;

static constexpr uint32 MaxNumMessagesLogged = 64;                           // ** TODO Try to increase this?
static constexpr uint32 MaxLogMessageLen     = sizeof(ListItem::text) - 10;  // ** TODO Try to increase this?
static constexpr uint32 MaxChatMessageLen    = min(sizeof(ChatCommand::message), MaxLogMessageLen);

static MessageLogEntry<MaxLogMessageLen> g_messageLogRb[MaxNumMessagesLogged] = { };
static char g_chatBarMessage[MaxLogMessageLen]   = { };
static char g_statusBarMessage[MaxLogMessageLen] = { };

// =====================================================================================================================
// Replacement resource template names in .rc files must be defined using the RESOURCE_REPLACE macro defined in Util.h.
static std::string FindResourceReplacement(
  const char* pResType,
  const char* pTemplate,
  HMODULE*    phModule)
{
  const auto hModule = *phModule;

  std::string moduleName;
  if ((hModule == NULL) || (hModule == g_tApp.hOut2ResLib_)) {
    moduleName = "OUT2RES.DLL";
  }
  else if (hModule == g_tApp.hInstance_) {
    moduleName = "OUTPOST2.EXE";
  }
  else if (hModule == GetModuleHandleA("op2shres.dll")) {
    moduleName = "OP2SHRES.DLL";
  }
  else {
    char buf[MAX_PATH] = "";
    GetModuleFileNameA(hModule, &buf[0], sizeof(buf));
    moduleName = std::filesystem::path(buf).filename().string();
    std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::toupper);
  }

  char buf[256] = "";
  if (IS_INTRESOURCE(pTemplate)) {
    snprintf(
      &buf[0], sizeof(buf), RESOURCE_REPLACE_STR(%s, %i), moduleName.data(), reinterpret_cast<int>(pTemplate));
  }
  else {
    snprintf(&buf[0], sizeof(buf), RESOURCE_REPLACE_STR(%s, %s), moduleName.data(), pTemplate);
  }

  std::string result = "";
  if (FindResourceA(static_cast<HMODULE>(g_hInst), &buf[0], pResType) != NULL) {
    result = buf;
    *phModule = static_cast<HMODULE>(g_hInst);
  }

  return result;
}

// =====================================================================================================================
// Replaces several UI dialogs, as well as the icon used by game windows.
bool SetUiResourceReplacePatch(
  bool enable)
{
  static Patcher::PatchContext op2Patcher;
  static Patcher::PatchContext shellPatcher("OP2Shell.dll", true);
  bool success = true;

  if (enable) {
    // ** TODO Consider hooking the Win32 API imports instead?
    op2Patcher.Hook(IDlgWnd::Vtbl()->pfnDoModal, SetCapturedTrampoline, ThiscallFunctor(
      [F = decltype(IDlgWnd::VtblType::pfnDoModal){}](IDlgWnd* pThis, const char* pTmpl, HMODULE hMod) {
        const  std::string newName = FindResourceReplacement(RT_DIALOG, pTmpl, &hMod);
        return F(pThis, (newName.empty() ? pTmpl : newName.data()), hMod);
      }));

    op2Patcher.Hook(IDlgWnd::Vtbl()->pfnDoModeless, SetCapturedTrampoline, ThiscallFunctor(
      [F = decltype(IDlgWnd::VtblType::pfnDoModeless){}](IDlgWnd* pThis, const char* pTmpl, HMODULE hMod, HWND hWnd) {
        const  std::string newName = FindResourceReplacement(RT_DIALOG, pTmpl, &hMod);
        return F(pThis, (newName.empty() ? pTmpl : newName.data()), hMod, hWnd);
      }));

    static constexpr char NewIconName[] = "FENRISUL_OP2_ICON";

    // In GameFrame::RegisterClass()
    op2Patcher.HookCall(0x49B1C4, StdcallLambdaPtr([](HMODULE, const char*)
      { return LoadIconA(g_hInst, &NewIconName[0]); }));

    // In OP2Shell::RegisterClass()
    shellPatcher.HookCall(0x130092ED, StdcallLambdaPtr([](HMODULE, const char*)
      { return LoadIconA(g_hInst, &NewIconName[0]); }));

    // In IWnd::RegisterClass()
    op2Patcher.LowLevelHook(0x43151D, [](Esp<void*> pEsp)
      { static_cast<WNDCLASSA*>(PtrInc(pEsp,  8))->hIcon = LoadIconA(g_hInst, &NewIconName[0]); });

    // In AviWnd::RegisterClass()
    shellPatcher.LowLevelHook(0x1300129E, [](Esp<void*> pEsp)
      { static_cast<WNDCLASSA*>(PtrInc(pEsp, 20))->hIcon = LoadIconA(g_hInst, &NewIconName[0]); });

    success = ((op2Patcher.GetStatus() == PatcherStatus::Ok) && (shellPatcher.GetStatus() == PatcherStatus::Ok));
  }

  if ((enable == false) || (success == false)) {
    success &= (op2Patcher.RevertAll()   == PatcherStatus::Ok);
    success &= (shellPatcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
static int __fastcall MessageLog_AddMessage(
  MessageLog*  pThis,  int,
  uint32       pixelX,
  uint32       pixelY,
  char*        pText,
  SoundID      soundID)
{
  if (soundID == SoundID{0}) {
    soundID = SoundID::Beep8;
  }

  int result = 1;
  if ((soundID < SoundID::SavantBegin) || (soundID > SoundID::SavantEnd)) {
    g_soundManager.AddGameSound(soundID, -1);
  }
  else if (pThis->ShouldAddMessage(soundID, pixelX, pixelY)) {
    g_soundManager.AddSavantSound(soundID, pixelX, pixelY, -1);
  }
  else {
    result = 0;
  }

  if (result != 0) {
    pThis->timestamps_[0] = timeGetTime();

    int index = ((pThis->rbBegin_ + pThis->numRbElements_) % MaxNumMessagesLogged);
    if (pThis->numRbElements_ == MaxNumMessagesLogged) {
      pThis->rbBegin_ = ((pThis->rbBegin_ + 1) % MaxNumMessagesLogged);
    }
    else {
      ++(pThis->numRbElements_);
    }

    auto& entry  = g_messageLogRb[index];
    entry.pixelX = pixelX;
    entry.pixelY = pixelY;
    g_gameFrame.SetChatBarText(pText, 0xFFFFFF);
    result = snprintf(&entry.message[0], MaxChatMessageLen, "<N>%i: %s", TethysGame::Mark(), pText);
  }

  return result;
}

// =====================================================================================================================
static int __fastcall CommunicationListData_GetString(
  void*   pThis,  int,
  int     index,
  char*   pBuffer,
  size_t  bufferSize)
{
  auto& entry = g_messageLogRb[g_messageLog.GetEntrySlotFromIndex<MaxNumMessagesLogged>(index)];
  return snprintf(
    pBuffer, bufferSize, (((entry.pixelX != -2) || (entry.pixelY != -1)) ? "<C2>%s" : "%s"), &entry.message[0]);
}

// =====================================================================================================================
static void __fastcall MessageLogJumpTo_OnClick(
  void*  pThis)
{
  int index = g_messageLog.GetEntrySlotFromIndex<MaxNumMessagesLogged>(OP2Mem<0x567D30, int&>());
  if (index != -1) {
    auto& entry = g_messageLogRb[index];
    if (entry.pixelY != -1) {
      g_gameFrame.detailPane_.CenterViewOn(entry.pixelX, entry.pixelY);
    }
  }
}

// =====================================================================================================================
static void __fastcall CommunicationsReport_CenterViewOn(
  void*  pThis,  int,
  int    index)
{
  if (index != -1) {
    auto& entry = g_messageLogRb[g_messageLog.GetEntrySlotFromIndex<MaxNumMessagesLogged>(index)];
    if (entry.pixelY != -1) {
      g_gameFrame.detailPane_.CenterViewOn(entry.pixelX, entry.pixelY);
    }
  }
}

// =====================================================================================================================
static void __fastcall CommunicationsReport_SetJumpButtonEnabled(
  void*  pThis,  int,
  int    index)
{
  auto*const pButton = static_cast<UIButton*>(PtrInc(pThis, 2876));  // ** TODO come up with a def for CommsReport

  if (index == -1) {
    pButton->SetEnabledState(false);
  }
  else {
    auto& slot  = *static_cast<int*>(PtrInc(pThis, 3040));
    slot        = g_messageLog.GetEntrySlotFromIndex<MaxNumMessagesLogged>(index);
    auto& entry = g_messageLogRb[slot];
    pButton->SetEnabledState(entry.pixelY != -1);
  }
}

// =====================================================================================================================
// Increases the max length of chat messages, and bypasses forced uppercasing of chat bar display.
bool SetChatLengthPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    constexpr uint32 MaxPrefixLen     = sizeof("<N>12345: MaxLenName12: ") - 1;
    constexpr uint32 ReservePrefixLen = max(MaxPrefixLen - (MaxLogMessageLen - MaxChatMessageLen), 0);
    constexpr uint32 MaxInputLen      = (MaxChatMessageLen - ReservePrefixLen) - 1;

    // Status bar (chat input) patches

    // In StatusBar::OnUIEvent()
    patcher.LowLevelHook(0x4118C0, [](Eax<uint32> len) { return (len <= MaxInputLen) ? 0x4118C9 : 0x41196C; });
    patcher.LowLevelHook(0x4118EA, [](Eax<uint32> len, Ebx<char> val)
      { g_statusBarMessage[len] = val;  return 0x4118EE; });
    patcher.LowLevelHook(0x4119DB, [](Edi<CommandPacket*> pPacket, Eax<uint32> len)
      { strncpy_s(&pPacket->data.chat.message[0], MaxChatMessageLen, &g_statusBarMessage[0], len);  return 0x4119F1; });

    // In StatusBar::SetTopText()
    patcher.LowLevelHook(0x411D30, [](Ebx<char*>& pDst) { pDst = &g_statusBarMessage[0]; });
    patcher.LowLevelHook(0x411D80, [](Edi<int>& i)
      { const auto index = i++;  g_statusBarMessage[index] = toupper(g_statusBarMessage[index]);  return 0x411D93; });
    patcher.LowLevelHook(0x411DB4, [](Eax<int> i) { return (g_statusBarMessage[i] == '&') ? 0x411DC5 : 0x411DBA; });
    patcher.LowLevelHook(0x411DE0, [](Eax<int> len, Edx<char>& chr) { chr = g_statusBarMessage[len]; });

    // In StatusBar::Paint()
    // ** TODO Probably worth it to just reimplement this function entirely, but this works
    patcher.LowLevelHook(0x411ABA, [](Eax<char*>& pStr) { pStr = &g_statusBarMessage[0]; });
    patcher.LowLevelHook(0x411ADB, [](Eax<char*>& pStr) { pStr = &g_statusBarMessage[0]; });
    patcher.LowLevelHook(0x411B90, [](Edx<char*>& pStr) { pStr = &g_statusBarMessage[0]; });
    patcher.LowLevelHook(0x411BC3, [](Eax<char*>& pStr) { pStr = &g_statusBarMessage[0]; });

    patcher.LowLevelHook(0x411B02, [](Eax<char*>& pAmpersand, Eax<int> offset)
      { pAmpersand = &g_statusBarMessage[1 + offset];  return 0x411B07; });
    patcher.LowLevelHook(0x411C2F, [](Eax<char*>& pAmpersand, Eax<int> offset)
      { pAmpersand = &g_statusBarMessage[1 + offset];  return 0x411C34; });

    patcher.LowLevelHook(0x411B17, [](Ecx<int>& len, Eax<char*>& pAfterAmp, Eax<int> offset)
      { len -= 2;  pAfterAmp = &g_statusBarMessage[2 + offset];  return 0x411B1F; });
    patcher.LowLevelHook(0x411BE9, [](Ecx<int>& len, Eax<char*>& pAfterAmp, Eax<int> offset)
      { len -= 2;  pAfterAmp = &g_statusBarMessage[2 + offset];  return 0x411BF1; });

    // DansRule (chat display) patches

    // Reimplement GameFrame::SetChatBarText()
    patcher.Hook(0x49BFE0, ThiscallLambdaPtr([](GameFrame* pThis, char* pText, COLORREF color) {
      pThis->chatColor_  = color;
      pThis->chatLength_ = strlen(pText);
      strncpy_s(&g_chatBarMessage[0], MaxChatMessageLen, pText, _TRUNCATE);
      InvalidateRect(pThis->hWnd_, &pThis->chatRect_, FALSE);
      }));
    // In DansRule::OnPaint()
    patcher.LowLevelHook(0x499F22, [](Edx<char*>& pChatMessage) { pChatMessage = &g_chatBarMessage[0]; });
    // In DansRule::WndProc()
    patcher.LowLevelHook(0x49BE87, [] { g_chatBarMessage[0] = '\0'; });

    // Communications log patches

    patcher.Hook(0x439070, &MessageLog_AddMessage);
    patcher.Hook(0x466760, &CommunicationListData_GetString);
    patcher.Hook(0x466800, &MessageLogJumpTo_OnClick);
    patcher.Hook(0x466B30, &CommunicationsReport_CenterViewOn);
    patcher.Hook(0x466AA0, &CommunicationsReport_SetJumpButtonEnabled);

    // In MessageLog::Save()
    patcher.LowLevelHook(0x439350, [](Eax<int> index, Edi<StreamIO*> pStream)
      { return (pStream->Write(sizeof(g_messageLogRb[0]), &g_messageLogRb[index]) != 0) ? 0x439367 : 0x43937E; });
    // In MessageLog::Load()
    // ** TODO See if this can be made backwards compatible with older saves
    patcher.LowLevelHook(0x43941D, [](Eax<int> index, Esi<StreamIO*> pStream)
      { return (pStream->Read(sizeof(g_messageLogRb[0]), &g_messageLogRb[index])  != 0) ? 0x439434 : 0x43944B; });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Replaces usage of Arial in the game's UI with the given font.
// ** TODO Figure out how to make antialiasing look better.  Replacing main menu fonts requires replacing all dialogs.
bool SetFontPatch(
  const char* pNewFont)
{
  static Patcher::PatchContext patcher;
  static std::string  fontStr;

  bool success   = true;
  bool isDefault = ((pNewFont == nullptr) || (_stricmp(pNewFont, "Arial") == 0));

  if (isDefault) {
    pNewFont = "Arial";
  }

  fontStr = pNewFont;
  
  LOGFONTA createInfo = {};
  createInfo.lfCharSet        = 1;
  createInfo.lfOutPrecision   = 7;
  createInfo.lfPitchAndFamily = 82;
  strncpy_s(createInfo.lfFaceName, sizeof(createInfo.lfFaceName), pNewFont, _TRUNCATE);

  createInfo.lfHeight = -11;
  createInfo.lfWeight = 400;

  // ** TODO Define these font fields
  for (Font* pFont: { g_gameFrame.pFont1_, (Font*)PtrInc(&g_gameFrame, 0x2828), (Font*)PtrInc(&g_gameFrame, 0x4488) }) {
    pFont->Init(createInfo);
  }

  if (isDefault == false) {
    createInfo.lfHeight = 13;
    createInfo.lfWeight = 700;
    patcher.Write(&g_gameFrame.hChatFont_,        CreateFontIndirectA(&createInfo));
    patcher.Write(&g_gameFrame.statusBar_.hFont_, CreateFontIndirectA(&createInfo));

    createInfo.lfHeight = 48;
    patcher.Write(&g_gameFrame.detailPane_.hLargeMessageFont_, CreateFontIndirectA(&createInfo));

    patcher.ReplaceReferencesToGlobal(0x4DEB40, 1, fontStr.data());  // Replace "Arial"

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if (isDefault || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Swap the behavior of holding shift vs. not when using control group hotkeys, such that using shift centers your view.
// ** TODO Would like to make it so if you double-select a group (e.g. press "1" twice) that also centers your view
bool SetControlGroupHotkeyPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In TApp::HandleCommand()
    patcher.WriteBytes(0x486901, { 0x40, 0x04, });  // 0x2C, 0x02
    patcher.WriteBytes(0x486917, { 0x0A, 0x04, });  // 0xF9, 0x01
    patcher.WriteBytes(0x486989, { 0x87, 0x01, });  // 0x98, 0x03
    patcher.WriteBytes(0x4869A4, { 0x89, 0x01, });  // 0x9D, 0x03
    patcher.WriteBytes(0x486B16, { 0x21, 0x9D, });  // 0xDE, 0x9C
    patcher.WriteBytes(0x486D27, { 0xDE, 0x9C, });  // 0x21, 0x9D

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Patch for setting default focus of UI elements.
// ** TODO Reimplement this with C++ hooks
bool SetUiHighlightFix(
  bool enable)
{
  static Patcher::PatchContext patcher("odasl.dll", true);
  bool success = true;

  if (enable) {
    patcher.WriteBytes(0x20004E92, {
      0xF6, 0xC1, 0x04,                          // test cl, 0x4
      0x75, 0x19,                                // jne  0x1E (0x20004EB0)
      0xF6, 0xC1, 0x10,                          // test cl, 0x10
      0x75, 0x09,                                // jne  0x13 (0x20004EA5)
      0xF6, 0xC1, 0x01,                          // test cl, 0x1
      0x75, 0x01,                                // jne  0x10 (0x20004EA2)
      0xC3,                                      // retn
      0xB0, 0x03,                                // mov  al, 0x3
      0xC3,                                      // retn
      0xF6, 0xC1, 0x01,                          // test cl, 0x1
      0x75, 0x03,                                // jne  0x1B (0x20004EAD)
      0xB0, 0x02,                                // mov  al, 0x2
      0xC3,                                      // retn
      0xB0, 0x04,                                // mov  al, 0x4               
      0xC3,                                      // retn
      0xF6, 0xC1, 0x01,                          // test cl, 0x1
      0x75, 0x03,                                // jne  0xB (0x20004EB8)
      0xB0, 0x01,                                // mov  al, 0x1
      0xC3,                                      // retn
      0xF6, 0x05, 0xB9, 0x21, 0x01, 0x20, 0x10,  // test BYTE PTR ds:0x200121B9, 0x10
      0x74, 0xE0,                                // je   -0xC (0x20004EA1)
      0xB0, 0x05,                                // mov  al, 0x5
      0xC3,                                      // retn

      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,  // nop
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Set default focus of the checkboxes in the broadcast/IP address window.
bool SetIpWindowFocusPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In IPDialog::DlgProc()
    patcher.Write<uint32>(0x419779, 1008);  // GetDlgItem.nIDDlgItem 1005 => 1008
    patcher.Write<uint32>(0x41979B, 1007);  // GetDlgItem.nIDDlgItem 1005 => 1007
    patcher.Write<uint8>(0x4197AE,  1);     // EnableWindow.bEnable false => true
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Check bottom coordinate instead of top coordinate of window clip rect in minimap logic.
bool SetMiniMapFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In MiniMapPane::WndProc()
    success = (patcher.Write<uint8>(0x45865F, 0x1C) == PatcherStatus::Ok);  // 0x14
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Makes vehicles display their cargo in mouseover tooltips and in destroyed notifications in the message log.
bool SetVehicleCargoDisplayPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable && (patcher.NumPatches() == 0)) {
    static constexpr int UnitStrSize = sizeof(MapObjectType::unitName_);

    static constexpr const char* TruckCargoStrings[] = {
      "empty",
      "Food",
      "Common Ore",
      "Rare Ore",
      "Common Metals",
      "Rare Metals",
      "Common Rubble",
      "Rare Rubble",
      "Starship Module",
      "Wreckage",
      "Gene Bank"
    };
    static_assert(TethysUtil::ArrayLen(TruckCargoStrings) == size_t(TruckCargo::Count),
                  "The TruckCargoStrings table needs to be updated.");

    // Make ConVecs, Cargo Trucks, and Evacuation Transports display their cargo in mouseover tooltips.
    static auto GetCargoStr = [](Vehicle* pVec) -> std::string {
      switch (pVec->GetTypeID()) {
      case mapCargoTruck: {
        const auto       cargoType  = TruckCargo(max(pVec->truckCargoType_, 0));
        const char*const pCargoName =
          (cargoType == TruckCargo::Spaceport) ? &MapObjectType::GetInstance(pVec->truckCargoAmount_)->unitName_[0] :
          (cargoType <  TruckCargo::Count)     ? TruckCargoStrings[size_t(cargoType)] : nullptr;
        const auto       quantity   = ((pVec->truckCargoAmount_ > 1) && (cargoType <= TruckCargo::RareRubble)) ?
          std::to_string(pVec->truckCargoAmount_) : "";

        return (pCargoName != nullptr) ? (quantity + ((quantity[0] != '\0') ? " " : "") + pCargoName) : "empty";
      }
      case mapConVec: {
        auto*const pCargoName  = &MapObjectType::GetInstance(pVec->cargo_)->unitName_[0];
        auto*const pWeaponName =
          (pVec->weaponOfCargo_ != mapNone) ? &MapObjectType::GetInstance(pVec->weaponOfCargo_)->unitName_[0] : "";

        return std::string(pWeaponName) + ((pWeaponName[0] != '\0') ? " " : "") + pCargoName;
      }
      case mapEvacuationTransport:  return (pVec->cargo_ != 0) ? "Colonists" : "empty";
      default:                      return "";
      }
    };

    Vehicle::VtblType*const pVtbls[] =
      { MapObj::ConVec::Vtbl(), MapObj::CargoTruck::Vtbl(), MapObj::EvacuationTransport::Vtbl() };

    for (auto* pVtbl : pVtbls) {
      patcher.Write(
        &pVtbl->pfnGetMouseOverStr,
        ThiscallLambdaPtr([](Vehicle* pThis, char* pDst, int size) {
          auto*const pUnitTypeName = &pThis->GetType()->unitName_[0];
          return ((pThis->cargo_ != 0) && Player[pThis->ownerNum_].IsAlliedTo(TethysGame::LocalPlayer()))   ?
            _snprintf_s(pDst, size_t(size), _TRUNCATE, "%s (%s)", pUnitTypeName, GetCargoStr(pThis).data()) :
            _snprintf_s(pDst, size_t(size), _TRUNCATE, "%s",      pUnitTypeName);
        }));
    }

    // Hide redundant vehicle cargo text in storage bay command pane views.
    // In CommandPaneView::BuildingStorageBays::Draw()
    patcher.LowLevelHook(0x464050, [](Ecx<MapObject*> pUnit, Eax<char*> pBuffer) {
      const MapID id = pUnit->GetTypeID();
      if ((id == mapConVec) || (id == mapCargoTruck) || (id == mapEvacuationTransport)) {
        strncpy_s(pBuffer, UnitStrSize, &pUnit->GetType()->unitName_[0], _TRUNCATE);
        return 0x464058;
      }
      else {
        return 0;
      }
    });

    // Display cargo in vehicle destroyed messages.
    // In Unit::ProcessForGameCycle()
    patcher.LowLevelHook(0x43DC24, [](Esi<Vehicle*> pThis, Eax<char*> pBuffer) {
      const MapID id = pThis->GetTypeID();
      if (((id == mapConVec) || (id == mapCargoTruck) || (id == mapEvacuationTransport)) && (pThis->cargo_ != 0)) {
        char unitName[UnitStrSize] = "";
        pThis->GetSelectionStr(&unitName[0], UnitStrSize);
        _snprintf_s(pBuffer, UnitStrSize, _TRUNCATE, "%s (%s)", &unitName[0], GetCargoStr(pThis).data());
      }
      else {
        pThis->GetSelectionStr(pBuffer, UnitStrSize);
      }
      return 0x43DC2E;
    });
    
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Silences spammy or broken Savant colony status notifications, and adds "enemy unit sighted" alert for light towers.
// ** TODO Disaster warnings should only be reported near units, walls, maybe unit paths.
bool SetSavantNotificationPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Skip alliance formed/broken with player X messages at the start of the game, or if either player is AI or gaia.
    // In Player::ProcessCommandPacket()
    for (uintptr loc : { 0x40FFC5, 0x410051 }) {
      patcher.LowLevelHook(loc, [](Ecx<int> p1, Esi<int> p2) {
        return ((p1 >= 6) || (p2 >= 6) || Player[p1].IsAI() || Player[p2].IsAI() || (TethysGame::Tick() == 0)) ?
               0x4100C8 : 0;
      });
    }

    // Skip transfer received/complete with player X messages if either player is AI or gaia.
    // In Player::ProcessCommandPacket()
    // ** TODO pSrcPlayer == nullptr sometimes??
    patcher.LowLevelHook(0x40F61A, [](Ebp<PlayerImpl*> pSrcPlayer, Esi<uint16> dstPlayerNum)
      { return ((pSrcPlayer != nullptr) && pSrcPlayer->isHuman_ && Player[dstPlayerNum].IsHuman()) ? 0 : 0x4101D3; });

    static SoundID prevPowerSound      = {};
    static SoundID prevFoodSound       = {};
    static SoundID prevMoraleSound     = {};
    static bool    commonStorageNeeded = false;
    static bool    rareStorageNeeded   = false;

    // In SoundManager::Reset()
    patcher.LowLevelHook(0x47E4CC, [] {
      prevPowerSound      = prevFoodSound     = prevMoraleSound = {};
      commonStorageNeeded = rareStorageNeeded = false;
    });

    // In Player::UpdateColonyStatusAndReport()
    // Silence repeated "power levels optimal".
    patcher.HookCall(
      0x47275B,
      ThiscallLambdaPtr([](MessageLog* pThis, int pixelX, int pixelY, char* pMsg, SoundID soundID) -> int {
        const bool skip = (soundID == SoundID::Savant27) && (prevPowerSound == soundID);
        prevPowerSound  = (skip || pThis->AddMessage(pixelX, pixelY, pMsg, soundID)) ? soundID : SoundID{};
        return (prevPowerSound != SoundID{});
      }));

    // Silence repeated "metals storage needed".
    static auto DoMetalAlert = [](int capacity, int amount, size_t strIdx, SoundID soundID, bool* pStorageNeeded) {
      const int  threshold = *pStorageNeeded ? 5000 : 1000;  // Remaining storage threshold for the alert
      const bool doAlert   = (capacity < 100000) && ((capacity - threshold) < amount);
      if (*pStorageNeeded != doAlert) {
        *pStorageNeeded = doAlert && g_messageLog.AddMessage(-1, -1, GetLocalizedString(strIdx), soundID);
      }
    };

    patcher.LowLevelHook(0x472832, [](Ecx<int> commonCap, Edx<int> curCommon) {
      DoMetalAlert(
        commonCap, curCommon, LocalizedString::CommonMetalsStorageNeeded, SoundID::Savnt221, &commonStorageNeeded);
      return 0x472855;
    });
    patcher.LowLevelHook(0x472878, [](Eax<int> rareCap,   Ecx<int> curRare) {
      DoMetalAlert(
        rareCap,   curRare,   LocalizedString::RareMetalsStorageNeeded,   SoundID::Savant51, &rareStorageNeeded);
      return 0x47289A;
    });

    // Fix the "food stores are plentiful" message to correctly trigger when (stored-netConsumed) >= (population*15),
    // instead of when population == 0.
    //
    // This actually fixes the condition for "food production in surplus"; the original is (1.05p>c), preceded by (c>p)
    // for "diminishing", thus the logic to get to "plentiful" fully simplifies to (p == 0), which is nonsense.
    patcher.LowLevelHook(0x4721B0, [](Edi<int> production, Esi<int> consumption)
      { return (production > ((consumption * 20) / 19)) ? 0x4721C2 : 0x4721CB; });

    // Silence repeated "food production in surplus"/"food stores are plentiful".
    patcher.HookCall(
      0x4721EA,
      ThiscallLambdaPtr([](MessageLog* pThis, int pixelX, int pixelY, char* pMsg, SoundID soundID) -> int {
        const bool skip =
          ((soundID == SoundID::Savant24) || (soundID == SoundID::Savnt226)) && (prevFoodSound == soundID);
        prevFoodSound   = (skip || pThis->AddMessage(pixelX, pixelY, pMsg, soundID)) ? soundID : SoundID{};
        return (prevFoodSound != SoundID{});
      }));

    // Silence repeated "morale is good/excellent".  Skip any repeated morale notifications if morale level is forced.
    // In Morale::UpdatePlayerMorale()
    patcher.HookCall(
      0x44F065,
      ThiscallLambdaPtr([](MessageLog* pThis, int pixelX, int pixelY, char* pMsg, SoundID soundID) -> int {
        const bool forced = (g_gameImpl.forceMorale_[0] | g_gameImpl.forceMorale_[1] | g_gameImpl.forceMorale_[2] |
                             g_gameImpl.forceMorale_[3] | g_gameImpl.forceMorale_[4]) & (1u << g_gameImpl.localPlayer_);
        const bool skip =
          (forced || (soundID == SoundID::Savant19) || (soundID == SoundID::Savant20)) && (prevMoraleSound == soundID);
        prevMoraleSound = (skip || pThis->AddMessage(pixelX, pixelY, pMsg, soundID)) ? soundID : SoundID{};
        return (prevMoraleSound != SoundID{});
      }));

    // ** TODO Silence irrelevant disaster warnings.  New behavior will be as follows:
    // Volcanoes:             Always alert.
    // Meteors and quakes:    Alert if any unit or unit path is near the damage radius.
    // Storms  and vortexes:  Alert if any unit or unit path is near the disaster's entire path.
    // Meteors and vortexes:  Any nearby walls will also trigger alerts.

    // Allow Light Towers to emit "enemy unit sighted!" alerts like Scouts do.

    // We repurpose the last byte of the LightTower instance as the "has sighted" flag.
    static auto LightTowerSightedFlag =
      [](MapObj::LightTower* pLtTower) -> uint8& { return *(reinterpret_cast<uint8*>(pLtTower) + MapObjectSize - 1); };

    // Initialize the "has sighted" flag to 0.
    // In MapObjType::LightTower::Create()
    patcher.LowLevelHook(0x442746, [](Eax<MapObj::LightTower*> pUnit) { LightTowerSightedFlag(pUnit) = 0; });

    patcher.Write(&MapObj::LightTower::Vtbl()->pfnDoEvent, ThiscallLambdaPtr([](MapObj::LightTower* pThis) {
      auto& hasSighted = LightTowerSightedFlag(pThis);
      if (pThis->IsEnemyUnitSighted()) {
        if (hasSighted == 0) {
          hasSighted = 1;
          if (pThis->ownerNum_ == TethysGame::LocalPlayer()) {
            g_messageLog.AddMessage(
              pThis->pixelX_, pThis->pixelY_, GetLocalizedString(LocalizedString::EnemySighted), SoundID::Savant14);
          }
        }
      }
      else {
        hasSighted = 0;
      }
    }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Replacement mission info for stock missions in select mission lists.
bool SetMissionListNamePatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static const std::map<std::string_view, std::string_view> missionNames = {
      { "mf2_01.dll",  "2P, SR, 'Close Encounter'"    },    { "mf3_02.dll",  "3P, SR, 'Biohazard'"        },
      { "mf3_08.dll",  "3P, SR, 'Three's a Crowd'"    },    { "mf4_03.dll",  "4P, SR, 'Rock Garden'"      },
      { "mf4_05.dll",  "4P, SR, 'Crash Zone'"         },    { "mf4_08.dll",  "4P, SR, 'The Rift'"         },
      { "mf6_02.dll",  "6P, SR, 'Around the World'"   },    { "mf6_12.dll",  "6P, SR, 'Root Canal'"       },
      { "ml2_01.dll",  "2P, LoS, 'Close Encounter'"   },    { "ml2_02.dll",  "2P, LoS, 'Great Divide'"    },
      { "ml2_03.dll",  "2P, LoS, 'River of Fire'"     },    { "ml2_05.dll",  "2P, LoS, 'Wishbone'"        },
      { "ml2_08.dll",  "2P, LoS, 'Ambush Alley'"      },    { "ml2_10.dll",  "2P, LoS, 'Beach Party'"     },
      { "ml2_11.dll",  "2P, LoS, 'Labyrinth'"         },    { "ml2_21.dll",  "2P, LoS, 'Peacekeeper'"     },
      { "ml2_31.dll",  "2P, LoS, 'Tug-O-War'"         },    { "ml3_08.dll",  "3P, LoS, 'Three's a Crowd'" },
      { "ml4_01.dll",  "4P, LoS, 'Coliseum'"          },    { "ml4_05.dll",  "4P, LoS, 'Crash Zone'"      },
      { "ml4_07.dll",  "4P, LoS, 'Four Mesas'"        },    { "ml4_08.dll",  "4P, LoS, 'The Rift'"        },
      { "ml4_10.dll",  "4P, LoS, 'Double Cross'"      },    { "ml4_12.dll",  "4P, LoS, 'Olympus Mons'"    },
      { "ml4_13.dll",  "4P, LoS, 'Unsettled Earth'"   },    { "ml4_21.dll",  "4P, LoS, 'La Corrida'"      },
      { "ml4_41.dll",  "4P, LoS, 'Handprint'"         },    { "ml6_01.dll",  "6P, LoS, 'Axen's Home'"     },
      { "ml6_02.dll",  "6P, LoS, 'Around the World'"  },    { "ml6_03.dll",  "6P, LoS, 'Allied Siege'"    },
      { "ml6_12.dll",  "6P, LoS, 'Root Canal'"        },    { "ml6_21.dll",  "6P, LoS, 'Pie Chart'"       },
      { "ml6_31.dll",  "6P, LoS, 'Earlobe'"           },    { "mm2_07.dll",  "2P, Midas, 'Twin Valleys'"  },
      { "mm3_08.dll",  "3P, Midas, 'Three's a Crowd'" },    { "mm4_01.dll",  "4P, Midas, 'Coliseum'"      },
      { "mm4_04.dll",  "4P, Midas, 'Barren Land'"     },    { "mm4_08.dll",  "4P, Midas, 'The Rift'"      },
      { "mm4_11.dll",  "4P, Midas, 'Hidden Treasure'" },    { "mr2_10.dll",  "2P, RR, 'Beach Party'"      },
      { "mr3_08.dll",  "3P, RR, 'Three's a Crowd'"    },    { "mr4_03.dll",  "4P, RR, 'Rock Garden'"      },
      { "mr4_05.dll",  "4P, RR, 'Crash Zone'"         },    { "mr4_10.dll",  "4P, RR, 'Double Cross'"     },
      { "mr4_12.dll",  "4P, RR, 'Olympus Mons'"       },    { "mr6_01.dll",  "6P, RR, 'Axen's Home'"      },
      { "mu2_02.dll",  "2P, LR, 'Great Divide'"       },    { "mu2_10.dll",  "2P, LR, 'Beach Party'"      },
      { "mu3_08.dll",  "3P, LR, 'Three's a Crowd'"    },    { "mu4_03.dll",  "4P, LR, 'Rock Garden'"      },
      { "mu4_04.dll",  "4P, LR, 'Barren Land'"        },    { "mu4_07.dll",  "4P, LR, 'Four Mesas'"       },
      { "mu4_08.dll",  "4P, LR, 'The Rift'"           },    { "mu4_13.dll",  "4P, LR, 'Unsettled Earth'"  },
      { "mu4_31.dll",  "4P, LR, 'Flood Plain'"        },    { "mu6_01.dll",  "6P, LR, 'Axen's Home'"      },
      { "mu6_02.dll",  "6P, LR, 'Around the World'"   },    { "mu6_12.dll",  "6P, LR, 'Root Canal'"       },
      { "mu6_41.dll",  "6P, LR, 'Fractures'"          },
    };

    // In GetModuleDesc()
    patcher.LowLevelHook(0x402627, [](Esi<const char*> pModuleName, Esp<void*> pEsp, Eax<const ModDesc*>& pModDesc) {
      std::string moduleName(pModuleName);

      const size_t separatorPos = moduleName.find_last_of("\\/");
      if (separatorPos != std::string::npos) {
        moduleName.erase(moduleName.begin(), moduleName.begin() + separatorPos + 1);
      }

      std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);

      const auto it = missionNames.find(moduleName);
      const auto**const ppLevelDesc = static_cast<const char**>(PtrInc(pEsp, 36));
      *ppLevelDesc = (it != missionNames.end()) ? it->second.data() : nullptr;

      // us_them is a useless unit reference-type mission where both players are uncontrollable AIs (forced to GoAI())
      // that is set to MissionType::Colony.  Set its mission type to 0 to hide it from mission list UIs.
      if (moduleName == "us_them.dll") {
        static const ModDesc Desc =
          { MissionType{0}, pModDesc->numPlayers, pModDesc->maxTechLevel, pModDesc->unitMission };
        pModDesc = &Desc;
      }
    });

    patcher.LowLevelHook(0x40265E, [](Eax<const char*> pLevelDesc, Esp<void*> pEsp) {
      auto**const ppLevelDesc = static_cast<const char**>(PtrInc(pEsp, 40));
      if (*ppLevelDesc == nullptr) {
        *ppLevelDesc = pLevelDesc;
      }
      return 0x402662;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
