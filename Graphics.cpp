
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#include <versionhelpers.h>

#include "Patcher.h"
#include "Util.h"
#include "Library.h"

#include "Tethys/API/TethysGame.h"
#include "Tethys/API/GameMap.h"
#include "Tethys/Game/MineManager.h"
#include "Tethys/UI/GameFrame.h"
#include "Tethys/Resource/SpriteManager.h"
#include "Tethys/Resource/GFXSurface.h"
#include "Tethys/Resource/GFXBitmap.h"
#include "Tethys/Resource/CConfig.h"

#include <algorithm>

using namespace Tethys;
using namespace Tethys::API;
using namespace Patcher::Util;
using namespace Patcher::Registers;

// =====================================================================================================================
// Disable DirectDraw mode, always forcing windowed mode (pure GDI mode).  Also fixes crash bugs with 4k resolutions and
// when the game window is bigger than the current map.
bool SetWindowFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Bypass 16-bit color check and DirectDraw::SetDisplayMode()
    // In TApp::Init()
    patcher.WriteBytes(0x485C06, { 0xEB, 0x21 });  // je => jmp 0x23
    patcher.WriteBytes(0x486072, { 0xEB, 0x7E });  // je => jmp 0x80

    // Dynamically allocate bit vectors that store what parts of the screen to redraw to avoid crashes on 4k displays.
    // In Viewport::Init()
    patcher.LowLevelHook(0x46F1D2, [](Esi<Viewport*> pThis, Ecx<size_t>& counter) {
      // Try to allocate bit vectors only once, and make them the virtual screen size in case the game is resized.
      // The game can behave strangely (e.g. flickering structure shadows, missing sprites) in certain cases at high
      // resolutions if the buffers get reallocated mid-game.  Assume there is only one active Viewport (detail pane's).
      static const int    screenX     = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
      static const int    screenY     = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
      static const size_t bitVecSize  = (((screenX * screenY) + 31) / 32) / 8;  // Round up to nearest tile size (32 px)
      static auto*const   pBitVectors = static_cast<uint8*>(OP2Alloc(bitVecSize * 5));
      static const auto   cleanup     = atexit([] { OP2Free(pBitVectors); });

      pThis->pRedrawBitVector_    = &pBitVectors[0 * bitVecSize];
      pThis->pOldRedrawBitVector_ = &pBitVectors[1 * bitVecSize];
      pThis->pLightBitVector_     = &pBitVectors[2 * bitVecSize];
      pThis->pOldLightBitVector_  = &pBitVectors[3 * bitVecSize];
      pThis->pUnknownBitVector_   = &pBitVectors[4 * bitVecSize];

      memset(pBitVectors, 0, bitVecSize * 5);

      // Skip the original code that zeroes the fixed size buffers
      // Set ecx = 0 to negate the effects of a rep stosb instruction when we jump back
      counter = 0;
      return 0x46F229;
    });

    static bool isDetailPaneSmall = false;

    // Prevent detail pane from getting larger than the map size by hooking the DeferWindowPos call that sets its size.
    // In GameFrame::RepositionWindowOnResize()
    patcher.HookCall(0x4999FC, StdcallLambdaPtr(
      [](HDWP hWinPosInfo, HWND hWnd, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags) -> HDWP {
        const MapRect clipRect = GameMap::GetClipRect();
        const int maxDisplayableMapWidth  = 32 * (std::max)(64, clipRect.x2 - clipRect.x1 + 1);
        // ** TODO Displaying the last row of the map causes severe glitches similar to the window being too tall, so we
        // subtract one tile.  We assume the map is 64x64 minimum.
        const int maxDisplayableMapHeight = 32 * (std::max)(63, clipRect.y2 - clipRect.y1 + 1);
        const int origCx = cx;
        const int origCy = cy;
        cx = (std::min)(cx, maxDisplayableMapWidth);
        cy = (std::min)(cy, maxDisplayableMapHeight);
        // Center the detail pane in the original area.
        if (cx < origCx) {
          x += (origCx - cx) / 2;
        }
        if (cy < origCy) {
          y += (origCy - cy) / 2;
        }
        isDetailPaneSmall = (cx < origCx) || (cy < origCy);
        return ::DeferWindowPos(hWinPosInfo, hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
      }));

    // Repaint the background with a black color to handle when the detail pane is smaller than the actual window.
    // In GameFrame::OnPaint()
    patcher.LowLevelHook(0x499CA8, [](Esi<GameFrame*> pThis, Eax<HDC> hDc) {
      if (isDetailPaneSmall) {
        RECT frameRect;
        ::GetClientRect(pThis->hWnd_, &frameRect);
        ::PatBlt(hDc, 0, 0, frameRect.right, frameRect.bottom, BLACKNESS);
      }
    });

    // Force a window resize on game load.
    // In GameImpl::PrepareGame()
    patcher.LowLevelHook(0x48990D, [] { ::SendMessage(g_gameFrame.hWnd_, WM_SIZE, 0, 0); });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Returns if Windows Desktop Window Manager Compositioning is enabled.
// Never enabled in WinXP, may be enabled or disabled in Vista and Win7, always enabled in Win8 and Win10+.
static bool IsDwmCompositionEnabled(
  const Library& dwmApi)
{
  BOOL dwmEnabled = IsWindows8OrGreater();

  if (dwmEnabled == false) {
    static auto*const pfnIsCompositionEnabled = dwmApi.Get<HRESULT WINAPI(BOOL*)>("DwmIsCompositionEnabled");

    if (pfnIsCompositionEnabled != nullptr) {
      pfnIsCompositionEnabled(&dwmEnabled);
    }
  }

  return dwmEnabled;
}

// =====================================================================================================================
// Fixes various issues related to DWM desktop compositioning on Vista and newer, such as issues where there is visible
// stuttering, and where the Aero frame is drawn over the game frame.
bool SetDwmFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static Library dwmApi("dwmapi.dll");

    int forceRedraw = g_configFile.GetInt("Game", "ForceFullRedraw", -1);
    if (forceRedraw == -1) {
      forceRedraw = IsDwmCompositionEnabled(dwmApi) || IsVirtualMachine();  // XP VM running on Vista+ host is affected
      g_configFile.SetInt("Game", "ForceFullRedraw", forceRedraw);
    }
    
    if (forceRedraw) {
      // Workaround the redraw stuttering issue by forcing the entire detail pane to be redrawn every frame.
      // ** TODO Implement Direct2D presents to fix stuttering issues with redrawing the other parts of the UI?
      // In DetailPane::OnIdle()
      patcher.LowLevelHook(0x407EE3, [](Ecx<Viewport*> pViewport)
        { pViewport->MarkForRedraw({ 0, 0, pViewport->height_, pViewport->width_ }); });
    }

    static auto*const pfnDwmSetWindowAttribute =
      dwmApi.Get<HRESULT WINAPI(HWND, DWORD, LPCVOID, DWORD)>("DwmSetWindowAttribute");

    // Prevent the default Aero window frame from being drawn over the game's custom frame.
    if (pfnDwmSetWindowAttribute != nullptr) {
      // In IWnd::CreateEx()
      patcher.LowLevelHook(0x4316A4, [](Eax<HWND> hWnd) {
        const DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
        pfnDwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
      });
    }

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Sets the main thread to be DPI-unaware, to negate the GOG distro exe's manifest settings.  Win10+ only.
bool SetDpiFix(
  bool enable)
{
  bool success = true;

  static Library user32("User32.dll");
  static auto*const pfnSetThreadDpiAwarenessContext =
    user32.Get<DPI_AWARENESS_CONTEXT WINAPI(DPI_AWARENESS_CONTEXT)>("SetThreadDpiAwarenessContext");
  static DPI_AWARENESS_CONTEXT oldCtx = {};

  if (pfnSetThreadDpiAwarenessContext != nullptr) {
    if (enable) {
      int dpiAwareness = g_configFile.GetInt("Game", "DPIAwareness", INT_MAX);
      if (dpiAwareness == INT_MAX) {
        dpiAwareness = int(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);  // Default to DPI-unaware with GDI scaling
        g_configFile.SetInt("Game", "DPIAwareness", dpiAwareness);
      }

      if (dpiAwareness != 0) {
        oldCtx  = pfnSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT(dpiAwareness));
        success = (oldCtx != NULL);

        if (success) {
          static const auto cleanup = atexit([] { SetDpiFix(false); });
        }
      }
    }
    else if (oldCtx != NULL) {
      success = (pfnSetThreadDpiAwarenessContext(oldCtx) != NULL);
    }
  }

  return true;
}

