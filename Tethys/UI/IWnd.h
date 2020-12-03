
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/UI/Filter.h"

namespace Tethys {

class GFXSurface;

/// Exported base window class.
class IWnd : public OP2Class<IWnd> {
public:
  IWnd()                  { InternalCtor<0x431310>();                     }
  IWnd(const IWnd& src)   { InternalCtor<0x483D90, const IWnd&>(src);     }
  IWnd(InternalCtorChain) {                                               }

  virtual const char* RegisterClassA() { return Thunk<0x4314D0, &$::RegisterClassA>(); }

  virtual void* Destroy(ibool freeMem = 0) { return Thunk<0x483DF0, &$::Destroy>(freeMem); }  // virtual ~IWnd()

  virtual void DestroyWindow() { return Thunk<0x4318A0, &$::DestroyWindow>(); }
  virtual int  CreateEx(DWORD        dwExStyle,
                        const char*  pWndName,
                        DWORD        dwStyle,
                        int          x,
                        int          y,
                        int          w,
                        int          h,
                        HWND         hWndParent,
                        HMENU        hMenu,
                        void*        lpParam,
                        HINSTANCE    hInst)
  {
    return Thunk<0x4315D0, &$::CreateEx>(dwExStyle, pWndName, dwStyle, x, y, w, h, hWndParent, hMenu, lpParam, hInst);
  }

  virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
    { return Thunk<0x4316B0, &$::WndProc>(uMsg, wParam, lParam); }
  virtual int CallFilters(uint32 a, uint32 b, long c) { return Thunk<0x431740, &$::CallFilters>(a, b, c); }

#define OP2_IWND_VTBL($)  $(RegisterClassA)  $(Destroy)  $(DestroyWindow)  $(CreateEx)  $(WndProc)  $(CallFilters)
  DEFINE_VTBL_TYPE(OP2_IWND_VTBL, 0x4D0120);

  void InstallFilter(Filter*   pFilter, int a, FilterPositions position, FilterOptions options)
    { return Thunk<0x431380, &$::InstallFilter>(pFilter, a, position, options); }
  void RemoveFilter(Filter*    pFilter, int a) { return Thunk<0x431420, &$::RemoveFilter>(pFilter, a); }
  FilterNode* FindNode(Filter* pFilter, int a) { return Thunk<0x4314A0, &$::FindNode>(pFilter, a);     }

  static IWnd* FASTCALL FromHWND(HWND hWnd) { return OP2Thunk<0x483D80, &$::FromHWND>(hWnd); }
  static long  STDCALL IWndCbtFilterHook(int a, uint32 b, long c)
    { return OP2Thunk<0x431550, &$::IWndCbtFilterHook>(a, b, c); }
  static long  STDCALL _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    { return OP2Thunk<0x4312D0, &$::_WndProc>(hWnd, uMsg, wParam, lParam); }

public:
  ibool       isNotWindowOwner_;
  HWND        hWnd_;
  FilterNode* pTailFilterNode_;
  FilterNode* pHeadFilterNode_;
};


/// Exported base dialog window class.
class IDlgWnd : public IWnd {
  using $ = IDlgWnd;
private:
  void InsertHWNDChain() { return Thunk<0x431A10, &$::InsertHWNDChain>(); }
  void RemoveHWNDChain() { return Thunk<0x431A50, &$::RemoveHWNDChain>(); }

  static int STDCALL _DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    { return OP2Thunk<0x431980, &$::_DlgProc>(hWnd, uMsg, wParam, lParam); }

  static int*      GetModelessCount() { return OP2Mem<0x4E0BD8, int*>();      }
  static IDlgWnd** GetFirstDialog()   { return OP2Mem<0x4E0BD4, IDlgWnd**>(); }

public:
  IDlgWnd()                   : IWnd(UseInternalCtorChain) { InternalCtor<0x4318B0>();                     }
  IDlgWnd(const IDlgWnd& src) : IWnd(UseInternalCtorChain) { InternalCtor<0x483E20, const IDlgWnd&>(src);  }
  IDlgWnd(InternalCtorChain)  : IWnd(UseInternalCtorChain) {                                               }

  void* Destroy(ibool freeMem = 0) override { return Thunk<0x483EB0, &$::Destroy>(freeMem); }

  virtual int DoModal(const char* pResName, HINSTANCE hResInst)
    { return Thunk<0x431A80, &$::DoModal>(pResName, hResInst); }
  virtual int DoModeless(const char* pResName, HINSTANCE hResInst, HWND hWnd)
    { return Thunk<0x431BF0, &$::DoModeless>(pResName, hResInst, hWnd); }
  virtual INT_PTR DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    { return Thunk<0x431DB0, &$::DlgProc>(uMsg, wParam, lParam); }

#define OP2_IDLGWND_VTBL($)  $(DoModal)  $(DoModeless)  $(DlgProc)
  DEFINE_VTBL_TYPE(OP2_IDLGWND_VTBL, 0x4D0138);

  int  IsModalParentSet() const { return Thunk<0x483E10, &$::IsModalParentSet>();  }
  void SetAsModalParent(int a)  { return Thunk<0x431D90, &$::SetAsModalParent>(a); }

  static IDlgWnd* FASTCALL DlgFromHWND(HWND hWnd) { return OP2Thunk<0x483ED0, &$::DlgFromHWND>(hWnd); }

  static HWND FASTCALL GetModalParent()                { return OP2Thunk<0x431950, &$::GetModalParent>();           }
  static int  FASTCALL PretranslateModeless(MSG* pMsg) { return OP2Thunk<0x431DD0, &$::PretranslateModeless>(pMsg); }

public:
  IDlgWnd* pPrev_;
  IDlgWnd* pNext_;
  int      flags_;
};


/// Base pane child window class.
class TPane : public IWnd {
  using $ = TPane;
public:
  // ** TODO
  void* Destroy(ibool freeMem = 0) override;

  LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

  virtual void OnHide();
  virtual int  OnShow(int zoom);
  virtual void OnIdle();
  virtual void F1();
  virtual int  GetAbsolutePos(int pixelX, int pixelY, POINT* pPos);

#define OP2_TPANE_VTBL($)  $(OnHide)  $(OnShow)  $(OnIdle)  $(F1)  $(GetAbsolutePos)
  DEFINE_VTBL_TYPE(OP2_TPANE_VTBL, 0x4D6A50);

public:
  GFXSurface* pGfxSurface_;
  void*       pControls_[20];
  int         numControls_;
};

} // Tethys
