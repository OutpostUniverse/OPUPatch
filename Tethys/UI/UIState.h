
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class UIState : public OP2Class<UIState> {
public:
  explicit UIState(uint32 controlID)  { InternalCtor<0x483EE0, uint32>(controlID); }
           UIState(InternalCtorChain) {                                            }

  virtual void SetText(const char* pText) { return Thunk<0x49CD10, &$::SetText>(pText); }
  virtual void SetCheck(int a)            { return Thunk<0x49CD20, &$::SetCheck>(a);    }
  virtual void Enable(int a)              { return Thunk<0x49CD30, &$::Enable>(a);      }

#define OP2_UISTATE_VTBL($)  $(SetText)  $(SetCheck)  $(Enable)
  DEFINE_VTBL_TYPE(OP2_UISTATE_VTBL, 0x4D6138);

public:
  uint32 controlID_;
};


class BoolState : public UIState {
public:
  explicit BoolState(uint32 controlID) : UIState(UseInternalCtorChain) { InternalCtor<0x483F90, uint32>(controlID); }

  void Enable(int a) override { return Thunk<0x41C640, &$::Enable>(a); }

  DEFINE_VTBL_GETTER(0x4CFC90);
};


class MenuState : public UIState {
public:
  MenuState(HMENU hMenu, uint32 controlID)
    : UIState(UseInternalCtorChain) { InternalCtor<0x49CD40, HMENU, uint32>(hMenu, controlID); }

  void SetText(const char* pText) override { return Thunk<0x49CD70, &$::SetText>(pText); }
  void SetCheck(int a)            override { return Thunk<0x49CDB0, &$::SetCheck>(a);    }
  void Enable(int a)              override { return Thunk<0x49CDE0, &$::Enable>(a);      }

  DEFINE_VTBL_GETTER(0x4D6148);

public:
  HMENU hMenu_;
};

} // Tethys
