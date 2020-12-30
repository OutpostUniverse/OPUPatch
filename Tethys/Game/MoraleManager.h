
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

namespace MapIDImpl { enum MapID : int; }
using MapID = MapIDImpl::MapID;
class PlayerImpl;

enum class MoraleBonusModifier : size_t {
  PointsTotal = 0,  ///< 'M_PTS_TOT' in sheets
  PowerBonus,       ///< 'M_POW_BONUS'
  ResearchBonus,    ///< 'M_RSCH_BONUS'
  FactoryBonus,     ///< 'M_FACT_BONUS'
  FoodBonus,        ///< 'M_FOOD_BONUS'
  DefectRate,       ///< 'M_DEFECT_RATE'
  FertilityRate,    ///< 'M_FERT_RATE'
  DeathRate,        ///< 'M_DIE_RATE'
  Count
};

enum class MoraleModifierType : size_t {
  // Events
  KidDies = 0,              ///< 'KID_DIES' in sheets
  AdultDies,                ///< 'ADULT_DIES'
  KidBorn,                  ///< 'KID_BORN'
  GoodBuildingDies,         ///< 'GOOD_BUILD_DIES'
  RegularBuildingDies,      ///< 'REG_BUILD_DIES'
  ResearchCompleted,        ///< 'NEW_TECH_BONUS'
  DisasterNoWarn,           ///< 'DISASTER_NO_WARN'
  DisasterWarned,           ///< 'DISASTER_WARNED'
  ConsumerGoods1,           ///< 'CONSUMER_GOODS_1'
  ConsumerGoods2,           ///< 'CONSUMER_GOODS_2'
  ConsumerGoods3,           ///< 'CONSUMER_GOODS_3'
  EnemyGoodBuildingDies,    ///< 'ENEMY_GOOD_DIES'
  EnemyBadBuildingDies,     ///< 'ENEMY_BAD_DIES'
  EnemyVehicleDies,         ///< 'ENEMY_VEH_DIES'
  WorkerTrained,            ///< 'TECH_SCHOOL'
  ScientistTrained,         ///< 'PHD_TRAINED'
  CommandCenterCompleted,   ///< 'CC_BORN'
  EventDeclineRate,         ///< 'EVENT_DEC_RATE'

  // Conditions
  CrowdedNot,               ///< 'CROWDED_NOT'
  CrowdedLow,               ///< 'CROWDED_LOW'
  CrowdedMed,               ///< 'CROWDED_MED'
  CrowdedHigh,              ///< 'CROWDED_HIGH'
  CrowdedMax,               ///< 'CROWDED_MAX'
  FoodSurplus,              ///< 'FOOD_SURPLUS'
  FoodStoresPlentiful,      ///< 'FOOD_DEFICIT_BIGSUPPLY'
  FoodDeficit,              ///< 'FOOD_DEFICIT'
  FoodStarving,             ///< 'FOOD_STARVING'
  DisabledBuildingsLow,     ///< 'DIS_BLD_LOW'
  DisabledBuildingsMed,     ///< 'DIS_BLD_MED'
  DisabledBuildingsHigh,    ///< 'DIS_BLD_HIGH'
  DisabledBuildingsMax,     ///< 'DIS_BLD_MAX'
  RecCenterLow,             ///< 'REC_UT_LOW'
  RecCenterMed,             ///< 'REC_UT_MED'
  RecCenterHigh,            ///< 'REC_UT_HIGH'
  RecCenterMax,             ///< 'REC_UT_MAX'
  ForumLow,                 ///< 'FORUM_UT_LOW'
  ForumMed,                 ///< 'FORUM_UT_MED'
  ForumHigh,                ///< 'FORUM_UT_HIGH'
  ForumMax,                 ///< 'FORUM_UT_MAX'
  MedCenterLow,             ///< 'MED_UT_LOW'
  MedCenterMed,             ///< 'MED_UT_MED'
  MedCenterHigh,            ///< 'MED_UT_HIGH'
  MedCenterMax,             ///< 'MED_UT_MAX'
  NurseryActive,            ///< 'NURSERY_ON'
  UniversityActive,         ///< 'UNIV_ON'
  HasMeteorWarning,         ///< 'DIS_MET_ON'
  HasVolcanoWarning,        ///< 'DIS_VOL_ON'
  HasTornadoWarning,        ///< 'DIS_SAND_ON'
  HasEarthquakeWarning,     ///< 'DIS_QUAKE_ON'
  HasLightningWarning,      ///< 'DIS_ELEC_ON'
  DIRTLow,                  ///< 'DIRT_LOW'
  DIRTMed,                  ///< 'DIRT_MED'
  DIRTHigh,                 ///< 'DIRT_HIGH'
  DIRTMax,                  ///< 'DIRT_MAX'
  UnoccupiedLow,            ///< 'UNOC_LOW'
  UnoccupiedMed,            ///< 'UNOC_MED'
  UnoccupiedMax,            ///< 'UNOC_MAX'
  ScientistsAsWorkersLow,   ///< 'PHD_WHINE_LOW'
  ScientistsAsWorkersMed,   ///< 'PHD_WHINE_MED'
  ScientistsAsWorkersHigh,  ///< 'PHD_WHINE_HIGH'
  ScientistsAsWorkersMax,   ///< 'PHD_WHINE_MAX'
  DifficultyEasy,           ///< 'DIFF_EASY'
  DifficultyMed,            ///< 'DIFF_MED'
  DifficultyHard,           ///< 'DIFF_HARD'
  DifficultyEasyPly,        ///< 'DIFF_EASY_PLY'
  DifficultyMedPly,         ///< 'DIFF_MED_PLY'
  DifficultyHardPly,        ///< 'DIFF_HARD_PLY'

