
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Location.h"

namespace Tethys {

class GFXBitmap;

/// Background scanline draw methods.
enum class DrawBackgroundMethod : int {
  _8Pal16                    = 0,
  _8Pal16MaskedBlightDither,
  _8Pal16DayNightTransition,
  FadeDestSourceFadeMask,
  Copy16Raw,
  Copy16Raw_2,                      ///< Same as previous?
  Copy16Raw_3,                      ///< Same as previous?
  Copy16Raw_4,                      ///< Same as previous?
  FadeDestSourceFadeMask_2,         ///< Same as FadeDestSourceFadeMask?
  _8Pal16FullBlightDither    = 15,
};

/// Sprite scanline draw methods.
enum class DrawSpriteMethod : int {
  _8Pal16Transparent0 = 0,
  _8Pal16DayNightTransitionTransparent0,
  _8Pal16DayNightTransitionTransparent0_2,  ///< Same as previous?
  FadeDestSourceFadeMask,
};


class GFXSurface : public OP2Class<GFXSurface> {
public:
  // ** TODO
  virtual void  VF0(int, int, int);
  virtual void  Lock();
  virtual void  Unlock();
  virtual void  LockDC();
  virtual void  UnlockDC();
  virtual void* Destroy(ibool freeMem = 0);  // virtual ~GFXSurface()
  virtual int   VF18();
  virtual int   VF1C();
  virtual void  Realloc(HWND hWnd);
  virtual void  VF24();
  virtual void  Draw(const RECT& rect);
  virtual void  Draw();
  virtual void  VF30(int, int, int);
  
public:
  int   field_04;
  int   field_08;
  int   field_0C;
  int   maxX_;
  int   maxY_;
  int   field_18;
  int   field_1C;
  int   field_20;
  int   field_24;
  int   field_28;
  int   field_2C;
  int   field_30;
  void* pLockedImageBuffer_;
  int   pitch_;
  int   width_;
  int   height_;
  int   bpp_;
  int   field_48;
  int   field_4C;
  int   field_50;
  int   field_54;
  int   field_58;
  int   surfaceType_;
  int   field_60;
  int   lockCount_;
  int   field_68;
  HDC   hLockedDC_;
  int   field_70;
  int   field_74;
};
static_assert(sizeof(GFXSurface) == 0x78, "incorrect GFXSurface size");

class GFXCDSSurface : public GFXSurface {
public:
  // ** TODO
  void  VF0(int, int, int)         override;
  void  Lock()                     override;
  void  Unlock()                   override;
  void  LockDC()                   override;
  void  UnlockDC()                 override;
  void* Destroy(ibool freeMem = 0) override;
  int   VF18()                     override;
  int   VF1C()                     override;
  void  Realloc(HWND hWnd)         override;
  void  VF24()                     override;
  void  Draw(const RECT& rect)     override;
  void  Draw()                     override;
  void  VF30(int, int, int)        override;

public:
  HBITMAP  hDibSection_;
  void*    pBitmapBits_;
  HDC      hWindowDC_;
  HDC      hMemoryDC_;
  HPALETTE hPalette_;
  HPALETTE hOldPalette_;
  HBITMAP  hOldDibSection_;
  HWND     hDstWnd_;
};
static_assert(sizeof(GFXCDSSurface) == 0x98, "Incorrect GFXCDSSurface siz.");

class GFXMemSurface : public GFXSurface {
public:
  // ** TODO
  void  VF0(int, int, int)         override;
  void  Lock()                     override;
  void  Unlock()                   override;
  void  LockDC()                   override;
  void  UnlockDC()                 override;
  void* Destroy(ibool freeMem = 0) override;
  int   VF18()                     override;
  int   VF1C()                     override;
  void  VF24()                     override;
  void  Draw(const RECT& rect)     override;
  void  VF30(int, int, int)        override;

public:
  void* pBitmap_;
  ibool isBitmapOwned_;
  // ** TODO Probably more after this
};
static_assert(sizeof(GFXMemSurface) == 0x80, "Incorrect GFXMemSurface size.");

class Viewport : public OP2Class<Viewport> {
public:
  void MarkForRedraw(const RECT& rect) { return Thunk<0x46F670, &$::MarkForRedraw>(rect); }

  void MarkTileRowForRedraw(int tileY, int tileX1, int tileX2)
    { return Thunk<0x46F290, &$::MarkTileRowForRedraw>(tileY, tileX1, tileX2); }

  // ** TODO more member functions
  /* ???                                     0x403AE0
     ???2                                    0x46F480
     Constructor                             0x46F0D0
     Destructor                              0x46F150
     DrawBackground                          0x470080
     DrawCheatedGameMessageAmongOtherThings? 0x46F940
     Initialize                              0x46F160
     MarkAnimatedTilesForRedraw              0x46FF80
     MarkBackgroundRectToRedraw              0x403D90
     SetRedrawFlags                          0x46F840
     MarkBackgroundRowToRedraw               0x46F290
     MarkEverywhereForRedraw                 0x46FCC0
     ScrollBitVector                         0x470730
     ScrollBuffers                           0x46FD70
     SetViewPos                              0x46FD10
     UpdateDayNightPosition                  0x46FE70 */

