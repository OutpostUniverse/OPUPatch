
#pragma once

#include "Tethys/Common/Util.h"
#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/UIElement.h"

namespace Tethys {

class StreamIO;
class GFXSurface;
class CommandPaneView;
enum class SoundID : int;

class CommandViewButton : public UIGraphicalButton {
  using $ = CommandViewButton;
public:
  CommandViewButton()                  : UIGraphicalButton(UseInternalCtorChain) { InternalCtor<0x45D2A0>(); }
  CommandViewButton(InternalCtorChain) : UIGraphicalButton(UseInternalCtorChain) {                           }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x45C5D0, &$::Destroy>(freeMem); }
  void  OnClick()                  override { return Thunk<0x45C760, &$::OnClick>();        }

  virtual void F1() { return Thunk<0x45C7F0, &$::F1>(); }

#define OP2_COMMANDVIEWBUTTON_VTBL($)  $(F1)
  DEFINE_VTBL_TYPE(OP2_COMMANDVIEWBUTTON_VTBL);  // ** TODO vtbl address?

public:
  CommandPaneView* pView_;
  ibool            deselectUnits_;
};


class CommandPane : public TPane {
  using $ = CommandPane;
public:
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x45C860, &$::Destroy>(freeMem); }

  LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)    override; // 0x45C970
  int     CallFilters(uint32, uint32, long)                   override;
  int     OnShow(int zoom)                                    override; // 0x45CC60
  void    OnIdle()                                            override;
  int     GetAbsolutePos(int pixelX, int pixelY, POINT* pPos) override;

  virtual void MarkRectForRedraw(const RECT& rect) { return Thunk<0x45CF30, &$::MarkRectForRedraw>(rect); }
  virtual void Paint(const RECT& rect)             { return Thunk<0x45CE30, &$::Paint>(rect);             }

#define OP2_COMMANDPANE_VTBL($)  $(MarkRectForRedraw)  $(Paint)

  // ** TODO member functions
  /* ?                     0x49D220
     ActivateReport        0x45D160 */

  void OnUnitSelectionChange() { return Thunk<0x45CD70, &$::OnUnitSelectionChange>(); }

public:
  CommandViewButton reportButtons_[6];
  RECT              redrawRect_;
  int               field_484;
  int               controlNum_;
  int               selectedControl_;
  void*             pCurrentView_;
  void*             pViewTop_;
};
static_assert(sizeof(CommandPane) == 0x498, "Incorrect CommandPane size.");


class CommandPaneView : public OP2Class<CommandPaneView> {
public:
  virtual void  UpdateView()   { return Thunk<0x459F10, &$::UpdateView>();   }
  virtual void  OnAddView()    { return Thunk<0x459F20, &$::OnAddView>();    }
  virtual void  OnRemoveView() { return Thunk<0x459F30, &$::OnRemoveView>(); }
  virtual ibool IsNewView()    { return Thunk<0x459F40, &$::IsNewView>();    }

  virtual void Draw(RECT* pRect, GFXSurface* pSurface) = 0;

  virtual void SetReportPageIndex() { return Thunk<0x459F50, &$::SetReportPageIndex>(); }

  virtual ibool RefreshOnUnitSelection() { return Thunk<0x459F60, &$::RefreshOnUnitSelection>(); }

  virtual void OnAction() { return Thunk<0x46D960, &$::OnAction>(); }

  virtual int GetSelectedReportButtonIndex() { return Thunk<0x459F70, &$::GetSelectedReportButtonIndex>(); }

#define OP2_COMMANDPANEVIEW_VTBL($)                                                                                    \
  $(UpdateView)  $(OnAddView)  $(OnRemoveView)  $(IsNewView)  $(Draw)  $(SetReportPageIndex)  $(RefreshOnUnitSelection)\
  $(OnAction)  $(GetSelectedReportButtonIndex)
  DEFINE_VTBL_TYPE(OP2_COMMANDPANEVIEW_VTBL, 0x4D5200)
};


