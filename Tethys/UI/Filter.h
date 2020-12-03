
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class IWnd;
class Pane;
class Filter;
class GroupFilter;
class SubFilter;
class BoolState;
class MouseCommand;
class UIElement;

enum class FilterPositions : int {
  Last = 0,
  First
};

enum class FilterOptions : int {
  Mouse = 1,
  Keyboard
};

enum class BehaviorType : int {
  BandboxUnitSelect = 0,  ///< Left-clicking starts drawing a bandbox
  SetRectRegion,          ///< Bulldoze or Salvage region
  SetBuildLocation,       ///< Highlight area under mouse (tile aligned, multicoloured region)
  SetTarget,              ///< Left-click to set destination (Attack, Move, etc.) (don't select units)
  SetLineRegion,          ///< Build Tube, Wall, Lava Wall, or Microbe Wall
  MouseDisabled,          ///< Arrow cursor, clicking is disabled (including Right-click)
};


class FilterNode {
public:
  FilterNode*   pPrev;
  FilterNode*   pNext;
  Filter*       pFilter;
  int           data;
  FilterOptions options;
};


/// Exported UI input filter class.
class Filter : public OP2Class<Filter> {
public:
  Filter()                             { InternalCtor<0x483CB0>();                     }
  Filter(const Filter& src)            { InternalCtor<0x483CC0, const Filter&>(src);   }
  Filter(InternalCtorChain)            {                                               }

  static Filter* FASTCALL GetCapture() { return OP2Thunk<0x431210, &$::GetCapture>(); }

  virtual int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int data)
    { return Thunk<0x483CA0, &$::OnUIEvent>(pWnd, uMsg, wParam, lParam, data); }

#define OP2_FILTER_VTBL($)  $(OnUIEvent)
  DEFINE_VTBL_TYPE(OP2_FILTER_VTBL, 0x4D612C);

  void ReleaseCapture(IWnd* pWnd)           { return Thunk<0x431200, &$::ReleaseCapture>(pWnd);   }
  void SetCapture(IWnd*     pWnd, int data) { return Thunk<0x431190, &$::SetCapture>(pWnd, data); }

  static FilterNode** GetCaptureNode() { return OP2Mem<0x4E0BC4, FilterNode**>(); }
};


/// Exported UI input sub-filter class.
class SubFilter : public Filter {
  using $ = SubFilter;
public:
  int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int data) override
    { return Thunk<0x483D40, &$::OnUIEvent>(pWnd, uMsg, wParam, lParam, data); }

  DEFINE_VTBL_GETTER(0x4D6134);

  SubFilter()                     : Filter(UseInternalCtorChain) { InternalCtor<0x483D50>();                      }
  SubFilter(const SubFilter& src) : Filter(UseInternalCtorChain) { InternalCtor<0x483D60, const SubFilter&>(src); }

  void ReleaseCapture(GroupFilter* pGroupFilter, IWnd* pWnd)
    { return Thunk<0x431200, &$::ReleaseCapture>(pGroupFilter, pWnd); }
  void SetCapture(GroupFilter* pGroupFilter, IWnd* pWnd, int data)
    { return Thunk<0x431190, &$::SetCapture>(pGroupFilter ,pWnd, data); }
};


/// Exported UI input group filter class.
class GroupFilter : public Filter {
  using $ = GroupFilter;
public:
  GroupFilter()                       : Filter(UseInternalCtorChain) { InternalCtor<0x483CE0>();                       }
  GroupFilter(const GroupFilter& src) : Filter(UseInternalCtorChain) { InternalCtor<0x483D10,const GroupFilter&>(src); }

  int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int data) override
    { return Thunk<0x431220, &$::OnUIEvent>(pWnd, uMsg, wParam, lParam, data); }

  DEFINE_VTBL_GETTER(0x4D6130);

  SubFilter* GetSubFilter() const                { return Thunk<0x483D00, &$::GetSubFilter>();           }
  void       SetSubFilter(SubFilter* pSubFilter) { return Thunk<0x483CF0, &$::SetSubFilter>(pSubFilter); }