  void MarkTileForRedraw(Location where) { return MarkTileRowForRedraw(where.y, where.x, where.x); }

public:
  ibool  drawingEnabled_;
  int    redrawBitVectorLineWidth_;  ///< Bit vector row size in bytes.    RoundUpQuotient((maxTileX_ + 2), 8)
  uint32 redrawBitVectorSize_;       ///< Total bit vector size in bytes.  (maxTileY_ + 1) * redrawBitVectorLineWidth_
  int    scrollY_;
  int    scrollX_;
  int    height_;
  int    width_;
  int    field_1C;
  int    lightUpdateTileX_;
  RECT   relativeTileRect_;
  uint32 redrawBitVector_[100];
  uint32 oldRedrawBitVector_[100];
  uint32 unknownBitVector_[100];
  uint32 lightBitVector_[100];
  uint32 oldLightBitVector_[100];
  void*  pUnknownBitVector_;
  void*  pRedrawBitVector_;
  void*  pOldRedrawBitVector_;
  void*  pLightBitVector_;
  void*  pOldLightBitVector_;
  ibool  totalBackgroundRedraw_;
  int    maxTileX_;                  ///< RoundUpQuotient(pSurface_->width_,  32) + 1
  int    maxTileY_;                  ///< RoundUpQuotient(pSurface_->height_, 32) + 1
  void*  pSurface_;
  int    oldPixelX_;
  int    oldPixelY_;
  int    pixelX_;
  int    pixelY_;
  int    tileStartPixelOffsetX_;
  int    tileStartPixelOffsetY_;
  int    tileX_;
  int    tileY_;
  int    field_848;
};
static_assert(sizeof(Viewport) == 0x84C, "Incorrect Viewport size.");

class GFXClippedSurface : public GFXCDSSurface {
  using $ = GFXClippedSurface;
public:
  void DrawLine(const POINT& start, const POINT& end, COLORREF color16)
    { return Thunk<0x41EFF0, &$::DrawLine>(start, end, color16); }

  void DrawTile(GFXBitmap* pBitmap, int pixelX, int pixelY, int lightLevel, uint8 blightMask, uint8 darknessMask)
    { return Thunk<0x4A89A0, &$::DrawTile>(pBitmap, pixelX, pixelY, lightLevel, blightMask, darknessMask); }

  static void FASTCALL DrawBorder(
    GFXClippedSurface* pSurface, RECT* pPosition, uint16 topLeftColor16, uint16 bottomRightColor16, ibool reverse)
      { return OP2Thunk<0x40B2F0, &$::DrawBorder>(pSurface, pPosition, topLeftColor16, bottomRightColor16, reverse); }

  static void FASTCALL DrawBox(GFXClippedSurface* pSurface, RECT* pPosition, uint16 color16)
    { return OP2Thunk<0x40B4D0, &$::DrawBox>(pSurface, pPosition, color16); }

  static void FASTCALL DrawHpBar(
    RECT* pPosition, GFXClippedSurface* pSurface, int currentHp, int maxHp, short barColor16, uint16 bgColor16, int a1,
    uint16 a2Color16, int a3, uint16 a4Color16)
  {
    return OP2Thunk<0x45C3A0, &$::DrawHpBar>(
      pPosition, pSurface, currentHp, maxHp, barColor16, bgColor16, a1, a2Color16, a3, a4Color16);
  }

public:
  int         field_98;
  int         field_9C;
  int         field_A0;
  int         field_A4;
  int         field_A8;
  int         field_AC;
  RECT        drawRect_;
  ibool       screenRequiresUpdate_;
  ibool       markBackgroundOnRedraw_;
  BITMAPINFO  bitmapInfo_;
  BITMAPINFO* pBitmapInfo_;
  RECT        redrawRect_;
  int         field_108;
  int         zoom_;
  Viewport    viewport_;                ///< @note The Viewport is only actively used with the detail pane's instance.
};
static_assert(sizeof(GFXClippedSurface) == 0x95C, "Incorrect GFXClippedSurface size.");


struct BitmapCopyInfo {
  void*   pSrcImg;
  void*   pDstImg;
  void*   pOverlayMask;      ///< Virmask
  int     overlayBitOffset;
  int     srcWidth;
  int     srcHeight;
  int     srcPitch;
  int     dstPitch;
  int     drawMethod;
  uint16* pDarkPal16;        ///< Current light level (uint16[256])
  int     blightOverlay;     ///< 0..15
  uint16* pLightPal16;       ///< Full daylight (uint16[256])
};

struct ScanlineCopyInfo {
  uint16* pPalette;  ///< uint16[256]
  void*   pSrcImg;
  int     field_08;
  void*   pDstImg;
  int     width;
};

} // Tethys