// =====================================================================================================================
// Forces game to be rendered as often as possible and not just during game sim cycle updates.
// ** TODO Fix scrolling speed handled by DetailPane and MiniMapPane, test for possible desyncs?
bool SetFpsPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static int noRenderFpsLimit = g_configFile.GetInt("Game", "NoRenderFPSLimit", -1);
    if (noRenderFpsLimit == -1) {
      noRenderFpsLimit = 0;
      g_configFile.SetInt("Game", "NoRenderFPSLimit", noRenderFpsLimit);
    }

    static int timeMsPassed = 0;

    // In GameFrame::OnIdle()
    patcher.LowLevelHook(0x49C376, [](Eax<uint32> relTimeMs, Esi<GameFrame*> pThis) {
      if (noRenderFpsLimit && (pThis->iniSettings_.frameSkip <= 1)) {
        timeMsPassed = relTimeMs;
        pThis->detailPane_.OnIdle();
        pThis->miniMapPane_.OnIdle();
        pThis->commandPane_.OnIdle();
        timeMsPassed = 0;
      }
    });

    // In DetailPane::HandleScrolling()
    patcher.LowLevelHook(0x407B3F, [](Eax<uint32>& actualScrollRate) { return (timeMsPassed != 0) ? 0x407E3C : 0; });

    // Prevent fumarole ambient sound from being played many times
    // In Fumarole::Draw()
    patcher.LowLevelHook(0x405AA2, [] { return (timeMsPassed != 0) ? 0x405AB6 : 0; });

    // Prevent meteor defense sounds from being played many times
    // In Meteor::Draw(), EMPMissile::Draw()
    patcher.LowLevelHook(0x44ACB9, [] { return (timeMsPassed != 0) ? 0x44ACEC : 0; });
    patcher.LowLevelHook(0x48075B, [] { return (timeMsPassed != 0) ? 0x48078E : 0; });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Replaces translucency effects with real alpha blending, instead of emulated with checkerboard patterns.
