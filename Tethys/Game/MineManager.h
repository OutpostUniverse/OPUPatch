
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

/// Mining beacon ore type.
enum class OreType : int8 {
  Random = -1,  ///< 70% chance of Common, 30% chance of Rare upon creation.
  Common =  0,
  Rare   =  1,
  Count
};

/// Mining beacon "bar" yield type (major impact on ore output).
enum class OreYield : int {
  Random = -1,  ///< 20% chance of Bar3, 60% chance of Bar2, 20% chance of Bar1 upon creation.
  Bar3   =  0,
  Bar2   =  1,
  Bar1   =  2,
  Count
};

/// Mining beacon yield variant type (minor impact on ore output).
enum class OreVariant : int {
  Random = -1,  ///< Even chance to pick any variant upon creation.
  High   = 0,   ///< Rated by best minimum yield, then total # of truckloads.  (Internal: 2/3 bar = #2, 1 bar = #0)
  Mid,          ///< Rated by best peak yield, then total # of truckloads.     (Internal: 2/3 bar = #0, 1 bar = #1)
  Low,          ///< Neither of the above.                                     (Internal: 2/3 bar = #1, 1 bar = #2)
  Count
};


class MineManager : public OP2Class<MineManager> {
public:
  struct TruckLoadInfo;
  struct YieldPercentInfo;

  ibool LoadMinesFile() { return Thunk<0x44B010, &$::LoadMinesFile>(); }
  int   CalculateMineYield(OreYield yield, int variantNum, int numTruckLoadsSoFar) const
    { return Thunk<0x44B1C0, &$::CalculateMineYield>(yield, variantNum, numTruckLoadsSoFar); }

  static MineManager* GetInstance() { return OP2Mem<0x5651A0, MineManager*>(); }

  auto& Trucks(OreYield yield, int variantNum)       { return truckLoadInfo_[(variantNum * NumVariants) + int(yield)]; }
  auto& Trucks(OreYield yield, int variantNum) const { return truckLoadInfo_[(variantNum * NumVariants) + int(yield)]; }
  auto& Yields(OreYield yield, int variantNum)       { return  yieldPctInfo_[(variantNum * NumVariants) + int(yield)]; }
  auto& Yields(OreYield yield, int variantNum) const { return  yieldPctInfo_[(variantNum * NumVariants) + int(yield)]; }

  /// Gets the variantNum corresponding to the given OreYield and OreVariant.
  /// @note Supplying OreYield::Random will always output -1 for any OreVariant.
  int GetVariantNum(OreYield yield, OreVariant variant) const;

  /// Gets the OreVariant corresponding to the given OreYield and variant number, or MineVariant::Count if not found.
  /// @note Supplying OreYield::Random will always output OreVariant::Count for any variantNum.
  OreVariant GetOreVariant(OreYield yield, int variantNum) const {
    OreVariant out{0};
    for (; (out < OreVariant::Count) && (GetVariantNum(yield, out) != variantNum); ++(int&)(out));
    return out;
  }

public:
  static constexpr size_t NumYields   = size_t(OreYield::Count);
  static constexpr size_t NumVariants = 3;  ///< @note Variants are indexed rooted at 1 in mines.txt, but 0 internally.

  struct TruckLoadData {
    int field_00;
    int peakTruck;  ///< 'PEAK_TRUCK'
    int minTruck;   ///< 'MIN_TRUCK'
  } truckLoadInfo_[NumYields * NumVariants];

  struct YieldPercentData {
    int initialYield;  ///< 'INITIAL_YIELD_%' (= sheet value x10)
    int peakYield;     ///< 'PEAK_YIELD_%'    (= sheet value x10)
    int minYield;      ///< 'MIN_YIELD_%'     (= sheet value x10)
  } yieldPctInfo_[NumYields  * NumVariants];
};


// =====================================================================================================================
inline int MineManager::GetVariantNum(
  OreYield   yield,
  OreVariant variant
  ) const
{
  if (yield == OreYield::Random) {
    return -1;
  }
  else switch (int num = 0;  variant) {
    case OreVariant::High:  case OreVariant::Mid: {
      const auto cmp = (variant == OreVariant::High) ? &YieldPercentData::minYield : &YieldPercentData::peakYield;
      for (int v = 1; v < NumVariants; ++v) {
        if (const int delta = (Yields(yield, v).*cmp - Yields(yield, num).*cmp);  delta > 0) {
          num = v;
        }
        else if (const auto& cur = Trucks(yield, v), &best = Trucks(yield, num);  delta == 0) {
          num = ((cur.minTruck + cur.peakTruck) > (best.minTruck + best.peakTruck)) ? v : num;
        }
      }
      return num;
    }
    case OreVariant::Low:
      return (0 + 1 + 2) - GetVariantNum(yield, OreVariant::High) - GetVariantNum(yield, OreVariant::Mid);
    default:
      return -1;
  }
}

} // Tethys
