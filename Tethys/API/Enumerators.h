
#pragma once

#include "Tethys/API/Location.h"
#include "Tethys/API/ScGroup.h"
#include "Tethys/API/Unit.h"

// ** TODO The Enumerator API will be changed substantially to work more like std::iterator.
// The current interface should be considered a placeholder.

namespace Tethys::TethysAPI {

/// Group (enumerate all units in a group)
class GroupEnumerator : public OP2Class<GroupEnumerator> {
public:
  explicit GroupEnumerator(const ScGroup& group) { InternalCtor<0x47A830, const ScGroup&>(group); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A850, &$::GetNext>(currentUnit); }
  
public:
  struct UnitNode {
    UnitNode*  pPrev;  // ?
    UnitNode*  pNext;
    MapObject* pUnit;
  };

  UnitNode* pCurrentUnitNode_;
};

/// Vehicles (enumerate all vehicles for a certain player)
class PlayerVehicleEnum : public OP2Class<PlayerVehicleEnum> {
public:
  explicit PlayerVehicleEnum(int playerNum) { InternalCtor<0x47A480, int>(playerNum); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A4B0, &$::GetNext>(currentUnit); }
  
public:
  MapObject* pCurrentUnit_;
};

/// Buildings (enumerate all buildings of a certain type for a certain player)
class PlayerBuildingEnum : public OP2Class<PlayerBuildingEnum> {
public:
  PlayerBuildingEnum(int playerNum, MapID buildingType = MapID::None)
    { InternalCtor<0x47A4E0, int, MapID>(playerNum, buildingType); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A510, &$::GetNext>(currentUnit); }
  
public:
  MapObject* pCurrentUnit_;
  MapID      buildingType_;
};

/// Units (enumerate all units of a certain player)
class PlayerUnitEnum : public OP2Class<PlayerUnitEnum> {
public:
  explicit PlayerUnitEnum(int playerNum) { InternalCtor<0x47A580, int>(playerNum); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A5B0, &$::GetNext>(currentUnit); }
  
public:
  MapObject* pCurrentUnit_;
  int        playerNum_;
};

/// InRange (enumerate all units within a given distance of a given location)
class InRangeEnumerator : public OP2Class<InRangeEnumerator> {
public:
  InRangeEnumerator(const Location& centerPoint, int maxTileDistance)
    { InternalCtor<0x47A740, const Location&, int>(centerPoint, maxTileDistance); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A780, &$::GetNext>(currentUnit); }
  
public:
  MapObject* pCurrentUnit_;
  int        field_04[12];
};

/// InRect (enumerate all units within a given rectangle)
class InRectEnumerator : public OP2Class<InRectEnumerator> {
public:
  explicit InRectEnumerator(const MapRect& rect) { InternalCtor<0x47A610, const MapRect&>(rect); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A6A0, &$::GetNext>(currentUnit); }
  
public:
  int field_00[13];
};

/// Location (enumerate all units at a given location)
class LocationEnumerator : public OP2Class<LocationEnumerator> {
public:
  explicit LocationEnumerator(const Location& location) { InternalCtor<0x47A6D0, const Location&>(location); }

  ibool GetNext(Unit& currentUnit) { return Thunk<0x47A710, &$::GetNext>(currentUnit); }
  
public:
  int field_00[13];
};

/// Closest (enumerate all units ordered by their distance to a given location)
class ClosestEnumerator : public OP2Class<ClosestEnumerator> {
public:
  explicit ClosestEnumerator(const Location& location) { InternalCtor<0x47A7B0, const Location&>(location); }

  ibool GetNext(Unit& currentUnit, uint32& pixelDistance)
    { return Thunk<0x47A7F0, &$::GetNext>(currentUnit, pixelDistance); }
  
public:
  int field_00[13];
};

} // Tethys
