
#pragma once

#include "Tethys/Common/Memory.h"

namespace Tethys {

enum class GameTermReasons : int;

struct GameStartInfo;
struct MissionResults;
struct CommandPacket;

class IDirectDraw;
class DirectDrawWindow;
class IDlgWnd;
class TFileDialog;
class TLobby;
class TFrame;
class UIState;

class NetTransportLayer;
class GurManager;

/// Exported API controlling the main game instance.
class TApp : public OP2Class<TApp> {
public:
  uint32 GetVersion()           { return Thunk<0x488200, &$::GetVersion>();     }
  void   GetAppDesc(char* pBuf) { return Thunk<0x487A40, &$::GetAppDesc>(pBuf); }

  int  Init()                           { return Thunk<0x485B20, &$::Init>();                     }
  int  InitForDirectDraw()              { return Thunk<0x485EC0, &$::InitForDirectDraw>();        }
  void ParseCommandLine(char* pCmdLine) { return Thunk<0x486350, &$::ParseCommandLine>(pCmdLine); }
  void Run()                            { return Thunk<0x486650, &$::Run>();                      }
  void ShutDown()                       { return Thunk<0x4866E0, &$::ShutDown>();                 }
  int  CreateFrames()                   { return Thunk<0x486590, &$::CreateFrames>();             }
  int  ActivateGame()                   { return Thunk<0x486F40, &$::ActivateGame>();             }
  int  ActivateShell(GameTermReasons gameTermReasons, MissionResults* pMissionResults)
    { return Thunk<0x486DB0, &$::ActivateShell>(gameTermReasons, pMissionResults); }

  ibool AppActive() const { return Thunk<0x401d60, &$::AppActive>(); }
  ibool AppPaused() const { return Thunk<0x401d70, &$::AppPaused>(); }

  void BeginWaitCursor() { return Thunk<0x487150, &$::BeginWaitCursor>(); }
  void EndWaitCursor()   { return Thunk<0x487180, &$::EndWaitCursor>();   }

  uint32 ChecksumExe()   { return Thunk<0x488010, &$::ChecksumExe>();   }
  uint32 ChecksumShell() { return Thunk<0x4880F0, &$::ChecksumShell>(); }

  int ModeSwitch()           { return Thunk<0x486020, &$::ModeSwitch>();           }
  int DisableModeSwitching() { return Thunk<0x486110, &$::DisableModeSwitching>(); }
  int EnableModeSwitching()  { return Thunk<0x486170, &$::EnableModeSwitching>();  }

  IDirectDraw* GetDirectDraw()    const     { return Thunk<0x401D50, &$::GetDirectDraw>();         }
  int          DirectDrawActive() const     { return Thunk<0x401D40, &$::DirectDrawActive>();      }
  int          HideDirectDraw(HWND    hWnd) { return Thunk<0x486190, &$::HideDirectDraw>(hWnd);    }
  int          RestoreDirectDraw(HWND hWnd) { return Thunk<0x486280, &$::RestoreDirectDraw>(hWnd); }

  void  NewGame()                                      { return Thunk<0x4871A0, &$::NewGame>();                       }
  int   StartSingleGame(GameStartInfo* pGameStartInfo) { return Thunk<0x487F80, &$::StartSingleGame>(pGameStartInfo); }
  int   LoadGame(const char* pFilename)                { return Thunk<0x4878F0, &$::LoadGame>(pFilename);             }
  int   GetLoadName()                                  { return Thunk<0x487430, &$::GetLoadName>();                   }
  void  SaveGame(const char* pFilename, TFileDialog* pSaveDialog = nullptr)
    { return Thunk<0x4877E0, &$::SaveGame>(pFilename, pSaveDialog); }
  ibool NetActive()                                    { return Thunk<0x401D10, &$::NetActive>();                     }
  int   DoNetGame(int a, const char* pFilename)        { return Thunk<0x487C20, &$::DoNetGame>(a, pFilename);         }
  void  NetShutdown(int a)                             { return Thunk<0x487E30, &$::NetShutdown>(a);                  }
  ibool LobbyActive()                                  { return Thunk<0x401D20, &$::LobbyActive>();                   }
  void  SetLobby(TLobby* pLobby)                       { return Thunk<0x401D30, &$::SetLobby>(pLobby);                }
  void  LobbyShutdown()                                { return Thunk<0x487F60, &$::LobbyShutdown>();                 }

  void OnActivateApp(int a)                { return Thunk<0x4862F0, &$::OnActivateApp>(a);        }
  void OnPauseGame(int a)                  { return Thunk<0x486330, &$::OnPauseGame>(a);          }
  void OnLoadScript(const char* pFilename) { return Thunk<0x487590, &$::OnLoadScript>(pFilename); }
  void OnSaveSlot()                        { return Thunk<0x4871D0, &$::OnSaveSlot>();            }
  int  OnLoadSlot()                        { return Thunk<0x487300, &$::OnLoadSlot>();            }
  void OnSaveGame()                        { return Thunk<0x4874E0, &$::OnSaveGame>();            }
  int  OnLoadGame()                        { return Thunk<0x4874F0, &$::OnLoadGame>();            }

