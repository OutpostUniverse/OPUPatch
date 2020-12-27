
#pragma once

#include "Tethys/Common/Types.h"

#include <filesystem>
#include <vector>
#include <string>

// Gets the path to a game file as per GetSearchPaths().
std::filesystem::path GetFilePath(const std::filesystem::path& filename, bool searchForMission = false);

// Gets file search paths for a particular asset type.
std::vector<std::filesystem::path> GetSearchPaths(
  std::string extension = "", bool searchForMission = false, bool excludeStockDirs = false);

// Prefers loading DLL import dependencies from the new module's file directory first, instead of the base module's.
// Also adds module directory to PATH for the sake of LoadLibrary().
// See https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
HMODULE WINAPI LoadLibraryAltSearchPath(const char* pFilename);

// Adds DLL search paths.
// This does not use AddDllDirectory() because it's not WinXP-compatible, and multiple path search order is undefined.
// See https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-search-order
void AddModuleSearchPaths(const std::vector<std::filesystem::path>& paths, bool ifUnique = false);

// Removes DLL search paths.
void RemoveModuleSearchPaths(const std::vector<std::filesystem::path>& paths);
