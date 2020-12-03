
#pragma once

#include "Tethys/UI/IWnd.h"

namespace Tethys {

class TFileDialog : public IDlgWnd {
  using $ = TFileDialog;
public:
  TFileDialog() : IDlgWnd(UseInternalCtorChain) { InternalCtor<0x416C00>(); }

  void*   Destroy(ibool freeMem = 0)                       override { return Thunk<0x416CB0, &$::Destroy>(); }
  INT_PTR DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    { return Thunk<0x4174C0, &$::DlgProc>(uMsg, wParam, lParam); }

  // ** TODO virtual functions
  virtual void F1();  // ** TODO 0x416C20
  virtual void F2();  // ** TODO 0x416C30
  virtual void F3();  // ** TODO 0x416C40
  virtual void F4();  // ** TODO 0x416C50
  virtual void F5();  // ** TODO 0x416C60
  virtual void F6();  // ** TODO 0x416C70
  virtual void F7();  // ** TODO 0x416C80
  virtual void F8();  // ** TODO 0x416C90
  virtual void F9();  // ** TODO 0x416CA0
  virtual void F10(); // ** TODO 0x417250

#define OP2_TFILEDIALOG_VTBL($)  \
  $(F1)  $(F2)  $(F3)  $(F4)  $(F5)  $(F6)  $(F7)  $(F8)  $(F9)  $(F10)
  DEFINE_VTBL_TYPE(OP2_TFILEDIALOG_VTBL, 0x4CFBD8);

public:
  // ** TODO member fields
  int field_20;
  int field_24;
  int field_28;
};


class SaveGameDialog : public TFileDialog {
public:
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x4872E0, &$::Destroy>(); }

  void F1() override; // ** TODO 0x4872D0
  void F2() override; // ** TODO 0x419170
  void F3() override; // ** TODO 0x418910
  void F4() override; // ** TODO 0x418900
  void F5() override; // ** TODO 0x418890
  void F6() override; // ** TODO 0x4872C0

  DEFINE_VTBL_GETTER(0x4D6290);

public:
  // ** TODO member fields
  int field_2C[17373];
};
static_assert(sizeof(SaveGameDialog) == 0x10FA0, "Incorrect SaveGameDialog size.");

class LoadGameDialog : public TFileDialog {
public:
  void* Destroy(ibool freeMem = 0) override { return Thunk<0x487410, &$::Destroy>(); }

  void F2()  override; // ** TODO 0x419190
  void F3()  override; // ** TODO 0x418E90
  void F4()  override; // ** TODO 0x4188F0
  void F5()  override; // ** TODO 0x4188C0
  void F6()  override; // ** TODO 0x487400
  void F10() override; // ** TODO 0x418520

  DEFINE_VTBL_GETTER(0x4D62E0);

public:
  // ** TODO member fields
  int field_2C[17374];
};
static_assert(sizeof(LoadGameDialog) == 0x10FA4, "Incorrect LoadGameDialog size.");

} // Tethys
