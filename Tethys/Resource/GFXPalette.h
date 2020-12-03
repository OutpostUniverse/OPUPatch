
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

union Rgb555 {
  operator uint16&() { return u16All; }

  struct {
    uint16 b : 5;
    uint16 g : 5;
    uint16 r : 5;
    uint16 a : 1;
  };
  uint16 u16All;
};

class GFXPalette : public OP2Class<GFXPalette> {
public:
  // Constructor/Destructor
   GFXPalette() { InternalCtor<0x4AA860>(); }
  ~GFXPalette() { Thunk<0x4AA8C0>();        }

  void SetPalette(PALETTEENTRY* pNewEntryTable) { return Thunk<0x4AA930, void(PALETTEENTRY*)>(pNewEntryTable); }
  void SetPalette(RGBQUAD*      pNewEntryTable) { return Thunk<0x4AA960, void(RGBQUAD*)>(pNewEntryTable);      }
  void GetPalette(PALETTEENTRY* pEntryTable)    { return Thunk<0x4AA9B0, void(PALETTEENTRY*)>(pEntryTable);    }
  void GetPalette(RGBQUAD*      pRgbTable)      { return Thunk<0x4AA9E0, void(RGBQUAD*)>(pRgbTable);           }

  int Load(const char* pFilename, int unused)
    { return Thunk<0x4AAB60, int(const char*, int)>(pFilename, unused); }
  int Load(StreamIO* pStream, int unused)            { return Thunk<0x4AABE0, int(StreamIO*, int)>(pStream, unused);   }
  int LoadRiffPalette(StreamIO* pStream, int unused) { return Thunk<0x4AAE50, &$::LoadRiffPalette>(pStream, unused);   }
  int Save(const char* pFilename, int flags)         { return Thunk<0x4AAF20, int(const char*,int)>(pFilename, flags); }
  int Save(StreamIO* pStream, int flags)             { return Thunk<0x4AAF90, int(StreamIO*, int)>(pStream, flags);    }
  int SaveRiffPalette(StreamIO* pStream, int unused) { return Thunk<0x4AB1E0, &$::SaveRiffPalette>(pStream, unused);   }

  int F2() { return Thunk<0x4AB2C0, &$::F2>(); }

  static GFXPalette* FASTCALL CreatePalette(PALETTEENTRY* pColors)
    { return OP2Thunk<0x4AAA20, GFXPalette* FASTCALL(PALETTEENTRY*)>(pColors); }
  static GFXPalette* FASTCALL CreatePalette(RGBQUAD* pColors)
    { return OP2Thunk<0x4AAA60, GFXPalette* FASTCALL(RGBQUAD*)>(pColors); }
  static GFXPalette* FASTCALL CreatePalette(const char* pFilename)
    { return OP2Thunk<0x4AAAA0, GFXPalette* FASTCALL(const char*)>(pFilename); }
  static GFXPalette* FASTCALL CreatePalette(StreamIO* pStream)
    { return OP2Thunk<0x4AAB00, GFXPalette* FASTCALL(StreamIO*)>(pStream); }

public:
  int version_;
  int numColors_;
  int colors_[256];
  int field_404;
  int field_408;
  int field_40C;
  int field_410;
  int field_414;
  int field_418;
  int field_41C;
  int field_420;
  int field_424;
  int field_428;
  int field_42C;
  int field_430;
  int field_434;
  int field_438;
  int field_43C;
  int field_440;
  int field_444;
  int field_448;
  int field_44C;
};
static_assert(sizeof(GFXPalette) == 0x454, "Incorrect GFXPalette size.");

} // Tethys
