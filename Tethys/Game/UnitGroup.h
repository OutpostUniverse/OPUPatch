
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class IWnd;

class UnitSelectionHistory : public OP2Class<UnitSelectionHistory> {
public:
  void CopySelectionToHistory() { return Thunk<0x47BFD0, &$::CopySelectionToHistory>(); }

  // ** TODO more member functions
  /* ???0                      0x47BE10
     Deinit?                   0x47BF00
     ???2                      0x47BF20
     ???3                      0x47C050
     ???4                      0x47C080
     ???5                      0x47C150
     ???6                      0x47C220 */

  static UnitSelectionHistory* GetInstance() { OP2Mem<0x56CA00, UnitSelectionHistory*>(); }

public:
  // ** TODO member fields
};


class UnitHotKeyGroups : public OP2Class<UnitHotKeyGroups> {
public:
  // ** TODO more member functions
  /* Constructor          0x47B630
     Deinit?              0x47B650
     Save                 0x47B670
     Load                 0x47B730
     ???                  0x47B7F0
     SelectNextUnit()     0x47B870
     SelectPreviousUnit() 0x47B8C0
     ???2                 0x47B990
     ???3                 0x47BB00
     SetGroup             0x47BC70
     SelectGroup          0x47BD50 */

  static UnitHotKeyGroups* GetInstance() { OP2Mem<0x56C760, UnitHotKeyGroups*>(); }

public:
  // ** TODO member fields
};


class UnitGroup : public OP2Class<UnitGroup> {
public:
  int        GetNumUnits() const { return Thunk<0x41A9A0, &$::GetNumUnits>();  }
  const int* GetUnitIDs()  const { return Thunk<0x41A9B0, &$::GetUnitIDs>(); }

  void* WriteUnitIDs(void* pUnitIDBuffer, uint32 filterMoFlags = UINT32_MAX)
    { return Thunk<0x41A900, &$::WriteUnitIDs>(pUnitIDBuffer, filterMoFlags); }

  ibool Add(int unitIndex, IWnd* pWnd = nullptr) { return Thunk<0x41A830, &$::Add>(unitIndex, pWnd); }
  void  Trim()                                   { return Thunk<0x41A610, &$::Trim>();               }
  void  Clear(IWnd* pWnd = nullptr)              { return Thunk<0x41AA00, &$::Clear>(pWnd);          }

  bool Contains(int unitIndex) {
    int i = 0;
    for (; (i < numUnits_) && (unitIndex_[i] != unitIndex); ++i);
    return (numUnits_ > 0) && (unitIndex_[i] == unitIndex);
  }

  // ** TODO more member functions
  /* 0x41A9C0
     0x41A730
     0x41A770
     0x41A880
     0x41A6D0 (subclass function?) */

  static UnitGroup* GetSelectedUnitGroup() { return OP2Mem<UnitGroup*&>(0x48A5D2); }

public:
  int numUnits_;
  int unitIndex_[32];
};
static_assert(sizeof(UnitGroup) == 132, "Incorrect UnitGroup size.");


BEGIN_PACKED
struct PackedUnitGroup {
  uint8  numUnits;
  uint16 unitIndex[32];
};
static_assert(sizeof(PackedUnitGroup) == 65, "Incorrect PackedUnitGroup size.");
END_PACKED

} // Tethys
