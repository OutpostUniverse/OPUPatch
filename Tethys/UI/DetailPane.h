
#pragma once

#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/Filter.h"

namespace Tethys {

class Viewport;
class MapObject;

class DetailPane : public TPane {
  using $ = DetailPane;
public:
  // ** TODO
  void* Destroy(ibool freeMem = 0) override; // 0x407810 virtual ~DetailPane()

  LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)    override; // 0x407FA0
  int     CallFilters(uint32, uint32, long)                   override; // 0x407A50
  int     OnShow(int zoom)                                    override;
  void    OnIdle()                                            override; // 0x407E50
  int     GetAbsolutePos(int pixelX, int pixelY, POINT* pPos) override; // 0x408070

  virtual int GetViewportRelativePos(int pixelX, int pixelY, POINT* pPos);              // 0x4080E0
  virtual int GetViewportTilePositionAndSize(POINT* pTopLeftTilePos, POINT* pTileSize); // 0x408370

  void CenterViewOn(int pixelX, int pixelY) { return Thunk<0x408210, &$::CenterViewOn>(pixelX, pixelY); }

  // ** TODO more member functions
  /* ?                              0x407880
     CenterOnBookmarkedLocation     0x408510
     CenterViewOnSelectedUnits      0x408140
     ClipViewToMap                  0x407990
     Constructor                    0x4077D0
     CreateWindow                   0x4078A0
     Deinitialize                   0x4085A0
     Destructor                     0x407850
     DisplayLargeMessage            0x4085C0
     DrawLargeFontOverlayMessage    0x4086B0
     EraseLargeMessage              0x408680
     FadeOut                        0x4088C0
     GetViewCenter                  0x4082F0
     GetViewportRelativePos         0x4080E0
     GetViewportTilePositionAndSize 0x408370
     GotoBookmarkedLocation         0x4084F0
     HandleScrolling                0x407B00
     Load                           0x408420
     ReallocSurface                 0x407900
     Save                           0x4084A0 */

public:
  HDC   hDstDC_;
  int   field_70[15];
  char  largeMessage_[132];  ///< Game paused, mission accomplished, mission failed, victory is sweet, you have failed
  HFONT hLargeMessageFont_;
  int   field_134;
  int   field_138;
  int   field_13C;
  RECT  viewPosition_;
};


class DetailPaneFilter : public GroupFilter {
public:
  int OnUIEvent(IWnd* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, int userData) override;
};


class MapObjDrawList : public OP2Class<MapObjDrawList> {
public:
  void BuildDrawLists() { return Thunk(0x407ECD + OP2Mem<ptrdiff_t&>(0x407EC8)); }  // 0x49EB60
  void Draw()           { return Thunk<0x49E990, &$::Draw>();                    }
  void MarkForRedraw()  { return Thunk<0x49EC70, &$::MarkForRedraw>();           }
  void Func_1()         { return Thunk<0x49EAB0, &$::Func_1>();                  }

public:
  static constexpr size_t MaxPerGroup = 511;

  Viewport*  pViewport_;
  int        field_04;
  int        numUnits_;
  int        numEntities_;
  MapObject* pUnitDrawList_[MaxPerGroup];    ///< MapUnit (player) objects
  MapObject* pEntityDrawList_[MaxPerGroup];  ///< MapEntity (gaia) objects
};

} // Tethys
