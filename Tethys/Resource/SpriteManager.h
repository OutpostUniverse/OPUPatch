
#pragma once

#include "Tethys/Common/Memory.h"

class GFXSpriteBitmap;

struct ImageInfo {
  int    scanlineByteWidth;
  int    dataOffset;
  int    height;
  int    width;
  uint16 typeFlags;
  uint16 paletteIndex;
};

struct FrameComponentInfo {
  uint16 pixelXOffset;
  uint16 pixelYOffset;
  uint16 imageInfoIndex;
};

struct FrameOptionalInfo {
  int8 offsetX;
  int8 offsetY;
  int8 offsetX2;
  int8 offsetY2;
};

struct FrameInfo {
  struct {
    uint16 left;    ///< 0x7FFE = Auto compute bounding rect, 0x7FFF = Never compute bounding rect
    uint16 top;
    uint16 right;
    uint16 bottom;
  } rect;

  uint16              numFrameComponents;
  FrameComponentInfo* pFrameComponentInfo;
};

struct AnimationInfo {
  int         field_00;
  RECT        selectionBox;
  int         pixelXDisplacement;
  int         pixelYDisplacement;
  int         field_1C;
  int         numFrames;
  FrameInfo** ppFrameInfo;
  uint16      frameOptionalInfoStartIndex;
};

class SpriteManager : public OP2Class<SpriteManager> {
public:
   SpriteManager() { InternalCtor<0x404320>(); }
  ~SpriteManager() { Thunk<0x4043C0>();        }

  static SpriteManager* GetInstance() { return OP2Mem<SpriteManager*&>(0x40423B); }  // 0x4EFD68

  FrameOptionalInfo GetFrameOptionalInfo(int animIndex, int frameIndex)
    { return Thunk<0x404F00, &SpriteManager::GetFrameOptionalInfo>(animIndex, frameIndex); }

public:
  static constexpr size_t MaxNumImages     = 5608;
  static constexpr size_t MaxNumAnimations = 2176;

  uint8* pPaletteData_;
  int    paletteSize_;

  ImageInfo imageInfo_[MaxNumImages];
  int       numImages_;

  AnimationInfo animationInfo_[MaxNumAnimations];
  int           numAnimations_;

  GFXSpriteBitmap* pBitmap_;

  FrameInfo**          ppFrameInfo_;
  FrameComponentInfo** ppFrameComponentInfo_;
  FrameOptionalInfo**  ppFrameOptionalInfo_;
};
