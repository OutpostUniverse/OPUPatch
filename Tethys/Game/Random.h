
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

class Random : public OP2Class<Random> {
public:
  int Rand()          { return Thunk<0x46F000, int()>();         }
  int Rand(int range) { return Thunk<0x46F060, int(int)>(range); }

  void SetSeed(uint32 seed) { return Thunk<0x46EFB0, void(uint32)>(seed); }
  void SetSeed(uint64 seed) { return Thunk<0x46EFF0, void(uint64)>(seed); }

  uint64 GetSeed() const { return seed_; }  // 0x46EF90 (void(uint32* pLow, uint32* pHigh)

  static Random* GetInstance()      { return OP2Mem<0x56BE20, Random*>(); }  ///< Main RNG used in gameplay logic.
  static Random* GetLocalInstance() { return OP2Mem<0x574428, Random*>(); }  ///< RNG not synced over the network.

  ///@{ Fulfill C++ UniformRandomBitGenerator requirements.
  ///   This allows for usage such as e.g. std::shuffle(list.begin(), list.end(), g_gameRNG)
  using result_type = uint32;
  static constexpr result_type (min)() { return 0;       }
  static constexpr result_type (max)() { return INT_MAX; }
  result_type operator()() { return Rand(int((max)())); }
  ///@}

public:
  uint64 seed_;
};
static_assert(8 == sizeof(Random), "Incorrect Random size.");

inline auto& g_gameRNG  = *Random::GetInstance();
inline auto& g_localRNG = *Random::GetLocalInstance();

} // Tethys
