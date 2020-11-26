
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Patcher.h"

#include "Tethys/Game/TApp.h"
#include "Tethys/Game/MissionDLL.h"
#include "Tethys/Resource/CConfig.h"
#include "Tethys/Resource/ResManager.h"
#include "Tethys/UI/IWnd.h"

#include <iostream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>

using namespace Patcher;
using namespace Patcher::Util;

using MissionList = std::vector<std::pair<std::string, std::unique_ptr<AIModDesc, void(*)(AIModDesc*)>>>;

static constexpr wchar_t PathVar[] = L"PATH";

static constexpr char OPUDir[]  = "OPU";
static constexpr char CoreDir[] = "core";

static char g_baseModDir[] = "base";  // ** TODO this needs to be override-able by mods (e.g. Renegades)
static std::filesystem::path g_curMapPath;

static bool g_searchForMission = false;


// =====================================================================================================================
static std::vector<std::filesystem::path> GetSearchPaths(
  std::string  extension        = "",
  bool         searchForMission = false,
  bool         excludeStockDirs = false)
{
  constexpr auto SearchOptions = std::filesystem::directory_options::follow_directory_symlink |
                                 std::filesystem::directory_options::skip_permission_denied;

  static const auto opuPath(std::filesystem::path(g_resManager.installedDir_)/OPUDir);

  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  std::vector<std::filesystem::path> searchPaths;

  // Search 1) ini debug path if set (highest priority).
  {
    char debugPath[MAX_PATH] = "";
    g_configFile.GetString("DEBUG", "ART_PATH", &debugPath[0], MAX_PATH, "");
    if (debugPath[0] != '\0') {
      searchPaths.emplace_back(debugPath);
    }
  }

  // Search OPU subdirectories under 2) map dir, 3) mod dirs, 4) base mod dir, then 5) core dir.
  // ** TODO Search all mod dirs here instead of op2ext, for now mod dirs are checked with highest priority w/o subdirs
  const std::filesystem::path bases[] = { g_curMapPath, opuPath/g_baseModDir, opuPath/CoreDir };
  for (const auto& base : bases) {
    if (base.empty() == false) {
      static const std::multimap<std::string, std::string> assetDirs = {
        { ".dll",   "libs"     },
        { ".dll",   "maps"     },
        { ".map",   "maps"     },
        { ".txt",   "sheets"   },
        { ".txt",   "techs"    },
        { ".bmp",   "tilesets" },
        { ".bmp",   "sprites"  },
        { ".prt",   "sprites"  },
        { ".raw",   "sprites"  },
        { ".ani",   "cursors"  },
        { ".wav",   "sounds"   },
        { ".wav",   "voices"   },
        { ".wav",   "music"    },
        { ".mp3",   "music"    },
        { ".ogg",   "music"    },
        { ".flac",  "music"    },
        { ".avi",   "movies"   },
        { ".mp4",   "movies"   },
        { ".rtf",   "story"    },
      };

      const auto range = assetDirs.equal_range(extension);
      for (auto it = range.first; it != range.second; ++it) {
        const auto assetDir = base/it->second;

        if (searchForMission && (it->first == ".dll") && (it->second == "maps") && std::filesystem::exists(assetDir)) {
          // Search a) top level of OPU/[base]/maps/* subdirectories if we are searching for mission DLLs.
          for (auto& p : std::filesystem::directory_iterator(assetDir, SearchOptions)) {
            if (p.is_directory()) {
              searchPaths.emplace_back(p);
            }
          }
        }

        // Search b) OPU/[base]/[asset] directory.
        if ((searchForMission == false) || (it->second == "maps")) {
          searchPaths.emplace_back(assetDir);
        }
      }

      // Search c) OPU/[base] directory.
      searchPaths.emplace_back(base);

      if (std::filesystem::exists(base)) {
        // Search d) top level of any OPU/[base]/* subdirectories.
        for (auto& p : std::filesystem::directory_iterator(base, SearchOptions)) {
          if (p.is_directory()) {
            searchPaths.emplace_back(p);
          }
        }
      }
    }
  }

  // Search 6) OPU root directory.
  searchPaths.emplace_back(opuPath);

  // Search 7) all OPU/[base]/maps/* subdirectories if we are searching for .map files.
  if (extension == ".map") {
    for (const auto& base : bases) {
      if (std::filesystem::exists(base/"maps")) {
        for (auto& p : std::filesystem::recursive_directory_iterator(base/"maps", SearchOptions)) {
          if (p.is_directory()) {
            searchPaths.emplace_back(p);
          }
        }
      }
    }
  }

  if (excludeStockDirs == false) {
    // Search 8) Outpost2.exe directory, then 9) Outpost 2 CD directory if available.
    searchPaths.emplace_back(g_resManager.installedDir_);
    if (g_resManager.cdDir_[0] != '.') {
      searchPaths.emplace_back(g_resManager.cdDir_);
    }
  }

  return searchPaths;
}

