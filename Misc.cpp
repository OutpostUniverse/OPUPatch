
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>
#include <dsound.h>

#include "Tethys/API/TethysGame.h"
#include "Tethys/API/GameMap.h"
#include "Tethys/Resource/CConfig.h"
#include "Tethys/Resource/StreamIO.h"
#include "Tethys/Game/PathFinder.h"

#include "Patcher.h"
#include "Util.h"
#include "Stream.h"
#include "Resources.h"

#include <algorithm>

using namespace Tethys;
using namespace TethysAPI;
using namespace Patcher::Util;
using namespace Patcher::Registers;

// =====================================================================================================================
// Sets the game version per OP2_MAJOR_VERSION, OP2_MINOR_VERSION, OP2_STEPPING_VERSION defined in Version.h.
// This affects the version used for netplay compatibility checks, saved games, and a couple places in the UI not
// covered by resource replacements.
bool SetGameVersion(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // For netplay, TApp::Version()
    patcher.ReplaceReferencesToGlobal(0x4E973C, 1, OP2_VERSION_QUAD_STR);

    // For UI display
    patcher.ReplaceReferencesToGlobal(0x4E3200, 1, OP2_VERSION_TRIPLE_STR "-OPU");
    patcher.Write(0x566708 + 596,       "Version " OP2_VERSION_TRIPLE_STR "-OPU");

    // For saved games
    patcher.ReplaceReferencesToGlobal(0x4D6380, 1, "OUTPOST2 " OP2_VERSION_TRIPLE_STR "-OPU SAVE\034");

    // In GameImpl::LoadGame()
    patcher.LowLevelHook(0x48A413, [](Edi<StreamIO*> pStream) {
      const int saveVersion = GetSavedGameVersion(pStream);
      return ((saveVersion >= GameVersion{1, 2, 7}) && (saveVersion <= OP2Version)) ? 0x48A451 : 0x48A445;
    });

    // Replace GameImpl::VerifySavedGameFileTag()
    patcher.Hook(0x48A190, ThiscallLambdaPtr([](void* pThis, StreamIO* pStream) -> ibool {
      const int saveVersion = GetSavedGameVersion(pStream, false);
      return (saveVersion >= GameVersion{1, 2, 7}) && (saveVersion <= OP2Version);
    }));

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Disables checking for the Outpost 2 CD being inserted.
bool SetNoCdPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In ResManager::VerifyCD()
    patcher.Write<uint8>(0x471900, 0xC3);  // 0x81
    // In ResManager::Init()
    patcher.Write<uint8>(0x47104D, 0xEB);  // 0x75
    // In ResManager::InitCDDir()
    patcher.Write<uint8>(0x471A17, 0xEB);  // 0x75

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes an issue with TethysGame::ForceMorale*() when playerNum is not set to all players, that would force it to 99
// no matter what unless called twice.
bool SetForceMoraleFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In GameImpl::ProcessGameOptPacket()
    patcher.LowLevelHook(0x422E0A, [](Eax<uint32> mask, Edi<CommandPacket*> pPacket) {
      for (auto& morale : g_gameImpl.forceMorale_) {
        morale &= ~mask;
      }
      reinterpret_cast<uint32*>(&g_gameImpl)[pPacket->data.gameOpt.offsetInDwords] |= mask;
      return 0x422E66;
    });

    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Allows music to continue playing when the game window is not active.
bool SetGlobalMusicFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In MusicManager::InitializeDirectSound()
    patcher.LowLevelHook(0x450719, [](Eax<DSBUFFERDESC*> pBufferDesc) { pBufferDesc->dwFlags |= DSBCAPS_GLOBALFOCUS; });
    success = (patcher.GetStatus() == PatcherStatus::Ok);
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Fixes an issue with printf conversion of floats.
bool SetPrintfFloatFix(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  if (enable) {
    // In CRT __cftof()
    success = (patcher.Write<uint32>(0x4C55A2, 0x30303000) == PatcherStatus::Ok);  // add eax, 0x30303030 => 0x30303000
  }

  if ((enable == false) || (success == false)) {
    success &= (patcher.RevertAll() == PatcherStatus::Ok);
  }

  return success;
}

// =====================================================================================================================
// Forces the game loop to run at full speed for debugging purposes.  Not recommended for use in multiplayer.
bool SetSuperSpeedPatch(
  bool enable)
{
  static Patcher::PatchContext patcher;
  bool success = true;

  // 0 = Normal speed, 1 = super speed
  static int superSpeedMode = g_configFile.GetInt("Game", "SuperSpeed", -1);
  if (superSpeedMode == -1) {
    superSpeedMode = 0;
    g_configFile.SetInt("Game", "SuperSpeed", superSpeedMode);
  }

  enable &= (superSpeedMode != 0);
  if (enable) {
    patcher.Write(&g_gameFrame.iniSettings_.frameSkip, 2); // Skip rendering every other frame to further improve speed
    success = (patcher.Write<uint8>(0x49C374, 0xEB) == PatcherStatus::Ok); // 0x73
  }

  if ((enable == false) || (success == false)) {
    success &= ((patcher.RevertAll() == PatcherStatus::Ok));
  }

  return success;
}
