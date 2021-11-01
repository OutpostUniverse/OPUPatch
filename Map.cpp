
#include "Tethys/API/GameMap.h"
#include "Tethys/Resource/GFXBitmap.h"

#include "Patcher.h"
#include "Util.h"

#include <algorithm>

using namespace Tethys;
using namespace TethysAPI;
using namespace Patcher::Util;
using namespace Patcher::Registers;

static constexpr uint32 MaxMapWidth  = 1024;
static constexpr uint32 MaxMapHeight = 512;
static_assert((MaxMapWidth <= 1024) && (MaxMapHeight <= 512),
              "Map size is limited to 1024x512 due to certain data structures storing pixel X/Y in 15/14 bits.");

static constexpr uint32 MaxLightLevels = NumLightLevels;  // ** TODO Figure out how to increase this?

// Extended map flags used in place of the "is saved game" 32-bit boolean.
union MapFlags {
  struct {
    uint32 isSavedGame      :  1;  // 0 for map file, 1 for saved game file.
    uint32 forceWorldMapOn  :  1;  // Force world map (X wrap-around).  Warning: this is currently buggy and unstable!
    uint32 forceWorldMapOff :  1;  // Force non-world map.  Only meaningful for 512xY maps; ignored for 1024xY.
    uint32 reserved         : 29;  // Reserved for future use.
  };
  uint32 u32All;
};

// =====================================================================================================================
// Double the max map size to 1024x512 by fixing a crash with loading maps of width > 512, caused by fixed-size light
// level adjust table not being large enough.
// NOTE: This patch breaks compatibility with old versions of the CCF2 blight mod.  Maps using it need to be updated.
// ** TODO Fix crash bug at 4413DE, when AI units/groups attempt to pathfind to an unreachable destination
bool SetLargeMapPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  static uint8 lightAdjustTable[MaxMapWidth * 2] = { };

  if (enable && (patcher.NumPatches() == 0)) {
    static constexpr uint32 SizeOfOldTable = 1024;
    memcpy(&lightAdjustTable[0], OP2Mem(0x54F854), SizeOfOldTable);
    patcher.ReplaceReferencesToGlobal(0x54F854, SizeOfOldTable, &lightAdjustTable[0]);

    // In MapImpl::AllocateSpaceForMap()
    patcher.LowLevelHook(0x435748, [](Esi<int> curX) {
      lightAdjustTable[curX]                        = MaxLightLevels - 1;   // Tile
      lightAdjustTable[g_mapImpl.tileWidth_ + curX] = MaxLightLevels - 10;  // Sprite
      return 0x435787;
    });
    patcher.LowLevelHook(0x435773, [](Eax<uint8> lightLevel, Esi<int> curX)
      { lightAdjustTable[curX]                        = lightLevel;  return 0x435777; });
    patcher.LowLevelHook(0x435780, [](Eax<uint8> lightLevel, Esi<int> curX)
      { lightAdjustTable[g_mapImpl.tileWidth_ + curX] = lightLevel;  return 0x435787; });

    // In Map constructor
    patcher.LowLevelHook(0x4352FF, [] { memset(&lightAdjustTable[0], 0, sizeof(lightAdjustTable)); });

    // In Map destructor
    patcher.LowLevelHook(0x435418, [] { memset(&lightAdjustTable[0], 0, sizeof(lightAdjustTable)); });

    // Hook near GfxSurface.DrawGraphic() calls that assume the 2nd light level adjust table starts at (table + 512).
    static const auto SetLightMax   = [](int& lightPos) { lightPos  = (g_mapImpl.tileWidth_ * 2); };
    static const auto SetLightStart = [](int& lightPos) { lightPos +=  g_mapImpl.tileWidth_;      };

    // In MapObject::Draw()
    patcher.LowLevelHook(0x408F6C, [](Esi<int>& lightPos) {   SetLightMax(lightPos);                   });
    patcher.LowLevelHook(0x408FC1, [](Esi<int>& lightPos) { SetLightStart(lightPos);  return 0x408FC7; });
    // In Vehicle::Draw()
    patcher.LowLevelHook(0x44CAB2, [](Ebx<int>& lightPos) {   SetLightMax(lightPos);                   });
    patcher.LowLevelHook(0x44CB16, [](Ebx<int>& lightPos) { SetLightStart(lightPos);  return 0x44CB1C; });
    // In TankVehicle::Draw()
    patcher.LowLevelHook(0x484D36, [](Edi<int>& lightPos) {   SetLightMax(lightPos);                   });
    patcher.LowLevelHook(0x484D86, [](Edi<int>& lightPos) { SetLightStart(lightPos);  return 0x484D8C; });
    // In SmallCapacityAirTransport::Draw()
    patcher.LowLevelHook(0x4746AC, [](Ebx<int>& lightPos) {   SetLightMax(lightPos);                   });
    // In Agridome::DrawDock()
    patcher.LowLevelHook(0x401A25, [](Edi<int>& lightPos) {   SetLightMax(lightPos);  return 0x401A77; });
    // In CommonStorage::DrawDock()
    patcher.LowLevelHook(0x409645, [](Esi<int>& lightPos) {   SetLightMax(lightPos);  return 0x409697; });
    // In RareStorage::DrawDock()
    patcher.LowLevelHook(0x4098F5, [](Esi<int>& lightPos) {   SetLightMax(lightPos);  return 0x409947; });
    // In Garage::DrawDock()
    patcher.LowLevelHook(0x41D08D, [](Ebx<int>& lightPos) {   SetLightMax(lightPos);  return 0x41D0E7; });
    // In GuardPost::Draw()
    patcher.LowLevelHook(0x42D473, [](Ebx<int>& lightPos) {   SetLightMax(lightPos);                   });
    // In MineBuilding::Draw()
    patcher.LowLevelHook(0x44B2F2, [](Esi<int>& lightPos) {   SetLightMax(lightPos);  return 0x44B349; });

    // Disable globe minimap view for 1024xY maps due to crash issues.
    // In Minimap::InitButtons()
    patcher.LowLevelHook(0x458B96, [](Eax<bool>& disableGlobeView)
      { disableGlobeView = (g_mapImpl.tileWidth_ != 512) || (g_mapImpl.paddingOffsetTileX_ != 0); });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Extends the bIsSavedGame field of the map header to support additional behavior flags, such as allowing world map
// wrap-around on smaller maps.
// ** TODO look into why game freezes on forced world maps when a unit tries to move, minimap bugs with 128xY,
// major rendering bugs with 64xY
bool SetCustomMapFlagsPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static MapFlags mapFlags = { };
  
    // In MapImpl::LoadMapData()
    patcher.WriteNop(0x435E1C);
    patcher.LowLevelHook(0x435E22, [](Eax<MapFlags> flags, Esp<void*> pEsp) {
      mapFlags.u32All = flags->u32All;  return (flags->isSavedGame == *PtrInc<bool*>(pEsp, 44)) ? 0x435E2A : 0x435FB1;
    });

    // In MapImpl::Save()
    patcher.LowLevelHook(0x43581F, [](Eax<MapFlags>& flags)
      { const bool isSave = flags->isSavedGame;  flags = mapFlags;  flags->isSavedGame = isSave; });

    // In MapImpl::AllocateMap()
    patcher.LowLevelHook(0x43552D, [] { return mapFlags.forceWorldMapOn  ? 0x435550 :
                                               mapFlags.forceWorldMapOff ? 0x435532 : 0; });
    
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
