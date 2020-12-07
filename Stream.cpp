
// ** TODO Most of this should probably be moved into op2ext due to overlap with mod loader functionality.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#include "Patcher.h"
#include "Library.h"

#include "Tethys/Game/TApp.h"
#include "Tethys/Game/MissionManager.h"
#include "Tethys/Resource/CConfig.h"
#include "Tethys/Resource/ResManager.h"
#include "Tethys/UI/IWnd.h"

#include <iostream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>

using namespace Tethys;
using namespace Patcher::Util;
using namespace Patcher::Registers;

using MissionList = std::vector<std::pair<std::string, std::unique_ptr<AIModDesc, void(*)(AIModDesc*)>>>;

static constexpr wchar_t PathVar[] = L"PATH";

static constexpr char OPUDir[]  = "OPU";
static constexpr char BaseDir[] = "base";

static std::filesystem::path g_curMapPath;

static bool g_searchForMission = false;


// =====================================================================================================================
static std::vector<std::filesystem::path> GetModPaths() {
  static std::vector<std::filesystem::path> paths;

  static Library op2Ext("op2ext.dll");
  static auto*const pfnGetModuleDirectoryCount = op2Ext.Get<size_t CDECL()>("GetModuleDirectoryCount");
  static auto*const pfnGetModuleDirectory      =
    op2Ext.Get<size_t CDECL(size_t moduleIndex, char* buffer, size_t bufferSize)>("GetModuleDirectory");

  if (paths.empty() && (pfnGetModuleDirectoryCount != nullptr) && (pfnGetModuleDirectory != nullptr)) {
    char buf[MAX_PATH] = "";
    for (size_t i = 0, count = pfnGetModuleDirectoryCount(); i < count; ++i) {
      if (pfnGetModuleDirectory(i, &buf[0], MAX_PATH) == 0) {
        paths.emplace_back(&buf[0]);
      }
    }
  }

  return paths;
}


