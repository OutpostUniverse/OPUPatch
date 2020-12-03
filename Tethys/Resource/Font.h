
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class  GFXSurface;
struct RenderDataBase;
struct Size;

struct GlyphMetrics {
  POINT gmptGlyphOrigin;
  int   gmCellIncX;
  int   gmCellIncY;
  UINT  gmBlackBoxX;
  UINT  gmBlackBoxY;
  int   bufferIndex;      ///< [init: bufferSize return value]
};
static_assert(sizeof(GlyphMetrics) == 0x1C, "Incorrect GlyphMetrics size.");

class FontBase : public OP2Class<FontBase> {
public:
  virtual ~FontBase();      // 0x416BE0  (scalar-deleting: 0,1)
};

class Font : public FontBase {
  using $ = Font;
public:
  virtual ~Font();          // 0x415CB0  (scalar-deleting: 0,1)
  virtual Size* GetTextSize(Size* pSize, char* pStr, int strLen)
    { return Thunk<0x415F60, &$::GetTextSize>(pSize, pStr, strLen); }

  // ** TODO vtbl at 0x4CFBC8

  // Constructor/Destructor
  // Font();                // 0x415C80
  //~Font();                // 0x415CF0  (non-virtual destructor)

  void Init(const LOGFONT& createInfo) { return Thunk<0x415D10, &$::Init>(createInfo); }

  void DrawString(const char* pStr, int strLen, GFXSurface* pSurface, POINT* pPos, RECT* pRect, COLORREF color) {
    return Thunk<0x4162B0, void(const char*, int, GFXSurface*, POINT*, RECT*, COLORREF)>(
      pStr, strLen, pSurface, pPos, pRect, color);
  }

  void DrawString(
    const char* pStr, RenderDataBase* pRenderData, GFXSurface* pSurface, POINT* pPos, RECT* pRect, COLORREF color,
    int vSpacing)
  {
    return Thunk<0x4161F0, void(const char*, RenderDataBase*, GFXSurface*, POINT*, RECT*, COLORREF, int)>(
      pStr, pRenderData, pSurface, pPos, pRect, color, vSpacing);
  }

  void DrawString(
    const char* pStr, RenderDataBase* pRenderData, GFXSurface* pSurface, RECT* pTextRect, RECT* pDrawRect, int vSpacing)
  {
    return Thunk<0x416B20, void(const char*, RenderDataBase*, GFXSurface*, RECT*, RECT*, int)>(
      pStr, pRenderData, pSurface, pTextRect, pDrawRect, vSpacing);
  }

  void ParseString(const char* pStr, int strLen, int boundWidth, RenderDataBase* pRenderData, Size* pSize) {
    return Thunk<0x416770, void(const char*, int, int, RenderDataBase*, Size*)>(
      pStr, strLen, boundWidth, pRenderData, pSize);
  }

  void ParseString(const char* pStr, int strLen, int boundWidth, RenderDataBase* pRenderData, Size* pSize, int a) {
    return Thunk<0x415FE0, void(const char*, int, int, RenderDataBase*, Size*, int)>(
      pStr, strLen, boundWidth, pRenderData, pSize, a);
  }

public:
  LOGFONT      createInfo_;
  int          field_40;                ///< Gets MemCopied with LOGFONT
  int          tmHeight_;
  int          tmAscent_;
  int          tmDescent_;
  int          tmInternalLeading_;
  int          tmExternalLeading_;
  int          tmMaxCharWidth_;         ///< MaxCharWidth+1
  GlyphMetrics glyphMetrics_[256];
  uint8*       pCharacterImageBuffer_;
};
static_assert(sizeof(Font) == 0x1C60, "Incorrect Font size.");

struct RenderChunk {
  int      xOffset;      ///< Pixel offset of this chunk of text
  int      stringStart;  ///< Index of first string char in this chunk
  int      stringLen;    ///< Length of this chunk in characters
  ibool    isEOL;        ///< Is-End-Of-Line, used for text wrapping
  COLORREF color;        ///< Color used to render this chunk of text
};

struct RenderDataBase {
  int numChunks;                        ///< Total allocated RenderChunk space
  int numLines;
  int numChunksUsed;                    ///< Used RenderChunk space

  RenderDataBase() { numChunks = 0; };  ///< Convenient initalizer for the struct array size
};

// For convenience to make a variable of a specific size
template <int N>
struct RenderData : public RenderDataBase {
  RenderChunk renderChunk[N];          ///< Describes a section of unbroken text in a single color

  RenderData<N>() { numChunks = N; };  ///< Convenient initalizer for the struct array size
};

} // Tethys
