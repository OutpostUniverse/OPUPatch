
#include <algorithm>

#include "Patcher.h"
#include "Util.h"

#include "Tethys/Game/ScBase.h"

using namespace Patcher;

// ** TODO: * Finish extended array.  Needs to work with saved games (it might work as is?)
//          * Fix the DestWaypoints leak during ScGroup::TakeUnit() (causes the "acid trip" bug)
//          * Implement extended trigger functionality, mainly exposing FunctionReference more directly, and passing
//            trigger info to the callback function
#define SCSTUB_USE_EXTENDED_ARRAY 0

#if SCSTUB_USE_EXTENDED_ARRAY
class ScStubListEx : public ScStubList {
public:
  ScStubListEx() : ScStubList(), pScStubArrayExt_(), lastCreatedGroupIndex_(NilIndex) { }
  static constexpr size_t MaxNumScStubs = 4096;

public:
  ScBase* pScStubArrayExt_[MaxNumScStubs - ScStubList::MaxNumScStubs]; // Note: access using pScStubArray_ or operator[]
  uint32  lastCreatedGroupIndex_;
};

static ScStubListEx g_scStubListEx;
#endif

// =====================================================================================================================
// Replacement member function for ScStubFactory::Create.
static ScBase* __fastcall ScStubFactory_Create(
  ScStubFactory*  pThis)
{
  auto*const  pAllocation = OP2Alloc(pThis->elementSizeInBytes_);
  ScBase*     pScStub     = (pAllocation != nullptr) ? pThis->pfnCreateStub_(pAllocation) : nullptr;

  if (pScStub != nullptr) {
    pScStub->Init();

#if SCSTUB_USE_EXTENDED_ARRAY
    // Test if this is a Group, i.e. used by units.
    bool isGroup = true;
    for (auto* pCurType = pThis; pCurType != nullptr; pCurType = pCurType->pParent_) {
      if ((strcmp(pCurType->pName_, "Group") == 0) || (strcmp(pCurType->pName_, "Strategy") == 0)) {
        isGroup = true;
        break;
      }
    }

    auto& stubList = g_scStubListEx;

    // Units store ScStub ID in a uint8, and therefore can only use IDs 0-254 (255 = sentinel value for no group).
    auto*const   pLastCreated = isGroup ? &stubList.lastCreatedGroupIndex_ : &stubList.lastCreatedIndex_;
    const uint32 end          = isGroup ?  stubList.NilIndex               :  stubList.MaxNumScStubs;
    const uint32 start        = isGroup ?  0                               :  stubList.NilIndex + 1;
#else
    auto& stubList = *ScStubList::GetInstance();

    auto*const       pLastCreated = &stubList.lastCreatedIndex_;
    constexpr uint32 end          =  stubList.MaxNumScStubs;
    constexpr uint32 start        =  0;
#endif

    const auto Inc = [start, end](uint32& index) -> uint32& { return (index = (std::max)((index + 1) % end, start)); };

    uint32 index = *pLastCreated;
    for (const uint32 lastCreated = index; stubList[Inc(index)] != nullptr;) {
      if (index == lastCreated) {
        // ScStub array is full.
        pScStub->index_ = stubList.NilIndex;
        pScStub->Destroy(true);
        pScStub = nullptr;
        break;
      }
    }

    if (pScStub != nullptr) {
      pScStub->index_ = *pLastCreated = index;
      stubList[index] = pScStub;
    }
  }

  return pScStub;
}

// =====================================================================================================================
bool SetScStubPatch(
  bool enable)
{
  static PatchContext patcher;
  bool success = true;

  static auto*const pOldScStubArray = OP2Mem<ScStubList*>(0x56C358);

  if (enable && (patcher.NumPatches() == 0)) {
#if SCSTUB_USE_EXTENDED_ARRAY
    // Replace the ScStub array with a larger one.
    memcpy(&g_scStubListEx, pOldScStubArray, sizeof(*pOldScStubArray));
    patcher.ReplaceReferencesToGlobal(pOldScStubArray, &g_scStubListEx);

    // In ScStubList::Init()
    patcher.LowLevelHook(0x47AAC3, [](Ecx<ScStubListEx*> pThis) {
      const size_t count = (pThis == &g_scStubListEx) ? pThis->MaxNumScStubs : ScStubList::MaxNumScStubs;
      memset(&(*pThis)[0], 0, count * sizeof((*pThis)[0]));
      return 0x47AACA;
    });

    // In ScStubList::Load()
    patcher.LowLevelHook(0x47ADF6, [](Edi<ScStubListEx*> pThis) {
      const size_t count = (pThis == &g_scStubListEx) ? pThis->MaxNumScStubs : ScStubList::MaxNumScStubs;
      memset(&(*pThis)[0], 0, count * sizeof((*pThis)[0]));
      return 0x47AE00;
    });
    
    // In ScStubList::Reset()
    patcher.LowLevelHook(0x47AB11, [](Ebx<ScStubListEx*> pThis, Esi<uint32>& counter) {
      counter = (pThis == &g_scStubListEx) ? ScStubListEx::MaxNumScStubs : ScStubList::MaxNumScStubs;  return 0x47AB16;
    });
    patcher.LowLevelHook(0x47AB2C, [](Ebx<ScStubListEx*> pThis)
      { if (pThis == &g_scStubListEx) pThis->lastCreatedGroupIndex_ = pThis->NilIndex; });
#endif

    // Reimplement ScStubFactory::Create to make ScStub recycling work properly (and use extended array).
    patcher.Hook(0x47B410, &ScStubFactory_Create);

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

#if SCSTUB_USE_EXTENDED_ARRAY
  if ((enable == false) && (patcher.NumPatches() != 0)) {
    memcpy(pOldScStubArray, &g_scStubListEx, sizeof(*pOldScStubArray));
    memset(&g_scStubListEx[0], 0, sizeof(ScStubList::pScStubArray_));  // Skip destroying non-extended-space ScStubs.
    g_scStubListEx.Reset();  // Destroy all the ScStubs in extended space and null out their pointers.
  }
#endif

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