// =====================================================================================================================
static std::vector<std::filesystem::path> GetSearchPaths(
  std::string  extension        = "",
  bool         searchForMission = false,
  bool         excludeStockDirs = false)
{
  constexpr auto SearchOptions = std::filesystem::directory_options::follow_directory_symlink |
                                 std::filesystem::directory_options::skip_permission_denied;

  const auto installPath(std::filesystem::path(g_resManager.installedDir_));
  const auto opuPath(installPath/OPUDir);

  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  std::vector<std::filesystem::path> searchPaths;
  std::set<std::filesystem::path>    knownPaths;
  auto AddPath = [&searchPaths, &knownPaths](const std::filesystem::path& path) {
    if (path.is_absolute() && (knownPaths.count(path) == 0)) {
      searchPaths.emplace_back(path);
      knownPaths.insert(path);
    }
  };

  // Search 1) ini debug path if set.
  {
    char debugPath[MAX_PATH] = "";
    g_configFile.GetString("DEBUG", "ART_PATH", &debugPath[0], MAX_PATH);
    if (debugPath[0] != '\0') {
      AddPath(debugPath);
    }
  }

  // Search OPU subdirectories under 2) map dir, 3) mod dirs, 4) base dir, then 5) OPU dir.
  // ** TODO For now mod dirs also get checked with highest priority by op2ext's hook
  std::vector<std::filesystem::path> bases;

  if ((g_curMapPath != opuPath/BaseDir) && (g_curMapPath != opuPath) && (g_curMapPath != installPath)) {
    bases.push_back(g_curMapPath);
  }

  const auto& modPaths = GetModPaths();
  bases.insert(bases.end(), modPaths.begin(), modPaths.end());

  bases.push_back(opuPath/BaseDir);
  bases.push_back(opuPath);

  for (const auto& base : bases) {
    if (base.empty() == false) {
      static const std::multimap<std::string, std::string> assetDirs = {
        { ".ax",    "libs"     },
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
        { ".op2",   "saves"    }
      };

      const auto range = assetDirs.equal_range(extension);
      for (auto it = range.first; it != range.second; ++it) {
        const auto assetDir = base/it->second;

        if (std::filesystem::exists(assetDir)) {
          if (searchForMission && (it->first == ".dll") && (it->second == "maps")) {
            // Search a) top level of [base]/maps/* subdirectories iff we are searching for mission DLLs.
            // This lets e.g. OPU/base/maps/AxensHome/ml6_21.dll be found.
            for (auto& p : std::filesystem::directory_iterator(assetDir, SearchOptions)) {
              if (p.is_directory()) {
                AddPath(p);
              }
            }
          }

          // Search b) [base]/[asset] directory.
          AddPath(assetDir);
        }
      }

      if (std::filesystem::exists(base)) {
        // Search c) [base] directory.
        AddPath(base);
      }
    }
  }

  // Search 6) OPU root directory.
  searchPaths.emplace_back(opuPath);

  // Search 7) all OPU/[base]/maps/* subdirectories if we are searching for .map files.
  // This lets e.g. OPU/base/maps/misc/ademo1.dll refer to OPU/base/maps/campaign/eden04.map.
  if (extension == ".map") {
    for (const auto& base : bases) {
      if (std::filesystem::exists(base/"maps")) {
        for (auto& p : std::filesystem::recursive_directory_iterator(base/"maps", SearchOptions)) {
          if (p.is_directory()) {
            AddPath(p);
          }
        }
      }
    }
  }

  if (excludeStockDirs == false) {
    // Search 8) Outpost2.exe directory, then 9) Outpost 2 CD directory if available.
    AddPath(g_resManager.installedDir_);
    AddPath(g_resManager.cdDir_);
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
  bool result = filename.is_absolute() && std::filesystem::exists(filename);

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
            AIModDesc*const pAIModDesc = MissionManager::GetModuleDesc(path.string().data());

            if (pAIModDesc != nullptr) {
              if ((pAIModDesc->descBlock.missionType >= minMissionType) &&
                  (pAIModDesc->descBlock.missionType <= maxMissionType) &&
                  (pAIModDesc->descBlock.numPlayers  >= maxPlayers))
              {
                missions.emplace_back(
                  std::piecewise_construct,
                  std::forward_as_tuple(path.filename().string()),
                  std::forward_as_tuple(pAIModDesc, [](AIModDesc* p) { MissionManager::FreeModuleDesc(p); }));
              }
              else {
                MissionManager::FreeModuleDesc(pAIModDesc);
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
    char*const pFilenameBuf = static_cast<char*>(OP2Alloc(filename.length() + 1));
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
// Replacement function for SinglePlayerGameDialog::PopulateMissionList()
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
// See https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
static void AddModuleSearchPaths(
  const std::vector<std::filesystem::path>&  paths,
  bool                                       ifUnique = false)
{
  std::wstring pathEnv  = GetPathEnv();
  bool         modified = false;

  std::wstringstream ss(ifUnique ? pathEnv : L"");
  for (auto& src: paths) {
    const std::filesystem::path path = (src.is_relative() || src.empty()) ? (g_resManager.installedDir_/src) : src;
    if (std::filesystem::exists(path)) {
      bool found = false;

      if (ifUnique) {
        ss.seekg(0);
        for (std::wstring token; ((found == false) && std::getline(ss, token, L';'));) {
          found = (path == token) || ((path/"") == token);
        }
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
  const std::vector<std::filesystem::path>& paths)
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
// Prefers loading DLL import dependencies from the new module's file directory first, instead of the base module's.
// Also adds module directory to PATH for the sake of LoadLibrary().
// See https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
static HMODULE WINAPI LoadLibraryAltSearchPath(
  const char* pFilename)
{
  const std::filesystem::path path = GetFilePath(pFilename, g_searchForMission);
  if (path.is_absolute()) {
    AddModuleSearchPaths({ path.parent_path() }, true);
  }
  return LoadLibraryExW(path.wstring().data(), NULL, path.is_absolute() ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);
}

// =====================================================================================================================
void InitModuleSearchPaths() {
  static std::vector<HMODULE> hModules;

  const std::filesystem::path opuPath(OPUDir);
  const std::filesystem::path base(opuPath/BaseDir);

  // If this is called before TApp::Init(), then we need to init g_resManager's directories.
  g_resManager.InitInstalledDir();
  g_resManager.InitCDDir();

  // Set current directory to [install dir]/OPU if current directory was set to default.
  if ((std::filesystem::current_path()/"") == (std::filesystem::path(g_resManager.installedDir_)/"")) {
    SetCurrentDirectoryW((g_resManager.installedDir_/opuPath).wstring().data());
  }

  // Add default DLL search paths.
  AddModuleSearchPaths({ "", opuPath, opuPath/"libs" }, true);  // ** TODO remove this, op2ext handles these
  AddModuleSearchPaths({ base, base/"libs" });
  // ** TODO should add search paths for all mods

  // OPUPatch needs to keep some DLLs loaded to patch them;  load them here so they get loaded from the correct path.
  if (hModules.empty()) {
    for (const char* pFilename : { "OP2Shell.dll", "odasl.dll" }) {
      const HMODULE hModule = LoadLibraryAltSearchPath(pFilename);
      if (hModule != NULL) {
        hModules.push_back(hModule);
      }
    }

    if (hModules.empty() == false) {
      atexit([] { for (HMODULE hModule : hModules) { FreeLibrary(hModule); }  hModules.clear(); });
    }
  }
}

// =====================================================================================================================
bool SetFileSearchPathPatch(
  bool enable)
{
  static Patcher::PatchContext op2Patcher;
  static Patcher::PatchContext shellPatcher;

  static const std::wstring oldPathEnv(GetPathEnv());
  static const auto oldCwd(std::filesystem::current_path());

  static bool inited  = false;
         bool success = true;

  if (enable) {
    // Replace ResManager::GetFilePath()
    op2Patcher.Hook(0x471590, &GetFilePathHook);

    if (inited == false) {
      InitModuleSearchPaths();
      if (shellPatcher.GetModule() == GetModuleHandleA(nullptr)) {
        shellPatcher.SetModule("OP2Shell.dll", true);
      }
      inited = true;
    }

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
    op2Patcher.HookCall(0x489433, SetCapturedTrampoline, ThiscallFunctor(
      [F = (ibool(__thiscall*)(ResManager*, char*, char*))0](ResManager* pResManager, char* pResName, char* pOut) {
        g_searchForMission = true;
        const ibool result = F(pResManager, pResName, pOut);
        g_searchForMission = false;

        const std::filesystem::path path(result ? pOut : "");
        if (path.is_absolute()) {
          g_curMapPath = path.parent_path();
        }

        return result;
      }));

    static std::wstring preMissionPathEnv;

    // In MissionManager::LoadScript()
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
      const HMODULE hModule = LoadLibraryAltSearchPath(path.string().data());
      g_searchForMission    = false;

      return hModule;
    }));

    // In MissionManager::Deinit()
    op2Patcher.HookCall(0x402C2A, StdcallLambdaPtr([](HMODULE hModule) -> BOOL {
      const BOOL result = FreeLibrary(hModule);
      if (preMissionPathEnv.empty() == false) {
        SetEnvironmentVariableW(PathVar, preMissionPathEnv.data());
        preMissionPathEnv.clear();
      }
      g_curMapPath.clear();
      return result;
    }));

    // In MissionManager::Load(), ChecksumScript(), MultiplayerLobbyDialog::SetControlInfo(), ???_460770()
    for (const uintptr loc : { 0x402E4B, 0x45003C, 0x460B13, 0x46078C }) {
      op2Patcher.HookCall(loc, SetCapturedTrampoline, ThiscallFunctor(
        [F = (ibool(__thiscall*)(ResManager*, char*, char*))0](ResManager* pResManager, char* pResName, char* pOut) {
          g_searchForMission = true;
          const ibool result = F(pResManager, pResName, pOut);
          g_searchForMission = false;
          return result;
        }));
    }

    // In StartSierraNW() (SierraNW.dll)
    op2Patcher.HookCall(0x47D2B1, &LoadLibraryAltSearchPath);
    // In StartSNWValid() (SNWValid.dll)
    op2Patcher.LowLevelHook(0x47D56C,     [](Ebp<decltype(&LoadLibraryA)>& pfn) { pfn = &LoadLibraryAltSearchPath; });
    // In TApp::Init() (Out2res.dll)
    op2Patcher.HookCall(0x485C76, &LoadLibraryAltSearchPath);
    // In OP2Shell::Init() (op2shres.dll)
    shellPatcher.LowLevelHook(0x13007B9D, [](Ebx<decltype(&LoadLibraryA)>& pfn) { pfn = &LoadLibraryAltSearchPath; });

    success = (op2Patcher.GetStatus() == PatcherStatus::Ok) && (shellPatcher.GetStatus() == PatcherStatus::Ok);

    if (success) {
      atexit([] { SetFileSearchPathPatch(false); });
    }
  }

  if ((enable == false) || (success == false)) {
    if (inited) {
      SetEnvironmentVariableW(PathVar, oldPathEnv.data());
      std::error_code error;
      std::filesystem::current_path(oldCwd, error);
      inited = false;
    }

    success &= (op2Patcher.RevertAll()   == PatcherStatus::Ok);
    success &= (shellPatcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}


// =====================================================================================================================
// Changes netplay game start checksum validation logic.
bool SetChecksumPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Reimplement ChecksumScript()
    patcher.Hook(0x44FFE0, FastcallLambdaPtr([](int pOut[14], const char* pFilename) -> ibool {
      static const uint32 op2extChecksum   = g_resManager.ChecksumStream("op2ext.dll");
      static const uint32 opuPatchChecksum = g_resManager.ChecksumStream("OPUPatch.dll");

      AIModDesc* pAIModDesc = nullptr;
      const std::filesystem::path scriptPath = GetFilePath(pFilename, true);
      if (scriptPath.empty() == false) {
        pAIModDesc = MissionManager::GetModuleDesc(scriptPath.string().data());
      }

      int  i      = 0;
      bool result = true;
      auto AddChecksum = [pOut, &result, &i](uint32 checksum) { pOut[i] = checksum;  result &= (checksum != 0); };

      // Checksum sheets
      for (const char* p : { "building.txt", "mines.txt", "morale.txt", "space.txt", "vehicle.txt", "weapons.txt" }) {
        AddChecksum(g_resManager.ChecksumStream(p));
      }

      // Normally, edentek.txt, ply_tek.txt, and multitek.txt get checksummed here.
      // Use these 3 slots instead for the actual map's tech checksum, op2ext.dll, and OPUPatch.dll.
      // ** TODO tech checksum could be smarter to factor out localization etc. for now we checksum spoof the files
      AddChecksum((pAIModDesc == nullptr) ? 0 : g_resManager.ChecksumStream(pAIModDesc->pTechtreeName));
      AddChecksum(op2extChecksum);
      AddChecksum(opuPatchChecksum);

      // Checksum OP2Shell.dll (seems pointless, maybe we can reuse this for something else)
      AddChecksum(g_tApp.ChecksumShell());

      // Checksum Outpost2.exe (seems pointless as long as this is a const, maybe we can reuse this for something else)
      AddChecksum(0x59010E28);

      // Checksum mission script
      AddChecksum((pAIModDesc == nullptr) ? 0 : pAIModDesc->checksum);

      // Checksum map file
      AddChecksum((pAIModDesc == nullptr) ? 0 : g_resManager.ChecksumStream(pAIModDesc->pMapName));

      // Overall checksum
      pOut[i] = TApp::Checksum(pOut, sizeof(int) * i);
      assert(i == 13);

      if (pAIModDesc != nullptr) {
        MissionManager::FreeModuleDesc(pAIModDesc);
      }

      return result;
    }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}


// =====================================================================================================================
// Locally injects the Indeo 4.1 codec needed for game cutscenes if it is not installed in the OS.
bool SetCodecPatch(
  bool enable)
{
  constexpr DWORD IndeoFourCC = mmioFOURCC('i', 'v', '4', '1');

  static Patcher::PatchContext patcher;
  static bool useLocalIndeo = false;
         bool success       = true;

  if (enable) {
    // Fucking drivers, how do they work?
    // Microsoft VfW API is a piece of shit and the documentation is an even bigger piece of shit,
    // work around some issues that cause the Indeo codec to crash when registered as a function via ICInstall().
    auto IndeoDriverProc = [](DWORD_PTR dwDriverId, HDRVR hDrvr, UINT msg, LONG lParam1, LONG lParam2) {
      static HMODULE hMod       = LoadLibraryAltSearchPath("ir41_32.ax");
      static auto    driverProc = (hMod != NULL) ? DRIVERPROC(GetProcAddress(hMod, "DriverProc")) : nullptr;
      
      LRESULT result = 0;
      
      if (driverProc != nullptr) {
        // When registered as a function, the codec crashes if ICM_GETINFO is sent, so we handle that ourselves here.
        // This is also the first message sent, so we send the real codec DRV_LOAD and DRV_ENABLE messages as would get
        // sent otherwise (for some reason they aren't sent to function-based codecs).
        if (msg == ICM_GETINFO) {
          if ((driverProc(dwDriverId, hDrvr, DRV_LOAD,   lParam1, lParam2) != 0) &&
              (driverProc(dwDriverId, hDrvr, DRV_ENABLE, lParam1, lParam2) != 0) &&
              (lParam2 >= sizeof(ICINFO)))
          {
            auto*const pInfo = (ICINFO*)(lParam1);
            pInfo->dwSize           = sizeof(ICINFO);
            pInfo->fccType          = ICTYPE_VIDEO;
            pInfo->fccHandler       = IndeoFourCC;
            pInfo->dwFlags          = VIDCF_DRAW;
            pInfo->dwVersion        = 0;
            pInfo->dwVersionICM     = ICVERSION;
            pInfo->szName[0]        = '\0';
            pInfo->szDescription[0] = '\0';
            result = sizeof(ICINFO);
          }
        }
        else {
          result = driverProc(dwDriverId, hDrvr, msg, lParam1, lParam2);
        }
      }
      
      return result;
    };

    // Check for Indeo 4.1 codec and install as a function if missing.
    if (useLocalIndeo == false) {
      ICINFO icInfo;
      const bool indeo41Present = ICInfo(ICTYPE_VIDEO, IndeoFourCC, &icInfo);

      if (indeo41Present == false) {
        useLocalIndeo =
          ICInstall(ICTYPE_VIDEO, IndeoFourCC, LPARAM(StdcallLambdaPtr(IndeoDriverProc)), nullptr, ICINSTALL_FUNCTION);
        success &= useLocalIndeo;
      }
    }

    if (success) {
      atexit([] { SetCodecPatch(false); });
    }
  }

  if (((enable == false) || (success == false)) && useLocalIndeo) {
    useLocalIndeo = (ICRemove(ICTYPE_VIDEO, IndeoFourCC, 0) == false);
    success      &= (useLocalIndeo == false);
  }

  return true;  // We currently don't treat this as fatal.
}
