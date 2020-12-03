
#pragma once

#include "Tethys/API/Location.h"
#include "Tethys/UI/IWnd.h"
#include "Tethys/UI/UIElement.h"
#include "Tethys/Resource/GFXPalette.h"

namespace Tethys {

class GFXClippedSurface;
class GFXBitmap;

class MiniMap : public OP2Class<MiniMap> {
public:
   MiniMap() { InternalCtor<0x48EC20>(); }
  ~MiniMap() { Thunk<0x48EB20>();        }

  void SetSurface(GFXClippedSurface* pSurface) { return Thunk<0x48C9B0, &$::SetSurface>(pSurface); }

  void DrawBackground() { return Thunk<0x48CC80, &$::DrawBackground>(); }
  void DrawUnits()      { return Thunk<0x48CDA0, &$::DrawUnits>();      }

  void SetViewRect(RECT rect, ibool update) { return Thunk<0x48D320, &$::SetViewRect>(rect, update); }

  ibool TranslateCoordinates(POINT point, POINT* pMapPixelLocation)
    { return Thunk<0x48D930, &$::TranslateCoordinates>(point, pMapPixelLocation); }

  void ZoomIn()  { return Thunk<0x48E730, &$::ZoomIn>();  }
  void ZoomOut() { return Thunk<0x48E7F0, &$::ZoomOut>(); }

  void ToggleInvertColors() { return Thunk<0x48EA30, &$::ToggleInvertColors>(); }
  void ToggleGlobeView()    { return Thunk<0x48EAB0, &$::ToggleGlobeView>();    }

  void SetColor(Location where, Rgb555 color) { return Thunk<0x48D2B0, &$::SetColor>(where, color); }
  void SetTile(Location  where, Rgb555 color) { return Thunk<0x48EEE0, &$::SetTile>(where,  color); }

  // ** TODO more member functions
  /* MiniMap.???()                0x48C9C0 
     MiniMap.???2                 0x48CC10 
     MiniMap.???3                 0x48CD40 
     MiniMap.???                  0x48D380 
     MiniMap.???(???)             0x48DBF0 
     MiniMap.???(???):???         0x48E160 
     MiniMap.???4                 0x48E460 
     MiniMap.???5                 0x48E670 
     MiniMap.???6                 0x48E8C0 
     MiniMap.???8                 0x48ECA0 
     MiniMap.???9                 0x48ECD0 
     MiniMap.Get???()             0x48ED10 
     MiniMap.Get???               0x48ED20 
     MiniMap.???10                0x48EF40 */

  /// Gets the global MiniMap instance.
  static MiniMap* GetInstance() { return OP2Mem<0x574458, MiniMap*>(); }

public:
  int                field_00[11];
  uint16*            pUpdatedPixel_[1023];    ///< Address of pixel overwritten with a unit dot
  int                numUpdatedPixels_;       ///< Number of unit dots drawn on minimap
  RECT               viewRectNew_;
  RECT               viewRectOld_;
  ibool              invertMapColors_;
  int                maxZoomOut_;             ///< Min value for zoom
  int                zoom_;                   ///< maxZoomOut .. 5
  int                field_1058[2];
  uint16             playerColor_[8];
  int                field_1070[4];
  GFXClippedSurface* pSurface_;
  uint32             flags_;
  ibool              globeView_;
  POINT              miniMapClickPos_;
  POINT              miniMapScrollPos_;
  int                field_109C;              ///< Day/Night related?
  GFXBitmap*         pMiniMapBackgroud_;      ///< Cached terrain map
  GFXBitmap*         pSurfaceBackBuffer_;     ///< Terrain with unit overlay (dots)
  GFXClippedSurface* pSurface2_;              ///< Duplicate of pSurface_??
  uint16**           ppBrightness_;           ///< Maybe?  [uint16[tileWidth*2]*]
  int                field_10B0;
  // ** TODO more fields?
};

inline auto& g_miniMap = *MiniMap::GetInstance();


class MiniMapButton : public UIGraphicalButton {
  using $ = MiniMapButton;
public:
  // ** TODO
  void OnClick() override;
};


class MiniMapPane : public TPane {
public:
  void* Destroy(ibool freeMem = 0) override; // virtual ~MiniMapPane()

  LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)    override;
  int     CallFilters(uint32, uint32, long)                   override;
  int     OnShow(int zoom)                                    override;
  void    OnIdle()                                            override;
  int     GetAbsolutePos(int pixelX, int pixelY, POINT* pPos) override;

  virtual int GetViewportRelativePos(int pixelX, int pixelY, POINT* pPos);
  virtual int GetViewportTilePositionAndSize(POINT* pTopLeftTilePos, POINT* pTileSize);
  // ** TODO

public:
  int           field_6C;
  int           field_70;
  int           field_74;
  int           field_78;
  int           field_7C;
  MiniMapButton buttons_[4];
  int           buttonPosition_;
};

static_assert(sizeof(MiniMapPane) == 0x314, "Incorrect MiniMapPane size.");

} // Tethys
