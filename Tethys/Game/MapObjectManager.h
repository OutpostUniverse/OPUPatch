
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

namespace MapIDImpl { enum MapID : int; }
using MapID = MapIDImpl::MapID;
class MapObject;
class MapObjectType;
class StreamIO;

class MapObjectManager : public OP2Class<MapObjectManager> {
public:
  MapObject* CreateMapObject(
    MapID unitType, int pixelX, int pixelY, int creatorNum, MapID cargo, int unitIndex, ibool centered)
      { return Thunk<0x4467C0, &$::CreateMapObject>(unitType, pixelX, pixelY, creatorNum, cargo, unitIndex, centered); }

  MapObjectType* GetMapObjectType(int mapID) { return Thunk<0x446AB0, &$::GetMapObjectType>(mapID); }

  ibool LoadSheetFiles() { return Thunk<0x445180, &$::LoadSheetFiles>(); }

  ibool Load(StreamIO* pSavedGame) { return Thunk<0x446740, &$::Load>(pSavedGame); }
  ibool Save(StreamIO* pSavedGame) { return Thunk<0x446780, &$::Save>(pSavedGame); }

  static MapObjectManager* GetInstance() { return OP2Mem<0x55B780, MapObjectManager*>(); }

  static MapObjectType** GetMapObjTypeTable() { return OP2Mem<MapObjectType**&>(0x446743); }  // 0x4E1348

  static size_t NumMapObjTypes() { return OP2Mem<MapObjectType**&>(0x44675C) - GetMapObjTypeTable(); }  // 115

public:
  int field_00;
  int field_04;
};
static_assert(8 == sizeof(MapObjectManager), "Incorrect MapObjectManager size.");

inline auto& g_mapObjManager = *MapObjectManager::GetInstance();

} // Tethys