// =====================================================================================================================
static std::filesystem::path GetFilePath(
  const std::filesystem::path&  filename,
  bool                          searchForMission = false)
{
  constexpr auto SearchOptions = std::filesystem::directory_options::follow_directory_symlink |
    std::filesystem::directory_options::skip_permission_denied;

  static const auto opuPath(std::filesystem::path(g_resManager.installedDir_)/OPUDir);

  std::filesystem::path path = filename;
  bool result = filename.is_absolute();

  if (filename.is_relative()) {
    for (auto& searchPath : GetSearchPaths(filename.extension().string(), searchForMission)) {
      path = searchPath/filename;
      if (std::filesystem::exists(path)) {
        result = true;
        break;
      }
    }
  }

  return result ? path : std::filesystem::path();
}

// =====================================================================================================================
// Replacement function for ResManager::GetFilePath()
static ibool __fastcall GetFilePathHook(
  ResManager*  pThis,  int,
  char*        pResName,
  char*        pOutputFilename)
{
  const std::filesystem::path path = GetFilePath(pResName);

  if (pOutputFilename != nullptr) {
    pOutputFilename[0] = '\0';
    if (path.empty() == false) {
      strncpy_s(pOutputFilename, MAX_PATH, path.string().data(), _TRUNCATE);
    }
  }

  return (path.empty() == false);
}


// =====================================================================================================================
static MissionList GetMissionList(
  MissionType  minMissionType,
  MissionType  maxMissionType,
  int          maxPlayers = 0)
{
  constexpr auto SearchOptions = std::filesystem::directory_options::follow_directory_symlink |
                                 std::filesystem::directory_options::skip_permission_denied;

  if (minMissionType > maxMissionType) {
    std::swap(minMissionType, maxMissionType);
  }

  std::unordered_set<std::string> tested;
  MissionList                     missions;

  for (const auto& searchPath : GetSearchPaths(".dll", true)) {
    if (std::filesystem::exists(searchPath)) {
      for (auto& p : std::filesystem::directory_iterator(searchPath, SearchOptions)) {
        const auto& path = p.path();

        if (path.has_extension() && (path.extension() == ".dll")) {
          std::string filename = path.filename().string();
          std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

          if (tested.count(filename) == 0) {
            AIModDesc*const pModDesc = MissionDLL::GetModuleDesc(path.string().data());

            if (pModDesc != nullptr) {
              if ((pModDesc->descBlock.missionType >= minMissionType) &&
                  (pModDesc->descBlock.missionType <= maxMissionType) &&
                  (pModDesc->descBlock.numPlayers  >= maxPlayers))
              {
                missions.emplace_back(
                  filename,
                  MissionList::value_type::second_type(pModDesc, [](AIModDesc* p) { MissionDLL::FreeModuleDesc(p); }));
              }
              else {
                MissionDLL::FreeModuleDesc(pModDesc);
              }
            }

            tested.emplace(filename);
          }
        }
      }
    }
  }

  return missions;
}

// =====================================================================================================================
static MissionList GetMissionList(
  MissionType  missionType,
  int          maxPlayers = 0)
{
  return GetMissionList(missionType, missionType, maxPlayers);
}

// =====================================================================================================================
static MissionList GetMissionList(
  int maxPlayers = 0)
{
  return GetMissionList(MissionType{INT32_MIN}, MissionType{INT32_MAX}, maxPlayers);
}

// =====================================================================================================================
static void* ShellAlloc(
  size_t size)
{
  static auto*const pfnAlloc =
    reinterpret_cast<void*(CDECL*)(size_t)>(0x1300A320 - OP2ShellBase + uintptr(GetModuleHandleA("OP2Shell.dll")));
  return pfnAlloc(size);
}