public:
  SubFilter* pSubFilter_;
};


/// Internal mouse command input filter class.
class MouseCommandFilter : public SubFilter {
  using $ = MouseCommandFilter;
public:
  // ** TODO
  int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int userData) override;

  virtual void F1();  // ** TODO init?
  virtual void ClearMouseCommand();

  virtual int  GetControlBoolState(int controlId);
  virtual int  GetControlState(BoolState* pControlState);

  virtual void DoMouseOverEvents();

  virtual void OnUnitSelectionChange(int userData, int reason);

  virtual HCURSOR GetMouseCursor();

  virtual int  F4();
  virtual void DrawSelectionBoxesAndMouseOverText(Pane* pPane);
  // ...?

  /// SetMouseCommand
  /// 3, 7, 0, 1  Move, StandGround, DestroyTubeOrWall, Guard, Repair, Reprogram, Dismantle
  /// 2, 7, 0, 1  Build, BuildGeoPlantOrMine
  /// 4, 0, 0, 1  BuildWalld
  /// 3, ?, 0, 1  Attack, PatrolSetFinalPoint
  /// 1, 0, 0, 1  Bulldoze, Salvage
  /// 3, 1, 0, 1  SalvageSetRouteToGorf, Launch
  /// 3, 5, 0, 1  SetOreRouteToMine
  /// 3, ?, 0, ?  SetOreRouteToSmelter/BackToMine
  /// 3, 6, 0, 1  PatrolSetInitialPoint
  /// 3, ?, 1, 1  MoveButton
  /// 0, 0, 1, 1  (LaunchDest), (CPV[Building]/[Vehicle].OnRemoveView), (???), (OnUnitSelectionChange),
  ///             (CPV[StorageBay]/[TrainScientists].OnSetupDisplay), (CPVReport[Factory]/[Resources].???),
  ///             (CPVReport[GameOptions]/[Communications]/[NetStats]/[BuildList]/[VehicleList]/[Lab]/[ResearchList]/
  ///              [Satellite].OnAddView)
  void SetMouseCommand(BehaviorType behaviorType, MouseCommand* pMouseCommand, int maxNumWaypoints, int a, int b)
    { return Thunk<0x41B670, &$::SetMouseCommand>(behaviorType, pMouseCommand, maxNumWaypoints, a, b); }

  void SetOverlaySize(int pixelX, int pixelY) { return Thunk<0x41B6D0, &$::SetOverlaySize>(pixelX, pixelY); }

  void GetHighlightRect(RECT* pHighlightRect) { return Thunk<0x41B6F0, &$::GetHighlightRect>(pHighlightRect); }

  static MouseCommandFilter* GetInstance() { return OP2Mem<0x5471B0, MouseCommandFilter*>(); }

public:
  union {  // ** TODO Make sure this does what I mean
    struct {
      POINT     mapMousePos_;
      int       overlayPixelWidth_;
      int       overlayPixelHeight_;
    };
    RECT        pixelRegion_;
  };
  int           mouseOverUnitIndex_;  ///< Set when the mouse hovers over a unit, and none are selected
  int           mouseOverTick_;       ///< Set when the mouse hovers over a unit, and none are selected
  int           field_1C;
  int           field_20;
  BehaviorType  behaviorType_;        ///< [param1]
  IWnd*         pCaptureWnd_;
  MouseCommand* pCommand_;            ///< [param2] 
  int           field_30;             ///< [param4] 
  int           maxNumWaypoints_;     ///< [param3]
  int           field_38;             ///< [param5] 
  int           field_3C;  
  // ** TODO more fields?
};


/// Internal keyboard command input filter class.
class HotKeyFilter : public Filter {
public:
  // ** TODO
  int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int userData) override;

  ibool SetHotKeyButton(UIElement* pButton, int hotKey);
  void  RemoveHotKey(int hotKey);
  int   SetPauseCount(ibool paused);

public:
  UIElement* pHotKeyTable_[256];
  int        pauseCount_;
};

} // Tethys