// ** TODO Currently only affects Acid Cloud drawing
bool SetAlphaBlendPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Hook acid cloud drawing code.
    patcher.LowLevelHook(*OP2Mem<void**>(0x586521), [](
      Ebp<Rgb555[256]> palette, Ecx<uint32> width, Edi<Rgb555*>& pDstOut, Esi<uint8*>& pSrcPaletteIdxOut) {
        Rgb555* pDst           = pDstOut;
        uint8*  pSrcPaletteIdx = pSrcPaletteIdxOut;

        for (uint32 i = width; i > 0; --i, ++pDst, ++pSrcPaletteIdx) {
          if (*pSrcPaletteIdx != 0) {
            // 50% alpha blend.
            const auto& src = palette[*pSrcPaletteIdx];
            auto dst = *pDst;
            dst = {
              uint16((dst.b / 2) + (src.b / 2)),  uint16((dst.g / 2) + (src.g / 2)),  uint16((dst.r / 2) + (src.r / 2))
            };
            pDst->u16All = dst.u16All;
          }
        }

        pSrcPaletteIdxOut = pSrcPaletteIdx;
        pDstOut           = pDst;

        return 0x5868B0;
      });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Alters the mining beacon animation based on the mine's MineVariant.
bool SetMineVariantVisibilityPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    patcher.LowLevelHook(
      0x4054AC,
      [](Esi<MapObj::MiningBeacon*> pThis, Edi<int> animIndex, Eax<int> numFrames, Edx<int>& frame, Ebp<int>& pixelX) {
        int curFrame = -1;

        if (pThis->IsSurveyed(TethysGame::LocalPlayer())) {
          OreVariant mid  = OreVariant::_1;  // OreVariant with the best peak yield per bar yield class
          OreVariant high = OreVariant::_1;  // OreVariant with the best minimum yield per bar yield class

          auto*const pMineManager = MineManager::GetInstance();
          for (OreVariant v = OreVariant::_2; v < OreVariant::Count; ++(int&)(v)) {
            auto*const pYieldInfo = pMineManager->GetYieldInfo(pThis->mineYield_, v);
            if ((pYieldInfo->peakYield > pMineManager->GetYieldInfo(pThis->mineYield_, high)->peakYield)) {
              mid = v;
            }
            else if ((pYieldInfo->minYield > pMineManager->GetYieldInfo(pThis->mineYield_, high)->minYield)) {
              high = v;
            }
          }

          if (pThis->mineVariant_ == mid) {
            curFrame = (TethysGame::Tick() % (numFrames * 4)) / 2;
            if (curFrame >= numFrames) {
              curFrame = 0;
            }
          }
          else if (pThis->mineVariant_ != high) {
            curFrame = 0;
          }
        }

        frame  = (curFrame != -1) ? curFrame : (TethysGame::Tick() % numFrames);
        pixelX = pThis->pixelX_;
        return 0x4054B6;
      });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
