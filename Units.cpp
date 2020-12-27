
#include "Tethys/API/TethysGame.h"
#include "Tethys/API/GameMap.h"
#include "Tethys/API/Player.h"

#include "Tethys/Game/MapObject.h"
#include "Tethys/Game/PathFinder.h"

#include "Tethys/UI/GameFrame.h"
#include "Tethys/UI/UICommand.h"

#include "Tethys/Resource/GFXBitmap.h"
#include "Tethys/Resource/GFXSurface.h"
#include "Tethys/Resource/SpriteManager.h"
#include "Tethys/Resource/SoundID.h"

#include "Patcher.h"
#include "Util.h"

using namespace Tethys;
using namespace Tethys::API;
using namespace Patcher::Util;
using namespace Patcher::Registers;

static constexpr uint32 MaxUnits = 2048;
static_assert(MaxUnits <= (1u << 11),            "MaxUnits cannot exceed 2048 (map encodes unit index in 11 bits).");
static_assert((MaxUnits & (MaxUnits - 1)) == 0,  "MaxUnits must be power-of-two.");

// Replacement per-player count unit limits:  1P   2P   3P   4P   5P   6P
static constexpr uint32 VehicleLimits[]   = { 400, 320, 230, 180, 154, 130 };
static constexpr uint32 StructureLimits[] = { 950, 350, 200, 150, 100, 96  };

