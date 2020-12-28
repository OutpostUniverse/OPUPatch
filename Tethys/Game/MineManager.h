
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

/// Mining beacon ore type.
enum class OreType : int8 {
  Random = -1,
  Common =  0,
  Rare   =  1,
  Count
};

/// Mining beacon "bar" yield type (major impact on ore output).
enum class OreYield : int {
  Random = -1,
  Bar3   =  0,
  Bar2   =  1,
  Bar1   =  2,
  Count
};

/// Mining beacon yield variant type (minor impact on ore output).
enum class OreVariant : int {
  Random = -1,
  _1     =  0,  ///< Bar3, Bar2:  Best peak yield;  Bar1:  Best min yield
  _2     =  1,  ///<                                Bar1:  Best peak yield
  _3     =  2,  ///< Bar3, Bar2:  Best min yield
  Count
};


class MineManager : public OP2Class<MineManager> {
public:
  struct TruckLoadInfo;
  struct YieldPercentInfo;

  ibool LoadMinesFile() { return Thunk<0x44B010, &$::LoadMinesFile>(); }
  int   CalculateMineYield(OreYield yield, OreVariant variant, int numTruckLoadsSoFar) const
    { return Thunk<0x44B1C0, &$::CalculateMineYield>(yield, variant, numTruckLoadsSoFar); }

  static MineManager* GetInstance() { return OP2Mem<0x5651A0, MineManager*>(); }
  
  TruckLoadInfo*    GetTruckInfo(OreYield    yield, OreVariant variant)
    { return &truckLoadInfo_[(size_t(variant) * NumVariants) + size_t(yield)]; }

  YieldPercentInfo* GetYieldInfo(OreYield yield, OreVariant variant)
    { return &yieldPctInfo_[(size_t(variant) * NumVariants) + size_t(yield)]; }

public:
  struct TruckLoadInfo {
    int field_00;
    int peakTruck;  ///< 'PEAK_TRUCK'
    int minTruck;   ///< 'MIN_TRUCK'
  };

  struct YieldPercentInfo {
    int initialYield;  ///< 'INITIAL_YIELD_%' (= sheet value x10)
    int peakYield;     ///< 'PEAK_YIELD_%'    (= sheet value x10)
    int minYield;      ///< 'MIN_YIELD_%'     (= sheet value x10)
  };

  static constexpr size_t NumYields   = size_t(OreYield::Count);
  static constexpr size_t NumVariants = size_t(OreVariant::Count);

  TruckLoadInfo    truckLoadInfo_[NumYields * NumVariants];
  YieldPercentInfo yieldPctInfo_[NumYields  * NumVariants];
};

} // Tethys