  Count
};

struct MoraleModifier {
  char* pName;     ///< Morale.txt field name
  int   field_04;
  int   field_08;
};


class MoraleManager : public OP2Class<MoraleManager> {
public:
  ibool LoadMoraleFile() { return Thunk<0x44E290, &$::LoadMoraleFile>(); }

  void UpdatePlayerMorale(int playerNum) { return Thunk<0x44E660, &$::UpdatePlayerMorale>(playerNum); }

  void OnPopulationDeaths(PlayerImpl* pPlayer, int numDeadKids, int numDeadScientists, int numDeadWorkers)
    { return Thunk<0x44F230, &$::OnPopulationDeaths>(pPlayer, numDeadKids, numDeadScientists, numDeadWorkers); }

  void OnNewScientists(PlayerImpl* pPlayer, int count) { return Thunk<0x44F440, &$::OnNewScientists>(count);       }
  void OnNewKids(PlayerImpl*       pPlayer, int count) { return Thunk<0x44F520, &$::OnNewKids>(pPlayer, count);    }
  void OnNewWorkers(PlayerImpl*    pPlayer, int count) { return Thunk<0x44F640, &$::OnNewWorkers>(pPlayer, count); }

  void OnBuildingDestroyed(PlayerImpl* pPlayer, MapID type)
    { return Thunk<0x44F7E0, &$::OnBuildingDestroyed>(pPlayer, type); }
  void OnBuildingConstructed(PlayerImpl* pPlayer, MapID type)
    { return Thunk<0x44F8B0, &$::OnBuildingConstructed>(pPlayer, type); }

  void OnNewTech(PlayerImpl* pPlayer, int techNum) { return Thunk<0x44F960, &$::OnNewTech>(pPlayer, techNum); }

  void OnDisasterHit(PlayerImpl* pPlayer, ibool warned) { return Thunk<0x44FA00, &$::OnDisasterHit>(pPlayer, warned); }

  void OnConsumerItemComplete(PlayerImpl* pPlayer, MapID type)
    { return Thunk<0x44FB30, &$::OnConsumerItemComplete>(pPlayer, type); }

  void OnKillEnemyBuilding(PlayerImpl* pPlayer, MapID type)
    { return Thunk<0x44FD00, &$::OnKillEnemyBuilding>(pPlayer, type); }
  void OnKillEnemyVehicle(PlayerImpl*  pPlayer, MapID type)
    { return Thunk<0x44FE60, &$::OnKillEnemyVehicle>(pPlayer, type); }

  /// Gets the global MoraleManager object instance.
  static MoraleManager* GetInstance() { return OP2Mem<0x565280, MoraleManager*>(); }

  /// Gets morale event/condition modifiers.
  static auto& GetMoraleModifier(MoraleModifierType type) { return OP2Mem<0x4E1F10, MoraleModifier*>()[size_t(type)]; }

  /// Gets morale bonus modifiers.
  static uint32& GetMoraleBonusModifier(MoraleBonusModifier type) { return OP2Mem<0x565290, uint32*>()[size_t(type)]; }

  /// Gets morale bonus modifier names (field names in Morale.txt).
  static auto& GetMoraleBonusModifierName(MoraleBonusModifier type) { return OP2Mem<0x4E2238, char**>()[size_t(type)]; }

public:
  // ** TODO member variables
  int field_00;
};

} // Tethys