template <size_t MessageLen = 64>
struct MessageLogEntry {
  int  timestamp;
  int  pixelX;
  int  pixelY;
  char message[MessageLen];
};

/// Stores messages displayed in the message log command pane.
class MessageLog : public OP2Class<MessageLog> {
public:
  MessageLog() { Init(); }

  void Init() { return Thunk<0x439470, &$::Init>(); }

  void RefreshTimestamps() { return Thunk<0x439510, &$::RefreshTimestamps>(); }

  int   AddMessage(int pixelX, int pixelY, const char* pMsg, SoundID soundID = {})
    { return Thunk<0x439070, &$::AddMessage>(pixelX, pixelY, pMsg, soundID); }
  ibool ShouldAddMessage(SoundID soundID, int pixelX, int pixelY)
    { return Thunk<0x439590, &$::ShouldAddMessage>(soundID, pixelX, pixelY); }

  ibool Save(StreamIO* pSavedGame) { return Thunk<0x4392E0, &$::Save>(pSavedGame); }
  ibool Load(StreamIO* pSavedGame) { return Thunk<0x4393A0, &$::Load>(pSavedGame); }

  template <size_t RingBufferCapacity = 64>
  int GetEntrySlotFromIndex(int index)
    { return (index != -1) ? ((numRbElements_ - index + rbBegin_ - 1) % RingBufferCapacity) : index; }

  /// Gets the global MessageLog instance.
  static MessageLog* GetInstance() { return OP2Mem<0x054FCF0, MessageLog*>(); }

public:
  int                 rbBegin_;
  int                 numRbElements_;
  MessageLogEntry<64> entryRb_[64];
  DWORD               timestamps_[10];
  uint8               field_1330[800];
};

inline auto& g_messageLog = *MessageLog::GetInstance();


namespace UIElem {

class BayButton : public UIGraphicalButton {
  using $ = BayButton;
public:
  // BayButton(); // ** TODO

  void* Destroy(ibool freeMem = 0)                   override { return Thunk<0x462C30, &$::Destroy>(freeMem); }
  void GetHelpText(char* pDstBuffer, int bufferSize) override
    { return Thunk<0x462940, &$::GetHelpText>(pDstBuffer, bufferSize); }
  void OnClick()                                     override { return Thunk<0x4627C0, &$::OnClick>();        }

public:
  int bayNum_;
};

class MessageLogJumpToButton : public UIGraphicalButton {
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};

class VehicleListJumpToButton : public UIGraphicalButton {
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};


class LessMoreAssignResearchButton : public UIGraphicalButton {
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};

class LessMoreTrainScientistsButton : public UIGraphicalButton {
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};

class LessMoreGameOptionButton : public UIGraphicalButton {
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};

class MiniMapButton : public UIGraphicalButton {
  using $ = MiniMapButton;
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x458430, &$::Destroy>(freeMem); }
  void  OnClick()                  override { return Thunk<0x458360, &$::OnClick>();        }
};

class ResetNetworkStatsButton : public UIGraphicalButton {
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};


class BuildButton : public CommandViewButton {
  using $ = BuildButton;
public:
  // BuildButton(); // ** TODO

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x468100, &$::Destroy>(freeMem); }
  void  OnClick()                  override { return Thunk<0x467E80, &$::OnClick>();        }
};


enum class ReportPage : int {
  ResourcesReport = 0,
  ColonistsStatus,
  MoraleStatus,
  PowerStatus,
  FoodStatus,
  CommonMetalsStatus,
  RareMetalsStatus,
  ColonistStructures,
  MoraleStructures,
  PowerStructures,
  FoodStructures,
  CommonMetalsStructures,
  RareMetalsStructures,
};


class ReportPageButton : public CommandViewButton {
  using $ = ReportPageButton;
public:
  // ReportPageButton(); // ** TODO

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x45A090, &$::Destroy>(freeMem); }
  void  OnClick()                  override { return Thunk<0x45C5A0, &$::OnClick>();        }