// =====================================================================================================================
// Replacement function for PopulateMultiplayerMissionList()
static int __fastcall PopulateMultiplayerMissionListHook(
  HWND         hComboBoxWnd,
  int          maxPlayers,
  MissionType  maxMissionType,
  MissionType  minMissionType)
{
  SendMessageA(hComboBoxWnd, CB_RESETCONTENT, 0, 0);
  SendMessageA(hComboBoxWnd, WM_SETREDRAW,    0, 0);

  const auto missions = GetMissionList(minMissionType, maxMissionType, maxPlayers);

  for (const auto& [filename, pAIModDesc] : missions) {
    char*const pFilenameBuf = static_cast<char*>(ShellAlloc(filename.length() + 1));
    if (pFilenameBuf != nullptr) {
      strncpy_s(pFilenameBuf, filename.length() + 1, filename.data(), _TRUNCATE);
      const LRESULT entry = SendMessageA(hComboBoxWnd, CB_ADDSTRING, 0, LPARAM(pAIModDesc->pLevelDesc));
      SendMessageA(hComboBoxWnd, CB_SETITEMDATA, entry, LPARAM(pFilenameBuf));
    }
  }

  SendMessageA(hComboBoxWnd, CB_SETCURSEL, 0, 0);
  SendMessageA(hComboBoxWnd, WM_SETREDRAW, 1, 0);

  return missions.size();
}

// =====================================================================================================================
// Replacement function for SinglePlayerGameDialog::PopulateMissionLis()
static void __fastcall PopulateSinglePlayerMissionListHook(
  IDlgWnd* pThis)
{
  const HWND hListBoxWnd = GetDlgItem(pThis->hWnd_, 1076);
  auto*const pFileFilter = static_cast<char*>(PtrInc(pThis, 0xAA));  // ** TODO Define SinglePlayerGameDialog
  const MissionType type = (pFileFilter[0] == 't') ? MissionType::Tutorial : MissionType::Colony;

  SendMessageA(hListBoxWnd, LB_RESETCONTENT, 0, 0);
  SendMessageA(hListBoxWnd, WM_SETREDRAW,    0, 0);

  for (const auto& [filename, pAIModDesc] : GetMissionList(type)) {
    char*const pFilenameBuf = static_cast<char*>(ShellAlloc(filename.length() + 1));
    if (pFilenameBuf != nullptr) {
      strncpy_s(pFilenameBuf, filename.length() + 1, filename.data(), _TRUNCATE);
      const LRESULT listEntry = SendMessageA(hListBoxWnd, LB_ADDSTRING, 0, LPARAM(pAIModDesc->pLevelDesc));
      SendMessageA(hListBoxWnd, LB_SETITEMDATA, listEntry, LPARAM(pFilenameBuf));
    }
  }

  SendMessageA(hListBoxWnd, LB_SETCURSEL, 0, 0);
  SendMessageA(hListBoxWnd, WM_SETREDRAW, 1, 0);
}


// =====================================================================================================================
static std::wstring GetPathEnv() {
  std::unique_ptr<wchar_t[]> pTmp(new wchar_t[_MAX_ENV]);
  pTmp[0] = L'\0';
  GetEnvironmentVariableW(PathVar, pTmp.get(), _MAX_ENV);
  return std::wstring(pTmp.get());
}

// =====================================================================================================================
// Adds DLL search paths.
// This does not use AddDllDirectory() because it's not WinXP-compatible, and multiple path search order is undefined.
static void AddModuleSearchPaths(
  std::vector<std::filesystem::path>  paths,
  bool                                ifUnique = false)
{
  std::wstring pathEnv  = GetPathEnv();
  bool         modified = false;

  std::wstringstream ss(ifUnique ? pathEnv : L"");
  for (auto& src: paths) {
    const std::filesystem::path path = (src.is_relative() || src.empty()) ? (g_resManager.installedDir_/src) : src;
    if (std::filesystem::exists(path)) {
      bool found = (ifUnique == false);

      ss.seekg(0);
      for (std::wstring token; ((found == false) && std::getline(ss, token, L';'));) {
        found = (path == token) || ((path/"") == token);
      }

      if (found == false) {
        pathEnv.insert(0, path.wstring() + (pathEnv.empty() ? L"" : L";"));
        modified = true;
      }
    }
  }

  if (modified) {
    SetEnvironmentVariableW(PathVar, pathEnv.data());
  }
}