// =====================================================================================================================
// Doubles the max unit limit from 1024 to 2048.
// ** TODO IDs 2048+ could potentially be used for MapEntity, since only MapUnit IDs are limited to 11 bits
bool SetUnitLimitPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable && (patcher.NumPatches() == 0)) {
    // In MapImpl::AllocateSpaceForMap()
    patcher.LowLevelHook(0x435660, [](Ebx<MapImpl*> pThis) {
      static constexpr size_t AllocSize = MapObjectSize * (MaxUnits + 1);
      auto*const pMapObjArray  = pThis->pMapObjArray_ = static_cast<AnyMapObj*>(OP2Alloc(AllocSize));
      pThis->ppMapObjFreeList_ = static_cast<MapObject**>(OP2Alloc(sizeof(MapObject*) * MaxUnits));

      if (pMapObjArray != nullptr) {
        memset(pMapObjArray, 0xEA, AllocSize);

        for (uint32 i = 1; i <= MaxUnits; (&pThis->pMapObjArray_[i++])->pNext_ = reinterpret_cast<MapObject*>(~0));

        pThis->pMapObjListBegin_   = &pMapObjArray[0];
        pThis->pMapObjListEnd_     = &pThis->pMapObjArray_[MaxUnits];
        (&pMapObjArray[0])->pPrev_ = &pMapObjArray[0];
      }

      return 0x4356BE;
    });

    // In MapObjectType::AllocateMapObj()
    patcher.Write<uint32>(0x439A3F, (MaxUnits - 1));  // cmp eax, 1023 => 2047 (numUnits != 2047)
    patcher.Write<uint32>(0x439AA2, (MaxUnits - 1));  // and ebx, 1023 => 2047 (curFreeUnitIndex % 2048)
    patcher.Write<uint32>(0x439ABF, (MaxUnits - 1));  // and edx, 1023 => 2047 ((curFreeUnitIndex - 1) % 2048)
    patcher.Write<uint32>(0x439B13, (MaxUnits - 1));  // cmp eax, 1023 => 2047 (map.lastUsedUnitIndex != 2047)
    patcher.LowLevelHook(0x439ADF, [](Eax<int> firstFreeUnitIndex, Edx<int>& wrappedIndex)  // Replace buggy code
                                     { wrappedIndex = ((firstFreeUnitIndex + 1) % MaxUnits);  return 0x439AE8; });
    patcher.WriteNop(0x439A4E);                       // inc map.numUnits => nop
    patcher.LowLevelHook(0x439B89, [](Esi<MapObject*> pMo)
      { ++(g_mapImpl.numUnits_);  pMo->command_ = 0;  pMo->action_ = pMo->executingAction_ = {};  return 0x439B92; });

    // In Unit vector destructor
    patcher.Write<uint32>(0x439C4A, (MaxUnits - 1));  // and eax, 1023 => 2047 (numFreeUnits % 2048)
    // In Unit scalar destructor
    patcher.Write<uint32>(0x439CBA, (MaxUnits - 1));  // and eax, 1023 => 2047 (numFreeUnits % 2048)

    static uint32 miniMapUnitCache[MaxUnits] = { };
    memcpy(&miniMapUnitCache[0], OP2Mem(0x574484), (4 * 1024));

    // In MiniMap::DrawBackground()
    patcher.LowLevelHook(0x48CCB0, [](Edx<void*>& pCache) { pCache = &miniMapUnitCache[0]; });
    // In MiniMap::DrawUnitOnMinimap()
    patcher.LowLevelHook(0x48CDA8, [](Ebx<void*>& pCache) { pCache = &miniMapUnitCache[0]; });

    static MapObject* unitDrawList[MaxUnits]   = { };
    static MapObject* entityDrawList[MaxUnits] = { };
    memcpy(&unitDrawList[0],   OP2Mem(0x57C000), sizeof(MapObjDrawList::pUnitDrawList_));
    memcpy(&entityDrawList[0], OP2Mem(0x57C7FC), sizeof(MapObjDrawList::pEntityDrawList_));

    // In MapObjDrawList::DrawUnits()
    patcher.LowLevelHook(0x49E9BF, [](Ebx<void*>& pList) { pList = &unitDrawList[0];   });
    patcher.LowLevelHook(0x49E9F3, [](Ebx<void*>& pList) { pList = &entityDrawList[0]; });
    patcher.LowLevelHook(0x49EA15, [](Ebx<void*>& pList, Eax<int> count)
      { pList = &unitDrawList[0];  return (count > 0) ? 0x49EA1C : 0x49EA2F; });
    patcher.LowLevelHook(0x49EA55, [](Edi<void*>& pList) { pList = &entityDrawList[0]; });
    patcher.LowLevelHook(0x49EA5C, [](Ecx<void*>  pUnit) { return (pUnit == nullptr) ? 0x49EA69 : 0; });  // Workaround
    patcher.LowLevelHook(0x49EA81, [](Ebx<void*>& pList) { pList = &entityDrawList[0]; });
    // In MapObjDrawList::?1()
    patcher.LowLevelHook(0x49EAC9, [](Esi<void*>& pList) { pList = &unitDrawList[0];   });
    // In MapObjDrawList::MarkUnitsForRedraw()
    patcher.LowLevelHook(0x49EC90, [](Esi<MapObjDrawList*> pThis, Ecx<void*>& pList)
      { pList = &unitDrawList[0];  return (pThis->numUnits_ > 0) ? 0x49EC98 : 0x49ECBE; });
    patcher.LowLevelHook(0x49ECC7, [](Edi<void*>& pList, Eax<int> count)
      { pList = &unitDrawList[0];  return (count > 0) ? 0x49ECCE : 0x49ED10; });

    // Reimplement MapObjectDrawList::BuildDrawLists()
    patcher.HookCall(0x407EC7, ThiscallLambdaPtr([](MapObjDrawList* pThis) {
      if (Viewport*const pViewport = pThis->pViewport_;  pViewport != nullptr) {
        pThis->numUnits_ = pThis->numEntities_ = 0;
        for (auto* pMo = g_mapImpl.pMapObjListBegin_->pNext_;  pMo->pNext_ != pMo;  pMo = pMo->pNext_) {
          if (const bool isEntity = (pMo->flags_ & (MoFlagEntity | MoFlagEntChild));  pMo->IsVisible(pViewport)) {
            (isEntity ? entityDrawList[pThis->numEntities_++] : unitDrawList[pThis->numUnits_++]) = pMo;
          }
        }

        g_gameFrame.iniSettings_.showShadows = g_gameFrame.iniSettings_.showMobileShadows;
        for (uint32 i = 0; i < pThis->numUnits_; ++i) {
          if (unitDrawList[i]->flags_ & (MoFlagVehicle | MoFlagBldCmdCenterConnected | MoFlagSpecialDraw)) {
            unitDrawList[i]->MarkForRedraw(pViewport);
          }
        }

        if (g_gameImpl.daylightEverywhere_ == false) {
          memset(pViewport->pLightBitVector_, 0, pViewport->redrawBitVectorSize_);
          ++(pViewport->maxTileX_);
          ++(pViewport->maxTileY_);
          for (uint32 i = 0;  i < pThis->numUnits_;  unitDrawList[i++]->LightUpVisibleRange(pViewport));
          --(pViewport->maxTileX_);
          --(pViewport->maxTileY_);
        }
      }
    }));

    // In MapImpl::Save()
    patcher.Write<uint32>(0x435A7E, MaxUnits);        // mov  ecx, 1024 => 2048 (freeUnitListNumElements)
    patcher.Write<uint32>(0x435AA4, (MaxUnits * 4));  // push 4096      => 8192 (freeUnitListSizeInBytes)
    patcher.Write<uint32>(0x435AAE, MaxUnits);        // mov  eax, 1024 => 2048 (freeUnitListNumElements)
    // In MapImpl::SaveUnits() and related functions
    patcher.Write<uint32>(0x435BDA, (MaxUnits       * MapObjectSize));  // cmp  ebx, (1024 * 120) => (2048 * 120)
    patcher.Write<uint32>(0x435BEC, ((MaxUnits - 1) * MapObjectSize));  // push      (1023 * 120) => (2047 * 120)
    patcher.Write<uint32>(0x435C75, ((MaxUnits - 1) * MapObjectSize));  // push      (1023 * 120) => (2047 * 120)
    patcher.Write<uint32>(0x435D52, (MaxUnits       * MapObjectSize));  // cmp  edi, (1024 * 120) => (2048 * 120)
    patcher.Write<uint32>(0x435D64, MaxUnits);                          // mov  esi, 1024         => 2048
    patcher.Write<uint32>(0x435D96, MaxUnits);                          // mov  edx, 1024         => 2048

    // In MapImpl::Load()
    patcher.Write<uint32>(0x436206, ((MaxUnits - 1) * MapObjectSize));  // push (1023 * 120) => (2047 * 120)
    patcher.Write<uint32>(0x43626C, (MaxUnits * 4));                    // push 4096         => 8192
    patcher.Write<uint32>(0x436278, MaxUnits);                          // mov  eax, 1024    => 2048

    // Replace per-player count unit limit tables
    patcher.Write(0x4E9908, VehicleLimits);
    patcher.Write(0x4E9920, StructureLimits);

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Extends the size of the unit type array from 115 to 255.
// ** TODO also extend factory build lists, etc?
bool SetUnitTypeLimitPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  constexpr size_t MaxUnitTypes         = 255;
  constexpr uint32 EndOfBuildListMarker = 0xFFFFFF9C;

  static MapObjectType* pMoTypeArray[MaxUnitTypes + 1] = { };
  auto*const pOldMoTypeArray = OP2Mem<0x4E1348, MapObjectType**>();

  class DummyObjectType : public MapObjType::MaxObjectType {
  public:
    ibool Save(StreamIO* pSavedGame) override { return 1; }
    ibool Load(StreamIO* pSavedGame) override { return 1; }
  } static dummyMapObjType(*static_cast<DummyObjectType*>(pOldMoTypeArray[MapID::MaxObject]));

  if (enable) {
    // Initialize our extended map object type array.
    memcpy(&pMoTypeArray[0], pOldMoTypeArray, sizeof(MapObjectType*[MapID::MaxObject]));
    for (size_t i = MapID::MaxObject; i <= MaxUnitTypes; pMoTypeArray[i++] = &dummyMapObjType);

    // Replace the map object type array.
    patcher.ReplaceReferencesToGlobal(&pOldMoTypeArray[0], sizeof(MapObjectType*[MapID::MaxObject]), &pMoTypeArray[0]);
    patcher.ReplaceReferencesToGlobal(&pOldMoTypeArray[MapID::MaxObject], &pMoTypeArray[MaxUnitTypes + 1]);

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Prevent recursion depth crashes from Thor's Hammer rendering.
bool SetDrawLightningFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  static uint32 numLightningCalls = 0;

  if (enable) {
    // In ThorsHammer::DrawUnit() (calls DrawLightning())
    // ** TODO debug the case where this happens more closely, this should never be called with nullptr
    patcher.Hook(0x48AE40, SetCapturedTrampoline, ThiscallFunctor(
      [F = decltype(MapObject::VtblType::pfnDraw){}](MapObject* pThis, Viewport* pV) { if (pThis) { F(pThis, pV); } }));

    patcher.LowLevelHook(0x48AFF5, [](Esi<MapObj::ThorsHammer*> pThis) {
      auto*const pSrc   = MapObject::GetInstance(pThis->parentIndex_);
      const int  player = (pSrc != nullptr) ? pSrc->creatorNum_ : 6;
      auto*const pType  = pThis->GetType();
      // Recursion depth is 1 call per tile, which should never exceed weapon target range + scatter range.
      numLightningCalls = pType->playerStats_[player].sightRange + (pType->stats_.weapon.pixelsSkipped / 32) + 1;
    });

    // In ThorsHammer::DrawLightning() (recursive function)
    // ** TODO debug the case where infinite recursion happens more closely, possibly similar to issue where lasers
    // draw across the map
    patcher.LowLevelHook(0x48ACEB, [] { return (--numLightningCalls > 0) ? 0 : 0x48ACF8; });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes crash when units are transferred to gaia (P6).
bool SetTransferUnitToGaiaFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable && (patcher.NumPatches() == 0)) {
    // In MissionManager::RaiseUnitTransferredEvent()
    patcher.LowLevelHook(0x4031D6, [](Eax<int> dstPlayerId) { return (dstPlayerId >= 6) ? 0x4031F2 : 0; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes various issues with EMP Missiles and Meteor Defenses.
bool SetMissileFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    static constexpr uint32 NewRocketAnimsEden[] = {
      // Missile launch,  SULV launch,  RLV launch,  Missile land,
      1868,               1868,         1871,        1873,          0,
      // RLV land,        Missile fly,  SULV fly,    RLV fly,
      1864,               1872,         1872,        1874,          0
    };

    // Fix EMP Missiles not landing for Eden, caused by missing animation index table entries for missiles for Eden.
    patcher.Write(0x4E3F88, NewRocketAnimsEden);

    // In EMPMissile::DoEvent()
    // Fix an issue where, if an EMP Missile is targeted directly on top of any Meteor Defense, then all other Meteor
    // Defenses in range would always ignore it.
    patcher.Write<uint8>(0x48026B, 0x8F);  // 0x8D

    // Prevent Meteor Defenses from shooting down allied players' missiles.
    patcher.LowLevelHook(0x4802A2, [](Eax<int> owner, Ecx<int> target)
      { return ((owner != 6) && Player[owner].IsAlly(target)) ? 0x480414 : 0; });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Allow Unit::DoInfect() to work on maps with more than 1 player (human or AI).
bool SetDoInfectFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    success = (patcher.WriteNop(0x476BD1) == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes bugs where invisible perma-walls are left behind if an Earthworker dies while building a wall, and where you
// can't issue build wall commands in a 3x3 area around another wall being built, lava, or blight.
bool SetBuildWallFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Fix the "cannot build walls in a 3x3 area around another wall being built" issue.
    // In CheckCanBuildTubeOrWall()
    patcher.WriteNop(0x438E07);

    // Don't set the tile.isWall flag until the Earthworker has finished building the wall.
    // In Unit::ProcessForGameCycle()
    patcher.WriteNop(0x43BD2F);

    // If a unit is on top of the wall tile when it's supposed to finish, cancel and refund (similar to tube behavior).
    // In SetWallOrTubeTile()
    patcher.WriteNop(0x42374B);
    patcher.LowLevelHook(0x423762, [](Esi<MapObject*> pUnit, Eax<bool> isTileClear) {
      const auto t = pUnit->tubeOrWallType_;
      return (t == MapID::Tube) ? nullptr : isTileClear ? OP2Mem<void**>(0x423884)[t - MapID::Tube] : OP2Mem(0x423766);
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes a bug where, if wreckage is placed on certain terrain types, cargo trucks would pick it up as rubble.
// ** TODO Reimplement this with C++ hooks
bool SetWreckageFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In Unit::ProcessForGameCycle()
    patcher.Write<uint8>(0x43C4F9, 0x36);  // 0x4C
    patcher.Write<uint8>(0x43C531, 0x72);  // 0x14
    patcher.Write<uint8>(0x43C545, 0x10);  // 0x08
    // 0x8B, 0x4C, 0x24, 0x54, 0x8B, 0x5C, 0x24, 0x54, 0x85, 0xED, 0x75, 0x04, 0x33, 0xDB, 0x33, 0xC9
    patcher.WriteNop(0x43C546, 16);

    // Replace TerrainManager.TileHasCommonRubble()/RareRubble()
    patcher.WriteBytes(0x48BC50, {
      // TileHasCommonRubble(int mappingIndex)
      0x8B, 0x54, 0x24, 0x04,        // mov   edx, DWORD PTR [esp+0x4]
      0xE8, 0x1F, 0x00, 0x00, 0x00,  // call  0x28 (0x48BC78)
      0x09, 0xC0,                    // or    eax, eax
      0x74, 0x15,                    // jz    0x22 (0x48BC72)
      0x8B, 0x4C, 0x24, 0x04,        // mov   ecx, DWORD PTR [esp+0x4]
      0x0F, 0xBF, 0x50, 0x06,        // movsx edx, WORD  PTR [eax+0x6]
      0x33, 0xC0,                    // xor   eax, eax
      0x3B, 0xCA,                    // cmp   ecx, edx
      0x7C, 0x07,                    // jl    0x22 (0x48BC72)
      0x83, 0xC2, 0x04,              // add   edx, 0x4
      0x3B, 0xD1,                    // cmp   edx, ecx
      0x7F, 0x03,                    // jg    0x25 (0x48BC75)
      0xC2, 0x04, 0x00,              // retn  0x4
      0x40,                          // inc   eax
      0xEB, 0xFA,                    // jmp   0x22 (0x48BC72)

      // TileHasRubbleHelper()
      0x8B, 0x41, 0x0C,              // mov  eax, DWORD PTR [ecx+0xC]
      0x09, 0xC0,                    // or   eax, eax
      0x7E, 0x1A,                    // jle  0x49 (0x48BC99)
      0x50,                          // push eax
      0x83, 0xC1, 0x10,              // add  ecx, 0x10
      0x8B, 0x01,                    // mov  eax, DWORD PTR [ecx]
      0x66, 0x3B, 0x10,              // cmp  dx,  WORD  PTR [eax]
      0x7C, 0x06,                    // jl   0x8 (0x48BC90)
      0x66, 0x3B, 0x50, 0x02,        // cmp  dx, [eax+0x2]
      0x7E, 0x0C,                    // jle  0x14 (0x48BC9C)
      0x83, 0xC1, 0x04,              // add  eax, 0x4
      0xFF, 0x0C, 0x24,              // dec  DWORD PTR [esp]
      0x75, 0xEB,                    // jnz  -0x5 (0x48BC83)
      0x58,                          // pop  eax
      0x33, 0xC0,                    // xor  eax, eax
      0xC3,                          // retn
      0x59,                          // pop  ecx
      0xC3,                          // retn

      // TileHasRareRubble(int mappingIndex)
      0x90, 0x90,                    // nop
      0x8B, 0x54, 0x24, 0x04,        // mov   edx, [esp+0x4]
      0xE8, 0xCF, 0xFF, 0xFF, 0xFF,  // call  -0x10 (0x48BC78)
      0x09, 0xC0,                    // or    eax, eax
      0x74, 0x18,                    // jz    0x3D (0x48BCC5)
      0x0F, 0xBF, 0x50, 0x06,        // movsx edx, WORD  PTR [eax+0x6]
      0x8B, 0x4C, 0x24, 0x04,        // mov   ecx, DWORD PTR [esp+0x4]
      0x8D, 0x52, 0x04,              // lea   edx, [edx+0x4]
      0x3B, 0xCA,                    // cmp   ecx, edx
      0x7C, 0x07,                    // jl    0x3B (0x48BCC3)
      0x8D, 0x52, 0x04,              // lea   edx, [edx+0x4]
      0x3B, 0xD1,                    // cmp   edx, ecx
      0x7F, 0x05,                    // jg    0x40 (0x48BCC8)
      0x33, 0xC0,                    // xor   eax, eax
      0xC2, 0x04, 0x00,              // retn  0x4
      0xB8, 0x01, 0x00, 0x00, 0x00,  // mov   eax, 0x1
      0xC2, 0x04, 0x00,              // retn  0x4

      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // nop
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Workaround PathContext memory leaks caused by ScGroup::TakeUnit() (any others?).
// ** TODO Properly fix the leak instead of working around it
bool SetPathContextLeakFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Allocate 10x more space for PathContexts
    // In PathContextList::Init(), PathContextList::Load()
    patcher.Write<uint32>(0x446B07, sizeof(PathContext[20000]));
    patcher.Write<uint32>(0x446D24, sizeof(PathContext[20000]));
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Change structure docks vehicle damage mechanic so that allied vehicles are ignored.
// The old behavior can be forced on specific structures by calling CreateSpecialTarget with them.
// ** TODO Should figure out how to allow player repairing of allied AI structures to go along with this
bool SetNoAlliedDockDamageFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Replace Building::DoEvent()
    patcher.Hook(0x482D90, SetCapturedTrampoline, ThiscallFunctor(
      [F = decltype(Building::VtblType::pfnDoEvent){}](Building* pThis) {
        const Unit bld      = Unit(pThis);
        const Unit dockUnit = bld.GetUnitOnDock();

        if (dockUnit.IsVehicle() &&
            ((Player[pThis->ownerNum_].IsAlliedTo(dockUnit.GetOwner()) == false) || bld.HasFlag(MoFlagSpecialTarget)))
        {
          F(pThis);
        }
      }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Mining beacons will be marked as surveyed by an ally's EDWARD Satellite and not just the player's own.
bool SetAllyEdwardSurveyMinesPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Replace MiningMeacon::IsBeaconSurveyed() vftable entry with our function
    static auto*const pfnOldIsBeaconSurveyed = MapObj::MiningBeacon::Vtbl()->pfnIsBeaconSurveyed;
    patcher.Write(&MapObj::MiningBeacon::Vtbl()->pfnIsBeaconSurveyed, ThiscallLambdaPtr(
      [](MapObj::MiningBeacon* pThis, int playerNum) {
        ibool result = pfnOldIsBeaconSurveyed(pThis, playerNum);

        if (result == false) {
          uint32 mask = Player[playerNum].GetImpl()->alliedBy_;
          for (uint32 i = 0; TethysUtil::GetNextBit(&i, mask); mask &= ~(1u << i)) {
            if ((i != playerNum) && (Player[i].GetImpl()->GetSatelliteCount(MapID::EDWARDSatellite) != 0)) {
              result = true;
              break;
            }
          }
        }

        return result;
      }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Allows multiple selected ConVecs or Repair Vehicles to be given repair commands (like Spiders can).
bool SetMultipleRepairPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In UICmd::CommandRepair::IsEnabled()
    patcher.LowLevelHook(0x454952, [](Eax<MapID> t)
      { return ((t == MapID::ConVec) || (t == MapID::RepairVehicle) || (t == MapID::Spider)) ? 0x454969 : 0x454961; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Allows trucks with ore in them to set ore routes.
// ** TODO See if trucks with ore can go to the smelter first, or if mines can ignore already loaded trucks instead of
// them getting stuck on the dock;  remove "set path from smelter back to mine" step;  make it so EMP doesn't reset
bool SetOreRoutePatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In UICmd::CommandSetOreRoute::IsEnabled()
    patcher.LowLevelHook(0x453FFE, [](Esi<MapObj::CargoTruck*> pThis, Edi<ibool>& isEnabled) {
      const auto c      = CargoType(pThis->truckCargoType_);
      const bool result = (c == CargoType::Empty) || (c == CargoType::CommonOre) || (c == CargoType::RareOre);
      return result ? 0x454007 : 0x454005;
    });

    // In UICmd::CommandSetOreRoute::GetMouseCursor()
    patcher.LowLevelHook(0x4544B6, [](Edi<MapID> mine, Esp<void*> pEsp) {
      CargoType cargo  = CargoType::Empty;
      bool       result = (mine == MapID::CommonOreMine) || (mine == MapID::RareOreMine) || (mine == MapID::MagmaWell);

      if (result) {
        auto*const pSelection = UnitGroup::GetSelectedUnitGroup();

        for (int i = 0; i < pSelection->numUnits_; ++i) {
          const CargoType curCargo = Unit(pSelection->unitIndex_[i]).GetTruckCargo();
          if (curCargo != CargoType::Empty) {
            if ((cargo != CargoType::Empty) && (curCargo != cargo)) {
              result = false;
              break;
            }
            cargo = curCargo;
          }
        }
      }

      result &= (cargo == CargoType::Empty)                                         ||
               ((cargo == CargoType::CommonOre) &&  (mine == MapID::CommonOreMine)) ||
               ((cargo == CargoType::RareOre)   && ((mine == MapID::RareOreMine)    || (mine == MapID::MagmaWell)));

      return result ? 0x45453A : 0x4544C5;
    });
    
    // In PlayerImpl::ProcessCommandPacket()
    // ** TODO This is a temporary workaround to trucks with ore getting stuck on the mine's dock
    patcher.LowLevelHook(0x40E3B0, [](Ebp<PlayerImpl*> pThis, Edi<CommandPacket*> pPacket) {
      if (pPacket->type == CommandType::CargoRoute) {
        auto*const pUnitIDs = pThis->GetPacketUnitIDs(&pPacket->data);
        const auto count    = pThis->GetPacketUnitCount(&pPacket->data);
        for (int i = 0; i < count; ++i) {
          Unit truck(pUnitIDs[i]);
          if ((truck.GetType() == MapID::CargoTruck) && (truck.GetTruckCargo() != CargoType::Empty)) {
            truck.SetCargo(CargoType::Empty, 0);
            TethysGame::AddMapSound(SoundID::Dump, truck.GetLocation());
          }
        }
      }
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Draws single or dual-turret graphics on tank units based on their having the double fire rate flag.
bool SetTurretAnimationPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Replace {Lynx,Panther,Tiger}.SelectTurretGraphic() vftable entries with our function
    for (auto* pVtbl : { MapObj::Lynx::Vtbl(), MapObj::Panther::Vtbl(), MapObj::Tiger::Vtbl() }) {
      patcher.Write(&pVtbl->pfnSelectTurretGraphic, ThiscallLambdaPtr(
        [](TankVehicle* pThis, MapChildEntity* pWeaponUnit, int rotation) {
          // Select the weapon type animation index.
          auto*const pWeaponType = MapEntityType::GetInstance(pThis->weapon_);
          (pThis->flags_ & MoFlagDoubleFireRate) ? pWeaponType->SelectTigerGraphic(pThis, rotation)   :
          (pThis->GetTypeID() != MapID::Lynx)    ? pWeaponType->SelectPantherGraphic(pThis, rotation)
                                                 : pWeaponType->SelectLynxGraphic(pThis, rotation);

          const int rotationIndex           = (pThis->rotation_ + 8) / 16; // Round to nearest multiple of 16 (22.5 deg)
          const int animationIndex          = pThis->GetChassisAnimationIndex(rotationIndex);
          const FrameOptionalInfo frameInfo = SpriteManager::GetInstance()->GetFrameOptionalInfo(animationIndex, 0);

          // Update the selected turret animation global variable with the offsets.
          OP2Mem<0x4EA7BC, int&>() += frameInfo.offsetX;
          OP2Mem<0x4EA7C0, int&>() += frameInfo.offsetY;
        }));
    }

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Allows trucks to be partially loaded with metals/food/etc. instead of requiring it to be exactly == cargoCapacity.
bool SetTruckLoadPartialCargoPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Remove (truckCargoCapacity >= stored) checks.
    // In CommandLoadCargo::IsEnabled()
    patcher.Hook(0x457096, 0x4570A9);

    // If storage < truck capacity, load whatever is in storage.
    patcher.Hook(0x424090, ThiscallLambdaPtr([](Building* pThis) {
      if (Unit truck = Unit(pThis).GetUnitOnDock();  truck.IsValid()) {
        const int capacity = truck.GetMapObjectType()->playerStats_[truck.GetCreator()].vehicle.cargoCapacity;

        int  amount = 0;
        auto type   = CargoType::Empty;

        switch (pThis->GetTypeID()) {
        case MapID::CommonStorage:
        case MapID::CommonOreSmelter:
          amount = min(capacity, Player[truck.GetOwner()].GetCommonOre());
          type   = CargoType::CommonMetal;
          break;

        case MapID::RareStorage:
        case MapID::RareOreSmelter:
          amount = min(capacity, Player[truck.GetOwner()].GetRareOre());
          type   = CargoType::RareMetal;
          break;

        case MapID::Agridome:
          amount = min(capacity, Player[truck.GetOwner()].GetFoodStored());
          type   = CargoType::Food;
          break;

        default:
          break;
        }

        if (amount != 0) {
          truck.GetMapObject<MapObj::CargoTruck>()->SetCargoToLoad(type, amount, false);
        }
      }
    }));

    // In CargoTruck::TransferCargo()
    // Common storage, common ore smelter
    patcher.LowLevelHook(0x406833, [](Esi<MapObj::CargoTruck*> pThis) {
      if (pThis->truckCargoAmount_ = Player[pThis->ownerNum_].GetCommonOre();  pThis->truckCargoAmount_ == 0) {
        pThis->truckCargoType_ = 0;
      }
      return 0x406839;
    });
    patcher.LowLevelHook(0x406A9E, [](Esi<MapObj::CargoTruck*> pThis) {
      if (pThis->truckCargoAmount_ = Player[pThis->ownerNum_].GetCommonOre();  pThis->truckCargoAmount_ == 0) {
        pThis->truckCargoType_ = 0;
      }
      return 0x406AA4;
    });
    // Rare storage, rare ore smelter
    patcher.LowLevelHook(0x4068ED, [](Esi<MapObj::CargoTruck*> pThis) {
      if (pThis->truckCargoAmount_ = Player[pThis->ownerNum_].GetRareOre();    pThis->truckCargoAmount_ == 0) {
        pThis->truckCargoType_ = 0;
      }
      return 0x4068F3;
    });
    patcher.LowLevelHook(0x406B9A, [](Esi<MapObj::CargoTruck*> pThis) {
      if (pThis->truckCargoAmount_ = Player[pThis->ownerNum_].GetRareOre();    pThis->truckCargoAmount_ == 0) {
        pThis->truckCargoType_ = 0;
      }
      return 0x406BA0;
    });
    // Agridome
    patcher.LowLevelHook(0x4069DC, [](Esi<MapObj::CargoTruck*> pThis, Eax<int>& amount) {
      amount = min(pThis->GetType()->playerStats_[pThis->creatorNum_].vehicle.cargoCapacity,
                   Player[pThis->ownerNum_].GetFoodStored());
      return (amount != 0) ? 0x4069FA : 0x406A24;
    });

    // Special case CountTriggers that check for truck cargo - every 1000 units of cargo counts as 1 unit.
    // In Sheet::LoadSheetFiles()
    static int defaultTruckCapacity = 1000;
    patcher.LowLevelHook(0x446042, []
      { defaultTruckCapacity = MapObjType::CargoTruck::GetInstance()->playerStats_[0].vehicle.cargoCapacity; });

    // In CountTrigger::HasFired()
    patcher.LowLevelHook(0x493D5C, [](Eax<int> unitID, Esi<CargoType> cargo, Ebx<int>& counter) {
      if (auto* pTruck = MapObject::GetInstance(unitID);  CargoType(pTruck->truckCargoType_) == cargo) {
        counter += ((cargo > CargoType::Empty) || (cargo < CargoType::Spacecraft)) ? pTruck->truckCargoAmount_ : 1;
      }
      return 0x493D87;
    });
    patcher.LowLevelHook(0x493E14, [](Edi<MapObject*> pTruckInGarage, Ecx<CargoType> cargo, Ebx<int>& counter) {
      if (CargoType(pTruckInGarage->truckCargoType_) == cargo) {
        counter +=
          ((cargo > CargoType::Empty) || (cargo < CargoType::Spacecraft)) ? pTruckInGarage->truckCargoAmount_ : 1;
      }
      return 0x493E2A;
    });
    for (uintptr loc : { 0x493E50, 0x493FE5 }) {
      patcher.LowLevelHook(loc, [](Ebp<TriggerImpl*> pThis, Ebx<int>& counter) {
        const auto type      = *static_cast<MapID*>(PtrInc(pThis, 0x30));       // ** TODO Define CountTrigger
        const auto cargoType = *static_cast<CargoType*>(PtrInc(pThis, 0x34));
        if ((type == MapID::CargoTruck) && (cargoType > CargoType::Empty) && (cargoType < CargoType::Spacecraft)) {
          counter /= defaultTruckCapacity;
        }
      });
    }

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