public:
  ReportPage reportPageIndex_;
};

class UnitCommandView : public CommandPaneView {
  using $ = UnitCommandView;
public:
  void  UpdateView()                            override { return Thunk<0x4593D0, &$::UpdateView>();             }
  void  OnAddView()                             override { return Thunk<0x459470, &$::OnAddView>();              }
  void  Draw(RECT* pRect, GFXSurface* pSurface) override { return Thunk<0x4595C0, &$::Draw>(pRect, pSurface);    }
  ibool RefreshOnUnitSelection()                override { return Thunk<0x459F80, &$::RefreshOnUnitSelection>(); }

  DEFINE_VTBL_GETTER(0x4D5278);

public:
  // ** TODO member variables?
};

class ReportView : public CommandPaneView {
  using $ = ReportView;
public:
  void  Draw(RECT* pRect, GFXSurface* pSurface) override { return Thunk<0x459C90, &$::Draw>(pRect, pSurface);    }
  void  SetReportPageIndex()                    override { return Thunk<0x464A10, &$::SetReportPageIndex>();     }
  ibool RefreshOnUnitSelection()                override { return Thunk<0x464A20, &$::RefreshOnUnitSelection>(); }

  virtual void F1()      = 0;
  virtual void F2(int a) = 0;
  virtual void UpdateListSelection(int itemIndex) { return Thunk<0x459D60, &$::UpdateListSelection>(itemIndex); }
  virtual void SetScrollButtonsEnabledState()     { return Thunk<0x459DB0, &$::SetScrollButtonsEnabledState>(); }

#define OP2_REPORTVIEW_VTBL($)  $(F1)  $(F2)  $(UpdateListSelection)  $(SetScrollButtonsEnabledState)
  DEFINE_VTBL_TYPE(OP2_REPORTVIEW_VTBL, 0x4D5590);

public:
  // ** TODO these might belong in child classes?
  //int           field_04[112];
  //ReportPage    curReportPage_;
  //int           field_1C8[10];
  //Pane*         pReportPane_;
  // ** TODO more fields?
};

class BuildingStorageBayView : public CommandPaneView {
  using $ = BuildingStorageBayView;
public:
  void  UpdateView()                            override { return Thunk<0x462CB0, &$::UpdateView>();                   }
  void  OnAddView()                             override { return Thunk<0x463100, &$::OnAddView>();                    }
  void  OnRemoveView()                          override { return Thunk<0x463170, &$::OnRemoveView>();                 }
  void  Draw(RECT* pRect, GFXSurface* pSurface) override { return Thunk<0x4638F0, &$::Draw>(pRect, pSurface);          }
  ibool RefreshOnUnitSelection()                override { return Thunk<0x462C00, &$::RefreshOnUnitSelection>();       }
  int   GetSelectedReportButtonIndex()          override { return Thunk<0x462BF0, &$::GetSelectedReportButtonIndex>(); }

  DEFINE_VTBL_GETTER(0x4D54C0);

  static BuildingStorageBayView* GetInstance() { return OP2Mem<0x565D30, BuildingStorageBayView*>(); }

public:
  // ** TODO member variables
};

class CommunicationsReportView : public ReportView {
  using $ = CommunicationsReportView;
public:
  void UpdateView()                            override { return Thunk<0x4669F0, &$::UpdateView>();                   }
  void OnAddView()                             override { return Thunk<0x466A50, &$::OnAddView>();                    }
  void Draw(RECT* pRect, GFXSurface* pSurface) override { return Thunk<0x466B70, &$::Draw>(pRect, pSurface);          }
  int  GetSelectedReportButtonIndex()          override { return Thunk<0x466940, &$::GetSelectedReportButtonIndex>(); }
  void F1()                                    override { return Thunk<0x4671D0, &$::F1>();                           }
  void F2(int a)                               override { return Thunk<0x466B30, &$::F2>(a);                          }
  void UpdateListSelection(int itemIndex)      override { return Thunk<0x466AA0, &$::UpdateListSelection>(itemIndex); }