// =====================================================================================================================
// Removes DLL search paths.
static void RemoveModuleSearchPaths(
  std::vector<std::filesystem::path> paths)
{
  std::wstring pathEnv  = GetPathEnv();
  bool         modified = false;

  std::wstringstream ss(pathEnv);
  std::vector<std::pair<std::wstring::iterator, std::wstring::iterator>> toDelete;
  
  for (auto& src: paths) {
    const std::filesystem::path path = src.is_relative() ? (g_resManager.installedDir_/src) : src;
    ss.seekg(0);
    for (std::wstring token; std::getline(ss, token, L';');) {
      if ((path == token) || ((path/"") == token)) {
        auto end = pathEnv.begin() + ss.tellg();
        toDelete.emplace_back(end - token.length(), (end != pathEnv.end()) ? (end + 1) : end);
        modified = true;
      }
    }

    for (auto it = toDelete.rbegin(); it != toDelete.rend(); ++it) {
      pathEnv.erase(it->first, it->second);
    }
  }

  if (modified) {
    SetEnvironmentVariableW(PathVar, pathEnv.data());
  }
}


// =====================================================================================================================
static void SetBaseModuleSearchPaths(
  const char* pBaseModDir)
{
  static std::string oldBaseModDir = "";
  if (pBaseModDir != oldBaseModDir) {
    if (oldBaseModDir.empty() == false) {
      const auto oldBase(std::filesystem::path(OPUDir)/oldBaseModDir);
      RemoveModuleSearchPaths({ oldBase, oldBase/"maps", oldBase/"libs" });
    }
    oldBaseModDir = pBaseModDir;
  }

  const auto base(std::filesystem::path(OPUDir)/pBaseModDir);
  AddModuleSearchPaths({ base, base/"maps", base/"libs" });
}


