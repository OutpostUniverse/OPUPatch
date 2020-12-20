
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/Game/PlayerImpl.h"

namespace Tethys {

struct GameStartInfo;
class  StreamIO;
class  TextStream;

/// "CATEGORY" values (for both items and upgrades to these items)
enum class TechCategory : int {
  Free                =  0,  ///< Free technologies (and unavailable technologies)
  Basic               =  1,  ///< Basic labratory sciences
  Defenses            =  2,  ///< Defenses (GP upgrade, walls, and efficiency engineering)
  Power               =  3,  ///< Power
  Vehicles            =  4,  ///< Vehicles (which ones can be built, speed upgrades, armor upgrades)
  Food                =  5,  ///< Food
  Metals              =  6,  ///< Metals gathering
  Weapons             =  7,  ///< Weapons
  Space               =  8,  ///< Space (spaceport, observatory, launch vehicle, skydock)
  PopulationHappiness =  9,  ///< Population (happiness)
  Disaster            = 10,  ///< Disaster warning (and defense)
  PopulationGrowth    = 11,  ///< Population (health, growth)
  SpaceshipModule     = 12,  ///< Spaceship module
};

enum class TechLabType : int {
  BasicLab    = 1,
  StandardLab = 2,
  AdvancedLab = 3,
};

enum class TechUpgradeType : int {
  UnitProp       = 0,  ///< 'UNIT_PROP'
  PlayerProp     = 1,  ///< 'PLAYER_PROP';  @warning Desyncs in multiplayer!
  FunctionResult = 2,  ///< 'FUNCTION_RESULT'
};

struct TechProperty {
  TechUpgradeType type;          ///< Type of upgrade.
  char*           pUpgradeType;  ///< String that follows the "UNIT_PROP"/etc. tag in the sheets.
  union {
    size_t        offsetToProp;  ///< [UnitProp, PlayerProp]  Offset of property in the upgraded structure.
                                 ///                          MapObjectType::playerStats_[playerNum] for UnitProp;
                                 ///                          GameImpl::player_[playerNum] for PlayerProp.
    void(*pfnFunctionResult)();  ///< [FunctionResult]  Pointer to function to be called.
  };
};

struct TechUpgradeInfo {
  TechProperty* pType;     ///< Pointer to struct describing the type of upgrade.
  MapID         unitType;  ///< Type of unit this upgrade applies to.
  int           newValue;  ///< New value of property being upgraded.
};

struct TechInfo : public OP2Class<TechInfo> {
  ibool ParseTech(TextStream* pTechParser) { return Thunk<0x473A80, &$::ParseTech>(pTechParser); }

  int          techID;     ///< TechID found in the sheets files.
  TechCategory category;   ///< In sheets "CATEGORY" tells what kind this tech is.
  int          techLevel;  ///< Tech level of this tech (the thousands digit).

  int         plymouthCost;   ///< Amount of research required by Plymouth.
  int         edenCost;       ///< Amount of research required by Eden.
  int         maxScientists;  ///< Max number of scientists allowed to assign to research.
  TechLabType lab;            ///< Lab type research is done at.

  PlayerBitmask playerHasTechMask;  ///< Bitmask of which players have this technology.
  
  int numUpgrades;        ///< Number of unit upgrades it performs ("UNIT_PROP"s in tech file).
  int numRequiredTechs;   ///< Number of other techs required to research this one ("REQUIRES"s in tech file).

  char* pTechName;     ///< Name of the technology (in sheets files).
  char* pDescription;  ///< Description of tech given after research.
  char* pTeaser;       ///< Description of tech given before research.
  char* pImproveDesc;  ///< Description of the upgrades given by this tech.

  int*             pRequiredTechNum;    ///< Pointer to array of techNums that this tech requires.
  TechUpgradeInfo* pUpgrades;           ///< Pointer to array of structs containing upgrade info.
  int              numDependentTechs;   ///< Number of technologies dependent on this one.
  int*             pDependentTechNums;  ///< Pointer to array of techNums that depend on this tech.
};


/// Internal research manager class.
class Research : public OP2Class<Research> {
public:
  Research() { InternalCtor<0x472930>(); }

  int   GetTechNum(int techID)                      { return Thunk<0x472D90, &$::GetTechNum>(techID);                  }
  ibool HasTech(int playerNum, int techNum)         { return Thunk<0x472D10, &$::HasTech>(playerNum, techNum);         }
  ibool CanResearchTech(int playerNum, int techNum) { return Thunk<0x4733C0, &$::CanResearchTech>(playerNum, techNum); }

  void GiveInitialTechs()                      { return Thunk<0x472CF0, &$::GiveInitialTechs>();              }
  void SetTechLevel(int playerNum, int techID) { return Thunk<0x473030, &$::SetTechLevel>(playerNum, techID); }
  void MarkResearchComplete(int playerNum, int techNum)
    { return Thunk<0x4738F0, &$::MarkResearchComplete>(playerNum, techNum); }
  void GiveClosureOfTech(int playerNum, int techNum)
    { return Thunk<0x472E40, &$::GiveClosureOfTech>(playerNum, techNum); }
  void GiveTechUpgrades(int playerNum, int techNum)
    { return Thunk<0x473920, &$::GiveTechUpgrades>(playerNum, techNum); }

  void LoadTechFile(const char* pFilename, int maxTechLevel)
    { return Thunk<0x473470, &$::LoadTechFile>(pFilename, maxTechLevel); }
  void ParseTechFile(const char* pFilename, int maxTechID)
    { return Thunk<0x472940, &$::ParseTechFile>(pFilename, maxTechID); }

  void Deinit() { return Thunk<0x473240, &$::Deinit>(); }

  ibool SaveStartState(GameStartInfo* pGameStartInfo) { return Thunk<0x473700, &$::SaveStartState>(pGameStartInfo); }
  ibool LoadStartState(GameStartInfo* pGameStartInfo) { return Thunk<0x473830, &$::LoadStartState>(pGameStartInfo); }
  ibool Save(StreamIO* pSavedGame)                    { return Thunk<0x473580, &$::Save>(pSavedGame);               }
  ibool Load(StreamIO* pSavedGame)                    { return Thunk<0x473630, &$::Load>(pSavedGame);               }
  int   Checksum()                                    { return Thunk<0x472FF0, &$::Checksum>();                     }

  // ** TODO 0x473210
  // ** TODO 0x4732B0
  // ** TODO 0x473330

  /// Get the global Research object instance.
  static Research* GetInstance() { return OP2Mem<0x56C230, Research*>(); }

  // Helper functions
  TechInfo* GetTechInfo(int techID)
    { const int techNum = GetTechNum(techID);  return (techNum != -1) ? ppTechInfos_[techNum] : nullptr; }

  /// Table of techtree format UNIT_PROP/etc. mapping information.
  static TechProperty* TechPropertiesTable() { return OP2Mem<TechProperty*&>(0x473E88); }  // 0x4D5C48
  /// Number of array entries in TechPropertiesTable.  ((0x4D5D48 - 0x4D5C48) / sizeof(TechProperty) = 21)
  static size_t        NumTechProperties()   { return OP2Mem<TechProperty*&>(0x473E2A) - TechPropertiesTable(); }

public:
  int        numTechs_;     ///< Max = 256 (@ref ResearchState can only store 256 entries).
  TechInfo** ppTechInfos_;  ///< Sorted by tech ID (binary searchable).
  int        maxTechID_;    ///< (maxTechLevel * 1000) + 999.  Typically 12999 in non-campaign games.

  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
};

inline auto& g_research = *Research::GetInstance();

} // Tethys
