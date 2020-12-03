
#pragma once

#include "Tethys/Resource/GFXSurface.h"
#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/Filter.h"

namespace Tethys {

class Font;
class UICommand;
class CommandPaneView;

enum UIElementFlags : uint32 {
  UIElemDisabled   = (1u << 1),
  UIElemAddedToWnd = (1u << 2),
  UIElemHasFocus   = (1u << 3),
  UIElemHasHotKey  = (1u << 9),
};

class UIElement : public OP2Class<UIElement> {
public:
  UIElement()                  { InternalCtor<0x40A760>(); }
  UIElement(InternalCtorChain) {                           }

  // ** TODO verify these are correct
  virtual void* Destroy(ibool freeMem = 0) { return Thunk<0x40A770, &$::Destroy>(freeMem); } // virtual ~UIElement()

  virtual void Draw(GFXSurface* pSurface) { return Thunk<0x40B580, &$::Draw>(pSurface); }

  virtual int  OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    { return Thunk<0x40B590, &$::OnUIEvent>(pWnd, uMsg, wParam, lParam); }
  virtual void OnAdd(IWnd* pOwner)    { return Thunk<0x40A7E0, &$::OnAdd>(pOwner);    }
  virtual void OnRemove(IWnd* pOwner) { return Thunk<0x40A7F0, &$::OnRemove>(pOwner); }

  virtual void SetEnabledState(ibool on)  { return Thunk<0x40B5A0, &$::SetEnabledState>(on);  }

  virtual void InvalidateRect(IWnd* pWnd) { return Thunk<0x40A830, &$::InvalidateRect>(pWnd); }

#define OP2_UIELEMENT_VTBL($)  \
  $(Destroy)  $(Draw)  $(OnUIEvent)  $(OnAdd)  $(OnRemove)  $(SetEnabledState)  $(InvalidateRect)
  DEFINE_VTBL_TYPE(OP2_UIELEMENT_VTBL, 0x4CF898)

  void SetCapture(IWnd* pWnd)             { return Thunk<0x40A7A0, &$::SetCapture>(pWnd);         }
  void ReleaseCapture(IWnd* pWnd)         { return Thunk<0x40A7C0, &$::ReleaseCapture>(pWnd);     }
  void SetFlagsState(int flags, ibool on) { return Thunk<0x40A880, &$::SetFlagsState>(flags, on); }

public:
  int  flags_;
  RECT position_;
};
static_assert(sizeof(UIElement) == 24, "Incorrect UIElement size.");


class UIElementFilter : public Filter {
public:
  virtual int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int userData) override;

public:
  UIElement* pControl_;
};


class UIButton : public UIElement {
  using $ = UIButton;
public:
  UIButton()                  : UIElement(UseInternalCtorChain) { InternalCtor<0x40A8D0>(); }
  UIButton(InternalCtorChain) : UIElement(UseInternalCtorChain) {                           }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x40A8F0, &$::Destroy>(freeMem);    }
  void  Draw(GFXSurface* pSurface) override = 0;
  int   OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override
    { return Thunk<0x40AA50, &$::OnUIEvent>(pWnd, uMsg, wParam, lParam); }
  void  OnAdd(IWnd* pOwner)        override { return Thunk<0x40A920, &$::OnAdd>(pOwner);       }
  void  OnRemove(IWnd* pOwner)     override { return Thunk<0x40A950, &$::OnRemove>(pOwner);    }
  void  SetEnabledState(ibool on)  override { return Thunk<0x40A9B0, &$::SetEnabledState>(on); }

  virtual void GetHelpText(char* pDstBuffer, int bufferSize) = 0;
  virtual void OnClick()                                     = 0;

#define OP2_UIBUTTON_VTBL($)  $(GetHelpText)  $(OnClick)
  DEFINE_VTBL_TYPE(OP2_UIBUTTON_VTBL, 0x4CF8B8)

  void SetHotKey(int nVirtKey) { return Thunk<0x40A9F0, &$::SetHotKey>(nVirtKey); }

  // Variadic params = (UIButton* pButton, char* pText, char* pHelpText, ...)
  template <typename... Args>
  static void AddButtons(CommandPaneView* pView, int count, Args&&... args)
    { return OP2Thunk<0x458E50, void(CommandPaneView*, int, ...)>(pView, count, Args(args)...); }

public:
  int hotkey_;
};
static_assert(sizeof(UIButton) == 28, "Incorrect UIButton size.");


struct ButtonDisplayInfo {
  int     animationIndex;
  uint16  normalFrameIndex;
  uint16  activeFrameIndex;
  uint16  disabledFrameIndex;
  uint16  unknownFrameIndex;
  char*   pHelpText;
  char*   pButtonText;
  Font*   pFont;
  uint16  b1;
  uint16  b2;
  uint16  b3;
  uint16  padding;
};


class UIGraphicalButton : public UIButton {
  using $ = UIGraphicalButton;
public:
  UIGraphicalButton()                  : UIButton(UseInternalCtorChain) { InternalCtor<0x4666B0>(); }
  UIGraphicalButton(InternalCtorChain) : UIButton(UseInternalCtorChain) {                           }

  void* Destroy(ibool freeMem = 0)                    override { return Thunk<0x4666D0, &$::Destroy>(freeMem); }
  void  Draw(GFXSurface* pSurface)                    override { return Thunk<0x40AFD0, &$::Draw>(pSurface);   }
  void  GetHelpText(char* pDstBuffer, int bufferSize) override
    { return Thunk<0x40B2B0, &$::GetHelpText>(pDstBuffer, bufferSize); }

