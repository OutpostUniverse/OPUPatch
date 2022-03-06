
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <timeapi.h>
#include <wincodec.h>

#include "Patcher.h"
#include "Util.h"
#include "Resources.h"
#include "Stream.h"

#include "Tethys/Common/Library.h"

#include "Tethys/API/Mission.h"
#include "Tethys/API/Game.h"
#include "Tethys/API/GameMap.h"

#include "Tethys/Game/TApp.h"
#include "Tethys/Game/MapObject.h"

#include "Tethys/UI/GameFrame.h"
#include "Tethys/UI/Odasl.h"

#include "Tethys/Resource/CConfig.h"
#include "Tethys/Resource/StreamIO.h"
#include "Tethys/Resource/ResManager.h"
#include "Tethys/Resource/MemoryMappedFile.h"
#include "Tethys/Resource/Font.h"
#include "Tethys/Resource/SoundManager.h"
#include "Tethys/Resource/LocalizedStrings.h"
#include "Tethys/Resource/GFXBitmap.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <string>
#include <filesystem>
#include <map>
#include <set>
#include <deque>
#include <charconv>

using namespace Tethys;
using namespace TethysAPI;
using namespace Tethys::TethysUtil;
using namespace Patcher::Util;
using namespace Patcher::Registers;

static constexpr char NewIconName[] = "FENRISUL_OP2_ICON";

static constexpr uint32 MaxNumMessagesLogged = 64;                           // ** TODO Try to increase this?
static constexpr uint32 MaxLogMessageLen     = sizeof(ListItem::text) - 10;  // ** TODO Try to increase this?
static constexpr uint32 MaxChatMessageLen    = (std::min)(sizeof(ChatCommand::message), MaxLogMessageLen);

static MessageLogEntry<MaxLogMessageLen> g_messageLogRb[MaxNumMessagesLogged] = { };
static char g_chatBarMessage[MaxLogMessageLen]   = { };
static char g_statusBarMessage[MaxLogMessageLen] = { };

enum class PreserveAspect : int {
  Disabled = 0,
  Enabled,
  CropWidth,
  CropHeight
};

