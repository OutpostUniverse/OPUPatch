
// ** TODO A lot of this should probably be moved into op2ext due to overlap with mod loader functionality.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>
#include <intrin.h>

#include "Patcher.h"
#include "Util.h"
#include "Stream.h"

#include "Tethys/Common/Library.h"
#include "Tethys/Game/TApp.h"
#include "Tethys/Game/MissionManager.h"
#include "Tethys/Resource/StreamIO.h"
#include "Tethys/Resource/CConfig.h"
#include "Tethys/Resource/ResManager.h"
#include "Tethys/Resource/SoundManager.h"
#include "Tethys/Resource/MemoryMappedFile.h"
#include "Tethys/UI/IWnd.h"

#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <charconv>
#include <filesystem>
#include <memory>

using namespace Tethys;
using namespace TethysAPI;
using namespace Tethys::TethysUtil;
using namespace Patcher::Util;
using namespace Patcher::Registers;

using MissionList = std::vector<std::pair<std::string, std::unique_ptr<AIModDesc, void(*)(AIModDesc*)>>>;

static constexpr wchar_t PathVar[] = L"PATH";

static constexpr char OPUDir[]  = "OPU";
static constexpr char BaseDir[] = "base";

static std::filesystem::path g_curMapPath;

static bool g_searchForMission = false;
static ThreadLocal<bool> g_tlsSkipOsFileRemap;  // ** TODO?