  void SetDisplayInfo(ButtonDisplayInfo* pButtonDisplayInfo, int pixelX, int pixelY, int flags)
    { return Thunk<0x40AF40, void(ButtonDisplayInfo*, int, int, int)>(pButtonDisplayInfo, pixelX, pixelY, flags); }
  void SetDisplayInfo(ButtonDisplayInfo* pButtonDisplayInfo, RECT* pButtonRect, int flags)
    { return Thunk<0x40AD70, void(ButtonDisplayInfo*, RECT*, int)>(pButtonDisplayInfo, pButtonRect, flags); }

public:
  int                buttonTextStringLength_;
  uint16             field_20;
  uint16             field_22;
  int                field_24;
  int                field_28;
  uint8              field_2C[88];
  ButtonDisplayInfo  buttonDisplayInfo_;
};
static_assert(sizeof(UIGraphicalButton) == 164, "Incorrect UIGraphicalButton size.");

class UICommandButton : public UIGraphicalButton {
  using $ = UICommandButton;
public:
  UICommandButton()                  : UIGraphicalButton(UseInternalCtorChain) { InternalCtor<0x459EC0>(); }
  UICommandButton(InternalCtorChain) : UIGraphicalButton(UseInternalCtorChain) {                           }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x459EF0, &$::Destroy>(freeMem); }
  void  OnClick()                  override { return Thunk<0x458E00, &$::OnClick>();        }

public:
  int         commandParam_;
  UICommand*  pCommand_;
};
static_assert(sizeof(UICommandButton) == 172, "Incorrect UICommandButton size.");


// Max Size: 256  [Array indexing]
struct ListItem {
  int   listItemIndex;  // [dataListIndex?]
  char  text[120];      // [char[120?]]
  //RenderData<?> renderData;
};

class ListData : public OP2Class<ListData> {
public:
  virtual int GetSize();
  virtual int GetFirstIndex();              ///< [-1 = List Empty]
  virtual int GetNextIndex(int     index);  ///< [-1 = Past Last]
  virtual int GetPrevIndex(int     index);  ///< [-1 = Before First]
  virtual int GetCurrentIndex(int  index);  ///< Maybe? Return different index type?
  virtual int GetDisplayString(int index, char* pStr, int strLen);
  virtual int GetStatusString(int  index, char* pStr, int strLen);
public:
  // ...?
};


struct ListStyle {
  ListData*  pListData;
  Font*      pFont;
  int        selectedBorderWidth;
  uint16     field_0C;             ///< color16
  uint16     backColor16;
  int        field_10;
};

class UIListBox : public UIElement {
  using $ = UIListBox;
public:
  // ** TODO member functions
  // UIListBox();
  //~UIListBox();

  void* Destroy(ibool freeMem = 0) override;
  void  Draw(GFXSurface* pSurface) override = 0;
  int   OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
  void  OnAdd(IWnd* pOwner)        override;
  void  OnRemove(IWnd* pOwner)     override;
  //void SetEnabledState(ibool on)   override;
  //void InvalidateRect(IWnd* pWnd)  override;

  virtual void OnUpdateSelection();
  virtual void OnClick();
  // ** TODO more virtual functions?

public:
  // NOTE: Some fields are list indexes, and others are data indexes
  // ** TODO verify this is correct
  int       numLinesVisible_;      ///< Total display area (round up for partial lines/borders padded/integral height)
  int       lineHeight_;           ///< Size of a single line
  int       numLinesDisplayed_;    ///< Sum of lines of all items at least partially visible
  int       numItems_;             ///< Maybe?
  ListItem* pListItems_;
  int       scrollFirstListIndex_; ///< -1 = None, don't display list
  int       scrollLastListIndex_;  ///< Maybe?
  uint16    field_34;
  uint16    field_36;
  int       currentIndex_;
  int       lastClickTime_;        ///< Milliseconds
  int       lastClickedOnIndex_;   ///< Used to check if second of a double click is on same item
  ListStyle listStyle_;            ///< Maybe?
  ListData* pListData_;
  Font*     pFont_;
  int       selectedBorderWidth_;  ///< Typically set to 2
  uint16    field_50;
  uint16    backColor16_;
  int       field_54;
  // ** TODO more fields?
};

// Max Size: 0x68  [class member variable packing]
class UIDoubleClickListBox : public UIListBox {
  using $ = UIDoubleClickListBox;
public:
  // ** TODO member functions
  // UIDoubleClickListBox();
  //~UIDoubleClickListBox();

  //virtual ~UIDoubleClickListBox();
  //void Draw(GFXClippedSurface* surface);
  //int  OnUIEvent(IWnd *wnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  //void OnAdd(IWnd* ownerWnd);
  //void OnRemove(IWnd* ownerWnd);
  //void SetEnabledState(ibool on);
  //void InvalidateRect(IWnd* wnd);
public:
  // ** TODO member variables
};


namespace UIElem {

class ListScrollButton : public UIGraphicalButton {
  using $ = ListScrollButton;
public:
  // ** TODO member functions
  void* Destroy(ibool freeMem = 0) override;
  void  OnClick()                  override;
};

} // UIElem

} // Tethys
