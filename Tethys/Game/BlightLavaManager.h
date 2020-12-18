
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Location.h"

namespace Tethys {

class StreamIO;
class MapObject;

BEGIN_PACKED

/// Internal class managing Blight spread.  @note Many of the virtual functions are called non-virtually.
class BlightManager : public OP2Class<BlightManager> {
public:
  BlightManager()                  { InternalCtor<0x49DCD0>(); }
  BlightManager(InternalCtorChain) {                           }

  virtual void  TrySpread(Location where) { return Thunk<0x49DD60, &$::TrySpread>(where);    }
  virtual ibool Init()                    { return Thunk<0x49E250, &$::Init>();              }
  virtual void  Spread(Location    where) { return Thunk<0x49E340, &$::Spread>(where);       }

  virtual void SetOnTile(Location     where) { return Thunk<0x432310, &$::SetOnTile>(where);     }
  virtual void ToggleOnTile(Location  where) { return Thunk<0x49E470, &$::ToggleOnTile>(where);  }
  virtual void ClearFromTile(Location where) { return Thunk<0x49E750, &$::ClearFromTile>(where); }

  virtual ibool Save(StreamIO* pSavedGame) { return Thunk<0x49E8F0, &$::Save>(pSavedGame); }
  virtual ibool Load(StreamIO* pSavedGame) { return Thunk<0x49E860, &$::Load>(pSavedGame); }

  virtual void DrawOnMiniMap() { return Thunk<0x432670, &$::DrawOnMiniMap>(); }

  virtual void SetSpreadSpeed(int speed) { return Thunk<0x49E310, &$::SetSpreadSpeed>(speed); }

#define OP2_BLIGHTMANAGER_VTBL($)  \
  $(TrySpread)      $(Init)  $(Spread)  $(SetOnTile)  $(ToggleOnTile)  $(ClearFromTile)  $(Save)  $(Load)  \
  $(DrawOnMiniMap)  $(SetSpreadSpeed)
  DEFINE_VTBL_TYPE(OP2_BLIGHTMANAGER_VTBL, 0x4D6A88);

  /// Gets the global BlightManager instance.
  static BlightManager* GetInstance() { return OP2Mem<0x57B7D8, BlightManager*>(); }

public:
  int   currentPosition_;
  int   field_08;          ///< Flags? Gets set to 0 by SetSpreadSpeed, gets XOR 1'd by TrySpread, bit 2 is used for ???
  int   field_0C;
  int   spreadSpeed_;
  void* pVirMaskBuffer_;
};

/// Internal class managing lava spread.  @note Many of the virtual functions are called non-virtually.
class LavaManager : public BlightManager {
  using $ = LavaManager;
public:
  LavaManager() : BlightManager(UseInternalCtorChain) { InternalCtor<0x432390>(); }

  void  TrySpread(Location    where) override { return Thunk<0x4323E0, &$::TrySpread>(where);      }
  ibool Init()                       override { return Thunk<0x4323C0, &$::Init>();                }
  void  Spread(Location       where) override { return Thunk<0x4329B0, &$::Spread>(where);         }
  void  ToggleOnTile(Location where) override { return Thunk<0x432680, &$::ToggleOnTile>(where);   }
  void  DrawOnMiniMap()              override { return Thunk<0x432670, &$::DrawOnMiniMap>();       }
  /// Creates an Eruption.  Only used for a debug UI command.
  void  SetSpreadSpeed(int speed)    override { return Thunk<0x432EC0, &$::SetSpreadSpeed>(speed); }

  /// "Real" set lava speed function.
  void SetLavaSpeed(int speed) { return Thunk<0x432F00, &$::SetLavaSpeed>(speed); }

  /// Gets the global LavaManager instance.
  static LavaManager* GetInstance() { return OP2Mem<0x54EFE0, LavaManager*>(); }

  DEFINE_VTBL_GETTER(0x4D0360);
};

END_PACKED

} // Tethys
