
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

/// Mining beacon ore type.
enum class OreType : int8 {
  Random = -1,  ///< 70% chance of Common, or 30% chance of Rare upon creation.
  Common =  0,  ///< Common ore.
  Rare   =  1,  ///< Rare ore.  @note Requires Rare Ore Processing to be visible.
  Count
};

/// Mining beacon "bar" yield type (major impact on ore output).
enum class OreYield : int {
  Random = -1,  ///< 20% chance of Bar3, 60% chance of Bar2, or 20% chance of Bar1 upon creation.
  Bar3   =  0,  ///< 50% yield -> 14/20/10 loads => 60/55/55% peak -> 50/60/50 loads => 30/30/35% min (variant #0/1/2)
  Bar2   =  1,  ///< 30% yield -> 10/12/20 loads => 40/40/35% peak -> 40/35/40 loads => 20/20/25% min (variant #0/1/2)
  Bar1   =  2,  ///< 10% yield -> 10/14/20 loads => 25/30/20% peak -> 40/45/50 loads => 15/10/10% min (variant #0/1/2)
  Count
};

/// Mining beacon yield variant type (minor impact on ore output).
enum class OreVariant : int {
  Random = -1,  ///< Even chance to pick any variant upon creation.
  High   = 0,   ///< Rated by best minimum yield, then total # of truckloads.  (Internal:  1 bar = #0,  2/3 bar = #2)
  Mid,          ///< Rated by best peak yield, then total # of truckloads.     (Internal:  1 bar = #1,  2/3 bar = #0)
  Low,          ///< Neither of the above.                                     (Internal:  1 bar = #2,  2/3 bar = #1)
  Count
};


/// Internal mine yield stats manager class.
class MineManager : public OP2Class<MineManager> {
public:
  struct TruckLoadInfo;
  struct YieldPercentInfo;

  ibool LoadMinesFile() { return Thunk<0x44B010, &$::LoadMinesFile>(); }  ///< Load mines.txt from sheets.

  /// Calculates a mine's output ore quantity based on its yield, variantNum, and number of truck loads thus far.
  int   CalculateMineYield(OreYield yield, int variantNum, int numTruckLoadsSoFar) const
    { return Thunk<0x44B1C0, &$::CalculateMineYield>(yield, variantNum, numTruckLoadsSoFar); }

  /// Gets the global MineManager instance.
  static MineManager* GetInstance() { return OP2Mem<0x5651A0, MineManager*>(); }

  ///@{ Gets yield percent or truck load data for the specified OreYield and variantNum.  Do not use Random for either.
  auto& Yields(OreYield yield, int variantNum)       { return  yieldPctInfo_[(variantNum * NumVariants) + int(yield)]; }
  auto& Yields(OreYield yield, int variantNum) const { return  yieldPctInfo_[(variantNum * NumVariants) + int(yield)]; }
  auto& Trucks(OreYield yield, int variantNum)       { return truckLoadInfo_[(variantNum * NumVariants) + int(yield)]; }
  auto& Trucks(OreYield yield, int variantNum) const { return truckLoadInfo_[(variantNum * NumVariants) + int(yield)]; }
  ///@}

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
    int peakTruck;     ///< 'PEAK_TRUCK'
    int minTruck;      ///< 'MIN_TRUCK'
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
