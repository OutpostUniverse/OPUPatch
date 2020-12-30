
#pragma once

#include "Tethys/API/Unit.h"

namespace Tethys::TethysAPI {

struct UnitRecord {
  constexpr UnitRecord(
    MapID type, int x, int y, int a, int rot, MapID weapon, UnitClassification cls, uint16 cargo, uint16 amount)
    : unitType(type), x(x), y(y), field_0C(a), rotation(rot), weaponType(weapon), unitClassification(cls),
      cargoType(cargo), cargoAmount(amount) { }
  constexpr UnitRecord()
    : unitType(), x(), y(), field_0C(), rotation(), weaponType(), unitClassification(), cargoType(), cargoAmount() { }

  MapID              unitType;
  int                x;
  int                y;
  int                field_0C;
  int                rotation;
  MapID              weaponType;
  UnitClassification unitClassification;
  uint16             cargoType;
  uint16             cargoAmount;
};


class UnitBlock : public OP2Class<UnitBlock> {
public:
  UnitBlock(UnitRecord* pUnitRecordTable) { InternalCtor<0x49D4A0, UnitRecord*>(pUnitRecordTable); }

  int CreateUnits(int playerNum, ibool lightsOn = true) const  ///< Returns numUnitsCreated
    { return Thunk<0x49D5D0, &$::CreateUnits>(playerNum, lightsOn); }

  /// Sort unitRecordTable and initialize classRange table
  void SortAndInit(UnitRecord* pUnitRecordTable) { return Thunk<0x49D4C0, &$::SortAndInit>(pUnitRecordTable); }

public:
  struct UnitRange {
    int startIndex;
    int untilIndex;
  };

  int         numUnits_;          ///< Range of unit indexes in pUnitRecordTable_ for each class
  UnitRange   classRange_[16];
  UnitRecord* pUnitRecordTable_;
};

} // Tethys::TethysAPI