  DEFINE_VTBL_GETTER(0x4D56E0);

  static CommunicationsReportView* GetInstance() { return OP2Mem<0x567150, CommunicationsReportView*>(); }

public:
  // ** TODO member variables
  //UIDoubleClickListBox buildListBox;
  //ListScrollButton     upButton;
  //ListScrollButton     downButton;
  // ...?
};

class FactoryReportView : public ReportView {
  using $ = FactoryReportView;
public:
  void UpdateView()                            override { return Thunk<0x464BB0, &$::UpdateView>();                   }
  void OnAddView()                             override { return Thunk<0x464BD0, &$::OnAddView>();                    }
  void Draw(RECT* pRect, GFXSurface* pSurface) override { return Thunk<0x464DC0, &$::Draw>(pRect, pSurface);          }
  int  GetSelectedReportButtonIndex()          override { return Thunk<0x464AD0, &$::GetSelectedReportButtonIndex>(); }
  void F1()                                    override { return Thunk<0x464D00, &$::F1>();                           }
  void F2(int a)                               override { return Thunk<0x464D40, &$::F2>(a);                          }
  void UpdateListSelection(int itemIndex)      override { return Thunk<0x464D60, &$::UpdateListSelection>(itemIndex); }
  
  // ** TODO
  virtual void F3() { }  // 0x464C20
  virtual void F4() { }  // 0x464DA0
  
#define OP2_FACTORYREPORTVIEW_VTBL($)  $(F3)  $(F4)
  DEFINE_VTBL_TYPE(OP2_FACTORYREPORTVIEW_VTBL, 0x4D55C8);

  static FactoryReportView* GetInstance() { return OP2Mem<0x566408, FactoryReportView*>(); }

public:
  // ** TODO member variables
  //UIDoubleClickListBox buildListBox;  // 0x4   [0x56640C]
  //ListScrollButton     downButton;    // 0x6C  [0x566474] down? maybe
  //ListScrollButton     upButton;      // 0x118 [0x566520] up? maybe
  //                                             [0x5665CC]
  //                                             [0x5665D0] BuildList? (object)
  //UICommandButton      buildButton;   //       [0x566658] build? maybe
  // ...?
};

class LabReportView : public ReportView {
  using $ = LabReportView;
public:
  void UpdateView()                            override { return Thunk<0x46B9A0, &$::UpdateView>();                   }
  void OnAddView()                             override { return Thunk<0x46B9C0, &$::OnAddView>();                    }
  void Draw(RECT* pRect, GFXSurface* pSurface) override { return Thunk<0x46BC30, &$::Draw>(pRect, pSurface);          }
  int  GetSelectedReportButtonIndex()          override { return Thunk<0x46B840, &$::GetSelectedReportButtonIndex>(); }
  void F1()                                    override { return Thunk<0x46BCB0, &$::F1>();                           }
  void F2(int a)                               override { return Thunk<0x46BDF0, &$::F2>(a);                          }
  void UpdateListSelection(int itemIndex)      override { return Thunk<0x46BE50, &$::UpdateListSelection>(itemIndex); }

  // ** TODO
  virtual void F3() { }  // 0x46BCB0
  virtual void F4() { }  // 0x46BDF0

  // ** TODO
  // virtual void SetScrollButtonsEnabledState();
  // ...

#define OP2_LABREPORTVIEW_VTBL($)  $(F3)  $(F4)  // $(SetScrollButtonsEnabledState), ...
  DEFINE_VTBL_TYPE(OP2_LABREPORTVIEW_VTBL, 0x4D59B8);

  static LabReportView* GetInstance() { return OP2Mem<0x568E88, LabReportView*>(); }

public:
  // ** TODO member variables
};

} // UIElem

} // Tethys