// =====================================================================================================================
// Prefers loading DLL import dependencies from the new module's file directory first, instead of the base module's.
// Also adds module directory to PATH for the sake of LoadLibrary().
// See https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
static HMODULE WINAPI LoadModuleAltSearchPath(
  const char* pFilename)
{
  const std::filesystem::path path = std::filesystem::absolute(GetFilePath(pFilename, g_searchForMission));
  AddModuleSearchPaths({ path.parent_path() }, true);
  return LoadLibraryExW(path.wstring().data(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
}

// =====================================================================================================================
void InitModuleSearchPaths() {
  const std::filesystem::path opuPath(OPUDir);
  const std::filesystem::path core(opuPath/CoreDir);

  // If this is called before TApp::Init(), then we need to init g_resManager's directories.
  if (g_resManager.installedDir_[0] == '\0') {
    g_resManager.InitInstalledDir();
  }
  if (g_resManager.cdDir_[0] == '\0') {
    g_resManager.InitCDDir();
  }

  if (std::filesystem::current_path() == std::filesystem::canonical(g_resManager.installedDir_)) {
    SetCurrentDirectoryW((g_resManager.installedDir_/opuPath).wstring().data());
  }

  AddModuleSearchPaths({ "", opuPath, core, core/"maps", core/"libs" });
  SetBaseModuleSearchPaths(&g_baseModDir[0]);  // ** TODO override?
}

// =====================================================================================================================
bool SetFileSearchPathPatch(
  bool enable)
{

  static PatchContext op2Patcher;
  static PatchContext shellPatcher;

  static std::vector<HMODULE> hModules;

  static const std::wstring oldPathEnv(GetPathEnv());
  static const std::wstring oldCwd = [] {
    wchar_t cwd[MAX_PATH] = L"";
    GetCurrentDirectoryW(MAX_PATH, &cwd[0]);
    return std::wstring(cwd);
  }();


  static bool inited  = false;
         bool success = true;

  if (enable) {
    if (inited == false) {
      InitModuleSearchPaths();
      inited = true;

      // OPUPatch needs to keep some DLLs loaded to patch them;  load them here so they get loaded from the correct path.
      if (hModules.empty()) {
        for (const char* pFilename : { "OP2Shell.dll", "odasl.dll" }) {
          const HMODULE hModule = LoadModuleAltSearchPath(pFilename);
          if (hModule != NULL) {
            hModules.push_back(hModule);
          }
        }

        if (hModules.empty() == false) {
          atexit([] { for (HMODULE hModule : hModules) { FreeLibrary(hModule); }  hModules.clear(); });
        }

        shellPatcher.SetModule("OP2Shell.dll", true);
      }
    }

    // Replace ResManager::GetFilePath()
    op2Patcher.Hook(0x471590, &GetFilePathHook);

    // Replace PopulateMultiplayerMissionList()
    op2Patcher.Hook(0x497780, &PopulateMultiplayerMissionListHook);
    
    // Replace {ColonyGameDialog,TutorialDialog}::PopulateMissionList()
    shellPatcher.Hook(0x13006860, &PopulateSinglePlayerMissionListHook);

    // In RunScriptDialog::DlgProc()
    shellPatcher.LowLevelHook(0x13004A79, [](Ebx<IDlgWnd*> pThis) {
      const HWND hListBoxWnd = GetDlgItem(pThis->hWnd_, 1076);
      for (const auto& [filename, pAIModDesc] : GetMissionList()) {
        SendMessageA(hListBoxWnd, LB_ADDSTRING, 0, LPARAM(filename.data()));
      }
      return 0x13004C38;
    });

    // In TethysGame::PrepareGame()
    op2Patcher.LowLevelHook(0x48942E, [] { g_searchForMission = true; });
    op2Patcher.HookCall(0x489444, FastcallLambdaPtr([](const char* pFilename) {
      g_searchForMission = false;
      const std::filesystem::path path(pFilename);
      if (path.is_absolute()) {
        g_curMapPath = path.parent_path();
      }
      return MissionDLL::GetModuleDesc(pFilename);
    }));

    static std::wstring preMissionPathEnv;

    // In MissionDLL::LoadScript()
    op2Patcher.HookCall(0x402B44, StdcallLambdaPtr([](const char* pFilename) {
      std::filesystem::path path(pFilename);

      if (path.is_relative()) {
        char buf[MAX_PATH] = "";
        if (g_resManager.GetFilePath(pFilename, &buf[0])) {
          path = buf;
        }
      }

      path                  = std::filesystem::absolute(path);
      g_curMapPath          = path.parent_path();
      g_searchForMission    = true;
      preMissionPathEnv     = GetPathEnv();
      AddModuleSearchPaths({ g_curMapPath, g_curMapPath/"libs" }, true);
      const HMODULE hModule = LoadModuleAltSearchPath(path.string().data());
      g_searchForMission    = false;

      return hModule;
    }));

    // In MissionDLL::Deinit()
    op2Patcher.HookCall(0x402C2A, StdcallLambdaPtr([](HMODULE hModule) -> BOOL {
      const BOOL result = FreeLibrary(hModule);
      if (preMissionPathEnv.empty() == false) {
        SetEnvironmentVariableW(PathVar, preMissionPathEnv.data());
        preMissionPathEnv.clear();
      }
      g_curMapPath.clear();
      return result;
    }));

    // In MissionDLL::Load()
    op2Patcher.LowLevelHook(0x402E46, [] { g_searchForMission = true;  });
    op2Patcher.LowLevelHook(0x402E50, [] { g_searchForMission = false; });

    // In ChecksumScript()
    op2Patcher.LowLevelHook(0x450036, [] { g_searchForMission = true;  });
    op2Patcher.LowLevelHook(0x450041, [] { g_searchForMission = false; });

    // In MultiplayerPreGameSetupWnd::SetControlInfo()
    op2Patcher.LowLevelHook(0x460B0C, [] { g_searchForMission = true;  });
    op2Patcher.LowLevelHook(0x460B18, [] { g_searchForMission = false; });

    // In ???
    op2Patcher.LowLevelHook(0x460786, [] { g_searchForMission = true;  });
    op2Patcher.LowLevelHook(0x460791, [] { g_searchForMission = false; });

    // In StartSierraNW() (SierraNW.dll)
    op2Patcher.HookCall(0x47D2B1, &LoadModuleAltSearchPath);
    // In StartSNWValid() (SNWValid.dll)
    op2Patcher.LowLevelHook(0x47D56C, [](Ebp<void*>& pfn) { pfn = &LoadModuleAltSearchPath; });
    // In TApp::Init() (Out2res.dll)
    op2Patcher.HookCall(0x485C76, &LoadModuleAltSearchPath);

    success = (op2Patcher.GetStatus() == PatcherStatus::Ok) && (shellPatcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    if (inited) {
      SetEnvironmentVariableW(PathVar, oldPathEnv.data());
      SetCurrentDirectoryW(oldCwd.data());
      inited = false;
    }

    success &= (op2Patcher.RevertAll() == PatcherStatus::Ok) && (shellPatcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}