// =====================================================================================================================
// Gets op2ext mod directories.
static std::vector<std::filesystem::path> GetModPaths() {
  static std::vector<std::filesystem::path> paths;

  static Library op2Ext("op2ext.dll");
  static auto*const pfnGetModuleDirectoryCount = op2Ext.Get<size_t CDECL()>("GetModuleDirectoryCount");
  static auto*const pfnGetModuleDirectory      = op2Ext.Get<size_t CDECL(size_t, char*, size_t)>("GetModuleDirectory");

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
// Gets base subdirectory paths: current map, mods, OPU/base, then OPU.
static std::vector<std::filesystem::path> GetBasePaths() {
  static std::vector<std::filesystem::path> bases;
  static std::filesystem::path              mapPath;

  if ((bases.empty()) || (mapPath != g_curMapPath)) {
    static const auto installPath(std::filesystem::path(g_resManager.installedDir_));
    static const auto opuPath(installPath/OPUDir);

    bases.clear();
    mapPath = g_curMapPath;

    if ((mapPath != opuPath/BaseDir) && (mapPath != opuPath) && (mapPath != installPath)) {
      bases.push_back(mapPath);
    }

    const auto& modPaths = GetModPaths();
    bases.insert(bases.end(), modPaths.begin(), modPaths.end());

    bases.push_back(opuPath/BaseDir);
    bases.push_back(opuPath);
  }

  return bases;
}

// =====================================================================================================================
// Gets file search paths for a particular asset type.
std::vector<std::filesystem::path> GetSearchPaths(
  std::string  extension,
  bool         searchForMission,
  bool         excludeStockDirs)
{
  constexpr auto SearchOptions = std::filesystem::directory_options::follow_directory_symlink |
                                 std::filesystem::directory_options::skip_permission_denied;

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
  const auto& bases = GetBasePaths();
  for (const auto& base : bases) {
    static const std::multimap<std::string, std::string> assetDirs = {
      { ".ax",    "libs"     },
      { ".dll",   "libs"     },
      { ".dll",   "maps"     },
      { ".py",    "maps"     },
      { ".map",   "maps"     },
      { ".txt",   "sheets"   },
      { ".txt",   "techs"    },
      { ".txt",   "story"    },
      { ".rtf",   "story"    },
      { ".ttf",   "ui"       },
      { ".ttc",   "ui"       },
      { ".otf",   "ui"       },
      { ".png",   "ui"       },
      { ".bmp",   "ui"       },
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
      { ".op2",   "saves"    }
    };

    const auto range = assetDirs.equal_range(extension);
    for (auto it = range.first; it != range.second; ++it) {
      const auto assetDir = base/it->second;

      if (std::filesystem::exists(assetDir)) {
        if (searchForMission && ((it->first == ".dll") || (it->first == ".py")) && (it->second == "maps")) {
          // Search a) top level of [base]/maps/* subdirectories iff we are searching for mission scripts.
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

  // Search 6) all OPU/[base]/maps/* subdirectories if we are searching for .map files.
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
    // Search 7) Outpost2.exe directory, then 8) Outpost 2 CD directory if available.
    AddPath(g_resManager.installedDir_);
    AddPath(g_resManager.cdDir_);
  }

  return searchPaths;
}

// =====================================================================================================================
// Gets the path to a game file as per GetSearchPaths().
std::filesystem::path GetFilePath(
  const std::filesystem::path&  filename,
  bool                          searchForMission)
{
  constexpr auto SearchOptions = std::filesystem::directory_options::follow_directory_symlink |
                                 std::filesystem::directory_options::skip_permission_denied;

  //g_tlsSkipOsFileRemap = true;

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

  //g_tlsSkipOsFileRemap = false;

  return result ? path : std::filesystem::path();
}

// =====================================================================================================================
// Replacement function for ResManager::GetFilePath().
static ibool __fastcall GetFilePathHook(
  ResManager*  pThis,  int,
  char*        pResName,
  char*        pOutputFilename)
{
  const std::filesystem::path path = GetFilePath(pResName, g_searchForMission);

  if (pOutputFilename != nullptr) {
    pOutputFilename[0] = '\0';
    if (path.empty() == false) {
      strncpy_s(pOutputFilename, MAX_PATH, path.string().data(), _TRUNCATE);
    }
  }

  return (path.empty() == false);
}


// =====================================================================================================================
// Gets the list of mission DLLs matching the specified type(s) and player count.
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

  for (const char* pExtension : { ".dll", ".py" }) {
    for (const auto& searchPath : GetSearchPaths(pExtension, true)) {
      if (std::filesystem::exists(searchPath)) {
        for (auto& p : std::filesystem::directory_iterator(searchPath, SearchOptions)) {
          const auto& path = p.path();

          if (path.has_extension() && (path.extension() == pExtension)) {
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
// malloc using OP2Shell's heap.
static void* ShellAlloc(
  size_t size)
{
  static auto*const pfnAlloc =
    reinterpret_cast<void*(CDECL*)(size_t)>(0x1300A320 - OP2ShellBase + uintptr(GetModuleHandleA("OP2Shell.dll")));
  return pfnAlloc(size);
}

// =====================================================================================================================
// Replacement function for PopulateMultiplayerMissionList().
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
    if (char*const pFilenameBuf = static_cast<char*>(OP2Alloc(filename.length() + 1));  pFilenameBuf != nullptr) {
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
// Replacement function for SinglePlayerGameDialog::PopulateMissionList().
static void __fastcall PopulateSinglePlayerMissionListHook(
  IDlgWnd* pThis)
{
  const HWND hListBoxWnd = GetDlgItem(pThis->hWnd_, 1076);
  auto*const pFileFilter = static_cast<char*>(PtrInc(pThis, 0xAA));  // ** TODO Define SinglePlayerGameDialog
  const MissionType type = (pFileFilter[0] == 't') ? MissionType::Tutorial : MissionType::Colony;

  SendMessageA(hListBoxWnd, LB_RESETCONTENT, 0, 0);
  SendMessageA(hListBoxWnd, WM_SETREDRAW,    0, 0);

  for (const auto& [filename, pAIModDesc] : GetMissionList(type)) {
    if (char*const pFilenameBuf = static_cast<char*>(ShellAlloc(filename.length() + 1));  pFilenameBuf != nullptr) {
      strncpy_s(pFilenameBuf, filename.length() + 1, filename.data(), _TRUNCATE);
      const LRESULT listEntry = SendMessageA(hListBoxWnd, LB_ADDSTRING, 0, LPARAM(pAIModDesc->pLevelDesc));
      SendMessageA(hListBoxWnd, LB_SETITEMDATA, listEntry, LPARAM(pFilenameBuf));
    }
  }

  SendMessageA(hListBoxWnd, LB_SETCURSEL, 0, 0);
  SendMessageA(hListBoxWnd, WM_SETREDRAW, 1, 0);
}

// =====================================================================================================================
// Returns the version of an Outpost 2 saved game file.
GameVersion GetSavedGameVersion(
  StreamIO* pSavedGame,
  bool      seekBack)
{
  static const StreamIO* pPrevious = nullptr;
  static GameVersion previousVersion = { };

  GameVersion version = { };

  if (pSavedGame == pPrevious) {
    version = previousVersion;
  }
  else if (const size_t oldPos = pSavedGame->Tell();  pSavedGame->Seek(0)) {
    if (char tag[25] = "";  pSavedGame->Read(sizeof(tag), &tag[0])) {
      if (strcmp("OUTPOST 2.00 SAVED GAME\032", &tag[0]) == 0) {
        version = {1, 2, 7};
      }
      else if ((strncmp("OUTPOST2 ", &tag[0], 9) == 0) && (strncmp("-OPU SAVE", &tag[14], 9) == 0)) {
        version = { ((tag[9] - '0') * 100), ((tag[11] - '0') * 10), (tag[13] - '0')};
      }
    }

    if (seekBack) {
      pSavedGame->Seek(oldPos);
    }

    pPrevious       = pSavedGame;
    previousVersion = version;
  }

  return version;
}


// =====================================================================================================================
// Gets PATH environment variable.
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
void AddModuleSearchPaths(
  const std::vector<std::filesystem::path>&  paths,
  bool                                       ifUnique)
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
void RemoveModuleSearchPaths(
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
HMODULE WINAPI LoadLibraryAltSearchPath(
  const char* pFilename)
{
  std::filesystem::path path = GetFilePath(pFilename, g_searchForMission);
  if (path.is_absolute()) {
    AddModuleSearchPaths({ path.parent_path() }, true);
  }
  else {
    path = pFilename;
  }
  return LoadLibraryExW(path.wstring().data(), NULL, path.is_absolute() ? LOAD_WITH_ALTERED_SEARCH_PATH : 0);
}

// =====================================================================================================================
static void InitModuleSearchPaths() {
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
  const auto& modPaths = GetModPaths();
  for (auto it = modPaths.rbegin(); it != modPaths.rend(); ++it) {
    AddModuleSearchPaths({ *it, (*it)/"libs" }, true);
  }

  // OPUPatch needs to keep some DLLs loaded to patch them;  load them here so they get loaded from the correct path.
  if (hModules.empty()) {
    for (const char* pFilename : { "OP2Shell.dll", "odasl.dll" }) {
      if (const HMODULE hModule = LoadLibraryAltSearchPath(pFilename);  hModule != NULL) {
        hModules.push_back(hModule);
      }
    }

    if (hModules.empty() == false) {
      static const auto cleanup = atexit([] { for (HMODULE h : hModules) { FreeLibrary(h); }  hModules.clear(); });
    }
  }
}

// =====================================================================================================================
// Hooks game file search path logic, allowing for loading files from context-based subdirs under the Outpost2/OPU dir.
bool SetFileSearchPathPatch(
  bool enable)
{
  static Patcher::PatchContext op2Patcher;
  static Patcher::PatchContext shellPatcher;
  static Patcher::PatchContext sysPatcher(&CreateFileA);

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

        if (const std::filesystem::path path(result ? pOut : "");  path.is_absolute()) {
          g_curMapPath = path.parent_path();
        }

        return result;
      }));

    static std::wstring preMissionPathEnv;

    // In MissionManager::LoadScript()
    op2Patcher.HookCall(0x402B44, StdcallLambdaPtr([](const char* pFilename) {
      std::filesystem::path path(pFilename);

      if (char buf[MAX_PATH] = "";  path.is_relative() && g_resManager.GetFilePath(pFilename, &buf[0])) {
        path = buf;
      }

      path              = std::filesystem::absolute(path);
      g_curMapPath      = path.parent_path();
      preMissionPathEnv = GetPathEnv();
      AddModuleSearchPaths({ g_curMapPath, g_curMapPath/"libs" }, true);

      g_searchForMission    = true;
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

    // Try to load sounds from files instead of only from VOLs
    // In SoundManager::Init()
    op2Patcher.HookCall(0x47E027, ThiscallLambdaPtr([](ResManager* pThis, const char* pFilename, size_t* pSize) {
      static std::map<std::string, MemoryMappedFile> soundData;
      const std::string filename(pFilename);

      auto it    = soundData.find(filename);
      bool found = (it != soundData.end()) && (it->second.pMappedAddress_ != nullptr);
      if (found == false) {
        const auto path = GetFilePath(filename);
        found = (path.empty() == false) && soundData[filename].OpenFile(path.string().data(), false);
        it    = soundData.find(filename);
      }

      if (found) {
        *pSize = it->second.size_;
      }
      return found ? it->second.pMappedAddress_ : pThis->LockStream(pFilename, pSize);
    }));

    // Look for saved games under OPU/saves
    // In TFileDialog::???()
    op2Patcher.HookCall(0x416F78, ThiscallLambdaPtr([](ResManager* pThis, char* pOut) {
      static const auto savesPath(std::filesystem::path(g_resManager.installedDir_)/OPUDir/"saves"/"");
      std::filesystem::create_directories(savesPath);
      strncpy_s(pOut, MAX_PATH, savesPath.string().data(), _TRUNCATE);
    }));

#if 0
    static const auto installPath = std::filesystem::path(g_resManager.installedDir_);
    static const auto RemapPath   = [](std::filesystem::path path) {
      const bool skip = ((g_tlsSkipOsFileRemap.IsValid() == false) || g_tlsSkipOsFileRemap);
      if ((skip == false) && path.has_filename() && path.has_parent_path() &&
          (path.parent_path()/"" == installPath/"") || (path.parent_path()/"" == installPath/OPUDir/""))
      {
        if (std::filesystem::path searchPath = GetFilePath(path.filename());  searchPath.empty() == false) {
          path = searchPath;
        }
      }
      return path;
    };

    // Hook Win32 APIs CreateFileA/W, (CreateFile2,) (CreateFileTransactedA/W,) ...?
    sysPatcher.Hook(&CreateFileA, SetCapturedTrampoline, StdcallFunctor([F = decltype(&CreateFileA){}](
      const char* pPath, DWORD access, DWORD share, SECURITY_ATTRIBUTES* pAttr, DWORD disp, DWORD flags, HANDLE hTmpl
      ) -> HANDLE
    {
      return F(RemapPath(pPath).string().data(), access, share, pAttr, disp, flags, hTmpl);
    }));

    sysPatcher.Hook(&CreateFileW, SetCapturedTrampoline, StdcallFunctor([F = decltype(&CreateFileW){}](
      const wchar_t* pPath, DWORD access, DWORD share, SECURITY_ATTRIBUTES* pAttr, DWORD disp, DWORD flags, HANDLE hTmpl
      ) -> HANDLE
    {
      return F(RemapPath(pPath).wstring().data(), access, share, pAttr, disp, flags, hTmpl);
    }));
#endif

    success = (op2Patcher.GetStatus()   == PatcherStatus::Ok) &&
              (shellPatcher.GetStatus() == PatcherStatus::Ok) &&
              (sysPatcher.GetStatus()   == PatcherStatus::Ok);

    if (success) {
      static const auto cleanup = atexit([] { SetFileSearchPathPatch(false); });
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
    success &= (sysPatcher.RevertAll()   == PatcherStatus::Ok);
  }

  return success;
}


// =====================================================================================================================
// Checksums a tech tree, factoring out non-breaking deltas like localization, definition order, whitespace, etc.
static uint32 ChecksumTech(
  const char* pFilename)
{
  uint32 checksum = 0;

  // Get tech file contents (use OpenStream to search in VOLs too).
  std::stringstream techFile;
  if (auto*const pStream = g_resManager.OpenStream(pFilename);  pStream != nullptr) {
    for (char c = '\0'; pStream->Read(1, &c); techFile << c);
    g_resManager.ReleaseStream(pStream);
  }

  // Use a sorted map, since OP2 sorts techs by ID. Map of TechID: list of normalized strings within BEGIN_TECH/END_TECH
  std::map<int, std::vector<std::string>> techInfos;
  int curTech = -1;

  for (std::string l; std::getline(techFile, l);) {
    // Strip side whitespace.
    const size_t left  = l.find_first_not_of(" \t\r");
    const size_t right = l.find_last_not_of(" \t\r");
    const std::string_view line((left  != std::string::npos) ? (l.begin() + left)      : l.end(),
                                (right != std::string::npos) ? (l.begin() + right + 1) : l.end());

    if (line.empty() || (line[0] == ';')) {
      // Ignore blank lines and comments.
      continue;
    }
    else if (curTech == -1) {
      if (line.find("BEGIN_TECH") == 0) {
        // Extract tech ID, ignore name string.
        if (size_t pos = line.find_last_of(" \t");  pos != std::string::npos) {
          const auto [p, ec] = std::from_chars(line.data() + pos + 1, line.data() + line.size(), curTech);
        }
      }
    }
    else {
      if (line.find("END_TECH") == 0) {
        curTech = -1;
      }
      else if ((line.find("TEASER") == 0) || (line.find("DESCRIPTION") == 0)) {
        // Ignore TEASER and DESCRIPTION to factor out localization.
        continue;
      }
      else if (line.find("IMPROVE_DESC") == 0) {
        // Include the IMPROVE_DESC token in the checksum since it has side effects, but ignore the string value.
        techInfos[curTech].emplace_back("IMPROVE_DESC");
      }
      else {
        // Normalize whitespace in inner parts of line.
        for (size_t pos = 0; ((pos = l.find("\t", pos)) != std::string::npos); l[pos++] = ' ');
        const auto end = std::copy_if(
          line.begin(), line.end(), l.begin(), [](const char& c) { return (c != ' ') || ((&c)[1] != ' '); });
        techInfos[curTech].emplace_back(l.begin(), end);
      }
    }
  }

  // Checksum the normalized tech definition contents.
  for (auto& [techID, techInfo] : techInfos) {
    std::sort(techInfo.begin(), techInfo.end());
    checksum ^= TApp::Checksum(techID);
    for (const auto& line : techInfo) {
      checksum ^= TApp::ChecksumData(line.data(), line.size());
    }
  }

  return checksum;
}

// =====================================================================================================================
// Changes netplay game start checksum validation logic.
// ** TODO output checksums to log file when checksum mismatch occurs, for now there's debug messages
bool SetChecksumPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // Reimplement ChecksumScript()
    patcher.Hook(0x44FFE0, FastcallLambdaPtr([](int pOut[14], const char* pFilename) -> ibool {
      static const uint32 op2ExtChecksum   = g_resManager.ChecksumStream("op2ext.dll");
      static const uint32 opuPatchChecksum = g_resManager.ChecksumStream("OPUPatch.dll");

      AIModDesc* pAIModDesc = nullptr;
      if (const std::filesystem::path scriptPath = GetFilePath(pFilename, true);  scriptPath.empty() == false) {
        pAIModDesc = MissionManager::GetModuleDesc(scriptPath.string().data());
        if (scriptPath.is_absolute()) {
          g_curMapPath = scriptPath.parent_path();
        }
      }

      int  i      = 0;
      bool result = true;
      auto AddChecksum = [pOut, &result, &i](uint32 checksum, const char* pName = nullptr) {
        pOut[i++] = checksum; result &= (checksum != 0); DEBUG_MSG("%s checksum = %08X", pName ? pName : "", checksum);
      };

      // Checksum sheets
      for (const char* p : { "building.txt", "mines.txt", "morale.txt", "space.txt", "vehicles.txt", "weapons.txt" }) {
        AddChecksum(g_resManager.ChecksumStream(p), p);
      }

      // Normally, edentek.txt, ply_tek.txt, and multitek.txt get checksummed here.
      // Use these 3 slots instead for the actual map's tech checksum, op2ext.dll, and OPUPatch.dll.
      AddChecksum((pAIModDesc != nullptr) ? ChecksumTech(pAIModDesc->pTechtreeName) : 0,
                  (pAIModDesc != nullptr) ? pAIModDesc->pTechtreeName               : "Invalid script's tech");
      AddChecksum(op2ExtChecksum,   "op2ext.dll");
      AddChecksum(opuPatchChecksum, "OPUPatch.dll");

      // Checksum OP2Shell.dll (seems pointless, multiplayer setup dialogs are all handled inside Outpost2.exe)
      AddChecksum(g_tApp.ChecksumShell(), "OP2Shell.dll");

      // Checksum Outpost2.exe (seems pointless as long as this is a const, maybe we can reuse this for something else)
      AddChecksum(0x59010E28, "Outpost2.exe");

      // Checksum mission script
      AddChecksum((pAIModDesc != nullptr) ? pAIModDesc->checksum : 0, pFilename);

      // Checksum map file
      AddChecksum((pAIModDesc != nullptr) ? g_resManager.ChecksumStream(pAIModDesc->pMapName) : 0,
                  (pAIModDesc != nullptr) ? pAIModDesc->pMapName                              : "Invalid script's map");

      // Overall checksum
      AddChecksum(TApp::ChecksumData(pOut, sizeof(int) * i), "Overall");
      assert(i == 14);

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
// Locally injects the Indeo 4.1 codec needed for game cutscenes if it is not registered in the OS.
bool SetCodecFix(
  bool enable)
{
  constexpr DWORD IndeoFourCC = mmioFOURCC('i', 'v', '4', '1');

  static bool useLocalIndeo = false;
         bool success       = true;

  if (enable) {
    // Check for Indeo 4.1 codec and install as a function if missing.
    if (useLocalIndeo == false) {
      ICINFO icInfo;
      const bool indeo41Present = ICInfo(ICTYPE_VIDEO, IndeoFourCC, &icInfo);

      if (indeo41Present == false) {
        static const HMODULE hMod          = LoadLibraryAltSearchPath("ir41_32.ax");
        static auto*const    pfnDriverProc = (hMod != NULL) ? DRIVERPROC(GetProcAddress(hMod, "DriverProc")) : nullptr;
        static const auto    cleanup       = (hMod != NULL) ? atexit([] { FreeLibrary(hMod); }) : 0;

        // Work around some issues that cause the Indeo codec to crash when registered as a function via ICInstall().
        auto IndeoDriverProc = [](DWORD_PTR dwDriverId, HDRVR hDrvr, UINT msg, LONG lParam1, LONG lParam2) {
          LRESULT result = 0;
      
          // When registered as a function, the codec crashes if ICM_GETINFO is sent, so we handle that ourselves here.
          // This is also the first message sent, so we send the real codec DRV_LOAD and DRV_ENABLE messages as would
          // get sent otherwise (for some reason they aren't sent to function-based codecs).
          if (msg == ICM_GETINFO) {
            if ((pfnDriverProc(dwDriverId, hDrvr, DRV_LOAD,   lParam1, lParam2) != 0) &&
                (pfnDriverProc(dwDriverId, hDrvr, DRV_ENABLE, lParam1, lParam2) != 0) &&
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
            result = pfnDriverProc(dwDriverId, hDrvr, msg, lParam1, lParam2);
          }
      
          return result;
        };

        useLocalIndeo = (pfnDriverProc != NULL) && ICInstall(
          ICTYPE_VIDEO, IndeoFourCC, LPARAM(StdcallLambdaPtr(IndeoDriverProc)), nullptr, ICINSTALL_FUNCTION);
        success &= useLocalIndeo;
      }
    }

    if (success) {
      static const auto cleanup = atexit([] { SetCodecFix(false); });
    }
  }

  if (((enable == false) || (success == false)) && useLocalIndeo) {
    useLocalIndeo = (ICRemove(ICTYPE_VIDEO, IndeoFourCC, 0) == false);
    success      &= (useLocalIndeo == false);
  }

  return true;  // We currently don't treat this as fatal.
}
