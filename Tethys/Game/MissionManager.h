
#pragma once

#include "Tethys/Common/Memory.h"
#include "Tethys/API/Mission.h"

namespace Tethys {

class MapObject;
class StreamIO;
class ScBase;

/// Structure describing a mission script.  Returned by GetModuleDesc().
struct AIModDesc {
  TethysAPI::ModDesc descBlock;
  char*              pMapName;
  char*              pLevelDesc;
  char*              pTechtreeName;
  int                checksum;
};

/// Internal MissionManager class.
class MissionManager : public OP2Class<MissionManager> {
public:
   MissionManager() { InternalCtor<0x402B10>(); }
  ~MissionManager() { Deinit();                 }

  void Deinit() { return Thunk<0x402C20, &$::Deinit>(); }

  ibool LoadScript(char* pFilename) { return Thunk<0x402B20, &$::LoadScript>(pFilename); }
  int   Checksum()                  { return Thunk<0x403650, &$::Checksum>();            }
  ibool InitLevel()                 { return Thunk<0x402F20, &$::InitLevel>();           }
  void  ProcessScStubs()            { return Thunk<0x403230, &$::ProcessScStubs>();      }

  void OnCreateUnit(MapObject*   pUnit, uint16 scGroupIndex)
    { return Thunk<0x403200, &$::OnCreateUnit>(pUnit, scGroupIndex); }
  void OnTransferUnit(MapObject* pUnit, int srcPlayerNum, int dstPlayerNum)
    { return Thunk<0x4031A0, &$::OnTransferUnit>(pUnit, srcPlayerNum, dstPlayerNum); }

  void GoAI(int    playerNum) { return Thunk<0x402F80, &$::GoAI>(playerNum);    }
  void GoHuman(int playerNum) { return Thunk<0x403070, &$::GoHuman>(playerNum); }

  ibool Save(StreamIO* pSavedGameFile) { return Thunk<0x402C60, &$::Save>(pSavedGameFile); }
  ibool Load(StreamIO* pSavedGameFile) { return Thunk<0x402DC0, &$::Load>(pSavedGameFile); }

  ScBase* GetDefaultScGroup(int playerNum) { return Thunk<0x4036A0, &$::GetDefaultScGroup>(playerNum); }
  void    SetDefaultScGroup(int playerNum, ScBase* pScGroup)
    { return Thunk<0x4036C0, &$::SetDefaultScGroup>(playerNum, pScGroup); }

  void* GetExportAddress(const char* pExportName, ibool useLevelModule)
    { return Thunk<0x4035D0, &$::GetExportAddress>(pExportName, useLevelModule); }

  // ** TODO 0x402950
  // ** TODO 0x402960
  // ** TODO 0x402980
  // ** TODO 0x402990
  // ** TODO 0x4029B0
  // ** TODO 0x4029C0 GetMissionTypeFromFilenameAndCheckIfMulti
  // ** TODO 0x4030C0 Something to do with StrategyLinkedList
  // ** TODO 0x4030E0 Something to do with TriggerLinkedList
  // ** TODO 0x403140 Something to do with GroupLinkedList
  // ** TODO 0x403520 Something to do with victory conditions
  // ** TODO 0x403540
  // ** TODO 0x403570
  // ** TODO 0x4035B0 Something to do with failure conditions

  static void* FASTCALL GetExportFileAddress(void* pMappedFileAddress, size_t fileSize, const char* pExportName)
    { return OP2Thunk<0x419480, &$::GetExportFileAddress>(pMappedFileAddress, fileSize, pExportName); }

  /// Exported global function to get DescBlock.  Call FreeModuleDesc() to destroy the returned memory.
  static AIModDesc* FASTCALL GetModuleDesc(const char* pFilename)
    { return OP2Thunk<0x4025B0, &$::GetModuleDesc>(pFilename); }

  /// Exported global function to free the AIModDesc allocated by GetModuleDesc().
  static void FASTCALL FreeModuleDesc(AIModDesc* pModDesc) { return OP2Thunk<0x402890, &$::FreeModuleDesc>(pModDesc); }

  /// Internal global function to get DescBlockEx.  Call FreeModuleDescEx() to destroy the returned memory.
  static TethysAPI::ModDescEx* GetModuleDescEx(const char* pFilename)
    { return OP2Thunk<0x402780, &$::GetModuleDescEx>(pFilename); }

  /// Frees the ModDescEx allocated by GetModuleDescEx.
  static void FreeModuleDescEx(TethysAPI::ModDescEx* pDescBlockEx) { operator delete(pDescBlockEx, OP2Heap); }

  /// Gets the global MissionManager instance.
  static MissionManager* GetInstance() { return OP2Mem<0x4EFD18, MissionManager*>(); }

public:
  struct DefaultScGroupInfo {
    int   defaultScGroupIndex;  ///< Initialized to -1 if human, or to a FightGroup index if AI
    ibool isUserManaged;        ///< Initialized to false.  If false, when SetDefaultGroup() is called, the old
                                ///  default group is destroyed and this gets set to true.
  };

  HINSTANCE             hModule_;
  int  (CDECL*          pfnInitProc_)();
  void (CDECL*          pfnAIProc_)();
  int  (CDECL*          pfnStatusProc_)();
  TethysAPI::SaveRegion saveRegion_;
  char*                 pScriptName_;
  DefaultScGroupInfo    defaultScGroupInfo_[6];
  AIModDesc*            pDescBlock_;
};

} // Tethys