// =====================================================================================================================
static HBITMAP LoadGdiImageFromFile(
  const          std::filesystem::path& path,
  int            scaleWidth     = 0,
  int            scaleHeight    = 0,
  PreserveAspect preserveAspect = PreserveAspect::Disabled,
  HDC            hDc            = NULL)
{
  HBITMAP hBitmapOut  = NULL;
  HRESULT hInitResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  HRESULT hResult     = hInitResult;

  IWICImagingFactory*    pWic       = nullptr;
  IWICBitmapDecoder*     pDecoder   = nullptr;
  IWICBitmapFrameDecode* pFrame     = nullptr;
  IWICBitmapScaler*      pScaler    = nullptr;
  IWICFormatConverter*   pConverter = nullptr;

  if (SUCCEEDED(hResult)) {
    hResult = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWic));
  }

  // Open the image.
  if (SUCCEEDED(hResult)) {
    const auto filename = std::filesystem::absolute(path);
    hResult = pWic->CreateDecoderFromFilename(
      filename.wstring().data(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
  }

  // Get frame 0 of the image.
  if (SUCCEEDED(hResult)) {
    hResult = pDecoder->GetFrame(0, &pFrame);
  }

  uint32 srcWidth  = 0;
  uint32 srcHeight = 0;
  if (SUCCEEDED(hResult)) {
    hResult = pFrame->GetSize(&srcWidth, &srcHeight);
  }

  // Convert the image to a GDI compatible bitmap.
  if (SUCCEEDED(hResult)) {
    hResult = pWic->CreateFormatConverter(&pConverter);
  }

  if (SUCCEEDED(hResult)) {
    hResult = pConverter->Initialize(
      pFrame, GUID_WICPixelFormat32bppBGR, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom);
  }

  // Scale the image, if requested.
  const bool scaled = (srcWidth > 0) && (srcHeight > 0) && (scaleWidth > 0) && (scaleHeight > 0);
  if (SUCCEEDED(hResult) && scaled) {
    auto ScaleWidth = [preserveAspect, srcWidth, srcHeight, &scaleWidth, &scaleHeight]() {
      if ((preserveAspect == PreserveAspect::Enabled) || (preserveAspect == PreserveAspect::CropWidth)) {
        scaleWidth = scaleHeight * srcWidth / srcHeight;
      }
    };
    auto ScaleHeight = [preserveAspect, srcWidth, srcHeight, &scaleWidth, &scaleHeight]() {
      if ((preserveAspect == PreserveAspect::Enabled) || (preserveAspect == PreserveAspect::CropHeight)) {
        scaleHeight = scaleWidth * srcHeight / srcWidth;
      }
    };

    if (scaleWidth >= scaleHeight) {
      ScaleWidth();
      ScaleHeight();
    }
    else {
      ScaleHeight();
      ScaleWidth();
    }

    if (hResult = pWic->CreateBitmapScaler(&pScaler);  SUCCEEDED(hResult)) {
      hResult = pScaler->Initialize(pConverter, scaleWidth, scaleHeight, WICBitmapInterpolationModeHighQualityCubic);
    }
  }
  else {
    scaleWidth  = srcWidth;
    scaleHeight = srcHeight;
  }

  if (SUCCEEDED(hResult) && (scaleWidth > 0) && (scaleHeight > 0)) {
    if (HDC hDcScreen = GetDC(NULL);  hDcScreen != NULL) {
      BITMAPINFO createInfo = { };
      createInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
      createInfo.bmiHeader.biWidth       =  scaleWidth;
      createInfo.bmiHeader.biHeight      = -scaleHeight;
      createInfo.bmiHeader.biPlanes      = 1;
      createInfo.bmiHeader.biBitCount    = 32;
      createInfo.bmiHeader.biCompression = BI_RGB;

      // Create DIB section and copy the image source to it.
      void*   pImageBuffer = nullptr;
      HBITMAP hDibBitmap   = CreateDIBSection(hDcScreen, &createInfo, DIB_RGB_COLORS, &pImageBuffer, NULL, 0);
      
      if (hDibBitmap != NULL) {
        const size_t scanlineSize = (((scaleWidth * 32) + 31) / 32) * 4;
        const size_t bufferSize   = scanlineSize * scaleHeight;

        // This actually performs the format conversion (and scaling, if requested).
        hResult = scaled ? pScaler->CopyPixels(nullptr, scanlineSize, bufferSize, (BYTE*)(pImageBuffer))
                      : pConverter->CopyPixels(nullptr, scanlineSize, bufferSize, (BYTE*)(pImageBuffer));

        if (SUCCEEDED(hResult)) {
          // Create the output compatible bitmap.
          hBitmapOut = CreateDIBitmap(
            hDc ? hDc : hDcScreen, &createInfo.bmiHeader, CBM_INIT, pImageBuffer, &createInfo, DIB_RGB_COLORS);
        }

        DeleteObject(hDibBitmap);
      }
      else {
        hResult = E_FAIL;
      }

      ReleaseDC(NULL, hDcScreen);
    }
    else {
      hResult = E_FAIL;
    }
  }

  if (SUCCEEDED(hInitResult)) {
    CoUninitialize();
  }

  return hBitmapOut;
}

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
      &buf[0], sizeof(buf), RESOURCE_REPLACE_NAME(%s, %i), moduleName.data(), reinterpret_cast<int>(pTemplate));
  }
  else {
    snprintf(&buf[0], sizeof(buf), RESOURCE_REPLACE_NAME(%s, %s), moduleName.data(), pTemplate);
  }

  std::string result = "";
  if (FindResourceA(g_hInst, &buf[0], pResType) != NULL) {
    // ** TODO This should search in all loaded modules
    result    = buf;
    *phModule = g_hInst;
  }

  return result;
}

