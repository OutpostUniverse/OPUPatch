
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/UIState.h"
#include "Tethys/UI/CommandPane.h"
#include "Tethys/UI/DetailPane.h"
#include "Tethys/UI/MiniMapPane.h"

namespace Tethys {

/// Struct defining user-controlled settings (mainly from CConfig).
struct IniSettings {
  int   scrollRate;
  int   zoom;
  ibool showAmbientAnimations;    ///< Draw structure animations.
  ibool showComputerOverlay;      ///< Draw mining beacons, structure status icons, blight overlay, etc.
  ibool showShadows;              ///< This gets set to either showStationaryShadows or showMobileShadows at draw time.
  ibool showStationaryShadows;    ///< Draw structure shadows.
  ibool showMobileShadows;        ///< Draw vehicle shadows.
  ibool showCompletedObjectives;
  int   field_18;
  int   frameSkip;                ///< Frame skip divisor (UI only exposes 0 or 1).
};


/// Partially-exported abstract class from which the main Outpost 2 game window is derived.
class TFrame : public IWnd {
  using $ = TFrame;
public:
  TFrame()                  : IWnd(UseInternalCtorChain) { InternalCtor<0x484070>();                   }
  TFrame(InternalCtorChain) : IWnd(UseInternalCtorChain) {                                             }
  TFrame(const TFrame& src) : IWnd(UseInternalCtorChain) { InternalCtor<0x484090, const TFrame&>(src); }

  void*   Destroy(ibool freeMem = 0)                       override { return Thunk<0x484100, &$::Destroy>(freeMem); }
  LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    { return Thunk<0x4889E0, &$::WndProc>(uMsg, wParam, lParam); }

  virtual void ShutDown() { return Thunk<0x488FC0, &$::ShutDown>(); }

  virtual ibool TFrame_F1(UIState* pState)  = 0;
  virtual ibool OnCommand(uint32 controlId) = 0;

  virtual int TranslateAccelerators(MSG* pMsg) = 0;

  virtual ibool Init()       = 0;
  virtual ibool WindowInit() = 0;

  virtual void Activate()   { return Thunk<0x484020, &$::Activate>();   }
  virtual void Deactivate() { return Thunk<0x484030, &$::Deactivate>(); }

  virtual void OnIdle() { return Thunk<0x484040, &$::OnIdle>(); }

  virtual void PreCreateDlg()  { return Thunk<0x484050, &$::PreCreateDlg>();  }
  virtual void PostCreateDlg() { return Thunk<0x484060, &$::PostCreateDlg>(); }

#define OP2_TFRAME_VTBL($)                                                                                \
  $(ShutDown)  $(TFrame_F1)  $(OnCommand)  $(TranslateAccelerators)  $(Init)  $(WindowInit)  $(Activate)  \
  $(Deactivate)  $(OnIdle)  $(PreCreateDlg)  $(PostCreateDlg)
  DEFINE_VTBL_TYPE(OP2_TFRAME_VTBL, 0x4D6158);

  void HandleMenuInit(HMENU a, uint32 b, int c) { return Thunk<0x488FD0, &$::HandleMenuInit>(a, b, c); }

  void PostDelayedLoadMessage(const char* pSavedGameFilename)
    { return Thunk<0x489040, &$::PostDelayedLoadMessage>(pSavedGameFilename); }
};


/// Top status text bar of GameFrame.
class StatusBar : public OP2Class<StatusBar> {
public:
   StatusBar() { InternalCtor<0x411480>(); }
  ~StatusBar() { Thunk<0x4114A0>();        }

  virtual void F1(int a, int b, int c, int d, int e) { return Thunk<0x4116E0, &$::F1>(a, b, c, d, e); }

#define OP2_STATUSBAR_VTBL($)  $(F1)
  DEFINE_VTBL_TYPE(OP2_STATUSBAR_VTBL, 0x4CFA24);

  ibool Initialize() { return Thunk<0x411620, &$::Initialize>(); }

  void Clear() { return Thunk<0x411EC0, &$::Clear>(); }
  void Reset() { return Thunk<0x411E60, &$::Clear>(); }

  int GetFontHeight() { return Thunk<0x411580, &$::GetFontHeight>(); }

  void Pause() { return Thunk<0x411590, &$::Pause>(); }

  void SetBounds(const RECT& rect) { return Thunk<0x4114F0, &$::SetBounds>(rect); }

  void SetTopText(const char* pMessage, int a) { return Thunk<0x411CE0, &$::SetTopText>(pMessage, a); }

  void Paint(HDC hDc, int a) { return Thunk<0x411A80, &$::Paint>(hDc, a); }

public:
  int    field_00;
  int    maxCharWidth_;
  int    fontHeight_;
  char   message[4];      // ** TODO array len?
  int    field_0C[24];
  int    gameTick_;
  ibool  isTypingChat_;
  size_t strLen_;
  RECT   rect_;
  int    lastDrawWidth_;
  int    field_94[4];
  HFONT  hFont_;
  HBRUSH hBrushWhite;
  HBRUSH hBrushRed;
};
static_assert(sizeof(StatusBar) == 0xB0, "Incorrect StatusBar size.");


/// Template type to allow returning different variations of this struct (e.g. bitmap handles, resource IDs, etc)
template <typename T>
struct UIFrameImages {
  // ** TODO: this whole thing is probably just a member of Dans_RULE_UIFrame.
  static UIFrameImages<T>* Get() { return OP2Mem<0x4D6990, UIFrameImages<T>*>(); }