  int HandleCommand(uint32 a)                           { return Thunk<0x486810, &$::HandleCommand>(a);               }
  int PlaybackCommand(CommandPacket* pCmdPacket, int a) { return Thunk<0x4864A0, &$::PlaybackCommand>(pCmdPacket, a); }

  int PumpMessage()                   { return Thunk<0x4867B0, &$::PumpMessage>();           }
  int SetUIState(UIState* pUIState)   { return Thunk<0x487030, &$::SetUIState>(pUIState);    }
  int InstallFilterFunc(int a, int b) { return Thunk<0x4883B0, &$::InstallFilterFunc>(a, b); }

  void      SetMainInst(HINSTANCE hInst) { return Thunk<0x401D80, &$::SetMainInst>(hInst); }
  HINSTANCE GetMainInst()  const         { return Thunk<0x401D90, &$::GetMainInst>();      }
  HINSTANCE GetAccelInst() const         { return Thunk<0x401DF0, &$::GetAccelInst>();     }
  HINSTANCE GetBmpInst()   const         { return Thunk<0x401DB0, &$::GetBmpInst>();       }
  HINSTANCE GetCurInst()   const         { return Thunk<0x401DC0, &$::GetCurInst>();       }
  HINSTANCE GetDlgInst()   const         { return Thunk<0x401DA0, &$::GetDlgInst>();       }
  HINSTANCE GetIconInst()  const         { return Thunk<0x401DE0, &$::GetIconInst>();      }
  HINSTANCE GetMenuInst()  const         { return Thunk<0x401DD0, &$::GetMenuInst>();      }
  void      GetHelp()                    { return Thunk<0x4882B0, &$::GetHelp>();          }

  static long STDCALL F1DetectHook(int m, uint32 w, long l) { return OP2Thunk<0x488420, &$::F1DetectHook>(m, w, l); }

  static TApp* GetInstance() { return OP2Mem<0x56E868, TApp*>(); }

  static GUID* GetGameIdentifier() { return OP2Mem<0x4E9B18, GUID*>(); }  // {5A55CF11-B841-11CE-9210-00AA006C4972}
  
  // Helpers for calling other global functions
  static int FASTCALL OP2MessageBox(HWND hOwnerWnd, const char* pMsg, const char* pTitle, uint32 flags)
    { return OP2Thunk<0x41E0E0, &$::OP2MessageBox>(hOwnerWnd, pMsg, pTitle, flags); }

  static uint32 FASTCALL ChecksumData(const void* pMemory, size_t size)
    { return OP2Thunk<0x40C050, &$::ChecksumData>(pMemory, size); }
  static ibool  FASTCALL ChecksumScript(const char* pFilename, int pOut[14])
    { return OP2Thunk<0x44FFE0, ibool FASTCALL(int*, const char*)>(&pOut[0], pFilename); }
  static uint32 FASTCALL ChecksumGameState()               { return OP2Thunk<0x40C0B0, &$::ChecksumGameState>(); }
  static void   SetDebugMsgOnChecksumGameState(bool state) { OP2Mem<0x4DF1A0, ibool&>() = state;                 }

  template <typename T>  static uint32 Checksum(const T& data) { return ChecksumData(&data, sizeof(data)); }

public:
  HINSTANCE hInstance_;
  HINSTANCE hOut2ResLib_;

  int   nShowCmd_;
  ibool bShowShell_;
  char* pSavedGameName_;
  int   field_14;
  ibool playback_;
  int   field_1C;
  char* pNetProtocolName_;
  MSG   msg_;

  HHOOK              hHelpHook_;
  TFrame*            pMainWindow_;
  HINSTANCE          hShellLib_;
  HWND               hShellWnd_;
  GurManager*        pGurManager_;
  TLobby*            pNetProtocolManager_;
  NetTransportLayer* pNetTransportLayer_;
  char               str_[MAX_PATH];

  int     cursorWaitCount_;
  HCURSOR hOldCursor_;

  ibool canUseMMX_;
  ibool gameInitialized_;
  ibool isDrawing_;
  BOOL  fActive_;
  ibool paused_;

  DWORD             pDirectDrawCreate_;
  IDirectDraw*      pDirectDraw_;
  HINSTANCE         hDDrawLib_;
  DirectDrawWindow* pDirectDrawWindow_;
};
static_assert(sizeof(TApp) == 0x18C,  "Incorrect TApp size.");

inline TApp& g_tApp = *TApp::GetInstance();

} // Tethys