// =====================================================================================================================
// Replaces main menu assets.
static bool SetMainMenuPatch(
  bool enable)
{
  static Patcher::PatchContext patcher("OP2Shell.dll", true);
  bool success = true;

  if (enable) {
    // Inject main menu background replacement, and draw the OP2 logo and game version number on the main menu screen.
    static HDC    hDcLogo      = NULL;
    static HANDLE hBitmapLogo  = NULL;
    static BITMAP hPvLogo      = { };
    static HFONT  hVersionFont = NULL;

    // In OP2Shell::Init()
    patcher.LowLevelHook(0x13007EB9, [](Eax<HBITMAP>& hBitmapBg, Esp<RECT*, 0xC> pRect) {
      // If files of the form "MainMenuBackground{00-99}.png" (continuous) exist, pick one at random.
      std::filesystem::path              bgPath;
      std::vector<std::filesystem::path> bgFilePaths;
      char curFilename[] = "MainMenuBackground00.png";
    
      if (auto dir = GetFilePath(curFilename);  dir.has_parent_path()) {
        dir = dir.parent_path();

        for (int i = 0; i < 100; ++i) {
          curFilename[sizeof("MainMenuBackground") - 1] = '0' + (i / 10);
          curFilename[sizeof("MainMenuBackground")]     = '0' + (i % 10);
          if (auto curPath = dir/curFilename;  std::filesystem::exists(curPath)) {
            bgFilePaths.emplace_back(std::move(curPath));
          }
          else {
            break;
          }
        }
      }

      if (bgFilePaths.empty() == false) {
        std::shuffle(bgFilePaths.begin(), bgFilePaths.end(), std::mt19937());
        bgPath = bgFilePaths[0];
      }

      if (hDcLogo = hDcLogo ? hDcLogo : CreateCompatibleDC(NULL);  (hDcLogo != NULL) && (hBitmapLogo == NULL)) {
        // Load the Outpost 2 logo image from op2shres.dll so we can draw it to the main menu.
        HMODULE hOp2ShRes = GetModuleHandleA("op2shres.dll");
        hBitmapLogo = (hOp2ShRes != NULL) ? LoadImageA(hOp2ShRes, MAKEINTRESOURCEA(145), IMAGE_BITMAP, 0, 0, 0) : NULL;
        if (hBitmapLogo != NULL) {
          hPvLogo = { };
          GetObjectA(hBitmapLogo, sizeof(hPvLogo), &hPvLogo);
        }
      }

      if (hVersionFont == NULL) {
        // Initialize the font to use to draw the version text on the main menu.
        LOGFONTA createInfo = { };
        strncpy_s(&createInfo.lfFaceName[0], sizeof(createInfo.lfFaceName), "Arial", _TRUNCATE);
        createInfo.lfHeight         = -11;
        createInfo.lfWeight         = 400;
        createInfo.lfCharSet        = 1;
        createInfo.lfOutPrecision   = 7;
        createInfo.lfPitchAndFamily = 82;
        hVersionFont = CreateFontIndirectA(&createInfo);
      }

      hBitmapBg =
        bgPath.empty() ? NULL : LoadGdiImageFromFile(bgPath, pRect->right, pRect->bottom, PreserveAspect::CropWidth);
      return (hBitmapBg != NULL) ? 0x13007ED3 : 0;  // Use original background image as a fallback.
    });

    // In OP2Shell::WndProc()
    // ** TODO Should do all blits to a backbuffer, modern Windows DWM presents after every window DC blit
    patcher.LowLevelHook(0x13008012, [](Ebx<void*> pThis, Esi<HDC> hDcWnd, Ebp<int> x, Edi<int> y) {
      const auto [cx, cy] = std::tie(*PtrInc<int*>(pThis, 0x1C), *PtrInc<int*>(pThis, 0x20)); // ** TODO define OP2Shell

      // Draw Outpost 2 logo.
      if ((hDcLogo != NULL) && (hBitmapLogo != NULL) && (hPvLogo.bmWidth != 0) && (hPvLogo.bmHeight != 0)) {
        SelectObject(hDcLogo, hBitmapLogo);
        BitBlt(
          hDcWnd, x + ((cx - hPvLogo.bmWidth) / 2), y + 40, hPvLogo.bmWidth, hPvLogo.bmHeight, hDcLogo, 0, 0, SRCCOPY);
      }

      // Draw OPU mod version number.
      if (hVersionFont != NULL) {
        static constexpr char Version[] = "OPU Mod v" OP2_VERSION_TRIPLE_STR "\n" __DATE__ " " __TIME__;

        RECT textRect = { 10, 10, 0, 0 };
        SetTextColor(hDcWnd, 0x606060);
        SetBkMode(hDcWnd, TRANSPARENT);
        SelectObject(hDcWnd, hVersionFont);
        if (DrawTextA(hDcWnd, &Version[0], -1, &textRect, DT_CALCRECT) != 0) {
          DrawTextA(hDcWnd, &Version[0], -1, &textRect, DT_TOP | DT_LEFT);
        }
      }
    });

    // In OP2Shell::ShutDown()
    patcher.LowLevelHook(0x13009383, [] {
      if (hDcLogo != NULL) {
        DeleteDC(hDcLogo);
        hDcLogo = NULL;
      }
      if (hBitmapLogo != NULL) {
        DeleteObject(hBitmapLogo);
        hBitmapLogo = NULL;
      }
      if (hVersionFont != NULL) {
        DeleteObject(hVersionFont);
        hVersionFont = NULL;
      }
    });

    // In OP2Shell::RegisterClass()
    patcher.HookCall(0x130092ED, StdcallLambdaPtr([](HMODULE, const char*)
      { return LoadIconA(g_hInst, &NewIconName[0]); }));

    // In AviWnd::RegisterClass()
    patcher.LowLevelHook(0x1300129E, [](Esp<WNDCLASSA*, 20> pWndClass)
      { pWndClass->hIcon = LoadIconA(g_hInst, &NewIconName[0]); });

    // Enable D keyboard shortcut for the hidden debug menu button on the main menu dialog.
    // Hook MainMenuDialog::DlgProc() (replace vtbl entry)
    patcher.Write(0x130110A0, ThiscallFunctor([F = decltype(IDlgWnd::VtblType::pfnDlgProc)(patcher.FixPtr(0x13002900))]
      (IDlgWnd* pThis, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        const INT_PTR result = F(pThis, uMsg, wParam, lParam);

        if (uMsg == WM_INITDIALOG) {
          if (const HWND hDebugButton = GetDlgItem(pThis->hWnd_, 1033);  hDebugButton != NULL) {
            SendMessageA(hDebugButton, WM_SETTEXT, 0, LPARAM("&DEBUG TEST..."));
          }
        }

        return result;
      }));

    // Increase width of campaign mission menu dialog so title text doesn't get clipped with alternate font enabled.
    // In MissionMenuDialog::DlgProc()
    static constexpr int AddMissionMenuWidth = 24;
    patcher.LowLevelHook(0x1300599B, [](Esi<IDlgWnd*> pThis) { *PtrInc<int*>(pThis, 0x20) += AddMissionMenuWidth; });
    patcher.LowLevelHook(0x13005A2D, [](Esi<IDlgWnd*> pThis) {
      if (RECT dlgRect = {};  GetWindowRect(pThis->hWnd_, &dlgRect)) {
        if (HDWP hWinPosInfo = BeginDeferWindowPos(5); hWinPosInfo != NULL) {
          for (int id : { 1091, 1090, 1005, 1095, 1094 }) {
            if (const HWND hButton = GetDlgItem(pThis->hWnd_, id);  hButton != NULL) {
              if (RECT rect = {};  GetWindowRect(hButton, &rect)) {
                const int x = rect.left - dlgRect.left + (AddMissionMenuWidth / 2);
                const int y = rect.top  - dlgRect.top  - 24;
                hWinPosInfo = DeferWindowPos(hWinPosInfo, hButton, NULL, x, y, 0, 0, SWP_NOSIZE);
              }
            }
          }
          EndDeferWindowPos(hWinPosInfo);
        }
      }
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Replaces several UI dialogs, as well as the icon used by game windows.
bool SetUiResourceReplacePatch(
  bool enable)
{
  static Patcher::PatchContext op2Patcher;
  static Patcher::PatchContext odaslPatcher("odasl.dll", true);
  static Patcher::PatchContext sysPatcher(&LoadStringA);

  static std::set<std::string> loadedFonts;

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

    // In GameFrame::RegisterClass()
    op2Patcher.HookCall(0x49B1C4, StdcallLambdaPtr([](HMODULE, const char*)
      { return LoadIconA(g_hInst, &NewIconName[0]); }));

    // In IWnd::RegisterClass()
    op2Patcher.LowLevelHook(0x43151D, [](Esp<WNDCLASSA*, 8> pWndClass)
      { pWndClass->hIcon = LoadIconA(g_hInst, &NewIconName[0]); });

    // In wplLoadResourceBitmap()
    static std::set<std::string> resourceNames;
    odaslPatcher.LowLevelHook(0x2000B129, [](Eax<const char*>& pName, Esp<HMODULE*, 0x10> phModule) {
      auto it = resourceNames.insert(FindResourceReplacement(RT_BITMAP, pName, phModule)).first;
      pName   = ((it == resourceNames.end()) || it->empty()) ? pName : it->data();
    });

    // Hook LoadStringA() to replace string table resources (which are used by odasl for fonts and element colors).
    sysPatcher.Hook(&LoadStringA, SetCapturedTrampoline, StdcallFunctor(
      [F = decltype(&LoadStringA){}](HMODULE hMod, UINT id, char* pBuffer, int size) -> int {
        if (auto name = FindResourceReplacement(RT_STRING, MAKEINTRESOURCEA(id), &hMod);  name.empty() == false) {
          if (HRSRC hRsrc = FindResourceA(hMod, name.data(), RT_STRING);  hRsrc != NULL) {
            if (size == 0) {
              return SizeofResource(hMod, hRsrc);
            }
            else if (HANDLE hRes = LoadResource(hMod, hRsrc);  hRes != NULL) {
              if (void* pStr = LockResource(hRes);  pStr != nullptr) {
                return (strncpy_s(pBuffer, size, static_cast<char*>(pStr), _TRUNCATE) == 0) ? strlen(pBuffer) : 0;
              }
            }
          }
        }
        return F(hMod, id, pBuffer, size);
      }));

    // Load Open Sans and Roboto fonts found in search paths.
    // ** TODO Load any *.ttf/*.ttc/*.otf font files found
    static constexpr const char* FontFilenames[] = {
      "OpenSans-Bold.ttf", "OpenSans-BoldItalic.ttf", "OpenSans-Italic.ttf", "OpenSans-Regular.ttf", "Roboto-Bold.ttf",
      "Roboto-BoldItalic.ttf", "Roboto-Italic.ttf", "Roboto-Regular.ttf"
    };
    for (const char* pFilename : FontFilenames) {
      if (auto path = GetFilePath(pFilename).string();  (path.empty() == false) && (loadedFonts.count(path) == 0)) {
        if (int count = AddFontResourceExA(path.data(), FR_PRIVATE, nullptr);  count > 0) {
          loadedFonts.insert(path);
        }
      }
    }

    success = (op2Patcher.GetStatus()   == PatcherStatus::Ok) &&
              (odaslPatcher.GetStatus() == PatcherStatus::Ok) &&
              (sysPatcher.GetStatus()   == PatcherStatus::Ok) &&
              SetMainMenuPatch(true);

    if (success) {
      static const auto cleanup = atexit([] { SetUiResourceReplacePatch(false); });
    }
  }

  if ((enable == false) || (success == false)) {
    for (const std::string& font : loadedFonts) {
      RemoveFontResourceExA(font.data(), FR_PRIVATE, nullptr);
    }
    loadedFonts.clear();

    success &= (op2Patcher.RevertAll()   == PatcherStatus::Ok);
    success &= (odaslPatcher.RevertAll() == PatcherStatus::Ok);
    success &= (sysPatcher.RevertAll()   == PatcherStatus::Ok);
    success &= (SetMainMenuPatch(false));
  }

  // Re-init Odasl to refresh loaded resources.
  if (HMODULE hShell = GetModuleHandleA("OP2Shell.dll");  hShell && g_configFile.GetInt("Game", "SpiffyDraw", 1)) {
    Odasl::wplExit();
    auto*const pfnShellInitOdasl = reinterpret_cast<ibool(FASTCALL*)(void* pShell)>(
      reinterpret_cast<uint8*>(hShell) - OP2ShellBase + 0x13007D30);
    pfnShellInitOdasl(reinterpret_cast<uint8*>(hShell) - OP2ShellBase + 0x130158F0);
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
    result = snprintf(&entry.message[0], MaxChatMessageLen, "<N>%i: %s", Game::Mark(), pText);
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
    patcher.LowLevelHook(0x43941D, [](Eax<int> index, Esi<StreamIO*> pStream) {
      const size_t size =
        (GetSavedGameVersion(pStream) >= GameVersion{1, 4, 0}) ? sizeof(g_messageLogRb[0]) : sizeof(MessageLogEntry<>);
      return (pStream->Read(size, &g_messageLogRb[index]) != 0) ? 0x439434 : 0x43944B;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Allows players to ping locations by sending chat messages of the form "@tileX,tileY".
// ** TODO Look into how to make it respond to spacebar like disaster warnings etc. do
// ** TODO Add a mouse command to ping by clicking on the detail pane or mini map
bool SetChatPingLocationPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In Player::ProcessCommandPacket()
    patcher.LowLevelHook(0x40FE07, [](Eax<char*> pText, Esi<ChatCommand*> pData) {
      bool isPing = false;
      const std::string_view msg(pData->message);

      if (size_t sep = msg.find(',');  (sep != std::string_view::npos) && (msg[0] == '@')) {
        int x = 0;
        int y = 0;

        const auto [p,  ec]  = std::from_chars(msg.data() + 1,       msg.data() + sep,        x);
        const auto [p2, ec2] = std::from_chars(msg.data() + sep + 1, msg.data() + msg.size(), y);
        isPing = (ec == std::errc()) && (ec2 == std::errc());

        if (isPing) {
          const Location loc = GameMap::At(x, y);
          g_messageLog.AddMessage(loc.GetPixelX(), loc.GetPixelY(), pText, SoundID::Beep9);
        }
      }

      return isPing ? 0x4101D3 : 0;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Converts escaped characters to real characters (in ini strings).
static std::string UnEscapeString(
  const std::string_view& in)
{
  static constexpr std::pair<const char*, const char*> pReplacements[] = {
    { "\\a", "\a" },  { "\\b",  "\b" },  { "\\f", "\f" },  { "\\n",  "\n" },  { "\\r", "\r" },  { "\\t", "\t" },
    { "\\v", "\v" },  { "\\\\", "\\" },  { "\\'", "\'" },  { "\\\"", "\"" },  { "\\?", "\?" }
  };

  std::string out(in);

  for (const auto [pEscaped, pUnescaped] : pReplacements) {
    for (size_t p = out.length(); ((p = out.rfind(pEscaped, p)) != std::string::npos); out.replace(p, 2, pUnescaped));
  }

  return out;
}

// =====================================================================================================================
// Sets localized string table entries in Outpost2.exe and OP2Shell.dll based on settings in language.ini.
// ** TODO replaced UI dialogs lose localization, mission DLLs aren't localized
bool SetLocalizationPatch(
  bool enable)
{
  static Patcher::PatchContext op2Patcher;
  static Patcher::PatchContext shellPatcher("OP2Shell.dll", true);
  static std::deque<char*>     pAllocations;
  bool success = true;

  if (enable && ((op2Patcher.NumPatches() + shellPatcher.NumPatches()) == 0)) {
    if (char confPath[MAX_PATH] = "";  g_resManager.GetFilePath("language.ini", &confPath[0])) {
      auto PatchStrings = [confPath](const char* pSectionName, auto& stringTable, Patcher::PatchContext* pPatcher) {
        bool success       = true;
        char setting[1024] = "";

        for (size_t i = 0; (success && (i < TethysUtil::ArrayLen(stringTable))); ++i) {
          const size_t len = GetPrivateProfileStringA(
            pSectionName, std::to_string(i).data(), "", &setting[0], sizeof(setting), &confPath[0]);

          if ((len != 0) && (setting[0] != '\0')) {
            std::string str = UnEscapeString(setting);
            if (char*const pBuf = static_cast<char*>(OP2Alloc(str.length() + 1));  success = (pBuf != nullptr)) {
              pAllocations.push_back(pBuf);
              strncpy_s(pBuf, str.length() + 1, str.data(), _TRUNCATE);
              success &= (pPatcher->Write(&stringTable[i], &pBuf[0]) == PatcherStatus::Ok);
            }
          }
        }

        return success;
      };

      success = PatchStrings("Game",  GetLocalizedStringTable(),      &op2Patcher) &&
                PatchStrings("Shell", GetShellLocalizedStringTable(), &shellPatcher);

      if (success) {
        static const auto cleanup = atexit([] { SetLocalizationPatch(false); });
      }
    }
  }

  if ((enable == false) || (success == false)) {
    for (char* pAllocation : pAllocations) {
      OP2Free(pAllocation);
    }
    pAllocations.clear();

    success &= (op2Patcher.RevertAll()   == PatcherStatus::Ok);
    success &= (shellPatcher.RevertAll() == PatcherStatus::Ok);
  }

  if (success) {
    OP2Thunk<0x45C710>();  // Call ReportButtonHelpText::Init() to refresh report button mouseover text
  }

  return success;
}

// =====================================================================================================================
// Replaces usage of Arial in the game's UI with the given font.
// ** TODO Figure out how to make antialiasing look better
bool SetFontPatch(
  const char* pNewFont)
{
  static Patcher::PatchContext patcher;
  static std::string fontStr;

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
  for (Font* pFont:
    { PtrInc<Font*>(&g_gameFrame, 0x0BC8), PtrInc<Font*>(&g_gameFrame, 0x2828), PtrInc<Font*>(&g_gameFrame, 0x4488) })
  {
    pFont->Init(createInfo);
  }

  if (isDefault == false) {
    createInfo.lfHeight = 13;
    createInfo.lfWeight = 700;
    patcher.Write(&g_gameFrame.hChatFont_,        CreateFontIndirectA(&createInfo));
    patcher.Write(&g_gameFrame.statusBar_.hFont_, CreateFontIndirectA(&createInfo));

    createInfo.lfHeight = 48;
    patcher.Write(&g_gameFrame.detailPane_.hLargeMessageFont_, CreateFontIndirectA(&createInfo));

    patcher.ReplaceStaticReferences(0x4DEB40, 1, fontStr.data());  // Replace "Arial"

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
// ** TODO Double clicking a unit should select all units of the same type on screen
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
      0xF6, 0x05,                                // test BYTE PTR ds:0x200121B9, 0x10
    });
    patcher.Write(0x20004EBA, patcher.FixPtr(0x200121B9));
    patcher.WriteBytes(0x20004EBE, {
      0x10,
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

    static auto GetCargoStr = [](Vehicle* pVec) -> std::string {
      static const char*const pTruckCargoStrings[] = {
        "empty",
        GetLocalizedString(LocalizedString::Food),
        GetLocalizedString(LocalizedString::CommonOre),
        GetLocalizedString(LocalizedString::RareOre),
        GetLocalizedString(LocalizedString::CommonMetals),
        GetLocalizedString(LocalizedString::RareMetals),
        GetLocalizedString(LocalizedString::CommonRubble),
        GetLocalizedString(LocalizedString::RareRubble),
        GetLocalizedString(LocalizedString::Spacecraft),
        GetLocalizedString(LocalizedString::Wreckage),
        GetLocalizedString(LocalizedString::GeneBank),
      };
      static_assert(TethysUtil::ArrayLen(pTruckCargoStrings) == size_t(CargoType::Count));

      switch (pVec->GetTypeID()) {
      case MapID::CargoTruck: {
        const auto        cargoType   = CargoType(max(pVec->truckCargoType_, 0));
        auto*const        pSpacecraft = MapObjectType::GetInstance(pVec->truckCargoAmount_);
        const char*const  pCargoName  =
           (cargoType == CargoType::Spacecraft) ? ((pSpacecraft != nullptr) ? &pSpacecraft->unitName_[0] : nullptr)   :
          ((cargoType <  CargoType::Count) && (cargoType > CargoType::Empty)) ? pTruckCargoStrings[size_t(cargoType)] :
          nullptr;
        const std::string quantity = ((pVec->truckCargoAmount_ > 1) && (cargoType <= CargoType::RareRubble)) ?
          std::to_string(pVec->truckCargoAmount_) : "";

        return (pCargoName != nullptr) ? (quantity + ((quantity[0] != '\0') ? " " : "") + pCargoName) : "empty";
      }
      case MapID::ConVec: {
        auto*const pCargo  = MapObjectType::GetInstance(pVec->cargo_);
        auto*const pWeapon =
          (pVec->weaponOfCargo_ != MapID::None) ? MapObjectType::GetInstance(pVec->weaponOfCargo_) : nullptr;

        auto*const pCargoName  = pCargo  ? &pCargo->unitName_[0]  : "";
        auto*const pWeaponName = pWeapon ? &pWeapon->unitName_[0] : "";

        return std::string(pWeaponName) + ((pWeaponName[0] != '\0') ? " " : "") + pCargoName;
      }
      case MapID::EvacuationTransport: return pVec->cargo_ ? GetLocalizedString(LocalizedString::Colonists) : "empty";
      default:                         return "";
      }
    };

    Vehicle::VtblType*const pVtbls[] =
      { MapObj::ConVec::Vtbl(), MapObj::CargoTruck::Vtbl(), MapObj::EvacuationTransport::Vtbl() };

    // Make ConVecs, Cargo Trucks, and Evacuation Transports display their cargo in mouseover tooltips.
    for (auto* pVtbl : pVtbls) {
      patcher.Write(
        &pVtbl->pfnGetMouseOverStr,
        ThiscallLambdaPtr([](Vehicle* pThis, char* pDst, int size) {
          auto*const pUnitTypeName = &pThis->GetType()->unitName_[0];
          return ((pThis->cargo_ != 0) && Player[pThis->ownerNum_].IsAlliedTo(Game::LocalPlayer()))   ?
            _snprintf_s(pDst, size_t(size), _TRUNCATE, "%s (%s)", pUnitTypeName, GetCargoStr(pThis).data()) :
            _snprintf_s(pDst, size_t(size), _TRUNCATE, "%s",      pUnitTypeName);
        }));
    }

    // Hide redundant vehicle cargo text in storage bay command pane views.
    // In CommandPaneView::BuildingStorageBays::Draw()
    patcher.LowLevelHook(0x464050, [](Ecx<MapObject*> pUnit, Eax<char*> pBuffer) {
      const MapID id = pUnit->GetTypeID();
      if ((id == MapID::ConVec) || (id == MapID::CargoTruck) || (id == MapID::EvacuationTransport)) {
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
      if (((id == MapID::ConVec) || (id == MapID::CargoTruck) || (id == MapID::EvacuationTransport)) && pThis->cargo_) {
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
        return ((p1 >= 6) || (p2 >= 6) || Player[p1].IsAI() || Player[p2].IsAI() || (Game::Tick() == 0)) ?
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
        const bool skip = ((soundID == SoundID::Savant27) && (prevPowerSound == soundID)) ||
                          (g_gameImpl.GetPlayer(Game::LocalPlayer())->amountPowerConsumed_ == 0);
        prevPowerSound  = (skip || pThis->AddMessage(pixelX, pixelY, pMsg, soundID)) ? soundID : SoundID{};
        return (prevPowerSound != SoundID{});
      }));

    // Silence repeated "metals storage needed".
    static auto DoMetalAlert = [](int capacity, int amount, size_t strIdx, SoundID soundID, bool* pStorageNeeded) {
      const int  threshold = *pStorageNeeded ? 5000 : 1000;  // Remaining storage threshold for the alert
      const bool doAlert   = (capacity != 0) && (capacity < 100000) && ((capacity - threshold) < amount);
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
          (((soundID == SoundID::Savant24) || (soundID == SoundID::Savnt226)) && (prevFoodSound == soundID)) ||
          (g_gameImpl.GetPlayer(Game::LocalPlayer())->totalFoodConsumption_ == 0);
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
          if (pThis->ownerNum_ == Game::LocalPlayer()) {
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
// ** TODO In the future, ModuleDesc could be defined in an external file and override baked-in definitions
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
    patcher.LowLevelHook(0x402627,
      [](Esi<const char*> pModuleName, Esp<const char**, 36> ppLevelDesc, Eax<const ModDesc*>& pModDesc) {
        std::string moduleName(pModuleName);

        const size_t separatorPos = moduleName.find_last_of("\\/");
        if (separatorPos != std::string::npos) {
          moduleName.erase(moduleName.begin(), moduleName.begin() + separatorPos + 1);
        }

        std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);

        const auto it = missionNames.find(moduleName);
        *ppLevelDesc = (it != missionNames.end()) ? it->second.data() : nullptr;

        // us_them is a useless unit reference-type mission where both players are uncontrollable AIs (forced to GoAI())
        // that is set to MissionType::Colony.  Force its mission type to 0 to hide it from normal mission list UIs.
        if (moduleName == "us_them.dll") {
          static const ModDesc desc =
            { MissionType{0}, pModDesc->numPlayers, pModDesc->maxTechLevel, pModDesc->unitMission };
          pModDesc = &desc;
        }
      });

    patcher.LowLevelHook(0x40265E, [](Eax<const char*> pLevelDesc, Esp<const char**, 40> ppLevelDesc) {
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

// =====================================================================================================================
// Changes some game ini setting defaults.
bool SetDefaultIniSettingsPatch(
  bool enable)
{
  static Patcher::PatchContext op2Patcher;
  static Patcher::PatchContext shellPatcher("OP2Shell.dll", true);
  bool success = true;

  if (enable) {
    // Change default game speed from 5 to 10.
    constexpr uint8 DefaultGameSpeed = 10;

    // In GameFrame::Init(), TApp::OnLoadScript(), TethysGame::LoadDebugMap(), MPLobby::ShowHostGame(), ::Show???()
    for (uintptr loc : { 0x49B6B6, 0x48760E, 0x489189, 0x45F208, 0x45F58A }) {
      op2Patcher.Write(loc, DefaultGameSpeed);
    }
    // In PreferencesDialog::DlgProc(), OP2Shell::WndProc() (x3)
    for (uintptr loc : { 0x13003378, 0x1300825B, 0x130086DF, 0x13008F10 }) {
      shellPatcher.Write(loc, DefaultGameSpeed);
    }

    success = (op2Patcher.GetStatus() == PatcherStatus::Ok) && (shellPatcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (op2Patcher.RevertAll()   == PatcherStatus::Ok);
    success &= (shellPatcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Force-enable the restart game button when running multiplayer missions locally via the debug (run script) dialog.
bool SetDebugRestartGamePatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In GameOptionsPane::AddControls()
    patcher.HookCall(0x465B6D, []() -> ibool { return g_gameImpl.gameStartInfo_.startupFlags.isMultiplayer; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Makes unit HP bars always visible if they're damaged.
bool SetUnitHpBarVisibilityPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  // 0 = Selected and mouseover only (original behavior), 1 = Always show damaged units' HP bars
  static int showHPBarsMode = g_configFile.GetInt("Game", "ShowUnitHPBars", -1);
  if (showHPBarsMode == -1) {
    showHPBarsMode = 1;
    g_configFile.SetInt("Game", "ShowUnitHPBars", showHPBarsMode);
  }

  enable &= (showHPBarsMode != 0);
  if (enable) {
    static constexpr uint32 DrawHPBar = 1u << 31;

    // In MapUnit::DrawUIOverlay()
    // Change the meaning of the drawFlags function param to add a flag for draw HP bars.
    patcher.LowLevelHook(0x43EB3A, [](Eax<uint32> flags) { return BitFlagTest(flags, DrawHPBar) ? 0x43EB42 : 0; });
    patcher.LowLevelHook(0x43EC1F, [](Esp<uint32&, 0x1E0> flags)
      { return BitFlagTest(flags, DrawHPBar) ? 0x43ED79 : 0; });
    patcher.LowLevelHook(0x43ED79, [](Esp<uint32&, 0x1E0> flags)
      { return BitFlagTest(flags, DrawHPBar) ? 0 : 0x43EF12; });
    // Disable red HP bar flashing.
    patcher.LowLevelHook(0x43EE5C, [](Ecx<uint32>& color) { color = ~0u; });

    // In MapObjDrawList::DrawUnits()
    patcher.LowLevelHook(0x49EA1C, [](Esi<MapObjDrawList*> pThis, Ebx<MapUnit**> ppDrawList) {
      auto*const pViewport = pThis->pViewport_;
      for (int i = 0, count = pThis->numUnits_; i < count; ppDrawList[i++]->Draw(pViewport));
      for (int i = 0, count = pThis->numUnits_; i < count; ++i) {
        static constexpr uint32 Blight = MoFlagBuilding | MoFlagBldInfected;
        if (auto*const pUnit = ppDrawList[i];  (pUnit->damage_ > 0) && (BitFlagsTest(pUnit->flags_, Blight) == false)) {
          // Don't draw HP bars for hidden enemy vehicles at night, unless they are too close to units hostile to them.
          const bool isNotVec    = (BitFlagTest(pUnit->flags_, MoFlagVehicle) == false);
          const bool isFriendly  = Player[Game::LocalPlayer()].IsAlliedBy(pUnit->ownerNum_);
          const bool hasLighting = (GameMap::GetLightLevel(pUnit->GetTile()) < (NumLightLevels / 2)) ||
                                   BitFlagTest(pUnit->flags_, MoFlagVecHeadlights | MoFlagForceFullLighting);
          if ((Game::UsesDayNight() == false) || hasLighting || isNotVec || isFriendly || pUnit->IsEnemyUnitSighted()) {
            pUnit->DrawUIOverlay(DrawHPBar, pViewport);
          }
        }
      }
      return 0x49EA2F;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