  T upperLeftCorner;
  T leftEdgeFull;
  T titleBarBlankArea;
  T upperRightCorner;
  T middleSplitterVertical;
  T rightEdgeFull;
  T lowerLeftCorner;
  T bottomEdge;
  T vertSplitterPart;
  T lowerRightCorner;
  T rightEdge;
  T leftEdge;
  T horzSplitterTop;
  T titleActive;
  T titleInactive;
  T minimizeNormal;
  T minimizePressed;
  T maximizeNormal;
  T maximizePressed;
  T closeNormal;
  T closePressed;
  T restoreNormal;
  T restorePressed;
  T commonOreIndicator;
  T rareOreIndicator;
};

/// Main Outpost 2 game window.  This class is the top-level manager of the game loop.  Also known as Dans_RULE_UIFrame.
class GameFrame : public TFrame {
  using $ = GameFrame;
public:
  GameFrame() : TFrame(UseInternalCtorChain) { InternalCtor<0x49AF30>(); }

  const char* RegisterClassA()                    override { return Thunk<0x49B190, &$::RegisterClassA>();            }
  void*   Destroy(ibool freeMem = 0)              override { return Thunk<0x49B0B0, &$::Destroy>(freeMem);            }
  LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    { return Thunk<0x49B740, &$::WndProc>(uMsg, wParam, lParam); }
  int     CallFilters(uint32 a, uint32 b, long c) override { return Thunk<0x49BF90, &$::CallFilters>(a, b, c);        }
  void    ShutDown()                              override { return Thunk<0x49B350, &$::ShutDown>();                  }
  ibool   TFrame_F1(UIState* pState)              override { return Thunk<0x4990E0, &$::TFrame_F1>(pState);           }
  ibool   OnCommand(uint32 controlID)             override { return Thunk<0x498960, &$::OnCommand>(controlID);        }
  int     TranslateAccelerators(MSG* pMsg)        override { return Thunk<0x49B3B0, &$::TranslateAccelerators>(pMsg); }
  ibool   Init()                                  override { return Thunk<0x49B210, &$::Init>();                      }
  ibool   WindowInit()                            override { return Thunk<0x49B430, &$::WindowInit>();                }
  void    Activate()                              override { return Thunk<0x49C680, &$::Activate>();                  }
  void    Deactivate()                            override { return Thunk<0x49C710, &$::Deactivate>();                }
  void    OnIdle()                                override { return Thunk<0x49C310, &$::OnIdle>();                    }

  DEFINE_VTBL_GETTER(0x4D6A08);

  void SetChatBarText(const char* pText, COLORREF color) { return Thunk<0x49BFE0, &$::SetChatBarText>(pText, color); }

  /// Sets game speed (value from 1-10+; gets scaled x4 internally).
  void SetGameSpeed(int newSpeed) { return Thunk<0x49C050, &$::SetGameSpeed>(newSpeed * 4); }

  // ** TODO more member functions
  /* CalcAndIssueCPMachineSettings 0x49CC30
     DisplayQuitMessage            0x49CB80
     DrawOreLevel                  0x499B00
     EnableChildWindows?           0x4885D0
     EndMission                    0x49C8F0
     GetUIState                    0x499120
     InstallHooks                  0x49C770
     ???                           0x49C140
     InvalidateOreDisplay          0x49C200
     LoadResourceImages            0x4998F0
     NCCalcSize                    0x4998A0
     NCHitTest                     0x49A4D0
     NCPaint                       0x499F50
     OnPaint                       0x499C90
     OnPauseGame                   0x49C0B0
     PostQuitMessage               0x49CB60
     RepositionWindowsOnResize     0x4999A0
     ResetActualGameSpeed?         0x49C2F0
     ShutDownInner                 0x499970 */

  // ** TODO define these as actual fields
  bool IsMouseOverDetailPane() { return OP2Mem<0x57B7AC, ibool&>(); }
  int  GetAbsoluteMouseX()     { return OP2Mem<0x57B7B4, int&>();   }
  int  GetAbsoluteMouseY()     { return OP2Mem<0x57B7B8, int&>();   }

  /// Gets the global GameFrame/Dans_RULE_UIFrame object instance.
  static GameFrame* GetInstance() { return OP2Mem<0x5756C0, GameFrame*>(); }

  Viewport* GetDetailPaneViewport() { return &static_cast<GFXClippedSurface*>(detailPane_.pGfxSurface_)->viewport_; }

public:
  static constexpr char WndClassName[] = "Dans_RULE_UIFrame";

  HFONT    hChatFont_;
  char     chatMessage_[80];
  int      chatLength_;
  COLORREF chatColor_;
  RECT     chatRect_;

  int field_80;
  int field_84;
  int field_88;
  int field_8C;
  int field_90;
  int textY_;
  int field_98;
  int field_9C;
  int displayedCommonOre_;
  int displayedRareOre_;
  int field_A8;
  int field_AC;

  LARGE_INTEGER performanceFreq_;
  int           lastProcessingLoopMs_;
  int           desiredMsPerGameTick_;
  int           msPerGameTick_;
  int           estimatedPacketLag_;
  int           lastNetExchangeMs_;
  int           numNetExchanges_;
  LARGE_INTEGER performanceCounts_[16];
  int           performanceCountIndex_;
  int           field_154;
  LARGE_INTEGER performanceCounts2_[16];
  int           performanceCountIndex2_;
  int           field_1DC;
  int           gamePausedCount_;

  HDC hMemDC_;

  HACCEL hViewAccel_;
  HACCEL hUnitAccel_;
  HACCEL hPauseAccel_;

  DetailPane  detailPane_;
  MiniMapPane miniMapPane_;
  CommandPane commandPane_;
  StatusBar   statusBar_;
  IniSettings iniSettings_;
  Font*       pFont1_;    // ** TODO what is this used for?
  // ** TODO more fields
};

inline auto& g_gameFrame = *GameFrame::GetInstance();

} // Tethys
