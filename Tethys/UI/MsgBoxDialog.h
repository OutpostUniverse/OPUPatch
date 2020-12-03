
#pragma once

#include "Tethys/UI/IWnd.h"

namespace Tethys {

class MsgBoxDlg : public IDlgWnd {
public:
  MsgBoxDlg() : IDlgWnd(UseInternalCtorChain) { InternalCtor<0x497DC0>(); }

  MsgBoxDlg(int titleIndex, const char* pMessage, int buttonTextIndex)
    : IDlgWnd(UseInternalCtorChain)
      { InternalCtor<0x497E20, int, const char*, int>(titleIndex, pMessage, buttonTextIndex); }

  MsgBoxDlg(int titleIndex, int messageIndex, int buttonTextIndex)
    : IDlgWnd(UseInternalCtorChain)
      { InternalCtor<0x497EC0, int, int, int>(titleIndex, messageIndex, buttonTextIndex); }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x497DA0, &$::Destroy>(freeMem); }
  int   DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    { return Thunk<0x498230, &$::DlgProc>(uMsg, wParam, lParam); }

public:
  int   field_20;
  int   field_24;
  int   field_28;
  int   field_2C;
  int   field_30;
  int   field_34;
  int   field_38;
  HFONT hFont1_;
  HFONT hFont2_;
  HFONT hFont3_;
  int   field_48;
  int   field_4C;
  int   field_50;
  int   field_54;
  int   field_58;
  int   field_5C;
  int   field_60;
  int   field_64;
  int   field_68;
  int   field_6C;
  int   field_70;
  int   field_74;
  char  title_[256]; // 78
  char  message_[256];
  char  buttonText_[256];
};

} // Tethys
