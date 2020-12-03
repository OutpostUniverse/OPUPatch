
#pragma once

#include "Tethys/Resource/GFXPalette.h"

namespace Tethys {

class StreamIO;

static constexpr size_t NumLightLevels = 32;

enum BitmapFlags : uint32 {
  BitmapFlagLoadPaletteData = (1u <<  0),
  BitmapFlagFreeImageBuffer = (1u <<  3),
  BitmapFlagSaveBmpFormat   = (1u << 31),
};

BEGIN_PACKED

class GFXBitmap : public OP2Class<GFXBitmap> {
public:
  GFXBitmap()                  { InternalCtor<0x419950>(); }
  GFXBitmap(InternalCtorChain) {                           }

  void* Destroy() { return Thunk<0x419970, &$::Destroy>(); }  // ~GFXBitmap() [0x4199C0 is an identical copy]

  static GFXBitmap* FASTCALL CreateBitmap(int width, int height, int bitDepth)
    { return OP2Thunk<0x419A10, &$::CreateBitmap>(width, height, bitDepth); }
  static GFXBitmap* FASTCALL LoadBmp(const char* pFilename, uint32 flags)
    { return OP2Thunk<0x419C90, GFXBitmap* FASTCALL(const char*, uint32)>(pFilename, flags); }
  static GFXBitmap* FASTCALL LoadBmp(StreamIO* pStream, uint32 flags)
    { return OP2Thunk<0x419D90, GFXBitmap* FASTCALL(StreamIO*, uint32)>(pStream, flags); }

  GFXBitmap* CreateSubBitmap(const RECT& srcRect)   { return Thunk<0x419B00, &$::CreateSubBitmap>(srcRect);            }
  void SetSourceBitmap(GFXBitmap* pSrcBmp, RECT* pSrcRect)
    { return Thunk<0x419BE0, &$::SetSourceBitmap>(pSrcBmp, pSrcRect); }
  int  Load(const char*    pFilename, uint32 flags) { return Thunk<0x419E30,int(const char*,uint32)>(pFilename,flags); }
  int  Load(StreamIO*        pStream, uint32 flags) { return Thunk<0x419EB0, int(StreamIO*, uint32)>(pStream, flags);  }
  int  LoadBmpFile(StreamIO* pStream, uint32 flags) { return Thunk<0x41A110, &$::LoadBmpFile>(pStream, flags);         }
  int  Save(const char*    pFilename, uint32 flags) { return Thunk<0x41A2B0,int(const char*,uint32)>(pFilename,flags); }
  int  Save(StreamIO*        pStream, uint32 flags) { return Thunk<0x41A320, int(StreamIO*, uint32)>(pStream, flags);  }
  int  SaveBmpFile(StreamIO* pStream, uint32 flags) { return Thunk<0x41A440, &$::SaveBmpFile>(pStream, flags);         }
  int  F1()                                         { return Thunk<0x41A5D0, &$::F1>();                                }

public:
  int   width_;
  int   height_;
  int   pitch_;
  int   bpp_;
  int   imageBufferSize_;
  void* pImageBuffer_;
  void* pPalette_;
  int   attributes_;
};

class GFXLightAdjustedBitmap : public GFXBitmap {
  using $ = GFXLightAdjustedBitmap;
public:
  GFXLightAdjustedBitmap()                   : GFXBitmap(UseInternalCtorChain) { InternalCtor<0x4A9B30>(); }
  GFXLightAdjustedBitmap(InternalCtorChain)  : GFXBitmap(UseInternalCtorChain) {                           }

  void* Destroy() { return Thunk<0x4A9B70, &$::Destroy>(); }  // ~GFXLightAdjustedBitmap()

  void Convert8To16() { return Thunk<0x4A9B90, &$::Convert8To16>(); }
  int  Load(const char* pFilename, ibool specialColorScale, int numLightLevels)
    { return Thunk<0x4A9CC0, &$::Load>(pFilename, specialColorScale, numLightLevels); }

  static GFXLightAdjustedBitmap* FASTCALL CreateBitmap(int width, int height, int bitDepth)
    { return OP2Thunk<0x4AA0D0, &$::CreateBitmap>(width, height, bitDepth); }

public:
  int   numLightLevels_;
  int   field_24;
  int   drawMethod_;
  void* pLightLevelPal_;                          ///< uint16[256 * numLightLevels]*
  uint8 mappedPlayerColorIndex_[NumLightLevels];
  int   field_50;
  void* pLightLevelPals_[NumLightLevels];         ///< Objects are 48 bytes each
  int   field_D4;
  int   field_D8;
};

class GFXTilesetBitmap : public GFXLightAdjustedBitmap {
  using $ = GFXTilesetBitmap;
public:
  GFXTilesetBitmap() : GFXLightAdjustedBitmap(UseInternalCtorChain) { InternalCtor<0x4A8330>(); }

  void* Destroy() { return Thunk<0x4A8340, &$::Destroy>(); }  // ~GFXTilesetBitmap()

  int  Load(int numTiles, const char* pFileBaseName, ibool specialScale)
    { return Thunk<0x4A8360, &$::Load>(numTiles, pFileBaseName, specialScale); }
  int  LoadAndFlatten(const char* pFileBaseName, ibool specialScale)
    { return Thunk<0x4A83E0, &$::LoadAndFlatten>(pFileBaseName, specialScale); }
  void Flatten() { return Thunk<0x4A8560, &$::Flatten>(); }
  void F2(int a) { return Thunk<0x4A8620, &$::F2>(a);     }
  GFXTilesetBitmap* Copy(GFXTilesetBitmap** ppSrc, int a) { return Thunk<0x4A86C0, &$::Copy>(ppSrc, a); }

  static void FASTCALL F3(PALETTEENTRY entryTable[256], RGBQUAD rgbTable[256])
    { return OP2Thunk<0x4A8680, &$::F3>(entryTable, rgbTable); }

public:
  int    numTiles_;
  int    numTilesRequested_;
  int    field_E4;
  int    field_E8;
  int    tileHeight_;
  int    tilesetHeight_;
  int    bytesPerTile_;
  uint8* pPixelData_;         ///< uint8[numTiles * bytesPerTile]* (not imageBufferSize!)
                              ///< Used for drawing tiles;  initial value is same as pImageBuffer.
};

class GFXSpriteBitmap : public GFXLightAdjustedBitmap {
  using $ = GFXSpriteBitmap;
public:
  GFXSpriteBitmap() : GFXLightAdjustedBitmap(UseInternalCtorChain) { InternalCtor<0x403730>(); }

  void* Destroy() { return Thunk<0x403760, &$::Destroy>(); }  // ~GFXSpriteBitmap()

  void Close() { return Thunk<0x4037C0, &$::Close>(); }
  int  Load(const char* pFileBaseName, int numLightLevels, int unused = 0)
    { return Thunk<0x403820, &$::Load>(pFileBaseName, numLightLevels, unused); }

public:
  void*  hFile_;
  void*  hMapping_;
  uint8* pBmpFileData_;
  int    fileSize_;
  uint8* pPixelData_;    ///< Used for drawing sprites;  initial value is same as pImageBuffer.
  int    field_F0;
  int    field_F4;
  int    field_F8;
};

END_PACKED

} // Tethys
